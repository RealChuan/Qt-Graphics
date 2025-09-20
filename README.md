# Qt Graphics

- [简体中文](README.md)
- [English](README.en.md)

> **注意**：效果图更新可能不够及时。

## 支持更多图片格式

如需支持更多图像格式，请参考 [kimageformats-binaries](https://github.com/RealChuan/kimageformats-binaries/tree/dev)：

1. 从 `Actions` 中下载 `Artifacts` 文件夹，将 `kimg_*` 库文件解压到 Qt 打包目录下的 `imageformats` 文件夹中；
2. 其他依赖库文件需放置在主程序同级目录或主程序可加载的位置。

---

## 使用 GPU 渲染 2D 纹理的优缺点（QRhiWidget、QVulkanWindow、QOpenGLWidget）

### 优点

- **性能优异**：相比基于 QGraphicsView 的图片查看界面，GPU 渲染更加流畅，CPU 占用极低。

### 缺点

- **旋转显示问题**：在任意角度旋转时，纹理宽高比可能发生变化，导致显示异常。
  - [openglview](src/gpugraphics/openglview.cc) 中提供了一种临时解决方案，可在旋转时维持原图宽高比，但需舍弃原有缩放比例，调整为适应窗口或原图大小。
  - 具体实现请参考 `rotatedTextureSize`、`rotateNinetieth` 和 `anti_rotateNinetieth` 函数。

---

## QVulkanWindow 编译说明

### 已知编译问题

1. **CMake（MacOS）**：无法找到 QVulkanWindow 相关头文件，编译失败。
2. **qmake**：
   - **MacOS**：缺少 QVulkanWindowRenderer 头文件，编译失败。
   - **Ubuntu**：缺少 QVulkanInstance 头文件，编译失败。
3. 目前已暂时禁用上述环境的编译。

---

## 多图像文件查看器

- 支持从单个文件（如 `ico`、`gif` 等）中读取多个图像并显示。
- 其他格式需依赖 `kimageformats-binaries` 插件支持。

---

## ICO 文件制作

### 已知问题

- **[FreeImage](https://github.com/danoli3/FreeImage)** 在生成含 `256x256` 图像的 ICO 文件时存在缺陷：它会错误地将宽高字段直接设为 `256`，而非格式规范要求的 `0` 或 `255`，导致 Windows 资源管理器无法正确识别该分辨率。
- **解决方案**：
  - 使用基于 [QtIcoHandler](https://github.com/qt/qtbase/blob/dev/src/plugins/imageformats/ico/qicohandler.h) 修改的 [icowriter](/src/utils/icowriter.hpp)。
- **推荐分辨率**：`256x256`、`128x128`、`64x64`、`48x48`、`32x32`、`16x16`。

---

## 功能界面示例

### 1. 图片查看界面

<div align=center>
<img src="docs/ImageView.png" width="90%" height="90%">
</div>

---

### 2. 马赛克绘制（橡皮擦效果）

<div align=center>
<img src="docs/MaskEdit.png" width="90%" height="90%">
</div>

---

### 3. 圆角/圆形图标编辑
>
> **注意**：请保存为 PNG 格式，否则圆角区域可能显示为黑色。
<div align=center>
<img src="docs/RoundEdit.jpg" width="90%" height="90%">
</div>

---

### 4. 简单图形绘制

<div align=center>
<img src="docs/DrawScene.png" width="90%" height="90%">
</div>

---

### 5. 电影字幕拼接

- 左侧为快速缩放预览（可能模糊），右侧为原图展示。
- 实际生成时基于原图裁剪，保存后可验证清晰度。

<div align=center>
<img src="docs/FilmSubTiltleSplicing.png" width="90%" height="90%">
</div>

---

### 6. GIF 录制（egif/gif-h 库）与截图功能

- 下图使用 GIF 录制功能演示截图过程：
- 截图后可使用（4）中的图形绘制功能进行编辑。

<div align=center>
<img src="docs/Record_Screenshot.gif" width="90%" height="90%">
</div>
