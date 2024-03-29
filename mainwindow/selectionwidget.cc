// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#include "selectionwidget.hpp"

#include <QtWidgets>

#define MARGIN (m_THandle.width())

SelectionWidget::SelectionWidget(QColor c, QWidget *parent)
    : QWidget(parent)
    , m_color(c)
    , m_activeSide(NO_SIDE)
{
    // prevents this widget from consuming CaptureToolButton mouse events
    setAttribute(Qt::WA_TransparentForMouseEvents);
    parent->installEventFilter(this);

    int sideVal = 16;
    int handleSide = sideVal / 2;
    const QRect areaRect(0, 0, sideVal, sideVal);

    const QRect handleRect(0, 0, handleSide, handleSide);
    m_TLHandle = m_TRHandle = m_BLHandle = m_BRHandle = m_LHandle = m_THandle = m_RHandle
        = m_BHandle = handleRect;
    m_TLArea = m_TRArea = m_BLArea = m_BRArea = areaRect;

    m_areaOffset = QPoint(-sideVal / 2, -sideVal / 2);
    m_handleOffset = QPoint(-handleSide / 2, -handleSide / 2);
}

/**
 * @brief Get the side where the mouse cursor is.
 * @param mousePos Mouse cursor position relative to the parent widget.
 */
SelectionWidget::SideType SelectionWidget::getMouseSide(const QPoint &mousePos) const
{
    if (!isVisible()) {
        return NO_SIDE;
    }
    QPoint localPos = mapFromParent(mousePos);
    if (m_TLArea.contains(localPos)) {
        return TOPLEFT_SIDE;
    } else if (m_TRArea.contains(localPos)) {
        return TOPRIGHT_SIDE;
    } else if (m_BLArea.contains(localPos)) {
        return BOTTOMLEFT_SIDE;
    } else if (m_BRArea.contains(localPos)) {
        return BOTTOMRIGHT_SIDE;
    } else if (m_LArea.contains(localPos)) {
        return LEFT_SIDE;
    } else if (m_TArea.contains(localPos)) {
        return TOP_SIDE;
    } else if (m_RArea.contains(localPos)) {
        return RIGHT_SIDE;
    } else if (m_BArea.contains(localPos)) {
        return BOTTOM_SIDE;
    } else if (rect().contains(localPos)) {
        return CENTER;
    } else {
        return NO_SIDE;
    }
}

QVector<QRect> SelectionWidget::handlerAreas()
{
    QVector<QRect> areas;
    areas << m_TLHandle << m_TRHandle << m_BLHandle << m_BRHandle << m_LHandle << m_THandle
          << m_RHandle << m_BHandle;
    return areas;
}

// helper function
SelectionWidget::SideType getProperSide(SelectionWidget::SideType side, const QRect &r)
{
    using SideType = SelectionWidget::SideType;
    int intSide = side;
    if (r.right() < r.left()) {
        intSide ^= SideType::LEFT_SIDE;
        intSide ^= SideType::RIGHT_SIDE;
    }
    if (r.bottom() < r.top()) {
        intSide ^= SideType::TOP_SIDE;
        intSide ^= SideType::BOTTOM_SIDE;
    }

    return (SideType) intSide;
}

void SelectionWidget::setGeometry(const QRect &r)
{
    QWidget::setGeometry(r + QMargins(MARGIN, MARGIN, MARGIN, MARGIN));
    updateCursor();
}

QRect SelectionWidget::geometry() const
{
    return QWidget::geometry() - QMargins(MARGIN, MARGIN, MARGIN, MARGIN);
}

QRect SelectionWidget::fullGeometry() const
{
    return QWidget::geometry();
}

QRect SelectionWidget::rect() const
{
    return QWidget::rect() - QMargins(MARGIN, MARGIN, MARGIN, MARGIN);
}

bool SelectionWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        parentMouseReleaseEvent(static_cast<QMouseEvent *>(event));
    } else if (event->type() == QEvent::MouseButtonPress) {
        parentMousePressEvent(static_cast<QMouseEvent *>(event));
    } else if (event->type() == QEvent::MouseMove) {
        parentMouseMoveEvent(static_cast<QMouseEvent *>(event));
    }
    return false;
}

void SelectionWidget::parentMousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        return;
    }

    m_dragStartPos = event->pos();
    m_activeSide = getMouseSide(event->pos());
}

