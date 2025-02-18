#pragma once

#include <examples/common/viewer.hpp>

class RhiViewer : public Viewer
{
    Q_OBJECT
public:
    explicit RhiViewer(QWidget *parent = nullptr);
    ~RhiViewer() override;

signals:
    void imageReady(const QImage &);

private slots:
    void onOpenImage();
    void onBackendChanged(const QString &name);
    void onChangedImage(int index);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class RhiViewerPrivate;
    QScopedPointer<RhiViewerPrivate> d_ptr;
};
