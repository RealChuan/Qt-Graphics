#ifndef SECTIONALSUBTITLESVIEW_HPP
#define SECTIONALSUBTITLESVIEW_HPP

#include <graphics/imageview.h>

class SectionalSubtitlesView : public Graphics::ImageView
{
    Q_OBJECT
public:
    explicit SectionalSubtitlesView(QWidget *parent = nullptr);
    ~SectionalSubtitlesView();

    void setImagePath(const QString &path);

    void setImage(const QImage &image);

    QImage image() const;

    QImage clipImage() const;

private:
    struct SectionalSubtitlesViewPrivate;
    QScopedPointer<SectionalSubtitlesViewPrivate> d_ptr;
};

#endif // SECTIONALSUBTITLESVIEW_HPP
