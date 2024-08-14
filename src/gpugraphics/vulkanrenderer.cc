#include "vulkanrenderer.hpp"

#include <QApplication>
#include <QFile>
#include <QPainter>
#include <QVulkanFunctions>

namespace GpuGraphics {

// Use a triangle strip to get a quad.
//
// Note that the vertex data and the projection matrix assume OpenGL. With
// Vulkan Y is negated in clip space and the near/far plane is at 0/1 instead
// of -1/1. These will be corrected for by an extra transformation when
// calculating the modelview-projection matrix.
static float vertexData[] = { // Y up, front = CW
    // x, y, z, u, v
    -1, -1, 0, 0, 1, -1, 1, 0, 0, 0, 1, -1, 0, 1, 1, 1, 1, 0, 1, 0};

static const int UNIFORM_DATA_SIZE = 16 * sizeof(float);

static inline auto aligned(VkDeviceSize v, VkDeviceSize byteAlign) -> VkDeviceSize
{
    return (v + byteAlign - 1) & ~(byteAlign - 1);
}

struct Texture
{
    Texture()
    {
        whiteImage = QImage(10, 10, QImage::Format_RGBA8888_Premultiplied);
        whiteImage.fill(Qt::white);
    }

    void cleanup(VkDevice &dev, QVulkanDeviceFunctions *deviceFunctions)
    {
        if (texStaging != VK_NULL_HANDLE) {
            deviceFunctions->vkDestroyImage(dev, texStaging, nullptr);
            texStaging = VK_NULL_HANDLE;
        }

        if (texStagingMem != VK_NULL_HANDLE) {
            deviceFunctions->vkFreeMemory(dev, texStagingMem, nullptr);
            texStagingMem = VK_NULL_HANDLE;
        }

        if (texView != VK_NULL_HANDLE) {
            deviceFunctions->vkDestroyImageView(dev, texView, nullptr);
            texView = VK_NULL_HANDLE;
        }

        if (texImage != VK_NULL_HANDLE) {
            deviceFunctions->vkDestroyImage(dev, texImage, nullptr);
            texImage = VK_NULL_HANDLE;
        }

        if (texMem != VK_NULL_HANDLE) {
            deviceFunctions->vkFreeMemory(dev, texMem, nullptr);
            texMem = VK_NULL_HANDLE;
        }

        texLayoutPending = false;
        texStagingPending = false;
    }

    VkImage texImage = VK_NULL_HANDLE;
    VkDeviceMemory texMem = VK_NULL_HANDLE;
    bool texLayoutPending = false;
    VkImageView texView = VK_NULL_HANDLE;
    VkImage texStaging = VK_NULL_HANDLE;
    VkDeviceMemory texStagingMem = VK_NULL_HANDLE;
    bool texStagingPending = false;
    QSize texSize;
    VkFormat texFormat;

    QImage whiteImage;
};

class VulkanRenderer::VulkanRendererPrivate
{
public:
    explicit VulkanRendererPrivate(VulkanRenderer *q)
        : q_ptr(q)
    {}

    [[nodiscard]] auto createShader(const QString &name) const -> VkShaderModule
    {
        QFile file(name);
        if (!file.open(QIODevice::ReadOnly)) {
            qWarning("Failed to read shader %s", qPrintable(name));
            return VK_NULL_HANDLE;
        }
        QByteArray blob = file.readAll();
        file.close();

        VkShaderModuleCreateInfo shaderInfo;
        memset(&shaderInfo, 0, sizeof(shaderInfo));
        shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderInfo.codeSize = blob.size();
        shaderInfo.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
        VkShaderModule shaderModule;
        VkResult err = deviceFunctions->vkCreateShaderModule(window->device(),
                                                             &shaderInfo,
                                                             nullptr,
                                                             &shaderModule);
        if (err != VK_SUCCESS) {
            qWarning("Failed to create shader module: %d", err);
            return VK_NULL_HANDLE;
        }

        return shaderModule;
    }

