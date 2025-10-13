#include "boxfilter.hpp"

#include <utils/utils.hpp>

#include <QtWidgets>

#include <opencv2/imgproc.hpp>

namespace OpenCVUtils {

class BoxFilter::BoxFilterPrivate
{
public:
    explicit BoxFilterPrivate(BoxFilter *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(BoxFilter::tr("Box Filter"));

        kWidthComboBox = new QComboBox(groupBox);
        kHeightComboBox = new QComboBox(groupBox);
        for (int i = 3; i <= 21; i += 2) {
            kWidthComboBox->addItem(QString::number(i), i);
            kHeightComboBox->addItem(QString::number(i), i);
        }

        kNormalizeCheckBox = new QCheckBox(tr("Normalize"), groupBox);
        kNormalizeCheckBox->setChecked(true);

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
        fromLayout->addRow(BoxFilter::tr("Kernel Width:"), kWidthComboBox);
        fromLayout->addRow(BoxFilter::tr("Kernel Height:"), kHeightComboBox);
        fromLayout->addWidget(kNormalizeCheckBox);
        fromLayout->addRow(BoxFilter::tr("Border Type:"), borderTypeComboBox);
    }

    BoxFilter *q_ptr;

    QGroupBox *groupBox;
    QComboBox *kWidthComboBox;
    QComboBox *kHeightComboBox;
    QCheckBox *kNormalizeCheckBox;
    QComboBox *borderTypeComboBox;
};

BoxFilter::BoxFilter(QObject *parent)
    : Filter(parent)
    , d_ptr(new BoxFilterPrivate(this))
{
    d_ptr->setupUI();
}

BoxFilter::~BoxFilter() {}

auto BoxFilter::canApply() const -> bool
{
    return true;
}

auto BoxFilter::apply(const cv::Mat &src) -> cv::Mat
{
    auto kWidth = d_ptr->kWidthComboBox->currentData().toInt();
    auto kHeight = d_ptr->kHeightComboBox->currentData().toInt();
    auto kNormalize = d_ptr->kNormalizeCheckBox->isChecked();
    auto borderType = d_ptr->borderTypeComboBox->currentData().toInt();

    return Utils::asynchronous<cv::Mat>([src, kWidth, kHeight, kNormalize, borderType]() -> cv::Mat {
        cv::Mat dst;
        try {
            cv::boxFilter(src,
                          dst,
                          src.depth(),
                          cv::Size(kWidth, kHeight),
                          cv::Point(-1, -1),
                          kNormalize,
                          borderType);
        } catch (const cv::Exception &e) {
            qWarning() << "BoxFilter:" << e.what();
        }
        return dst;
    });
}

auto BoxFilter::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

} // namespace OpenCVUtils
