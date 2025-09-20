#pragma once

#include <examples/common/viewer.hpp>

class OpenglViewer : public Viewer
{
    Q_OBJECT
public:
    explicit OpenglViewer(QWidget *parent = nullptr);
    ~OpenglViewer() override;

private slots:
    void onOpenImage();
    void onChangedImage(int index);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class OpenglViewerPrivate;
    QScopedPointer<OpenglViewerPrivate> d_ptr;
};
