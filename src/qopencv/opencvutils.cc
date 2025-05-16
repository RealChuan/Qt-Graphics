#include "opencvutils.hpp"

#include <QDebug>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

auto qImageToMat(const QImage &qimage) -> cv::Mat
{
    cv::Mat mat;

    switch (qimage.format()) {
    case QImage::Format_Grayscale8:
        mat = cv::Mat(qimage.height(),
                      qimage.width(),
                      CV_8UC1,
                      (void *) qimage.constBits(),
                      qimage.bytesPerLine())
                  .clone();
        break;
    case QImage::Format_Grayscale16:
        mat = cv::Mat(qimage.height(),
                      qimage.width(),
                      CV_16UC1,
                      (void *) qimage.constBits(),
                      qimage.bytesPerLine())
                  .clone();
        break;
    case QImage::Format_Indexed8: {
        auto qimage_rgb = qimage.convertToFormat(QImage::Format_RGB888);
        mat = cv::Mat(qimage_rgb.height(),
                      qimage_rgb.width(),
                      CV_8UC3,
                      (void *) qimage_rgb.constBits(),
                      qimage_rgb.bytesPerLine())
                  .clone();
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    }
    case QImage::Format_RGB32:
    case QImage::Format_RGB888:
        mat = cv::Mat(qimage.height(),
                      qimage.width(),
                      CV_8UC3,
                      (void *) qimage.constBits(),
                      qimage.bytesPerLine())
                  .clone();
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_RGBA8888:
    case QImage::Format_RGBA8888_Premultiplied:
    case QImage::Format_RGBX8888: {
        auto qimage_rgb = qimage.convertToFormat(QImage::Format_RGB32);
        mat = cv::Mat(qimage_rgb.height(),
                      qimage_rgb.width(),
                      CV_8UC3,
                      (void *) qimage_rgb.constBits(),
                      qimage_rgb.bytesPerLine())
                  .clone();
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    }
    case QImage::Format_RGB16:
    case QImage::Format_RGB444:
    case QImage::Format_ARGB4444_Premultiplied:
    case QImage::Format_RGB555:
    case QImage::Format_ARGB8555_Premultiplied:
    case QImage::Format_RGB666:
    case QImage::Format_ARGB6666_Premultiplied:
    case QImage::Format_RGBX64:
    case QImage::Format_RGBA64:
    case QImage::Format_RGBA64_Premultiplied: {
        auto qimage_rgb = qimage.convertToFormat(QImage::Format_RGB32);
        mat = cv::Mat(qimage_rgb.height(),
                      qimage_rgb.width(),
                      CV_8UC3,
                      (void *) qimage_rgb.constBits(),
                      qimage_rgb.bytesPerLine())
                  .clone();
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    }
    case QImage::Format_RGB30:
    case QImage::Format_A2BGR30_Premultiplied:
    case QImage::Format_A2RGB30_Premultiplied: {
        auto qimage_rgb = qimage.convertToFormat(QImage::Format_RGB32);
        mat = cv::Mat(qimage_rgb.height(),
                      qimage_rgb.width(),
                      CV_8UC3,
                      (void *) qimage_rgb.constBits(),
                      qimage_rgb.bytesPerLine())
                  .clone();
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    }
    case QImage::Format_RGBX16FPx4:
    case QImage::Format_RGBA16FPx4:
    case QImage::Format_RGBA16FPx4_Premultiplied:
    case QImage::Format_RGBX32FPx4:
    case QImage::Format_RGBA32FPx4:
    case QImage::Format_RGBA32FPx4_Premultiplied: {
        auto qimage_rgb = qimage.convertToFormat(QImage::Format_RGB32);
        mat = cv::Mat(qimage_rgb.height(),
                      qimage_rgb.width(),
                      CV_8UC3,
                      (void *) qimage_rgb.constBits(),
                      qimage_rgb.bytesPerLine())
                  .clone();
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    }
    default: qWarning() << "Unsupported QImage format:" << qimage.format(); break;
    }

    return mat;
}

auto matToQImage(const cv::Mat &mat) -> QImage
{
    QImage qimage;

    switch (mat.type()) {
    case CV_8UC1: {
        qimage = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
        break;
    }
    case CV_8UC3: {
        cv::Mat mat_rgb;
        cv::cvtColor(mat, mat_rgb, cv::COLOR_BGR2RGB);
        qimage = QImage(mat_rgb.data, mat_rgb.cols, mat_rgb.rows, mat_rgb.step, QImage::Format_RGB888)
                     .copy();
        break;
    }
    case CV_8UC4: {
        cv::Mat mat_rgba;
        cv::cvtColor(mat, mat_rgba, cv::COLOR_BGRA2RGBA);
        qimage = QImage(mat_rgba.data,
                        mat_rgba.cols,
                        mat_rgba.rows,
                        mat_rgba.step,
                        QImage::Format_RGBA8888)
                     .copy();
        break;
    }
    case CV_16UC1: {
        qimage = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale16).copy();
        break;
    }
    case CV_32FC1: {
        cv::Mat mat_8u;
        cv::normalize(mat, mat_8u, 0, 255, cv::NORM_MINMAX, CV_8U);
        qimage = QImage(mat_8u.data, mat_8u.cols, mat_8u.rows, mat_8u.step, QImage::Format_Grayscale8)
                     .copy();
        break;
    }
    case CV_32FC3: {
        cv::Mat mat_8u;
        cv::normalize(mat, mat_8u, 0, 255, cv::NORM_MINMAX, CV_8U);
        cv::Mat mat_rgb;
        cv::cvtColor(mat_8u, mat_rgb, cv::COLOR_BGR2RGB);
        qimage = QImage(mat_rgb.data, mat_rgb.cols, mat_rgb.rows, mat_rgb.step, QImage::Format_RGB888)
                     .copy();
        break;
    }
    default: qWarning() << "Unsupported cv::Mat type:" << mat.type(); break;
    }

    return qimage;
}

} // namespace OpenCVUtils
