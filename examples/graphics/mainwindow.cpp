#include "mainwindow.h"
#include "capturewidget.hpp"
#include "drawwidget.h"
#include "icoconverterwidget.hpp"
#include "imageviewer.h"
#include "multiimagefileviewer.hpp"
#include "openglviewer.hpp"
#include "recordwidget.hpp"
#include "subtitlsplicingwidget.hpp"
#ifdef BUILD_VULKAN
#include "vulkanviewer.hpp"
#endif

#include <utils/utils.h>

#include <QtWidgets>

class MainWindow::MainWindowPrivate
{
public:
    explicit MainWindowPrivate(MainWindow *q)
        : q_ptr(q)
    {
        imageViewer = new ImageViewer(q_ptr);
        openglViewer = new OpenglViewer(q_ptr);
        multiImageFileViewer = new MultiImageFileViewer(q_ptr);
        icoConverterWidget = new IcoConverterWidget(q_ptr);
        subtitlSplicingWidget = new SubtitlSplicingWidget(q_ptr);
        drawWidget = new DrawWidget(q_ptr);

        stackedWidget = new QStackedWidget(q_ptr);
        stackedWidget->addWidget(imageViewer);
        stackedWidget->addWidget(openglViewer);
#ifdef BUILD_VULKAN
        if (VulkanViewer::isSupported()) {
            vulkanViewer = new VulkanViewer(q_ptr);
            stackedWidget->addWidget(vulkanViewer);
        }
#endif
        stackedWidget->addWidget(multiImageFileViewer);
        stackedWidget->addWidget(icoConverterWidget);
        stackedWidget->addWidget(subtitlSplicingWidget);
        stackedWidget->addWidget(drawWidget);
    }
    ~MainWindowPrivate() {}

    MainWindow *q_ptr;

    ImageViewer *imageViewer;
    OpenglViewer *openglViewer;
#ifdef BUILD_VULKAN
    VulkanViewer *vulkanViewer = nullptr;
#endif
    MultiImageFileViewer *multiImageFileViewer;
    IcoConverterWidget *icoConverterWidget;
    SubtitlSplicingWidget *subtitlSplicingWidget;
    DrawWidget *drawWidget;
    QStackedWidget *stackedWidget;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , d_ptr(new MainWindowPrivate(this))
{
    setupUI();
    resize(1000, 618);
    Utils::windowCenter(this);

#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    // 图片读取限制大小
    qInfo() << tr("QImage memory allocation above this limit: %1MB.")
                   .arg(QImageReader::allocationLimit());
    //QImageReader::setAllocationLimit();
#endif
    // fix QOpenGLWidget initialize
    QMetaObject::invokeMethod(
        this,
        [this] {
            d_ptr->stackedWidget->setCurrentWidget(d_ptr->openglViewer);
            d_ptr->stackedWidget->setCurrentWidget(d_ptr->imageViewer);
        },
        Qt::QueuedConnection);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    initMenuBar();
    setCentralWidget(d_ptr->stackedWidget);
}

void MainWindow::initMenuBar()
{
    auto *actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    auto setCheckable = [actionGroup](QAction *action) {
        actionGroup->addAction(action);
        action->setCheckable(true);
    };

    auto *menu = new QMenu(tr("Select Widget"), this);
    auto *action = menu->addAction(tr("Image Viewer"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->imageViewer);
    });
    setCheckable(action);
    action->setChecked(true);

    setCheckable(menu->addAction(tr("Opengl Viewer"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->openglViewer);
    }));
#ifdef BUILD_VULKAN
    if (nullptr != d_ptr->vulkanViewer) {
        setCheckable(menu->addAction(tr("Vulakn Viewer"), this, [this] {
            d_ptr->stackedWidget->setCurrentWidget(d_ptr->vulkanViewer);
        }));
    }
#endif
    setCheckable(menu->addAction(tr("Multi Image File Viewer"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->multiImageFileViewer);
    }));
    setCheckable(menu->addAction(tr("ICO Converter"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->icoConverterWidget);
    }));
    setCheckable(menu->addAction(tr("Subtitle Splicing"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->subtitlSplicingWidget);
    }));
    setCheckable(menu->addAction(tr("Draw"), this, [this] {
        d_ptr->stackedWidget->setCurrentWidget(d_ptr->drawWidget);
    }));
    menuBar()->addMenu(menu);
    menuBar()->addAction(tr("Screenshots"), this, [this] {
        showMinimized();
        auto captureWidget = new CaptureWidget;
        captureWidget->show();
    });
    menuBar()->addAction(tr("Record GIf"), this, [this] {
        showMinimized();
        auto recordWidget = new RecordWidget;
        recordWidget->show();
    });
}