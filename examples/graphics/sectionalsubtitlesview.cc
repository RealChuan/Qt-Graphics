#include "sectionalsubtitlesview.hpp"
#include "customlineitem.hpp"

#include <QtWidgets>

class SectionalSubtitlesView::SectionalSubtitlesViewPrivate
{
public:
    SectionalSubtitlesViewPrivate(QWidget *parent)
        : q_ptr(parent)
        , line1Ptr(new CustomLineItem)
        , line2Ptr(new CustomLineItem)
    {}
    ~SectionalSubtitlesViewPrivate() {}

    QWidget *q_ptr;
    QScopedPointer<CustomLineItem> line1Ptr;
    QScopedPointer<CustomLineItem> line2Ptr;
};

SectionalSubtitlesView::SectionalSubtitlesView(QWidget *parent)
    : ListItemView(parent)
    , d_ptr(new SectionalSubtitlesViewPrivate(this))
{
    buildConnect();
}

SectionalSubtitlesView::~SectionalSubtitlesView() {}

void SectionalSubtitlesView::setImageAfter()
{
    auto rect = sceneRect();
    auto y = rect.height() / 5.0 * 4;
    d_ptr->line1Ptr->setLine(QLineF(1, y, rect.width() - 1, y));
    d_ptr->line2Ptr->setLine(QLineF(1, rect.height() - 2, rect.width() - 1, rect.height() - 2));
    scene()->addItem(d_ptr->line1Ptr.data());
    scene()->addItem(d_ptr->line2Ptr.data());
}

auto SectionalSubtitlesView::clipImage() const -> QImage
{
    auto imageSize = realImageSize();
    auto y1 = line1RatioOfHeight() * imageSize.height();
    auto y2 = line2RatioOfHeight() * imageSize.height();
    if (y1 > y2) {
        std::swap(y1, y2);
    }
    auto image = this->image();
    image = image.copy(QRect(QPoint(0, y1), QPoint(imageSize.width(), y2)));
    return image;
}

auto SectionalSubtitlesView::info() const -> StitchingImageInfo
{
    auto imageSize = realImageSize();
    auto y1 = line1RatioOfHeight() * imageSize.height();
    auto y2 = line2RatioOfHeight() * imageSize.height();
    if (y1 > y2) {
        std::swap(y1, y2);
    }
    auto rect = QRect(QPoint(0, y1), QPoint(imageSize.width(), y2));
    return {imagePath(), rect};
}

auto SectionalSubtitlesView::line1RatioOfHeight() const -> double
{
    return (d_ptr->line1Ptr->line().y1() / pixmap().size().height());
}

void SectionalSubtitlesView::setLine1RatioOfHeight(double value)
{
    auto line = d_ptr->line1Ptr->line();
    setLineHeightOfRatio(line, value);
    d_ptr->line1Ptr->setLine(line);
}

auto SectionalSubtitlesView::line2RatioOfHeight() const -> double
{
    return (d_ptr->line2Ptr->line().y1() / pixmap().size().height());
}

void SectionalSubtitlesView::setLine2RatioOfHeight(double value)
{
    auto line = d_ptr->line2Ptr->line();
    setLineHeightOfRatio(line, value);
    d_ptr->line2Ptr->setLine(line);
}

void SectionalSubtitlesView::buildConnect()
{
    connect(d_ptr->line1Ptr.data(),
            &CustomLineItem::lineChanged,
            this,
            &SectionalSubtitlesView::line1Changed);
    connect(d_ptr->line2Ptr.data(),
            &CustomLineItem::lineChanged,
            this,
            &SectionalSubtitlesView::line2Changed);
}

void SectionalSubtitlesView::setLineHeightOfRatio(QLineF &line, double radio)
{
    Q_ASSERT(radio >= 0 && radio <= 1);
    auto y = pixmap().size().height() * radio;

    auto p1 = line.p1();
    auto p2 = line.p2();
    p1.setY(y);
    p2.setY(y);
    line = QLineF(p1, p2);
}