    void createBuffer(VkDeviceSize size,
                      VkBufferUsageFlags usage,
                      VkMemoryPropertyFlags properties,
                      VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory) const
    {
        auto *device = window->device();

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (deviceFunctions->vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        deviceFunctions->vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = window->hostVisibleMemoryIndex();

        if (deviceFunctions->vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory)
            != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        deviceFunctions->vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    auto createTextureImage(const QSize &size,
                            VkImage *image,
                            VkDeviceMemory *mem,
                            VkImageTiling tiling,
                            VkImageUsageFlags usage,
                            uint32_t memIndex) const -> bool
    {
        VkDevice dev = window->device();

        VkImageCreateInfo imageInfo;
        memset(&imageInfo, 0, sizeof(imageInfo));
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = texture.texFormat;
        imageInfo.extent.width = size.width();
        imageInfo.extent.height = size.height();
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = tiling;
        imageInfo.usage = usage;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;

        VkResult err = deviceFunctions->vkCreateImage(dev, &imageInfo, nullptr, image);
        if (err != VK_SUCCESS) {
            qWarning("Failed to create linear image for texture: %d", err);
            return false;
        }

        VkMemoryRequirements memReq;
        deviceFunctions->vkGetImageMemoryRequirements(dev, *image, &memReq);

        if ((memReq.memoryTypeBits & (1 << memIndex)) == 0U) {
            VkPhysicalDeviceMemoryProperties physDevMemProps;
            window->vulkanInstance()
                ->functions()
                ->vkGetPhysicalDeviceMemoryProperties(window->physicalDevice(), &physDevMemProps);
            for (uint32_t i = 0; i < physDevMemProps.memoryTypeCount; ++i) {
                if ((memReq.memoryTypeBits & (1 << i)) == 0U) {
                    continue;
                }
                memIndex = i;
            }
        }

        VkMemoryAllocateInfo allocInfo = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                          nullptr,
                                          memReq.size,
                                          memIndex};
        qDebug("allocating %u bytes for texture image", static_cast<uint32_t>(memReq.size));

        err = deviceFunctions->vkAllocateMemory(dev, &allocInfo, nullptr, mem);
        if (err != VK_SUCCESS) {
            qWarning("Failed to allocate memory for linear image: %d", err);
            return false;
        }

        err = deviceFunctions->vkBindImageMemory(dev, *image, *mem, 0);
        if (err != VK_SUCCESS) {
            qWarning("Failed to bind linear image memory: %d", err);
            return false;
        }

        return true;
    }

    auto writeLinearImage(const QImage &img, VkImage image, VkDeviceMemory memory) const -> bool
    {
        VkDevice dev = window->device();

        VkImageSubresource subres = {VK_IMAGE_ASPECT_COLOR_BIT,
                                     0, // mip level
                                     0};
        VkSubresourceLayout layout;
        deviceFunctions->vkGetImageSubresourceLayout(dev, image, &subres, &layout);

        uchar *p;
        VkResult err = deviceFunctions->vkMapMemory(dev,
                                                    memory,
                                                    layout.offset,
                                                    layout.size,
                                                    0,
                                                    reinterpret_cast<void **>(&p));
        if (err != VK_SUCCESS) {
            qWarning("Failed to map memory for linear image: %d", err);
            return false;
        }

        for (int y = 0; y < img.height(); ++y) {
            const uchar *line = img.constScanLine(y);
            memcpy(p, line, static_cast<size_t>(img.width()) * 4);
            p += layout.rowPitch;
        }

        deviceFunctions->vkUnmapMemory(dev, memory);
        return true;
    }

    void ensureTexture()
    {
        if (!texture.texLayoutPending && !texture.texStagingPending) {
            return;
        }

        Q_ASSERT(texture.texLayoutPending != texture.texStagingPending);
        VkCommandBuffer cb = window->currentCommandBuffer();

        VkImageMemoryBarrier barrier;
        memset(&barrier, 0, sizeof(barrier));
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.levelCount = barrier.subresourceRange.layerCount = 1;

        if (texture.texLayoutPending) {
            texture.texLayoutPending = false;

            barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.image = texture.texImage;

            deviceFunctions->vkCmdPipelineBarrier(cb,
                                                  VK_PIPELINE_STAGE_HOST_BIT,
                                                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                  0,
                                                  0,
                                                  nullptr,
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &barrier);
        } else {
            texture.texStagingPending = false;

            barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.image = texture.texStaging;
            deviceFunctions->vkCmdPipelineBarrier(cb,
                                                  VK_PIPELINE_STAGE_HOST_BIT,
                                                  VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                  0,
                                                  0,
                                                  nullptr,
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &barrier);

            barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.image = texture.texImage;
            deviceFunctions->vkCmdPipelineBarrier(cb,
                                                  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                                  VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                  0,
                                                  0,
                                                  nullptr,
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &barrier);

            VkImageCopy copyInfo;
            memset(&copyInfo, 0, sizeof(copyInfo));
            copyInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyInfo.srcSubresource.layerCount = 1;
            copyInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyInfo.dstSubresource.layerCount = 1;
            copyInfo.extent.width = texture.texSize.width();
            copyInfo.extent.height = texture.texSize.height();
            copyInfo.extent.depth = 1;
            deviceFunctions->vkCmdCopyImage(cb,
                                            texture.texStaging,
                                            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                            texture.texImage,
                                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                            1,
                                            &copyInfo);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.image = texture.texImage;
            deviceFunctions->vkCmdPipelineBarrier(cb,
                                                  VK_PIPELINE_STAGE_TRANSFER_BIT,
                                                  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                                  0,
                                                  0,
                                                  nullptr,
                                                  0,
                                                  nullptr,
                                                  1,
                                                  &barrier);
        }
    }

    void updateUniformBuffer()
    {
        memcpy(uniformBuffersMapped[window->currentFrame()],
               transform.constData(),
               UNIFORM_DATA_SIZE);
    }

    void recordCommandBuffer()
    {
        VkCommandBuffer cb = window->currentCommandBuffer();
        const QSize sz = window->swapChainImageSize();

        VkClearColorValue clearColor = {{1, 1, 1, 1}};
        VkClearDepthStencilValue clearDS = {1, 0};
        VkClearValue clearValues[2];
        memset(clearValues, 0, sizeof(clearValues));
        clearValues[0].color = clearColor;
        clearValues[1].depthStencil = clearDS;

        VkRenderPassBeginInfo rpBeginInfo;
        memset(&rpBeginInfo, 0, sizeof(rpBeginInfo));
        rpBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpBeginInfo.renderPass = window->defaultRenderPass();
        rpBeginInfo.framebuffer = window->currentFramebuffer();
        rpBeginInfo.renderArea.extent.width = sz.width();
        rpBeginInfo.renderArea.extent.height = sz.height();
        rpBeginInfo.clearValueCount = 2;
        rpBeginInfo.pClearValues = clearValues;
        VkCommandBuffer cmdBuf = window->currentCommandBuffer();
        deviceFunctions->vkCmdBeginRenderPass(cmdBuf, &rpBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        deviceFunctions->vkCmdBindPipeline(cb, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        deviceFunctions->vkCmdBindDescriptorSets(cb,
                                                 VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                 pipelineLayout,
                                                 0,
                                                 1,
                                                 &descSet[window->currentFrame()],
                                                 0,
                                                 nullptr);
        VkDeviceSize vbOffset = 0;
        deviceFunctions->vkCmdBindVertexBuffers(cb, 0, 1, &vertexBuffer, &vbOffset);

        VkViewport viewport;
        viewport.x = viewport.y = 0;
        viewport.width = sz.width();
        viewport.height = sz.height();
        viewport.minDepth = 0;
        viewport.maxDepth = 1;
        deviceFunctions->vkCmdSetViewport(cb, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.offset.x = scissor.offset.y = 0;
        scissor.extent.width = viewport.width;
        scissor.extent.height = viewport.height;
        deviceFunctions->vkCmdSetScissor(cb, 0, 1, &scissor);

        deviceFunctions->vkCmdDraw(cb, 4, 1, 0, 0);

        deviceFunctions->vkCmdEndRenderPass(cmdBuf);
    }

    VulkanRenderer *q_ptr;

    QVulkanWindow *window;
    QVulkanDeviceFunctions *deviceFunctions;

    VkDescriptorPool descPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout descSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet descSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;

    VkSampler sampler = VK_NULL_HANDLE;

    Texture texture;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void *> uniformBuffersMapped;

    QMatrix4x4 transform;
    // float rotation = 0.0f;
    QSize windowSize;
};

VulkanRenderer::VulkanRenderer(QVulkanWindow *vulkanWindow)
    : d_ptr(new VulkanRendererPrivate(this))
{
    d_ptr->window = vulkanWindow;
}

VulkanRenderer::~VulkanRenderer() = default;

auto VulkanRenderer::createTexture(const QString &name, bool &img) -> bool
{
    QImage image(name);
    img = !image.isNull();
    if (!img) {
        qWarning("Failed to load image %s", qPrintable(name));
        // return false;
        image = d_ptr->texture.whiteImage;
    }

    // Convert to byte ordered RGBA8. Use premultiplied alpha, see
    // pColorBlendState in the pipeline.
    image = image.convertToFormat(QImage::Format_RGBA8888_Premultiplied);

    QVulkanFunctions *f = d_ptr->window->vulkanInstance()->functions();

    const bool srgb = QCoreApplication::arguments().contains(QStringLiteral("--srgb"));
    if (srgb) {
        qDebug("sRGB swapchain was requested, making texture sRGB too");
    }

    d_ptr->texture.texFormat = srgb ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;

    // Now we can either map and copy the image data directly, or have to go
    // through a staging buffer to copy and convert into the internal optimal
    // tiling format.
    VkFormatProperties props;
    f->vkGetPhysicalDeviceFormatProperties(d_ptr->window->physicalDevice(),
                                           d_ptr->texture.texFormat,
                                           &props);
    const bool canSampleLinear = (props.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
                                 != 0U;
    const bool canSampleOptimal = (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT)
                                  != 0U;
    if (!canSampleLinear && !canSampleOptimal) {
        qWarning("Neither linear nor optimal image sampling is supported for RGBA8");
        return false;
    }

    static bool alwaysStage = qEnvironmentVariableIntValue("QT_VK_FORCE_STAGE_TEX") != 0;

    if (canSampleLinear && !alwaysStage) {
        if (!d_ptr->createTextureImage(image.size(),
                                       &d_ptr->texture.texImage,
                                       &d_ptr->texture.texMem,
                                       VK_IMAGE_TILING_LINEAR,
                                       VK_IMAGE_USAGE_SAMPLED_BIT,
                                       d_ptr->window->hostVisibleMemoryIndex())) {
            return false;
        }

        if (!d_ptr->writeLinearImage(image, d_ptr->texture.texImage, d_ptr->texture.texMem)) {
            return false;
        }

        d_ptr->texture.texLayoutPending = true;
    } else {
        if (!d_ptr->createTextureImage(image.size(),
                                       &d_ptr->texture.texStaging,
                                       &d_ptr->texture.texStagingMem,
                                       VK_IMAGE_TILING_LINEAR,
                                       VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                                       d_ptr->window->hostVisibleMemoryIndex())) {
            return false;
        }

        if (!d_ptr->createTextureImage(image.size(),
                                       &d_ptr->texture.texImage,
                                       &d_ptr->texture.texMem,
                                       VK_IMAGE_TILING_OPTIMAL,
                                       VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                       d_ptr->window->deviceLocalMemoryIndex())) {
            return false;
        }

        if (!d_ptr->writeLinearImage(image,
                                     d_ptr->texture.texStaging,
                                     d_ptr->texture.texStagingMem)) {
            return false;
        }

        d_ptr->texture.texStagingPending = true;
    }

    d_ptr->texture.texSize = image.size();

    return true;
}

void VulkanRenderer::createDescriptorSetLayout()
{
    VkDevice dev = d_ptr->window->device();

    VkDescriptorSetLayoutBinding layoutBinding[2] = {{0, // binding
                                                      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                      1, // descriptorCount
                                                      VK_SHADER_STAGE_VERTEX_BIT,
                                                      nullptr},
                                                     {1, // binding
                                                      VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                      1, // descriptorCount
                                                      VK_SHADER_STAGE_FRAGMENT_BIT,
                                                      nullptr}};
    VkDescriptorSetLayoutCreateInfo descLayoutInfo
        = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
           nullptr,
           0,
           2, // bindingCount
           layoutBinding};
    auto err = d_ptr->deviceFunctions->vkCreateDescriptorSetLayout(dev,
                                                                   &descLayoutInfo,
                                                                   nullptr,
                                                                   &d_ptr->descSetLayout);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create descriptor set layout: %d", err);
    }
}

void VulkanRenderer::createGraphicsPipeline()
{
    VkDevice dev = d_ptr->window->device();
    // Pipeline cache
    VkPipelineCacheCreateInfo pipelineCacheInfo;
    memset(&pipelineCacheInfo, 0, sizeof(pipelineCacheInfo));
    pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    auto err = d_ptr->deviceFunctions->vkCreatePipelineCache(dev,
                                                             &pipelineCacheInfo,
                                                             nullptr,
                                                             &d_ptr->pipelineCache);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create pipeline cache: %d", err);
    }

    // Pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;
    memset(&pipelineLayoutInfo, 0, sizeof(pipelineLayoutInfo));
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &d_ptr->descSetLayout;
    err = d_ptr->deviceFunctions->vkCreatePipelineLayout(dev,
                                                         &pipelineLayoutInfo,
                                                         nullptr,
                                                         &d_ptr->pipelineLayout);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create pipeline layout: %d", err);
    }

    // Shaders
    VkShaderModule vertShaderModule = d_ptr->createShader(
        QString("%1/vulkan_shader/vulkan.vert.spv").arg(qApp->applicationDirPath()));
    VkShaderModule fragShaderModule = d_ptr->createShader(
        QString("%1/vulkan_shader/vulkan.frag.spv").arg(qApp->applicationDirPath()));

    VkVertexInputBindingDescription vertexBindingDesc = {0, // binding
                                                         5 * sizeof(float),
                                                         VK_VERTEX_INPUT_RATE_VERTEX};
    VkVertexInputAttributeDescription vertexAttrDesc[] = {{   // position
                                                           0, // location
                                                           0, // binding
                                                           VK_FORMAT_R32G32B32_SFLOAT,
                                                           0},
                                                          {// texcoord
                                                           1,
                                                           0,
                                                           VK_FORMAT_R32G32_SFLOAT,
                                                           3 * sizeof(float)}};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.flags = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexBindingDesc;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    vertexInputInfo.pVertexAttributeDescriptions = vertexAttrDesc;

    // Graphics pipeline
    VkPipelineShaderStageCreateInfo shaderStages[2]
        = {{VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_VERTEX_BIT,
            vertShaderModule,
            "main",
            nullptr},
           {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            nullptr,
            0,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            fragShaderModule,
            "main",
            nullptr}};

    VkPipelineInputAssemblyStateCreateInfo ia;
    memset(&ia, 0, sizeof(ia));
    ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

    // The viewport and scissor will be set dynamically via
    // vkCmdSetViewport/Scissor. This way the pipeline does not need to be touched
    // when resizing the window.
    VkPipelineViewportStateCreateInfo vp;
    memset(&vp, 0, sizeof(vp));
    vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    vp.viewportCount = 1;
    vp.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rs;
    memset(&rs, 0, sizeof(rs));
    rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rs.polygonMode = VK_POLYGON_MODE_FILL;
    rs.cullMode = VK_CULL_MODE_BACK_BIT;
    rs.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rs.lineWidth = 1.0F;

    VkPipelineMultisampleStateCreateInfo ms;
    memset(&ms, 0, sizeof(ms));
    ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo ds;
    memset(&ds, 0, sizeof(ds));
    ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkPipelineColorBlendStateCreateInfo cb;
    memset(&cb, 0, sizeof(cb));
    cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // assume pre-multiplied alpha, blend, write out all of rgba
    VkPipelineColorBlendAttachmentState att;
    memset(&att, 0, sizeof(att));
    att.colorWriteMask = 0xF;
    att.blendEnable = VK_TRUE;
    att.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    att.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att.colorBlendOp = VK_BLEND_OP_ADD;
    att.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    att.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    att.alphaBlendOp = VK_BLEND_OP_ADD;
    cb.attachmentCount = 1;
    cb.pAttachments = &att;

    VkDynamicState dynEnable[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dyn;
    memset(&dyn, 0, sizeof(dyn));
    dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dyn.dynamicStateCount = sizeof(dynEnable) / sizeof(VkDynamicState);
    dyn.pDynamicStates = dynEnable;

    VkGraphicsPipelineCreateInfo pipelineInfo;
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pInputAssemblyState = &ia;
    pipelineInfo.pViewportState = &vp;
    pipelineInfo.pRasterizationState = &rs;
    pipelineInfo.pMultisampleState = &ms;
    pipelineInfo.pDepthStencilState = &ds;
    pipelineInfo.pColorBlendState = &cb;
    pipelineInfo.pDynamicState = &dyn;
    pipelineInfo.layout = d_ptr->pipelineLayout;
    pipelineInfo.renderPass = d_ptr->window->defaultRenderPass();

    err = d_ptr->deviceFunctions->vkCreateGraphicsPipelines(dev,
                                                            d_ptr->pipelineCache,
                                                            1,
                                                            &pipelineInfo,
                                                            nullptr,
                                                            &d_ptr->pipeline);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create graphics pipeline: %d", err);
    }

    if (vertShaderModule != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyShaderModule(dev, vertShaderModule, nullptr);
    }
    if (fragShaderModule != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyShaderModule(dev, fragShaderModule, nullptr);
    }
}

void VulkanRenderer::createTextureSampler()
{
    VkDevice dev = d_ptr->window->device();
    // Sampler.
    VkSamplerCreateInfo samplerInfo;
    memset(&samplerInfo, 0, sizeof(samplerInfo));
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.maxAnisotropy = 1.0F;
    auto err = d_ptr->deviceFunctions->vkCreateSampler(dev, &samplerInfo, nullptr, &d_ptr->sampler);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create sampler: %d", err);
    }
}

void VulkanRenderer::createVertexBuffer()
{
    VkDevice dev = d_ptr->window->device();

    const VkPhysicalDeviceLimits *pdevLimits = &d_ptr->window->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevLimits->minUniformBufferOffsetAlignment;
    qDebug("uniform buffer offset alignment is %u", static_cast<uint>(uniAlign));
    const VkDeviceSize vertexAllocSize = aligned(sizeof(vertexData), uniAlign);

    d_ptr->createBuffer(vertexAllocSize,
                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        {},
                        d_ptr->vertexBuffer,
                        d_ptr->vertexBufferMemory);

    quint8 *p;
    auto err = d_ptr->deviceFunctions->vkMapMemory(dev,
                                                   d_ptr->vertexBufferMemory,
                                                   0,
                                                   vertexAllocSize,
                                                   0,
                                                   reinterpret_cast<void **>(&p));
    if (err != VK_SUCCESS) {
        qFatal("Failed to map memory: %d", err);
    }
    memcpy(p, vertexData, sizeof(vertexData));

    d_ptr->deviceFunctions->vkUnmapMemory(dev, d_ptr->vertexBufferMemory);
}

void VulkanRenderer::createUniformBuffers()
{
    VkDevice dev = d_ptr->window->device();

    const int concurrentFrameCount = d_ptr->window->concurrentFrameCount();
    const VkPhysicalDeviceLimits *pdevLimits = &d_ptr->window->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevLimits->minUniformBufferOffsetAlignment;
    qDebug("uniform buffer offset alignment is %u", static_cast<uint>(uniAlign));
    const VkDeviceSize uniformAllocSize = aligned(UNIFORM_DATA_SIZE, uniAlign);

    d_ptr->uniformBuffers.resize(concurrentFrameCount);
    d_ptr->uniformBuffersMemory.resize(concurrentFrameCount);
    d_ptr->uniformBuffersMapped.resize(concurrentFrameCount);
    for (int i = 0; i < concurrentFrameCount; i++) {
        d_ptr->createBuffer(uniformAllocSize,
                            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                            {},
                            d_ptr->uniformBuffers[i],
                            d_ptr->uniformBuffersMemory[i]);

        d_ptr->deviceFunctions->vkMapMemory(dev,
                                            d_ptr->uniformBuffersMemory[i],
                                            0,
                                            uniformAllocSize,
                                            0,
                                            &d_ptr->uniformBuffersMapped[i]);
    }
}

void VulkanRenderer::createDescriptorPool()
{
    VkDevice dev = d_ptr->window->device();
    const int concurrentFrameCount = d_ptr->window->concurrentFrameCount();
    // Set up descriptor set and its layout.
    VkDescriptorPoolSize descPoolSizes[2] = {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                              static_cast<uint32_t>(concurrentFrameCount)},
                                             {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                              static_cast<uint32_t>(concurrentFrameCount)}};
    VkDescriptorPoolCreateInfo descPoolInfo;
    memset(&descPoolInfo, 0, sizeof(descPoolInfo));
    descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descPoolInfo.maxSets = concurrentFrameCount;
    descPoolInfo.poolSizeCount = 2;
    descPoolInfo.pPoolSizes = descPoolSizes;
    auto err = d_ptr->deviceFunctions->vkCreateDescriptorPool(dev,
                                                              &descPoolInfo,
                                                              nullptr,
                                                              &d_ptr->descPool);
    if (err != VK_SUCCESS) {
        qFatal("Failed to create descriptor pool: %d", err);
    }
}

