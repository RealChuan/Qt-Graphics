#include "graphicspixmapitem.h"

#include <QCursor>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace Graphics {

namespace {

class CursorManager
{
public:
    CursorManager() = default;

    void updateScaleFactor(const QPainter *painter);
    QCursor createCursorForMode(GraphicsPixmapItem::MaskEditingMode mode);

    // 配置方法
    void setBrushSize(int size) { m_brushSize = qBound(1, size, 500); }
    int brushSize() const { return m_brushSize; }

    void setCheckerColor1(const QColor &color) { m_checkerColor1 = color; }
    QColor checkerColor1() const { return m_checkerColor1; }

    void setCheckerColor2(const QColor &color) { m_checkerColor2 = color; }
    QColor checkerColor2() const { return m_checkerColor2; }

    void setOutlineColor(const QColor &color) { m_outlineColor = color; }
    QColor outlineColor() const { return m_outlineColor; }

    double viewScaleFactor() const { return m_viewScaleFactor; }
    bool needsUpdate() const { return m_needsUpdate; }
    void setNeedsUpdate(bool needsUpdate) { m_needsUpdate = needsUpdate; }

    QPixmap currentBrushPattern() const { return m_currentBrushPattern; }
    void setCurrentBrushPattern(const QPixmap &pattern) { m_currentBrushPattern = pattern; }

private:
    QPixmap createCheckerboardPattern(int size) const;
    QPixmap createCursorPixmap(const QPixmap &brushPattern) const;

    int m_brushSize = 50;
    double m_viewScaleFactor = 1.0;
    QColor m_checkerColor1 = QColor(220, 220, 220);
    QColor m_checkerColor2 = Qt::white;
    QColor m_outlineColor = QColor(220, 220, 220);
    bool m_needsUpdate = true;
    QPixmap m_currentBrushPattern;
};

void CursorManager::updateScaleFactor(const QPainter *painter)
{
    if (!painter) {
        return;
    }

    const double newScaleFactor = painter->transform().m11();
    if (qAbs(newScaleFactor - m_viewScaleFactor) > 0.01) {
        m_viewScaleFactor = newScaleFactor;
        m_needsUpdate = true;
    }
}

QCursor CursorManager::createCursorForMode(GraphicsPixmapItem::MaskEditingMode mode)
{
    if (mode == GraphicsPixmapItem::MaskEditingMode::Normal) {
        return QCursor();
    }

    QPixmap brushPattern;
    if (mode == GraphicsPixmapItem::MaskEditingMode::Erase) {
        brushPattern = QPixmap(m_brushSize, m_brushSize);
        brushPattern.fill(Qt::white);
    } else if (mode == GraphicsPixmapItem::MaskEditingMode::Draw) {
        brushPattern = createCheckerboardPattern(m_brushSize);
    }

    m_currentBrushPattern = brushPattern;

    return QCursor(createCursorPixmap(brushPattern));
}

QPixmap CursorManager::createCheckerboardPattern(int size) const
{
    QPixmap pattern(size, size);
    pattern.fill(Qt::transparent);

    QPainter painter(&pattern);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const int halfSize = size / 2;
    painter.fillRect(0, 0, halfSize, halfSize, m_checkerColor1);
    painter.fillRect(halfSize, halfSize, halfSize, halfSize, m_checkerColor1);
    painter.fillRect(halfSize, 0, halfSize, halfSize, m_checkerColor2);
    painter.fillRect(0, halfSize, halfSize, halfSize, m_checkerColor2);

    return pattern;
}

QPixmap CursorManager::createCursorPixmap(const QPixmap &brushPattern) const
{
    const int scaledSize = m_brushSize * m_viewScaleFactor;
    auto brush = brushPattern.scaled(scaledSize,
                                     scaledSize,
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);

    QPixmap cursorPixmap(scaledSize, scaledSize);
    cursorPixmap.fill(Qt::transparent);

    QPainter painter(&cursorPixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen outlinePen(m_outlineColor, 1);
    painter.setPen(outlinePen);
    painter.setBrush(QBrush(brush));
    painter.drawEllipse(QRectF(0, 0, scaledSize, scaledSize));

    return cursorPixmap;
}

class MaskPainter
{
public:
    MaskPainter() = default;

    void reset(const QSize &size);
    bool isValid() const { return !m_maskImage.isNull(); }

    // 访问器方法
    const QImage &maskImage() const { return m_maskImage; }
    void setMaskImage(const QImage &image) { m_maskImage = image; }

    double opacity() const { return m_opacity; }
    void setOpacity(double opacity) { m_opacity = qBound(0.0, opacity, 1.0); }

    const QPointF &lastBrushPosition() const { return m_lastBrushPosition; }
    void setLastBrushPosition(const QPointF &position) { m_lastBrushPosition = position; }

    const QPointF &currentBrushPosition() const { return m_currentBrushPosition; }
    void setCurrentBrushPosition(const QPointF &position) { m_currentBrushPosition = position; }

private:
    QImage m_maskImage;
    double m_opacity = 0.5;
    QPointF m_lastBrushPosition;
    QPointF m_currentBrushPosition;
};

void MaskPainter::reset(const QSize &size)
{
    m_maskImage = QImage(size, QImage::Format_ARGB32_Premultiplied);
    m_maskImage.fill(Qt::transparent);
    m_lastBrushPosition = QPointF();
    m_currentBrushPosition = QPointF();
}

} // anonymous namespace

// 私有实现类
class GraphicsPixmapItem::GraphicsPixmapItemPrivate
{
public:
    explicit GraphicsPixmapItemPrivate(GraphicsPixmapItem *q)
        : q_ptr(q)
    {}

