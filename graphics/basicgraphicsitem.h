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
    enum Shape { LINE = /*UserType +*/ 1, RECT, CIRCLE, POLYGON, RING, ARC, ROTATEDRECT };
    Q_ENUM(Shape)

    enum MouseRegion { DotRegion, All, Edge, None };

    explicit BasicGraphicsItem(QGraphicsItem *parent = nullptr);
    ~BasicGraphicsItem() override;

    virtual bool isValid() const = 0;
    virtual int type() const override = 0;
    virtual QRectF boundingRect() const override;

    void setName(const QString &name);
    QString name() const;

    void setMargin(double m11);
    double margin() const;

    void setItemEditable(bool editable);

protected:
    //QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void setCache(const QPolygonF &);
    QPolygonF cache() const;

    void setClickedPos(const QPointF &p);
    QPointF clickedPos() const;

    MouseRegion mouseRegion() const;
    void setMouseRegion(const MouseRegion);

    int hoveredDotIndex() const;

    void drawAnchor(QPainter *painter);

    void setMyCursor(const QPointF &center, const QPointF &pos);

private:
    class BasicGraphicsItemPrivate;
    QScopedPointer<BasicGraphicsItemPrivate> d_ptr;
};

} // namespace Graphics

#endif // BASICGRAPHICSITEM_H
