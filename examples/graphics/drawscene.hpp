#ifndef DRAWSCENE_HPP
#define DRAWSCENE_HPP

#include <QGraphicsScene>

namespace Graphics {
class GraphicsTextItem;
}

class DrawScene : public QGraphicsScene
{
    Q_OBJECT
public:
    using QGraphicsScene::QGraphicsScene;

    void setDrawText(bool drawText) { m_drawText = drawText; }
    void setTextColor(const QColor &color);

signals:
    void deleteItem();
    void itemSelected(QGraphicsItem *item);

private slots:
    void editorLostFocus(Graphics::GraphicsTextItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    bool m_drawText = false;
    QColor m_textColor;
};

#endif // DRAWSCENE_HPP
