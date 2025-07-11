#include "settingswindow.h"
#include "board.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    resize(320, 240);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    // Board selection
    QLabel *changeBoardLabel = new QLabel("Change Board");
    boardComboBox = new QComboBox();
    boardComboBox->addItem(QIcon(":/board_colors/img/white-gray.png"), "White/Gray");
    boardComboBox->addItem(QIcon(":/board_colors/img/white_brown.png"), "White/Brown");
    boardComboBox->addItem(QIcon(":/board_colors/img/white-blue.png"), "White/Blue");
    boardComboBox->addItem(QIcon(":/board_colors/img/white-green.png"), "White/Green");
    boardComboBox->addItem(QIcon(":/board_colors/img/gray-darkgray.png"), "Gray/Dark Gray");
    QHBoxLayout *boardLayout = new QHBoxLayout;
    boardLayout->addWidget(changeBoardLabel);
    boardLayout->addWidget(boardComboBox);

    // Piece selection
    QLabel *changePieceLabel = new QLabel("Change Piece");
    QComboBox *pieceComboBox = new QComboBox();
    QHBoxLayout *pieceLayout = new QHBoxLayout;
    pieceLayout->addWidget(changePieceLabel);
    pieceLayout->addWidget(pieceComboBox);

    // On/Off sound
    soundCheckBox = new QCheckBox("Sound");
    soundCheckBox->setChecked(true);

    // Buttons (OK / Cancel)
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel); //ok->save
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SettingsWindow::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SettingsWindow::reject);

    // Add all to main layout
    mainLayout->addLayout(boardLayout);
    mainLayout->addLayout(pieceLayout);
    mainLayout->addWidget(soundCheckBox);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
}

void SettingsWindow::accept()
{
    QPair<QColor, QColor> boardColor;
    QString color = boardComboBox->currentText();
    qDebug() << color;
    if (color == "White/Gray") {
        boardColor = {Qt::white, Qt::gray};
    } else if (color == "White/Brown") {
        boardColor = {"#ffe4b8", "#c48564"};
    } else if (color == "White/Blue") {
        boardColor = {Qt::white, "#55add4"};
    } else if (color == "White/Green") {
        boardColor = {"#eeeed2", "#769656"};
    } else if (color == "Gray/Dark Gray") {
        boardColor = {"#c6c7c5", "#8f918d"};
    }

    Board::getInstance()->setBoardColor(boardColor);
    qDebug() << "acceped";
}

