#pragma once

#include <examples/common/viewer.hpp>

class MultiImageFileViewer : public Viewer
{
    Q_OBJECT
public:
    explicit MultiImageFileViewer(QWidget *parent = nullptr);
    ~MultiImageFileViewer() override;

    void setImageUrl(const QString &url);

private slots:
    void onOpenImage();
    void onChangedImage(int index);
    void onImageUrlChanged(const QString &url);

private:
    void setupUI();
    auto toolWidget() -> QWidget *;
    void buildConnect();

    class MultiImageFileViewerPrivate;
    QScopedPointer<MultiImageFileViewerPrivate> d_ptr;
};
