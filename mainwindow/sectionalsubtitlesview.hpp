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
    int index() const;
    void setImagePath(const QString &path);
    QString imagePath() const;

    void setImage(const QImage &image, const QSize &size);

    QImage image() const;
    QImage clipImage() const;

    StitchingImageInfo info() const;

    double line1RatioOfHeight() const;
    void setLine1RatioOfHeight(double value);

    double line2RatioOfHeight() const;
    void setLine2RatioOfHeight(double value);

signals:
    void up(int index);
    void down(int index);
    void line1Changed();
    void line2Changed();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    void setupUI();
    void buildConnect();
    void setLineHeightORatio(QLineF &line, double radio);

    class SectionalSubtitlesViewPrivate;
    QScopedPointer<SectionalSubtitlesViewPrivate> d_ptr;
};

#endif // SECTIONALSUBTITLESVIEW_HPP
