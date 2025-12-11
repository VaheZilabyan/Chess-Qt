#ifndef COLORSELECTIONDIALOG_H
#define COLORSELECTIONDIALOG_H

#include <QDialog>

class QPushButton;

class ColorSelectionDialog : public QDialog {
    Q_OBJECT

public:
    enum ColorChoice { White, Black };
    explicit ColorSelectionDialog(QWidget *parent = nullptr);
    ColorChoice getChoice() const;

private:
    ColorChoice choice;
};

#endif // COLORSELECTIONDIALOG_H
