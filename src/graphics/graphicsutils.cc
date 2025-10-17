#include "graphicsutils.hpp"

#include <QtMath>

namespace Graphics::Utils {

auto calculateCircle(const QPolygonF &pts, QPointF &center, double &radius) -> bool
{
    if (pts.size() < 3)
        return false;

    double X1(0), Y1(0), X2(0), Y2(0), X3(0), Y3(0);
    double X1Y1(0), X1Y2(0), X2Y1(0);

    for (int i = 0; i < pts.size(); i++) {
        double x = pts[i].x();
        double y = pts[i].y();
        X1 = X1 + x;
        Y1 = Y1 + y;
        X2 = X2 + x * x;
        Y2 = Y2 + y * y;
        X3 = X3 + x * x * x;
        Y3 = Y3 + y * y * y;
        X1Y1 = X1Y1 + x * y;
        X1Y2 = X1Y2 + x * y * y;
        X2Y1 = X2Y1 + x * x * y;
    }

    double C, D, E, G, H, N;
    double a, b, c;
    N = pts.size();
    C = N * X2 - X1 * X1;
    D = N * X1Y1 - X1 * Y1;
    E = N * X3 + N * X1Y2 - (X2 + Y2) * X1;
    G = N * Y2 - Y1 * Y1;
    H = N * X2Y1 + N * Y3 - (X2 + Y2) * Y1;
    a = (H * D - E * G) / (C * G - D * D);
    b = (H * C - E * D) / (D * D - G * C);
    c = -(a * X1 + b * Y1 + X2 + Y2) / N;

    double A, B;
    A = a / (-2);
    B = b / (-2);
    radius = qSqrt(a * a + b * b - 4 * c) / 2;
    center = QPointF(A, B);

    return true;
}

auto boundingFromLine(const QLineF &line, double margin) -> QPolygonF
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

auto createBoundingRect(const QPolygonF &ply, double margin) -> QRectF
{
    double addLen = margin / 2;
    return ply.boundingRect().adjusted(-addLen, -addLen, addLen, addLen);
}

auto normalizeAngle(double angle) -> double
{
    angle = std::fmod(angle, 360.0);
    return angle < 0.0 ? angle + 360.0 : angle;
}

auto distance(QPointF pos, QPointF center) -> double
{
    return QLineF(center, pos).length();
}

auto cursorForDirection(double angle) -> QCursor
{
    angle = normalizeAngle(angle);
    const int sector = static_cast<int>((angle + 22.5) / 45.0) % 8;
    static constexpr std::array<Qt::CursorShape, 8> cursors = {Qt::SizeVerCursor,
                                                               Qt::SizeFDiagCursor,
                                                               Qt::SizeHorCursor,
                                                               Qt::SizeBDiagCursor,
                                                               Qt::SizeVerCursor,
                                                               Qt::SizeFDiagCursor,
                                                               Qt::SizeHorCursor,
                                                               Qt::SizeBDiagCursor};

    return cursors[sector];
}

} // namespace Graphics::Utils
