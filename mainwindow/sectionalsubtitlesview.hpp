#ifndef SECTIONALSUBTITLESVIEW_HPP
#define SECTIONALSUBTITLESVIEW_HPP

#include "listiitemview.hpp"

struct StitchingImageInfo
{
    QString imagePath;
    QRect imageRect;
};

class SectionalSubtitlesView : public ListItemView
{
    Q_OBJECT
public:
    explicit SectionalSubtitlesView(QWidget *parent = nullptr);
    ~SectionalSubtitlesView();

    void setImageAfter() override;

    QImage clipImage() const;

    StitchingImageInfo info() const;

    double line1RatioOfHeight() const;
    void setLine1RatioOfHeight(double value);

    double line2RatioOfHeight() const;
    void setLine2RatioOfHeight(double value);

signals:
    void line1Changed();
    void line2Changed();

private:
    void buildConnect();
    void setLineHeightOfRatio(QLineF &line, double radio);

    class SectionalSubtitlesViewPrivate;
    QScopedPointer<SectionalSubtitlesViewPrivate> d_ptr;
};

#endif // SECTIONALSUBTITLESVIEW_HPP
