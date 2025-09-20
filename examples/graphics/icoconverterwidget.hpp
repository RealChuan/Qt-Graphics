#pragma once

#include <examples/common/viewer.hpp>

class IcoConverterWidget : public Viewer
{
    Q_OBJECT
public:
    explicit IcoConverterWidget(QWidget *parent = nullptr);
    ~IcoConverterWidget() override;

private slots:
    void onOpenImage();
    void onChangedImage(int index);
    void onPickSquareColor();
    void onBrowse();
    void onConvert();

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class IcoConverterWidgetPrivate;
    QScopedPointer<IcoConverterWidgetPrivate> d_ptr;
};
