// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2017-2019 Alejandro Sirgo Rica & Contributors

#pragma once

#include <QWidget>

using Areas = QList<QRect>;

class QPropertyAnimation;

class SelectionWidget : public QWidget
{
    Q_OBJECT
public:
    enum SideType {
        NO_SIDE = 0,
        TOP_SIDE = 0b0001,
        BOTTOM_SIDE = 0b0010,
        RIGHT_SIDE = 0b0100,
        LEFT_SIDE = 0b1000,
        TOPLEFT_SIDE = TOP_SIDE | LEFT_SIDE,
        BOTTOMLEFT_SIDE = BOTTOM_SIDE | LEFT_SIDE,
        TOPRIGHT_SIDE = TOP_SIDE | RIGHT_SIDE,
        BOTTOMRIGHT_SIDE = BOTTOM_SIDE | RIGHT_SIDE,
        CENTER = 0b10000,
    };

    explicit SelectionWidget(QColor c, QWidget *parent = nullptr);

    [[nodiscard]] auto getMouseSide(const QPoint &mousePos) const -> SideType;
    Areas handlerAreas();

    void setGeometry(const QRect &r);
    [[nodiscard]] auto geometry() const -> QRect;
    [[nodiscard]] auto fullGeometry() const -> QRect;

    [[nodiscard]] auto rect() const -> QRect;

signals:
    void geometryChanged();

protected:
    auto eventFilter(QObject *obj, QEvent *event) -> bool override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

private:
    void parentMousePressEvent(QMouseEvent *event);
    void parentMouseReleaseEvent(QMouseEvent *event);
    void parentMouseMoveEvent(QMouseEvent *event);

    void updateAreas();
    void updateCursor();

    QColor m_color;
    QPoint m_areaOffset;
    QPoint m_handleOffset;

    QPoint m_dragStartPos;
    SideType m_activeSide;

    // naming convention for handles
    // T top, B bottom, R Right, L left
    // 2 letters: a corner
    // 1 letter: the handle on the middle of the corresponding side
    QRect m_TLHandle, m_TRHandle, m_BLHandle, m_BRHandle;
    QRect m_LHandle, m_THandle, m_RHandle, m_BHandle;

    QRect m_TLArea, m_TRArea, m_BLArea, m_BRArea;
    QRect m_LArea, m_TArea, m_RArea, m_BArea;
};
