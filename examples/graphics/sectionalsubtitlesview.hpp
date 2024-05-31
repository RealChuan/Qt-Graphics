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
    ~SectionalSubtitlesView() override;

    void setImageAfter() override;

    [[nodiscard]] auto clipImage() const -> QImage;

    [[nodiscard]] auto info() const -> StitchingImageInfo;

    [[nodiscard]] auto line1RatioOfHeight() const -> double;
    void setLine1RatioOfHeight(double value);

    [[nodiscard]] auto line2RatioOfHeight() const -> double;
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
