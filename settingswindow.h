#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QComboBox>
#include <QCheckBox>

class SettingsWindow : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
public slots:
    void accept();
private:
    QComboBox *boardComboBox;
    QCheckBox *soundCheckBox;
};


#endif // SETTINGSWINDOW_H
