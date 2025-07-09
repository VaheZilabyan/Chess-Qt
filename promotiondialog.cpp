#include "PromotionDialog.h"
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSvgItem>
#include <QMap>

PromotionDialog::PromotionDialog(ChessPiece::Color color, QWidget *parent)
    : QDialog(parent), m_selectedType(ChessPiece::PieceType::Queen)
{
    setWindowTitle("Promote Pawn");
    setFixedSize(340,100);

    scene = new QGraphicsScene(this);
    QGraphicsView *view = new QGraphicsView(scene);
    view->setFixedSize(320, 80);
    view->setSceneRect(0, 0, 315, 75);

    scene->installEventFilter(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(view);
    setLayout(layout);

    QMap<ChessPiece::PieceType, QString> typeToString = {
        {ChessPiece::PieceType::Queen, "queen"},
        {ChessPiece::PieceType::Rook, "rook"},
        {ChessPiece::PieceType::Bishop, "bishop"},
        {ChessPiece::PieceType::Knight, "knight"}
    };

    QString colorStr = (color == ChessPiece::Color::White) ? "w" : "b";

    int x = 0;
    for (auto type : typeToString.keys()) {
        QString svgPath = QString(":/svg_files/img/%1-%2.svg").arg(typeToString[type], colorStr);

        ChessPiece *item = new ChessPiece(type, color, svgPath);
        item->setData(0, static_cast<int>(type));  // Store the enum as int
        item->setFlags(QGraphicsItem::ItemIsSelectable);
        item->setPos(x, 0);
        item->setScale(0.5);  // Scale to fit view if needed
        scene->addItem(item);

        x += 82;
    }
}

ChessPiece::PieceType PromotionDialog::getSelectedPieceType() const {
    return m_selectedType;
}

bool PromotionDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneMousePress) {
        auto *mouseEvent = static_cast<QGraphicsSceneMouseEvent *>(event);
        QPointF scenePos = mouseEvent->scenePos();
        QList<QGraphicsItem *> itemsAtClick = scene->items(scenePos);

        for (QGraphicsItem *item : itemsAtClick) {
            if (item->data(0).isValid()) {
                m_selectedType = static_cast<ChessPiece::PieceType>(item->data(0).toInt());
                accept();
                return true;
            }
        }
    }

    return QDialog::eventFilter(watched, event);  // pass to base class
}
