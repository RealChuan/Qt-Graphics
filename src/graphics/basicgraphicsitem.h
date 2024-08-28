#ifndef BASICGRAPHICSITEM_H
#define BASICGRAPHICSITEM_H

#include <QGraphicsPixmapItem>
#include <QObject>

#include "graphics_global.h"

namespace Graphics {

#define LineColor QColor(57, 163, 255)

class GRAPHICS_EXPORT BasicGraphicsItem : public QObject, public QAbstractGraphicsShapeItem
{
    Q_OBJECT
public:
    // Why does add UserType not draw
    enum Shape {
        LINE = /*UserType +*/ 1,
        RECT,
        ROUNDEDRECT,
        ROTATEDRECT,
        CIRCLE,
        POLYGON,
        RING,
        ARC
    };
    Q_ENUM(Shape)

    enum MouseRegion { DotRegion, All, Edge, None };

    explicit BasicGraphicsItem(QGraphicsItem *parent = nullptr);
    ~BasicGraphicsItem() override;

    [[nodiscard]] virtual auto isValid() const -> bool = 0;
    [[nodiscard]] auto type() const -> int override = 0;
    [[nodiscard]] auto boundingRect() const -> QRectF override;

    void setName(const QString &name);
    [[nodiscard]] auto name() const -> QString;

    void setMargin(double m11);
    [[nodiscard]] auto margin() const -> double;

    void setItemEditable(bool editable);

    void setShowBoundingRect(bool show);
    bool showBoundingRect() const;

protected:
    //QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void setCache(const QPolygonF &);
    [[nodiscard]] auto cache() const -> QPolygonF;

    void setClickedPos(const QPointF &p);
    [[nodiscard]] auto clickedPos() const -> QPointF;

    [[nodiscard]] auto mouseRegion() const -> MouseRegion;
    void setMouseRegion(const MouseRegion);

    [[nodiscard]] auto hoveredDotIndex() const -> int;

    void drawAnchor(QPainter *painter);
    void drawBoundingRect(QPainter *painter);

    void setMyCursor(const QPointF &center, const QPointF &pos);

private:
    class BasicGraphicsItemPrivate;
    QScopedPointer<BasicGraphicsItemPrivate> d_ptr;
};

} // namespace Graphics

#endif // BASICGRAPHICSITEM_H
