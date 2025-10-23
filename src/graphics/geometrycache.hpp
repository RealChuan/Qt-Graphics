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

    void setControlPoints(const QPolygonF &points) { setGeometryData(points, {}, {}); }

    void setGeometryData(const QPolygonF &points, const QRectF &bounds, const QPainterPath &path)
    {
        if (m_controlPoints == points && m_bounds == bounds && m_basePath == path) {
            return;
        }

        m_controlPoints = points;
        m_bounds = bounds;
        m_basePath = path;
        m_cacheDirty = true;
    }

    QPolygonF controlPoints() const { return m_controlPoints; }

    QRectF visualBoundingRect(double margin, double penWidth, double expandAmount)
    {
        updateCacheIfNeeded(margin, penWidth, expandAmount);
        return m_cachedBounds;
    }

    QPainterPath visualShape(double margin, double penWidth, double expandAmount)
    {
        updateCacheIfNeeded(margin, penWidth, expandAmount);
        return m_cachedPath;
    }

    bool hasValidGeometry() const { return !m_bounds.isNull() && !m_controlPoints.isEmpty(); }

private:
    void updateCacheIfNeeded(double margin, double penWidth, double expandAmount)
    {
        const double expansion = calculateTotalExpansion(margin, penWidth, expandAmount);
        if (!m_cacheDirty && qFuzzyCompare(m_cachedExpansion, expansion)) {
            return; // 缓存仍然有效
        }

        m_cacheDirty = false;
        m_cachedExpansion = expansion;
        m_cachedPath = m_basePath;
        m_cachedPath = Utils::expandAndUnitePath(m_cachedPath, expansion);
        m_cachedBounds = m_cachedPath.controlPointRect();
    }

    static double calculateTotalExpansion(double margin, double penWidth, double expandAmount)
    {
        return std::max({margin * 0.5, penWidth, expandAmount});
    }

    QPolygonF m_controlPoints; // 用于交互的锚点
    QRectF m_bounds;           // 边界矩形
    QPainterPath m_basePath;   // 路径

    bool m_cacheDirty = true;     // 路径是否需要重新计算
    double m_cachedExpansion = 0; // 路径扩展长度
    QPainterPath m_cachedPath;    // 缓存路径
    QRectF m_cachedBounds;        // 缓存矩形
};

using GeometryCachePtr = QScopedPointer<GeometryCache>;

} // namespace Graphics