void VulkanRenderer::createDescriptorSets()
{
    VkDevice dev = d_ptr->window->device();
    const int concurrentFrameCount = d_ptr->window->concurrentFrameCount();
    for (int i = 0; i < concurrentFrameCount; ++i) {
        VkDescriptorSetAllocateInfo descSetAllocInfo
            = {VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
               nullptr,
               d_ptr->descPool,
               1,
               &d_ptr->descSetLayout};
        auto err = d_ptr->deviceFunctions->vkAllocateDescriptorSets(dev,
                                                                    &descSetAllocInfo,
                                                                    &d_ptr->descSet[i]);
        if (err != VK_SUCCESS) {
            qFatal("Failed to allocate descriptor set: %d", err);
        }
    }
}

void VulkanRenderer::updateDescriptorSets()
{
    VkDevice dev = d_ptr->window->device();
    const int concurrentFrameCount = d_ptr->window->concurrentFrameCount();

    for (int i = 0; i < concurrentFrameCount; ++i) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = d_ptr->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = UNIFORM_DATA_SIZE;

        VkWriteDescriptorSet descWrite[2];
        memset(descWrite, 0, sizeof(descWrite));
        descWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite[0].dstSet = d_ptr->descSet[i];
        descWrite[0].dstBinding = 0;
        descWrite[0].descriptorCount = 1;
        descWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descWrite[0].pBufferInfo = &bufferInfo;

        VkDescriptorImageInfo descImageInfo = {d_ptr->sampler,
                                               d_ptr->texture.texView,
                                               VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL};

        descWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descWrite[1].dstSet = d_ptr->descSet[i];
        descWrite[1].dstBinding = 1;
        descWrite[1].descriptorCount = 1;
        descWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descWrite[1].pImageInfo = &descImageInfo;

        d_ptr->deviceFunctions->vkUpdateDescriptorSets(dev, 2, descWrite, 0, nullptr);
    }
}

