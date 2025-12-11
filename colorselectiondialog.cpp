#include "ColorSelectionDialog.h"
#include <QVBoxLayout>
#include <QPushButton>

ColorSelectionDialog::ColorSelectionDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Choose your side");
    setModal(true);
    setFixedSize(160, 100);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QPushButton *whiteBtn = new QPushButton("Play as White");
    QPushButton *blackBtn = new QPushButton("Play as Black");

    layout->addWidget(whiteBtn);
    layout->addWidget(blackBtn);

    connect(whiteBtn, &QPushButton::clicked, this, [=]() {
        choice = White;
        accept();
    });

    connect(blackBtn, &QPushButton::clicked, this, [=]() {
        choice = Black;
        accept();
    });

    setLayout(layout);
}

ColorSelectionDialog::ColorChoice ColorSelectionDialog::getChoice() const {
    return choice;
}
