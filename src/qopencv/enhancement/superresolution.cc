#include "superresolution.hpp"

#include <utils/utils.h>

#include <QtWidgets>

#include <opencv2/dnn_superres.hpp>

namespace OpenCVUtils {

class SuperResolution::SuperResolutionPrivate
{
public:
    explicit SuperResolutionPrivate(SuperResolution *q)
        : q_ptr(q)
    {
        groupBox = new QGroupBox(SuperResolution::tr("Super Resolution"));

        modelLineEdit = new QLineEdit(groupBox);
        modelButton = new QToolButton(groupBox);
        modelButton->setText(SuperResolution::tr("Select Model"));
        modelButton->setIcon(QIcon::fromTheme("document-open"));
    }

    void setupUI()
    {
        auto *layout = new QHBoxLayout(groupBox);
        layout->addWidget(new QLabel(SuperResolution::tr("Model:")));
        layout->addWidget(modelLineEdit);
        layout->addWidget(modelButton);
    }

    SuperResolution *q_ptr;

    QGroupBox *groupBox;
    QLineEdit *modelLineEdit;
    QToolButton *modelButton;
};

SuperResolution::SuperResolution(QObject *parent)
    : Enhancement{parent}
    , d_ptr{new SuperResolutionPrivate(this)}
{
    d_ptr->setupUI();
    buildConnect();
}

SuperResolution::~SuperResolution() {}

auto SuperResolution::canApply() const -> bool
{
    auto text = d_ptr->modelLineEdit->text().trimmed();
    return !text.isEmpty() && text.endsWith(".pb") && QFile::exists(text);
}

auto SuperResolution::apply(const cv::Mat &src) -> cv::Mat
{
    Q_ASSERT_X(canApply(), "SuperResolution::apply", "Model file not set or does not exist.");
    // e.g. ESPCN_2x.pb ESPCN_3x.pb ESPCN_4x.pb
    // 模型来自  https://github.com/opencv/opencv_contrib/tree/master/modules/dnn_superres
    auto modelPath = d_ptr->modelLineEdit->text().trimmed();

    return Utils::asynchronous<cv::Mat>([modelPath, src]() {
        auto list = QFileInfo(modelPath).baseName().split('_');
        Q_ASSERT_X(list.size() == 2, "SuperResolution::apply", "Invalid model name.");
        auto modelName = list.first().toLower();
        auto scale = list.last().remove('x').toInt();

        cv::Mat dst;
        try {
            cv::dnn_superres::DnnSuperResImpl sr;
            sr.readModel(modelPath.toStdString());
            sr.setModel(modelName.toStdString(), scale);
            sr.upsample(src, dst);
        } catch (const cv::Exception &e) {
            qWarning() << "SuperResolution:" << e.what();
        }
        return dst;
    });
}

void SuperResolution::onSelectModel()
{
    auto fileName = QFileDialog::getOpenFileName(Utils::getTopParentWidget(d_ptr->groupBox),
                                                 tr("Select Model"),
                                                 QStandardPaths::standardLocations(
                                                     QStandardPaths::DocumentsLocation)
                                                     .value(0, QDir::homePath()),
                                                 tr("Model Files (*.pb)"));
    if (fileName.isEmpty()) {
        return;
    }
    d_ptr->modelLineEdit->setText(fileName);
}

auto SuperResolution::createParamWidget() -> QWidget *
{
    return d_ptr->groupBox;
}

void SuperResolution::buildConnect()
{
    connect(d_ptr->modelButton, &QToolButton::clicked, this, &SuperResolution::onSelectModel);
}

} // namespace OpenCVUtils
