#ifndef LISTIITEMVIEW_HPP
#define LISTIITEMVIEW_HPP

#include <graphics/imageview.h>

class ListItemView : public Graphics::ImageView
{
    Q_OBJECT
public:
    explicit ListItemView(QWidget *parent = nullptr);
    ~ListItemView();

    void setIndex(int index, bool showUpButton, bool showDownButton);
    int index() const;
    void setImagePath(const QString &path);
    QString imagePath() const;

    void setImage(const QImage &image, const QSize &size);
    virtual void setImageAfter();

    QImage image() const;

signals:
    void up(int index);
    void down(int index);

protected:
    QSize realImageSize() const;

    void resizeEvent(QResizeEvent *event);

private:
    void setupUI();
    void buildConnect();

    class ListItemViewPrivate;
    QScopedPointer<ListItemViewPrivate> d_ptr;
};

#endif // LISTIITEMVIEW_HPP
