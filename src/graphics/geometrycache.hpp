#pragma once

#include <QPolygonF>
#include <QScopedPointer>

namespace Graphics {

class GeometryCache
{
    Q_DISABLE_COPY_MOVE(GeometryCache)
public:
    GeometryCache() = default;
    ~GeometryCache() = default;

    void setAnchorPoints(const QPolygonF &pts, const QRectF &boundingRect)
    {
        m_anchorPoints = pts;
        m_boundingRect = boundingRect;
        m_changed = true;
    }

    QPolygonF anchorPoints() const { return m_anchorPoints; }

    QRectF boundingRect(double margin, double penWidth)
    {
        auto addLen = qMax(margin / 2, penWidth);
        if (addLen == m_lastAddLen && !m_changed) {
            return m_lastBoundingRect;
        }
        m_changed = false;
        m_lastAddLen = addLen;
        m_lastBoundingRect = m_boundingRect.adjusted(-addLen, -addLen, addLen, addLen);
        return m_lastBoundingRect;
    }

    bool isValid() const { return !m_boundingRect.isNull() && !m_anchorPoints.isEmpty(); }
    void invalidate() { m_boundingRect = QRectF(); }

private:
    QPolygonF m_anchorPoints; // 用于交互的锚点
    QRectF m_boundingRect;    // 边界矩形

    bool m_changed = true;
    double m_lastAddLen = 0;   // 上一次的addLen
    QRectF m_lastBoundingRect; // 上一次的boundingRect
};

using GeometryCachePtr = QScopedPointer<GeometryCache>;

} // namespace Graphics