void VulkanRenderer::createTextureImageView()
{
    VkDevice dev = d_ptr->window->device();

    VkImageViewCreateInfo viewInfo;
    memset(&viewInfo, 0, sizeof(viewInfo));
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = d_ptr->texture.texImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = d_ptr->texture.texFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.levelCount = viewInfo.subresourceRange.layerCount = 1;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.baseArrayLayer = 0;

    VkResult err = d_ptr->deviceFunctions->vkCreateImageView(dev,
                                                             &viewInfo,
                                                             nullptr,
                                                             &d_ptr->texture.texView);
    if (err != VK_SUCCESS) {
        qWarning("Failed to create image view for texture: %d", err);
    }
}

void VulkanRenderer::initResources()
{
    qDebug("initResources");

    VkDevice dev = d_ptr->window->device();
    d_ptr->deviceFunctions = d_ptr->window->vulkanInstance()->deviceFunctions(dev);

    createDescriptorSetLayout();
    createGraphicsPipeline();
    createTextureSampler();

    createVertexBuffer();
    createUniformBuffers();

    // Texture.
    bool img;
    if (!createTexture("", img)) {
        qFatal("Failed to create texture");
    }

    createTextureImageView();

    createDescriptorPool();
    createDescriptorSets();
    updateDescriptorSets();
}

