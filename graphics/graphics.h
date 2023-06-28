#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QCursor>
#include <QLineF>
#include <QPolygonF>

namespace Graphics {

auto calculateCircle(const QPolygonF& pts, QPointF& center, double& radius) -> bool;
auto curorFromAngle(double angle) -> QCursor;
auto boundingFromLine(const QLineF& line, double margin) -> QPolygonF;
auto distance(QPointF pos, QPointF center) -> double;
auto ConvertTo360(double angle) -> double;

}

#endif // GRAPHICS_H