    GraphicsPixmapItem *q_ptr;

    CursorManager cursorManager;
    MaskPainter maskPainter;
    GraphicsPixmapItem::MaskEditingMode editingMode = GraphicsPixmapItem::MaskEditingMode::Normal;
};

GraphicsPixmapItem::GraphicsPixmapItem(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
    , d_ptr(new GraphicsPixmapItemPrivate(this))
{
    setAcceptHoverEvents(true);
    setFlags(flags() | ItemIsSelectable | ItemIsFocusable);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
    setTransformationMode(Qt::SmoothTransformation);
    setZValue(0);
}

GraphicsPixmapItem::~GraphicsPixmapItem() = default;

void GraphicsPixmapItem::setSourcePixmap(const QPixmap &pixmap)
{
    setPixmap(pixmap);
    if (pixmap.size() != d_ptr->maskPainter.maskImage().size()) {
        resetMask();
    }
    setBrushSize(qMin(pixmap.width(), pixmap.height()) / 20);
}

void GraphicsPixmapItem::setMaskImage(const QImage &mask)
{
    if (mask.isNull() || pixmap().isNull()) {
        return;
    }
    if (pixmap().size() != mask.size()) {
        qWarning() << "Mask size does not match pixmap size";
        return;
    }

    d_ptr->maskPainter.setMaskImage(mask);
    update();
}

auto GraphicsPixmapItem::maskImage() const -> QImage
{
    return d_ptr->maskPainter.maskImage();
}

void GraphicsPixmapItem::setMaskEditingMode(MaskEditingMode mode)
{
    if (d_ptr->editingMode == mode) {
        return;
    }
    d_ptr->editingMode = mode;
    d_ptr->cursorManager.setNeedsUpdate(true);
    updateCursor();
}

auto GraphicsPixmapItem::maskEditingMode() const -> MaskEditingMode
{
    return d_ptr->editingMode;
}

void GraphicsPixmapItem::setBrushSize(int size)
{
    if (d_ptr->cursorManager.brushSize() == size) {
        return;
    }

    d_ptr->cursorManager.setBrushSize(size);
    d_ptr->cursorManager.setNeedsUpdate(true);
    updateCursor();
}

auto GraphicsPixmapItem::brushSize() const -> int
{
    return d_ptr->cursorManager.brushSize();
}

void GraphicsPixmapItem::setMaskOpacity(double opacity)
{
    if (d_ptr->maskPainter.opacity() == opacity) {
        return;
    }
    d_ptr->maskPainter.setOpacity(opacity);
    update();
}

auto GraphicsPixmapItem::maskOpacity() const -> double
{
    return d_ptr->maskPainter.opacity();
}

void GraphicsPixmapItem::setCheckerColor1(const QColor &color)
{
    if (d_ptr->cursorManager.checkerColor1() == color) {
        return;
    }
    d_ptr->cursorManager.setCheckerColor1(color);
    d_ptr->cursorManager.setNeedsUpdate(true);
    updateCursor();
}

auto GraphicsPixmapItem::checkerColor1() const -> QColor
{
    return d_ptr->cursorManager.checkerColor1();
}

void GraphicsPixmapItem::setCheckerColor2(const QColor &color)
{
    if (d_ptr->cursorManager.checkerColor2() == color) {
        return;
    }
    d_ptr->cursorManager.setCheckerColor2(color);
    d_ptr->cursorManager.setNeedsUpdate(true);
    updateCursor();
}

auto GraphicsPixmapItem::checkerColor2() const -> QColor
{
    return d_ptr->cursorManager.checkerColor2();
}

void GraphicsPixmapItem::resetMask()
{
    if (!pixmap().isNull()) {
        d_ptr->maskPainter.reset(pixmap().size());
    }
    update();
}

void GraphicsPixmapItem::updateMaskWithBrushStroke()
{
    if (!d_ptr->maskPainter.isValid()) {
        return;
    }

    QImage maskCopy = d_ptr->maskPainter.maskImage();
    QPainter painter(&maskCopy);
    QPen pen;
    pen.setWidth(d_ptr->cursorManager.brushSize());
    pen.setCapStyle(Qt::RoundCap);

    // 根据模式设置画笔图案
    if (d_ptr->editingMode == MaskEditingMode::Erase) {
        pen.setBrush(Qt::white);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
    } else {
        QPixmap brushPattern = d_ptr->cursorManager.currentBrushPattern();
        if (!brushPattern.isNull()) {
            pen.setBrush(brushPattern);
        }
    }

    painter.setPen(pen);

    const QPointF lastLocal = mapFromScene(d_ptr->maskPainter.lastBrushPosition());
    const QPointF currLocal = mapFromScene(d_ptr->maskPainter.currentBrushPosition());
    painter.drawLine(lastLocal, currLocal);

    d_ptr->maskPainter.setMaskImage(maskCopy);
    d_ptr->maskPainter.setLastBrushPosition(d_ptr->maskPainter.currentBrushPosition());
    update();
}

void GraphicsPixmapItem::updateCursor()
{
    setCursor(d_ptr->cursorManager.createCursorForMode(d_ptr->editingMode));
    d_ptr->cursorManager.setNeedsUpdate(false);
}

void GraphicsPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mousePressEvent(event);

    if (event->button() != Qt::LeftButton) {
        return;
    }
    d_ptr->maskPainter.setLastBrushPosition(event->scenePos());
    d_ptr->maskPainter.setCurrentBrushPosition(d_ptr->maskPainter.lastBrushPosition());
}

void GraphicsPixmapItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseMoveEvent(event);

