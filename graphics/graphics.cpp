#include "graphics.h"

#include <QtMath>

bool Graphics::calculateCircle(const QPolygonF &pts, QPointF &center, double &radius)
{
    if(pts.size() < 3)
        return false;

    double X1(0), Y1(0), X2(0), Y2(0), X3(0), Y3(0);
    double X1Y1(0), X1Y2(0), X2Y1(0);

    for (int i = 0; i < pts.size(); i++){
        double x = pts[i].x();
        double y = pts[i].y();
        X1 = X1 +  x;
        Y1 = Y1 +  y;
        X2 = X2 +  x* x;
        Y2 = Y2 +  y* y;
        X3 = X3 +  x* x* x;
        Y3 = Y3 +  y* y* y;
        X1Y1 = X1Y1 +  x* y;
        X1Y2 = X1Y2 +  x* y* y;
        X2Y1 = X2Y1 +  x* x* y;
    }

    double C, D, E, G, H, N;
    double a, b, c;
    N = pts.size();
    C = N*X2 - X1*X1;
    D = N*X1Y1 - X1*Y1;
    E = N*X3 + N*X1Y2 - (X2 + Y2)*X1;
    G = N*Y2 - Y1*Y1;
    H = N*X2Y1 + N*Y3 - (X2 + Y2)*Y1;
    a = (H*D - E*G) / (C*G - D*D);
    b = (H*C - E*D) / (D*D - G*C);
    c = -(a*X1 + b*Y1 + X2 + Y2) / N;

    double A, B;
    A = a / (-2);
    B = b / (-2);
    radius = qSqrt(a*a + b*b - 4 * c) / 2;
    center = QPointF(A, B);

    return true;
}

QCursor Graphics::curorFromAngle(double angle)
{
    if(angle >= 0 && angle < 15)
        return Qt::SizeVerCursor;
    else if(angle >= 15 && angle < 75)
        return Qt::SizeFDiagCursor;
    else if(angle >= 75 &&angle < 105)
        return Qt::SizeHorCursor;
    else if(angle >= 105 && angle < 165)
        return Qt::SizeBDiagCursor;
    else if(angle >= 165 && angle < 195)
        return Qt::SizeVerCursor;
    else if(angle >= 195 && angle < 255)
        return Qt::SizeFDiagCursor;
    else if(angle >= 255 && angle < 285)
        return Qt::SizeHorCursor;
    else if(angle >= 285 && angle < 345)
        return Qt::SizeBDiagCursor;
    else
        return Qt::SizeVerCursor;
}

QPolygonF Graphics::boundingFromLine(const QLineF &line, double margin)
{
    QPolygonF ply;
    QPointF p1 = line.p1();
    QPointF p2 = line.p2();

    QLineF dl = line.normalVector();
    QPointF dp = QPointF(dl.dx() * margin, dl.dy() * margin) / dl.length();
    ply << p1 + dp;
    ply << p1 - dp;
    ply << p2 - dp;
    ply << p2 + dp;
    return ply;
}

double Graphics::distance(QPointF pos, QPointF center)
{
    return QLineF(center, pos).length();
}

double Graphics::ConvertTo360(double angle)
{
    if(angle >= 0 && angle < 360)
        return angle;
    else if(angle < 0)
        return ConvertTo360(angle + 360);
    else
        return ConvertTo360(angle - 360);
}
