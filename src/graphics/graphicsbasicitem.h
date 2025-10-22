#pragma once

#include "graphics_global.h"

#include <QAbstractGraphicsShapeItem>
#include <QList>

namespace Graphics {

class GeometryCache;

class GRAPHICS_EXPORT GraphicsBasicItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    enum Shape : int { LINE = 1, RECT, ROUNDEDRECT, ROTATEDRECT, CIRCLE, POLYGON, RING, ARC };
    Q_ENUM(Shape)

    enum class MouseRegion : int { NoSelection, AnchorPoint, EdgeArea, EntireShape };

    explicit GraphicsBasicItem(QGraphicsItem *parent = nullptr);
    ~GraphicsBasicItem() override;

    [[nodiscard]] virtual auto isValid() const -> bool;
    [[nodiscard]] auto type() const -> int override = 0;
    [[nodiscard]] auto boundingRect() const -> QRectF override;
    [[nodiscard]] auto shape() const -> QPainterPath override;

    void setName(const QString &name);
    [[nodiscard]] auto name() const -> QString;

    void setMargin(double margin);
    [[nodiscard]] auto margin() const -> double;

    void setItemEditable(bool editable);
    bool itemEditable() const;

    void setShowShape(bool show);
    bool showShape() const;

    void setShowBoundingRect(bool show);
    bool showBoundingRect() const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    void setMyCursor(const QPointF &center, const QPointF &pos);

    void setClickedPos(const QPointF &p);
    [[nodiscard]] auto clickedPos() const -> QPointF;

    [[nodiscard]] auto mouseRegion() const -> MouseRegion;
    void setMouseRegion(const MouseRegion);

    [[nodiscard]] auto hoveredDotIndex() const -> int;

    GeometryCache *geometryCache() const;

    virtual void drawContent(QPainter *painter);
    void drawAnchor(QPainter *painter);
    void drawBoundingRect(QPainter *painter);
    void drawShape(QPainter *painter);

    virtual void pointsChanged(const QPolygonF &ply) = 0;
    virtual void updateHoverPreview(const QPointF &scenePos) = 0;
    virtual MouseRegion detectEdgeRegion(const QPointF &scenePos);
    virtual void handleMouseMoveEvent(const QPointF &scenePos,
                                      const QPointF &clickedPos,
                                      const QPointF delta)
        = 0;

private:
    class GraphicsBasicItemPrivate;
    QScopedPointer<GraphicsBasicItemPrivate> d_ptr;
};

using GraphicsItemList = QList<GraphicsBasicItem *>;

} // namespace Graphics