    if (event->buttons() & Qt::LeftButton) {
        d_ptr->maskPainter.setCurrentBrushPosition(event->scenePos());
        if (d_ptr->editingMode != MaskEditingMode::Normal
            && d_ptr->maskPainter.currentBrushPosition() != d_ptr->maskPainter.lastBrushPosition()) {
            updateMaskWithBrushStroke();
        }
    }
}

void GraphicsPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseReleaseEvent(event);

    if (event->button() != Qt::LeftButton) {
        return;
    }
    d_ptr->maskPainter.setLastBrushPosition(QPointF());
    d_ptr->maskPainter.setCurrentBrushPosition(QPointF());
}

void GraphicsPixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mouseDoubleClickEvent(event);

    if (event->button() != Qt::LeftButton) {
        return;
    }

    d_ptr->maskPainter.setLastBrushPosition(QPointF());
    d_ptr->maskPainter.setCurrentBrushPosition(QPointF());
}

void GraphicsPixmapItem::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_UNUSED(option)
    QStyleOptionGraphicsItem op(*option);
    // op.initFrom(widget);
    op.state = QStyle::State_None;

    QGraphicsPixmapItem::paint(painter, &op, widget);

    if (!d_ptr->maskPainter.isValid()) {
        return;
    }

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(d_ptr->maskPainter.opacity());
    painter->drawImage(0, 0, d_ptr->maskPainter.maskImage());

    // 更新光标缩放因子
    d_ptr->cursorManager.updateScaleFactor(painter);
    if (d_ptr->cursorManager.needsUpdate()) {
        updateCursor();
    }
}

} // namespace Graphics
