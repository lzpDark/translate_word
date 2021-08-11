#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &appId, const QString &securityKey, QWidget *parent = nullptr);
    ~MainWindow();


public slots:
    void slot_translate();
private:
    void setValue(const QString& v);
private:
    Ui::MainWindow *ui;
    QString mAppId;
    const QString mSecurityKey;
};
#endif // MAINWINDOW_H
