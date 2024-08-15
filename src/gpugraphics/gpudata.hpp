#pragma once

namespace GpuGraphics {

static float vertices[] = {
    // positions             // texture coords
    1.0F,  1.0F,  0.0F, 1.0F, 1.0F, // top right
    1.0F,  -1.0F, 0.0F, 1.0F, 0.0F, // bottom right
    -1.0F, -1.0F, 0.0F, 0.0F, 0.0F, // bottom left
    -1.0F, 1.0F,  0.0F, 0.0F, 1.0F  // top left
};
static unsigned int indices[] = {0, 1, 3, 1, 2, 3};

} // namespace GpuGraphics
