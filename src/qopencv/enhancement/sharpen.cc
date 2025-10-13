#include "sharpen.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

std::vector<cv::Mat> robertsKernels()
{
    std::vector<cv::Mat> kernels;
    cv::Mat kernelGx = (cv::Mat_<double>(2, 2) << 1, 0, 0, -1);
    cv::Mat kernelGy = (cv::Mat_<double>(2, 2) << 0, 1, -1, 0);
    kernels.push_back(kernelGx);
    kernels.push_back(kernelGy);
    return kernels;
}

std::vector<cv::Mat> prewittKernels()
{
    std::vector<cv::Mat> kernels;
    cv::Mat kernelGx = (cv::Mat_<double>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    cv::Mat kernelGy = (cv::Mat_<double>(3, 3) << 1, 1, 1, 0, 0, 0, -1, -1, -1);
    kernels.push_back(kernelGx);
    kernels.push_back(kernelGy);
    return kernels;
}

std::vector<cv::Mat> sobelKernels()
{
    std::vector<cv::Mat> kernels;
    cv::Mat kernelGx = (cv::Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    cv::Mat kernelGy = (cv::Mat_<double>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
    kernels.push_back(kernelGx);
    kernels.push_back(kernelGy);
    return kernels;
}

cv::Mat laplacianKernel()
{
    return (cv::Mat_<double>(3, 3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
}

class Sharpen::SharpenPrivate
{
    Q_GADGET
public:
    enum Kernel : int { Roberts = 0, Prewitt, Sobel, Laplacian };
    Q_ENUM(Kernel);

    explicit SharpenPrivate(Sharpen *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Sharpen::tr("Sharpen"));

        auto metaEnum = QMetaEnum::fromType<Kernel>();
        kernelComboBox = new QComboBox(groupBox);
        for (int i = 0; i < metaEnum.keyCount(); ++i) {
            kernelComboBox->addItem(metaEnum.key(i), metaEnum.value(i));
        }
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(Sharpen::tr("Kernel:"), kernelComboBox);
    }

    Sharpen *q_ptr;

    QGroupBox *groupBox;
    QComboBox *kernelComboBox;
};

Sharpen::Sharpen(QObject *parent)
    : Enhancement(parent)
    , d_ptr(new SharpenPrivate(this))
{
    d_ptr->setupUI();
}

Sharpen::~Sharpen() {}

auto Sharpen::canApply() const -> bool
{
    return true;
}

auto Sharpen::apply(const cv::Mat &src) -> cv::Mat
{
    auto kernelType = static_cast<SharpenPrivate::Kernel>(
        d_ptr->kernelComboBox->currentData().toInt());
    std::vector<cv::Mat> kernels;
    switch (kernelType) {
    case SharpenPrivate::Roberts: kernels = robertsKernels(); break;
    case SharpenPrivate::Prewitt: kernels = prewittKernels(); break;
    case SharpenPrivate::Sobel: kernels = sobelKernels(); break;
    case SharpenPrivate::Laplacian: kernels = {laplacianKernel()}; break;
    default: break;
    }
    return Utils::asynchronous<cv::Mat>([src, kernelType, kernels]() -> cv::Mat {
        auto dst = src.clone();
        cv::Mat grayImage;
        try {
            if (src.channels() == 3) {
                cv::cvtColor(src, grayImage, cv::COLOR_BGR2GRAY);
            } else {
                grayImage = src.clone();
            }
            cv::Mat gradientX, gradientY, gradient;
            if (kernelType < SharpenPrivate::Laplacian) {
                cv::filter2D(grayImage, gradientX, CV_64F, kernels[0]);
                cv::filter2D(grayImage, gradientY, CV_64F, kernels[1]);
                cv::addWeighted(gradientX, 0.5, gradientY, 0.5, 0, gradient);
            } else {
                cv::filter2D(grayImage, gradient, CV_64F, kernels[0]);
            }
            cv::convertScaleAbs(gradient, gradient);
            cv::addWeighted(grayImage, 1.0, gradient, 1.0, 0, dst);
        } catch (const cv::Exception &e) {
            qWarning() << "Sharpen:" << e.what();
        }
        return dst;
    });
}

auto Sharpen::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils

#include "sharpen.moc"
