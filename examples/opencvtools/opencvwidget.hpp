#pragma once

#include <examples/common/viewer.hpp>

class OpenCVWidget : public Viewer
{
    Q_OBJECT
public:
    explicit OpenCVWidget(QWidget *parent = nullptr);
    ~OpenCVWidget() override;

private slots:
    void onOpenImage();
    void onChangedImage(int index);
    void onShowOriginalImage();
    void onTypeChanged();
    void onAlgorithmChanged();
    void onApply();

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class OpenCVWidgetPrivate;
    QScopedPointer<OpenCVWidgetPrivate> d_ptr;
};