void SelectionWidget::parentMouseReleaseEvent(QMouseEvent *event)
{
    // released outside of the selection area
    if (!getMouseSide(event->pos())) {
        hide();
    }

    m_activeSide = NO_SIDE;
    updateCursor();
}

void SelectionWidget::parentMouseMoveEvent(QMouseEvent *event)
{
    updateCursor();

    if (event->buttons() != Qt::LeftButton) {
        return;
    }

    SideType mouseSide = m_activeSide;
    if (!m_activeSide) {
        mouseSide = getMouseSide(event->pos());
    }

    QPoint pos;

    if (!isVisible() || !mouseSide) {
        show();
        m_activeSide = TOPLEFT_SIDE;
        pos = m_dragStartPos;
        setGeometry({pos, pos});
    } else {
        pos = event->pos();
    }

    auto geom = geometry();
    float aspectRatio = (float) geom.width() / (float) geom.height();
    bool symmetryMod = qApp->keyboardModifiers() & Qt::ShiftModifier;
    bool preserveAspect = qApp->keyboardModifiers() & Qt::ControlModifier;

    QPoint newTopLeft = geom.topLeft(), newBottomRight = geom.bottomRight();
    int oldLeft = newTopLeft.rx(), oldRight = newBottomRight.rx(), oldTop = newTopLeft.ry(),
        oldBottom = newBottomRight.ry();
    int &newLeft = newTopLeft.rx(), &newRight = newBottomRight.rx(), &newTop = newTopLeft.ry(),
        &newBottom = newBottomRight.ry();
    switch (mouseSide) {
    case TOPLEFT_SIDE:
        if (m_activeSide) {
            if (preserveAspect) {
                if ((float) (oldRight - pos.x()) / (float) (oldBottom - pos.y()) > aspectRatio) {
                    /* width longer than expected width, hence increase
                         * height to compensate for the aspect ratio */
                    newLeft = pos.x();
                    newTop = oldBottom - (int) (((float) (oldRight - pos.x())) / aspectRatio);
                } else {
                    /* height longer than expected height, hence increase
                         * width to compensate for the aspect ratio */
                    newTop = pos.y();
                    newLeft = oldRight - (int) (((float) (oldBottom - pos.y())) * aspectRatio);
                }
            } else {
                newTopLeft = pos;
            }
        }
        break;
    case BOTTOMRIGHT_SIDE:
        if (m_activeSide) {
            if (preserveAspect) {
                if ((float) (pos.x() - oldLeft) / (float) (pos.y() - oldTop) > aspectRatio) {
                    newRight = pos.x();
                    newBottom = oldTop + (int) (((float) (pos.x() - oldLeft)) / aspectRatio);
                } else {
                    newBottom = pos.y();
                    newRight = oldLeft + (int) (((float) (pos.y() - oldTop)) * aspectRatio);
                }
            } else {
                newBottomRight = pos;
            }
        }
        break;
    case TOPRIGHT_SIDE:
        if (m_activeSide) {
            if (preserveAspect) {
                if ((float) (pos.x() - oldLeft) / (float) (oldBottom - pos.y()) > aspectRatio) {
                    newRight = pos.x();
                    newTop = oldBottom - (int) (((float) (pos.x() - oldLeft)) / aspectRatio);
                } else {
                    newTop = pos.y();
                    newRight = oldLeft + (int) (((float) (oldBottom - pos.y())) * aspectRatio);
                }
            } else {
                newTop = pos.y();
                newRight = pos.x();
            }
        }
        break;
    case BOTTOMLEFT_SIDE:
        if (m_activeSide) {
            if (preserveAspect) {
                if ((float) (oldRight - pos.x()) / (float) (pos.y() - oldTop) > aspectRatio) {
                    newLeft = pos.x();
                    newBottom = oldTop + (int) (((float) (oldRight - pos.x())) / aspectRatio);
                } else {
                    newBottom = pos.y();
                    newLeft = oldRight - (int) (((float) (pos.y() - oldTop)) * aspectRatio);
                }
            } else {
                newBottom = pos.y();
                newLeft = pos.x();
            }
        }
        break;
    case LEFT_SIDE:
        if (m_activeSide) {
            newLeft = pos.x();
            if (preserveAspect) {
                /* By default bottom edge moves when dragging sides, this
                     * behavior feels natural */
                newBottom = oldTop + (int) (((float) (oldRight - pos.x())) / aspectRatio);
            }
        }
        break;
    case RIGHT_SIDE:
        if (m_activeSide) {
            newRight = pos.x();
            if (preserveAspect) {
                newBottom = oldTop + (int) (((float) (pos.x() - oldLeft)) / aspectRatio);
            }
        }
        break;
    case TOP_SIDE:
        if (m_activeSide) {
            newTop = pos.y();
            if (preserveAspect) {
                /* By default right edge moves when dragging sides, this
                     * behavior feels natural */
                newRight = oldLeft + (int) (((float) (oldBottom - pos.y()) * aspectRatio));
            }
        }
        break;
    case BOTTOM_SIDE:
        if (m_activeSide) {
            newBottom = pos.y();
            if (preserveAspect) {
                newRight = oldLeft + (int) (((float) (pos.y() - oldTop) * aspectRatio));
            }
        }
        break;
    default:
        if (m_activeSide) {
            move(this->pos() + pos - m_dragStartPos);
            m_dragStartPos = pos;
            /* do nothing special in case of preserveAspect */
        }
        return;
    }
    // finalize geometry change
    if (m_activeSide) {
        if (symmetryMod) {
            QPoint deltaTopLeft = newTopLeft - geom.topLeft();
            QPoint deltaBottomRight = newBottomRight - geom.bottomRight();
            newTopLeft = geom.topLeft() + deltaTopLeft - deltaBottomRight;
            newBottomRight = geom.bottomRight() + deltaBottomRight - deltaTopLeft;
        }
        geom = {newTopLeft, newBottomRight};
        setGeometry(geom.normalized());
        m_activeSide = getProperSide(m_activeSide, geom);
    }
    m_dragStartPos = event->pos();
}

void SelectionWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(m_color);
    p.drawRect(rect() + QMargins(0, 0, -1, -1));
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(m_color);
    auto areas = handlerAreas();
    for (const auto &rect : std::as_const(areas)) {
        p.drawEllipse(rect);
    }
}

void SelectionWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateAreas();
}

void SelectionWidget::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    updateAreas();
}

void SelectionWidget::updateAreas()
{
    QRect r = rect();
    m_TLArea.moveTo(r.topLeft() + m_areaOffset);
    m_TRArea.moveTo(r.topRight() + m_areaOffset);
    m_BLArea.moveTo(r.bottomLeft() + m_areaOffset);
    m_BRArea.moveTo(r.bottomRight() + m_areaOffset);

    m_LArea = QRect(m_TLArea.bottomLeft(), m_BLArea.topRight());
    m_TArea = QRect(m_TLArea.topRight(), m_TRArea.bottomLeft());
    m_RArea = QRect(m_TRArea.bottomLeft(), m_BRArea.topRight());
    m_BArea = QRect(m_BLArea.topRight(), m_BRArea.bottomLeft());

    m_TLHandle.moveTo(m_TLArea.center() + m_handleOffset);
    m_BLHandle.moveTo(m_BLArea.center() + m_handleOffset);
    m_TRHandle.moveTo(m_TRArea.center() + m_handleOffset);
    m_BRHandle.moveTo(m_BRArea.center() + m_handleOffset);
    m_LHandle.moveTo(m_LArea.center() + m_handleOffset);
    m_THandle.moveTo(m_TArea.center() + m_handleOffset);
    m_RHandle.moveTo(m_RArea.center() + m_handleOffset);
    m_BHandle.moveTo(m_BArea.center() + m_handleOffset);

    emit geometryChanged();
}

void SelectionWidget::updateCursor()
{
    SideType mouseSide = m_activeSide;
    if (!m_activeSide) {
        mouseSide = getMouseSide(parentWidget()->mapFromGlobal(QCursor::pos()));
    }

    switch (mouseSide) {
    case TOPLEFT_SIDE: setCursor(Qt::SizeFDiagCursor); break;
    case BOTTOMRIGHT_SIDE: setCursor(Qt::SizeFDiagCursor); break;
    case TOPRIGHT_SIDE: setCursor(Qt::SizeBDiagCursor); break;
    case BOTTOMLEFT_SIDE: setCursor(Qt::SizeBDiagCursor); break;
    case LEFT_SIDE: setCursor(Qt::SizeHorCursor); break;
    case RIGHT_SIDE: setCursor(Qt::SizeHorCursor); break;
    case TOP_SIDE: setCursor(Qt::SizeVerCursor); break;
    case BOTTOM_SIDE: setCursor(Qt::SizeVerCursor); break;
    default:
        if (m_activeSide) {
            setCursor(Qt::ClosedHandCursor);
        } else {
            unsetCursor();
        }
        break;
    }
}
