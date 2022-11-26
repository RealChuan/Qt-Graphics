#include "recordwidget.hpp"
#include "recordgifthread.hpp"

#include <utils/utils.h>

#include <QtWidgets>

class RecordWidget::RecordWidgetPrivate
{
public:
    RecordWidgetPrivate(QWidget *parent)
        : owner(parent)
    {
        frameRateSpinBox = new QSpinBox(owner);
        frameRateSpinBox->setRange(1, 60);
        frameRateSpinBox->setValue(10);
        frameRateSpinBox->setKeyboardTracking(false);
        widthSpinBox = new QSpinBox(owner);
        widthSpinBox->setKeyboardTracking(false);
        heightSpinBox = new QSpinBox(owner);
        heightSpinBox->setKeyboardTracking(false);
        startButton = new QToolButton(owner);
        startButton->setText(QObject::tr("Start", "RecordWidget"));
        titleWidget = new QWidget(owner);
        screenshotsWidget = new QWidget(owner);
        screenshotsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //screenshotsWidget->setStyleSheet("QWidget{background:transparent;}");
    }
    ~RecordWidgetPrivate() {}

    QWidget *owner;

    QSpinBox *frameRateSpinBox;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
    QToolButton *startButton;
    QWidget *titleWidget;
    QWidget *screenshotsWidget;

    int borderWidth = 10;
    //QColor backgroundColor = QColor(39, 205, 237);
    QColor backgroundColor = QColor(231, 231, 239);
    //记录鼠标位置
    QPoint lastPoint;

    QScopedPointer<RecordGifThread> recordGifThreadPtr;
};

RecordWidget::RecordWidget(QWidget *parent)
    : QWidget{parent}
    , d_ptr(new RecordWidgetPrivate(this))
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setupUI();
    buildConnect();
    resize(1000, 618);
}

RecordWidget::~RecordWidget() {}

void RecordWidget::onChangeSize()
{
    d_ptr->widthSpinBox->blockSignals(true);
    d_ptr->heightSpinBox->blockSignals(true);
    auto size = screen()->size();
    d_ptr->widthSpinBox->setRange(20, size.width());
    d_ptr->heightSpinBox->setRange(20, size.height());
    d_ptr->widthSpinBox->setValue(d_ptr->screenshotsWidget->width() - d_ptr->borderWidth * 2);
    d_ptr->heightSpinBox->setValue(d_ptr->screenshotsWidget->height() - d_ptr->borderWidth);
    d_ptr->widthSpinBox->blockSignals(false);
    d_ptr->heightSpinBox->blockSignals(false);
}

void RecordWidget::onResizeGifWidget()
{
    auto width = d_ptr->widthSpinBox->value() + d_ptr->borderWidth * 2;
    auto height = d_ptr->heightSpinBox->value() + d_ptr->borderWidth + d_ptr->titleWidget->height();
    resize(width, height);
}

void RecordWidget::onStart()
{
    d_ptr->startButton->setEnabled(false);
    auto text = d_ptr->startButton->text();
    if (text == tr("Start")) {
        start();
        d_ptr->startButton->setText(tr("Stop"));
    } else {
        finish();
        d_ptr->startButton->setText(tr("Start"));
    }
    d_ptr->startButton->setEnabled(true);
}

void RecordWidget::mousePressEvent(QMouseEvent *event)
{
    const QMargins margins(layout()->contentsMargins());
    const QRect rect(d_ptr->titleWidget->rect().adjusted(margins.left(),
                                                         margins.top(),
                                                         margins.right(),
                                                         margins.bottom()));
    if (rect.contains(event->pos())) {
        d_ptr->lastPoint = event->pos();
    }

    QWidget::mousePressEvent(event);
}

void RecordWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (isMaximized() || isFullScreen()) {
    } else if (!d_ptr->lastPoint.isNull()) {
        move(QCursor::pos() - d_ptr->lastPoint);
    }

    QWidget::mouseMoveEvent(event);
}

