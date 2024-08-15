#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include "gpugraphics_global.hpp"

#include <QVulkanWindowRenderer>

namespace GpuGraphics {

class GPUAPHICS VulkanRenderer : public QVulkanWindowRenderer
{
public:
    explicit VulkanRenderer(QVulkanWindow *vulkanWindow);
    ~VulkanRenderer() override;

    void initResources() override;
    void initSwapChainResources() override;
    void releaseSwapChainResources() override;
    void releaseResources() override;

    void startNextFrame() override;

    auto setImageUrl(const QString &imageUrl) -> QSize;

    void resetToOriginalSize();
    void fitToScreen();
    void scale(qreal factorWidth, qreal factorHeight);

    void rotateNinetieth();
    void anti_rotateNinetieth();

    [[nodiscard]] auto scaleFactor() const -> qreal;

private:
    void createDescriptorSetLayout();
    void createGraphicsPipeline();
    void createTextureSampler();
    void createVertexBuffer();
    void createIndexBuffer();
    void createUniformBuffers();
    auto createTexture(const QString &name, bool &img) -> bool;
    void createTextureImageView();
    void createDescriptorPool();
    void createDescriptorSets();
    void updateDescriptorSets();

    class VulkanRendererPrivate;
    QScopedPointer<VulkanRendererPrivate> d_ptr;
};

} // namespace GpuGraphics

#endif // VULKANRENDERER_HPP