void VulkanRenderer::initSwapChainResources()
{
    qDebug("initSwapChainResources");
    const auto swapChainImageSize = d_ptr->window->swapChainImageSize();
    if (d_ptr->windowSize.isValid()) {
        auto factor_w = static_cast<qreal>(d_ptr->windowSize.width()) / swapChainImageSize.width();
        auto factor_h = static_cast<qreal>(d_ptr->windowSize.height())
                        / swapChainImageSize.height();
        d_ptr->transform.scale(factor_w, factor_h, 1.0);
    }
    d_ptr->windowSize = swapChainImageSize;
}

void VulkanRenderer::releaseSwapChainResources()
{
    qDebug("releaseSwapChainResources");
}

void VulkanRenderer::releaseResources()
{
    qDebug("releaseResources");

    VkDevice dev = d_ptr->window->device();

    if (d_ptr->sampler != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroySampler(dev, d_ptr->sampler, nullptr);
        d_ptr->sampler = VK_NULL_HANDLE;
    }

    d_ptr->texture.cleanup(dev, d_ptr->deviceFunctions);

    if (d_ptr->pipeline != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyPipeline(dev, d_ptr->pipeline, nullptr);
        d_ptr->pipeline = VK_NULL_HANDLE;
    }

    if (d_ptr->pipelineLayout != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyPipelineLayout(dev, d_ptr->pipelineLayout, nullptr);
        d_ptr->pipelineLayout = VK_NULL_HANDLE;
    }

    if (d_ptr->pipelineCache != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyPipelineCache(dev, d_ptr->pipelineCache, nullptr);
        d_ptr->pipelineCache = VK_NULL_HANDLE;
    }

    if (d_ptr->descSetLayout != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyDescriptorSetLayout(dev, d_ptr->descSetLayout, nullptr);
        d_ptr->descSetLayout = VK_NULL_HANDLE;
    }

    if (d_ptr->descPool != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyDescriptorPool(dev, d_ptr->descPool, nullptr);
        d_ptr->descPool = VK_NULL_HANDLE;
    }

    if (d_ptr->vertexBuffer != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkDestroyBuffer(dev, d_ptr->vertexBuffer, nullptr);
        d_ptr->vertexBuffer = VK_NULL_HANDLE;
    }

    if (d_ptr->vertexBufferMemory != VK_NULL_HANDLE) {
        d_ptr->deviceFunctions->vkFreeMemory(dev, d_ptr->vertexBufferMemory, nullptr);
        d_ptr->vertexBufferMemory = VK_NULL_HANDLE;
    }

    const int concurrentFrameCount = d_ptr->window->concurrentFrameCount();
    for (size_t i = 0; i < concurrentFrameCount; i++) {
        d_ptr->deviceFunctions->vkDestroyBuffer(dev, d_ptr->uniformBuffers[i], nullptr);
        d_ptr->deviceFunctions->vkFreeMemory(dev, d_ptr->uniformBuffersMemory[i], nullptr);
    }
}