void RecordWidget::mouseReleaseEvent(QMouseEvent *event)
{
    d_ptr->lastPoint = QPoint();

    QWidget::mouseReleaseEvent(event);
}

void RecordWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    QMetaObject::invokeMethod(this, &RecordWidget::onChangeSize, Qt::QueuedConnection);
}

void RecordWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    auto rectGif = QRect(d_ptr->screenshotsWidget->pos(), d_ptr->screenshotsWidget->size());
    rectGif.adjust(d_ptr->borderWidth, 0, -d_ptr->borderWidth, -d_ptr->borderWidth);

    QPainter painter(this);
    //painter.setPen(Qt::NoPen);
    painter.setPen(QColor(0, 0, 0, 18));
    //painter.setOpacity(0.8);
    painter.setBrush(d_ptr->backgroundColor);
    painter.drawRoundedRect(rect(), 5, 5);
    painter.setCompositionMode(QPainter::CompositionMode_Clear);
    painter.fillRect(rectGif, Qt::SolidPattern);
}

void RecordWidget::setupUI()
{
    auto closeButton = new QToolButton(this);
    closeButton->setStyleSheet("QToolButton{border: none;}");
    closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    connect(closeButton, &QToolButton::clicked, this, &RecordWidget::close);

    auto titleLayout = new QHBoxLayout(d_ptr->titleWidget);
    titleLayout->addWidget(new QLabel(tr("Frame rate: "), this));
    titleLayout->addWidget(d_ptr->frameRateSpinBox);
    titleLayout->addWidget(new QLabel(tr("Width: "), this));
    titleLayout->addWidget(d_ptr->widthSpinBox);
    titleLayout->addWidget(new QLabel(tr("Height: "), this));
    titleLayout->addWidget(d_ptr->heightSpinBox);
    titleLayout->addWidget(d_ptr->startButton);
    titleLayout->addStretch();
    titleLayout->addWidget(closeButton);

    auto layout = new QGridLayout(this);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);
    layout->addWidget(d_ptr->titleWidget, 0, 0);
    layout->addWidget(d_ptr->screenshotsWidget, 1, 0);
    layout->addWidget(new QSizeGrip(this), 1, 0, Qt::AlignRight | Qt::AlignBottom);
}

void RecordWidget::buildConnect()
{
    connect(d_ptr->widthSpinBox, &QSpinBox::valueChanged, this, &RecordWidget::onResizeGifWidget);
    connect(d_ptr->heightSpinBox, &QSpinBox::valueChanged, this, &RecordWidget::onResizeGifWidget);

    connect(d_ptr->startButton, &QToolButton::clicked, this, &RecordWidget::onStart);
}

QRect RecordWidget::recordRect()
{
    auto pos = mapToGlobal(d_ptr->screenshotsWidget->pos());
    auto devicePixelRatio = d_ptr->screenshotsWidget->screen()->devicePixelRatio();
    auto rect = QRect(pos * devicePixelRatio, d_ptr->screenshotsWidget->size() * devicePixelRatio);
    auto borderWidth = d_ptr->borderWidth * devicePixelRatio;
    rect.adjust(borderWidth, 0, -borderWidth, -borderWidth);
    return rect;
}

void RecordWidget::start()
{
    auto delay = 1000.0 / d_ptr->frameRateSpinBox->value();

    d_ptr->recordGifThreadPtr.reset(new RecordGifThread);
    d_ptr->recordGifThreadPtr->startCapture(this, delay);
}

void RecordWidget::finish()
{
    if (!d_ptr->recordGifThreadPtr.isNull()) {
        d_ptr->recordGifThreadPtr->stopCapture();
    }
    auto path = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)
                    .value(0, QDir::homePath());
    const auto time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss");
    path = path + "/" + time + ".gif";
    const QString filename = QFileDialog::getSaveFileName(this,
                                                          tr("Save Image"),
                                                          path,
                                                          tr("Images (*.gif)"));
    if (!d_ptr->recordGifThreadPtr.isNull()) {
        d_ptr->recordGifThreadPtr->stop(filename);
    }
}
