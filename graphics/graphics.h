#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QCursor>
#include <QLineF>
#include <QPolygonF>

namespace Graphics {

bool calculateCircle(const QPolygonF& pts, QPointF& center, double& radius);
QCursor curorFromAngle(double angle);
QPolygonF boundingFromLine(const QLineF& line, double margin);
double distance(QPointF pos, QPointF center);
double ConvertTo360(double angle);

}

#endif // GRAPHICS_H
