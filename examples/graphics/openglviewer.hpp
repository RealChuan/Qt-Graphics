#pragma once

#include <examples/common/viewer.hpp>

class OpenglViewer : public Viewer
{
    Q_OBJECT
public:
    explicit OpenglViewer(QWidget *parent = nullptr);
    ~OpenglViewer() override;

signals:
    void imageReady(const QImage &);

private slots:
    void onOpenImage();
    void onChangedImage(int);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class OpenglViewerPrivate;
    QScopedPointer<OpenglViewerPrivate> d_ptr;
};