void VulkanRenderer::startNextFrame()
{
    // Add the necessary barriers and do the host-linear -> device-optimal copy,
    // if not yet done.
    d_ptr->ensureTexture();

    d_ptr->updateUniformBuffer();

    d_ptr->recordCommandBuffer();

    d_ptr->window->frameReady();
    d_ptr->window->requestUpdate(); // render continuously, throttled by the
                                    // presentation rate
}

auto VulkanRenderer::setImageUrl(const QString &imageUrl) -> QSize
{
    VkDevice dev = d_ptr->window->device();
    d_ptr->deviceFunctions->vkDeviceWaitIdle(dev);

    d_ptr->texture.cleanup(dev, d_ptr->deviceFunctions);

    bool img;
    createTexture(imageUrl, img);
    createTextureImageView();

    updateDescriptorSets();

    const auto swapChainImageSize = d_ptr->window->swapChainImageSize();
    if (d_ptr->texture.texSize.width() > swapChainImageSize.width()
        || d_ptr->texture.texSize.height() > swapChainImageSize.height()) {
        fitToScreen();
    } else {
        resetToOriginalSize();
    }

    return img ? d_ptr->texture.texSize : QSize{-1, -1};
}

void VulkanRenderer::resetToOriginalSize()
{
    const auto swapChainImageSize = d_ptr->window->swapChainImageSize();
    auto factor_w = static_cast<qreal>(d_ptr->texture.texSize.width()) / swapChainImageSize.width();
    auto factor_h = static_cast<qreal>(d_ptr->texture.texSize.height())
                    / swapChainImageSize.height();
    d_ptr->transform = d_ptr->window->clipCorrectionMatrix();
    d_ptr->transform.scale(factor_w, factor_h, 1.0);
}

