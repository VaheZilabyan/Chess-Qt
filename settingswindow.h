#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>
#include <QTimeEdit>

class SettingsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
public slots:
    void accept();
private:
    QComboBox *boardComboBox;
    QComboBox *stockfishLevel;
    QCheckBox *soundCheckBox;
    QTimeEdit *timeEdit;
};


#endif // SETTINGSWINDOW_H
