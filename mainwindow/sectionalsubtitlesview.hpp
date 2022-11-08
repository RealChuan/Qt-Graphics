#ifndef SECTIONALSUBTITLESVIEW_HPP
#define SECTIONALSUBTITLESVIEW_HPP

#include <graphics/imageview.h>

struct StitchingImageInfo
{
    QString imagePath;
    QRect imageRect;
};

class SectionalSubtitlesView : public Graphics::ImageView
{
    Q_OBJECT
public:
    explicit SectionalSubtitlesView(QWidget *parent = nullptr);
    ~SectionalSubtitlesView();

    void setIndex(int index, bool showUpButton, bool showDownButton);
    void setImagePath(const QString &path);
    QString imagePath() const;

    void setImage(const QImage &image, const QSize &size);

    QImage image() const;
    QImage clipImage() const;

    StitchingImageInfo info() const;

signals:
    void up(int index);
    void down(int index);

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void setupUI();
    void buildConnect();

    class SectionalSubtitlesViewPrivate;
    QScopedPointer<SectionalSubtitlesViewPrivate> d_ptr;
};

#endif // SECTIONALSUBTITLESVIEW_HPP
