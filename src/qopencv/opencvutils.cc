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
    case QImage::Format_Indexed8:
        mat = qImageToMat(qimage.convertToFormat(QImage::Format_RGB888));
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(qimage.height(),
                      qimage.width(),
                      CV_8UC3,
                      (void *) qimage.constBits(),
                      qimage.bytesPerLine())
                  .clone();
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_RGBA8888:
    case QImage::Format_RGBA8888_Premultiplied:
    case QImage::Format_RGBX64:
    case QImage::Format_RGBA64: {
        auto rgb888 = qimage.convertToFormat(QImage::Format_RGB888);
        mat = cv::Mat(rgb888.height(),
                      rgb888.width(),
                      CV_8UC3,
                      (void *) rgb888.constBits(),
                      rgb888.bytesPerLine())
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
    case CV_8UC1:
        qimage = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
        break;
    case CV_8UC3: {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        qimage = QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
        break;
    }
    case CV_8UC4: {
        cv::Mat rgba;
        cv::cvtColor(mat, rgba, cv::COLOR_BGRA2RGBA);
        qimage = QImage(rgba.data, rgba.cols, rgba.rows, rgba.step, QImage::Format_RGBA8888).copy();
        break;
    }
    case CV_16UC1:
        qimage = QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale16).copy();
        break;
    case CV_32FC1:
    case CV_32FC3:
    case CV_32FC4: {
        cv::Mat normalized;
        cv::normalize(mat, normalized, 0, 255, cv::NORM_MINMAX, CV_8U);
        qimage = matToQImage(normalized);
        break;
    }
    default: qWarning() << "Unsupported cv::Mat type:" << mat.type(); break;
    }

    return qimage;
}

} // namespace OpenCVUtils
