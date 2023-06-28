#ifndef LISTIITEMVIEW_HPP
#define LISTIITEMVIEW_HPP

#include <graphics/imageview.h>

class ListItemView : public Graphics::ImageView
{
    Q_OBJECT
public:
    explicit ListItemView(QWidget *parent = nullptr);
    ~ListItemView() override;

    void setIndex(int index, bool showUpButton, bool showDownButton);
    [[nodiscard]] auto index() const -> int;
    void setImagePath(const QString &path);
    [[nodiscard]] auto imagePath() const -> QString;

    void setImage(const QImage &image, const QSize &size);
    virtual void setImageAfter();

    [[nodiscard]] auto image() const -> QImage;

signals:
    void up(int index);
    void down(int index);

protected:
    [[nodiscard]] auto realImageSize() const -> QSize;

    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void buildConnect();

    class ListItemViewPrivate;
    QScopedPointer<ListItemViewPrivate> d_ptr;
};

#endif // LISTIITEMVIEW_HPP
