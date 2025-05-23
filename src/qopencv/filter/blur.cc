#include "blur.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class Blur::BlurPrivate
{
public:
    explicit BlurPrivate(Blur *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(Blur::tr("Blur"));

        kWidthComboBox = new QComboBox(groupBox);
        kHeightComboBox = new QComboBox(groupBox);
        for (int i = 3; i <= 21; i += 2) {
            kWidthComboBox->addItem(QString::number(i), i);
            kHeightComboBox->addItem(QString::number(i), i);
        }

        borderTypeComboBox = new QComboBox(groupBox);
        borderTypeComboBox->addItem("CONSTANT", cv::BORDER_CONSTANT);
        borderTypeComboBox->addItem("REPLICATE", cv::BORDER_REPLICATE);
        borderTypeComboBox->addItem("REFLECT", cv::BORDER_REFLECT);
        // borderTypeComboBox->addItem("WRAP", cv::BORDER_WRAP);
        borderTypeComboBox->addItem("REFLECT_1010", cv::BORDER_REFLECT_101);
        borderTypeComboBox->addItem("TRANSPARENT", cv::BORDER_TRANSPARENT);
        borderTypeComboBox->addItem("REFLECT101", cv::BORDER_REFLECT_101);
        borderTypeComboBox->addItem("DEFAULT", cv::BORDER_DEFAULT);
        borderTypeComboBox->addItem("ISOLATED", cv::BORDER_ISOLATED);
        borderTypeComboBox->setCurrentText("DEFAULT");
    }

    void setupUI()
    {
        auto *fromLayout = new QFormLayout(groupBox);
        fromLayout->addRow(Blur::tr("Kernel Width:"), kWidthComboBox);
        fromLayout->addRow(Blur::tr("Kernel Height:"), kHeightComboBox);
        fromLayout->addRow(Blur::tr("Border Type:"), borderTypeComboBox);
    }

    Blur *q_ptr;

    QGroupBox *groupBox;
    QComboBox *kWidthComboBox;
    QComboBox *kHeightComboBox;
    QComboBox *borderTypeComboBox;
};

Blur::Blur(QObject *parent)
    : Filter(parent)
    , d_ptr(new BlurPrivate(this))
{
    d_ptr->setupUI();
}

Blur::~Blur() {}

auto Blur::canApply() const -> bool
{
    return true;
}

auto Blur::apply(const cv::Mat &src) -> cv::Mat
{
    auto kWidth = d_ptr->kWidthComboBox->currentData().toInt();
    auto kHeight = d_ptr->kHeightComboBox->currentData().toInt();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>([src, kWidth, kHeight, borderType]() -> cv::Mat {
        cv::Mat dst;
        try {
            cv::blur(src, dst, cv::Size(kWidth, kHeight), cv::Point(-1, -1), borderType);
        } catch (const cv::Exception &e) {
            qWarning() << "Blur:" << e.what();
        }
        return dst;
    });
}

auto Blur::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils
