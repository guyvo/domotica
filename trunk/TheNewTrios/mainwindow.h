#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QListWidgetItem>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void on_pushButton_clicked();

    void on_backBuuton_clicked();

    void on_selectLights_itemClicked(QListWidgetItem *item);

    void on_stackedLights_currentChanged(int arg1);


    void on_lightValue_sliderReleased();

    void on_lightMin_sliderReleased();

    void on_lightMax_sliderReleased();

    void on_lightStep_sliderReleased();

    void on_back_clicked();


    void on_statusButton_clicked();

private:
    Ui::MainWindow *ui;
    int index;
};

#endif // MAINWINDOW_H