void VulkanRenderer::fitToScreen()
{
    const auto swapChainImageSize = d_ptr->window->swapChainImageSize();
    auto factor_w = static_cast<qreal>(swapChainImageSize.width()) / d_ptr->texture.texSize.width();
    auto factor_h = static_cast<qreal>(swapChainImageSize.height())
                    / d_ptr->texture.texSize.height();
    auto factor = qMin(factor_w, factor_h);
    d_ptr->transform = d_ptr->window->clipCorrectionMatrix();
    d_ptr->transform.scale(factor / factor_w, factor / factor_h, 1.0);
}

void VulkanRenderer::scale(qreal factorWidth, qreal factorHeight)
{
    d_ptr->transform.scale(factorWidth, factorHeight, 1.0);
}

void VulkanRenderer::rotateNinetieth()
{
    d_ptr->transform.rotate(90, 0, 0, 1);
}

void VulkanRenderer::anti_rotateNinetieth()
{
    d_ptr->transform.rotate(-90, 0, 0, 1);
}

auto VulkanRenderer::scaleFactor() const -> qreal
{
    auto factor = d_ptr->transform.toTransform().m11() * d_ptr->windowSize.width()
                  / d_ptr->texture.texSize.width();
    return factor;
}

} // namespace GpuGraphics
