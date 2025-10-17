#pragma once

#include "graphicsutils.hpp"

#include <QPainterPath>

namespace Graphics {

class GeometryCache
{
    Q_DISABLE_COPY_MOVE(GeometryCache)
public:
    GeometryCache() = default;
    ~GeometryCache() = default;

    void setAnchorPoints(const QPolygonF &pts) { setAnchorPoints(pts, {}, {}); }

    void setAnchorPoints(const QPolygonF &pts, const QRectF &boundingRect, const QPainterPath &shape)
    {
        m_anchorPoints = pts;
        m_boundingRect = boundingRect;
        m_shape = shape;
        m_boundingRectDirty = true;
        m_shapeDirty = true;
    }

    QPolygonF anchorPoints() const { return m_anchorPoints; }

    QRectF boundingRect(double margin, double penWidth, double expandSize)
    {
        auto addLen = calculateExpandSize(margin, penWidth, expandSize);
        if (addLen != m_rectAddLen || m_boundingRectDirty) {
            m_boundingRectDirty = false;
            m_rectAddLen = addLen;
            m_cacheBoundingRect = m_boundingRect.adjusted(-addLen, -addLen, addLen, addLen);
        }
        return m_cacheBoundingRect;
    }

    QPainterPath shape(double margin, double penWidth, double expandSize)
    {
        auto addLen = calculateExpandSize(margin, penWidth, expandSize);
        if (addLen != m_shapeAddLen || m_shapeDirty) {
            m_shapeDirty = false;
            m_shapeAddLen = addLen;
            m_cachedShape = m_shape;
            m_cachedShape = Utils::expandAndUnitePath(m_cachedShape, addLen);
        }
        return m_cachedShape;
    }

    bool isValid() const { return !m_boundingRect.isNull() && !m_anchorPoints.isEmpty(); }
    void invalidate() { m_boundingRect = QRectF(); }

private:
    double calculateExpandSize(double margin, double penWidth, double expandSize) const
    {
        return std::max({margin * 0.5, penWidth, expandSize});
    }

    QPolygonF m_anchorPoints; // 用于交互的锚点
    QRectF m_boundingRect;    // 边界矩形
    QPainterPath m_shape;     // 路径

    bool m_boundingRectDirty = true; // 矩形是否需要重新计算
    double m_rectAddLen = 0;         // 矩形扩展长度
    QRectF m_cacheBoundingRect;      // 缓存矩形

    bool m_shapeDirty = true;   // 路径是否需要重新计算
    double m_shapeAddLen = 0;   // 路径扩展长度
    QPainterPath m_cachedShape; // 缓存路径
};

using GeometryCachePtr = QScopedPointer<GeometryCache>;

} // namespace Graphics
