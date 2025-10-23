#include "sectionalsubtitlesview.hpp"
#include "subtitledividerlineitem.hpp"

#include <QtWidgets>

class SectionalSubtitlesView::SectionalSubtitlesViewPrivate
{
public:
    SectionalSubtitlesViewPrivate(QWidget *parent)
        : q_ptr(parent)
        , line1Ptr(new SubtitleDividerLineItem)
        , line2Ptr(new SubtitleDividerLineItem)
    {}
    ~SectionalSubtitlesViewPrivate() {}

    QWidget *q_ptr;
    SubtitleDividerLineItemPtr line1Ptr;
    SubtitleDividerLineItemPtr line2Ptr;
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
    scene()->addItem(d_ptr->line1Ptr.get());
    scene()->addItem(d_ptr->line2Ptr.get());

    const auto sceneRect = this->sceneRect();

    // 使用 sceneRect 的实际边界，而不是假设左上角为 (0,0)
    const auto left = sceneRect.left();
    const auto right = sceneRect.right();
    const auto top = sceneRect.top();
    const auto bottom = sceneRect.bottom();
    const auto width = sceneRect.width();
    const auto height = sceneRect.height();

    // 计算线的位置
    const auto topLineY = top + height * 0.8; // 从实际顶部开始计算的4/5位置

    // 计算边距，确保线条不会超出边界
    const auto margin = std::max({d_ptr->line1Ptr->margin(), d_ptr->line2Ptr->margin(), 1.0}) / 2.0;
    const auto x1 = left + margin;   // 左侧位置（考虑实际左边界）
    const auto x2 = right - margin;  // 右侧位置（考虑实际右边界）
    const auto y2 = bottom - margin; // 底部位置（考虑实际底部边界）

    // 创建水平线
    QLineF topLine(x1, topLineY, x2, topLineY);
    QLineF bottomLine(x1, y2, x2, y2);

    // 设置线条
    d_ptr->line1Ptr->setLine(topLine);
    d_ptr->line2Ptr->setLine(bottomLine);
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
    return d_ptr->line1Ptr->line().y1() / sceneRect().height();
}

void SectionalSubtitlesView::setLine1RatioOfHeight(double value)
{
    auto line = d_ptr->line1Ptr->line();
    setLineHeightOfRatio(line, value);
    d_ptr->line1Ptr->setLine(line);
}

auto SectionalSubtitlesView::line2RatioOfHeight() const -> double
{
    return d_ptr->line2Ptr->line().y1() / sceneRect().height();
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
            &SubtitleDividerLineItem::lineChanged,
            this,
            &SectionalSubtitlesView::line1Changed);
    connect(d_ptr->line2Ptr.data(),
            &SubtitleDividerLineItem::lineChanged,
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
