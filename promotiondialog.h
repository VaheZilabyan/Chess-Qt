#ifndef PROMOTIONDIALOG_H
#define PROMOTIONDIALOG_H

#include <QDialog>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>

#include "chesspiece.h"

class PromotionDialog : public QDialog {
    Q_OBJECT

public:
    explicit PromotionDialog(ChessPiece::Color color, QWidget *parent = nullptr);
    ChessPiece::PieceType getSelectedPieceType() const;
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QGraphicsScene *scene;
    ChessPiece::PieceType m_selectedType;
};

#endif // PROMOTIONDIALOG_H
