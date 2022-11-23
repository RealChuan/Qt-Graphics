# GifEncoder

C++ gif encoder with good quality!

Use Neural-Net quantization algorithm by Anthony Dekker for generating colormap.

Use [giflib](https://sourceforge.net/projects/giflib/) for encoding gif.

Use stb_image for image loading in demo code.

[![linux](https://github.com/xiaozhuai/GifEncoder/actions/workflows/linux.yml/badge.svg)](https://github.com/xiaozhuai/GifEncoder/actions/workflows/linux.yml)
[![macos](https://github.com/xiaozhuai/GifEncoder/actions/workflows/macos.yml/badge.svg)](https://github.com/xiaozhuai/GifEncoder/actions/workflows/macos.yml)
[![windows-x64](https://github.com/xiaozhuai/GifEncoder/actions/workflows/windows-x64.yml/badge.svg)](https://github.com/xiaozhuai/GifEncoder/actions/workflows/windows-x64.yml)
[![windows-x86](https://github.com/xiaozhuai/GifEncoder/actions/workflows/windows-x86.yml/badge.svg)](https://github.com/xiaozhuai/GifEncoder/actions/workflows/windows-x86.yml)

# Usage

Just copy gif directory to your project, and include `GifEncoder.h`, that's all.

```cpp
#include "GifEncoder.h"

int main() {
    // Suppose that you have three frame to be encoded
    const int w = 40;
    const int h = 40;
    uint32_t frame0[w * h];
    uint32_t frame1[w * h];
    uint32_t frame2[w * h];
    for (int i = 0; i < w * h; ++i) {
        frame0[i] = 0xFF0000FF; // red
        frame1[i] = 0xFF00FF00; // green
        frame2[i] = 0xFFFF0000; // blue
    }

    int quality = 10;
    bool useGlobalColorMap = true;
    int loop = 0;
    int preAllocSize = useGlobalColorMap ? w * h * 3 * 3 : w * h * 3;
    int delay = 20;

    GifEncoder gifEncoder;

    if (!gifEncoder.open("test.gif", w, h, quality, useGlobalColorMap, loop, preAllocSize)) {
        fprintf(stderr, "Error open gif file\n");
        return 1;
    }

    gifEncoder.push(GifEncoder::PIXEL_FORMAT_RGBA, (uint8_t *) frame0, w, h, delay);
    gifEncoder.push(GifEncoder::PIXEL_FORMAT_RGBA, (uint8_t *) frame1, w, h, delay);
    gifEncoder.push(GifEncoder::PIXEL_FORMAT_RGBA, (uint8_t *) frame2, w, h, delay);

    if (!gifEncoder.close()) {
        fprintf(stderr, "Error close gif file\n");
        return 1;
    }
}
```

More complex demo code is in `demo.cpp`.

# Build

Use cmake to build demo.

## Linux or MacOS

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
./egif_demo
```

## Windows

Open Visual Studio Command Prompt and run these command

```bat
md build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -G "NMake Makefiles" ..
nmake
egif_demo.exe
```

## Android & iOS

Build it by yourself
