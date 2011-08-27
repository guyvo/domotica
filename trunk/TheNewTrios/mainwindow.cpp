#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QCoreApplication>
#include <QDial>

#include "TriosModel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i=0 ; i < 24 ; i++){
        ui->selectLights->addItem(gpCLightNames[i]);
    }

    TriosInitBuffer();
    TriosSetEthernet("192.168.1.10",6969);
    TriosSendGetBuffer();
    ui->stackedLights->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#ifdef Q_OS_SYMBIAN
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::on_pushButton_clicked()
{
    for (int i=0 ; i<24 ; i++){
        gTriosLights[i].lights.value = 0;
    }

    TriosSendPostBuffer();
}

void MainWindow::on_backBuuton_clicked()
{
    ui->stackedLights->setCurrentIndex(0);
}

void MainWindow::on_selectLights_itemClicked(QListWidgetItem *item)
{
    TriosSendGetBuffer();
    index = ui->selectLights->row(item);
    ui->stackedLights->setCurrentIndex(1);
}

void MainWindow::on_stackedLights_currentChanged(int arg1)
{
    if (arg1==1){

        QDial * dialvalue =  ui->stackedLights->widget(arg1)->findChild<QDial *>("lightValue");
        QLCDNumber * lcdvalue =  ui->stackedLights->widget(arg1)->findChild<QLCDNumber *>("lcdValue");

        dialvalue->setValue(gTriosLights[index].lights.value);
        lcdvalue->display(gTriosLights[index].lights.value);

        QDial * dialmin =  ui->stackedLights->widget(arg1)->findChild<QDial *>("lightMin");
        QLCDNumber * lcdmin =  ui->stackedLights->widget(arg1)->findChild<QLCDNumber *>("minValue");

        dialmin->setValue(gTriosLights[index].lights.minimum);
        lcdmin->display(gTriosLights[index].lights.minimum);

        QDial * dialmax =  ui->stackedLights->widget(arg1)->findChild<QDial *>("lightMax");
        QLCDNumber * lcdmax=  ui->stackedLights->widget(arg1)->findChild<QLCDNumber *>("maxValue");

        dialmax->setValue(gTriosLights[index].lights.maximum);
        lcdmax->display(gTriosLights[index].lights.maximum);

        QDial * dialstep =  ui->stackedLights->widget(arg1)->findChild<QDial *>("lightStep");
        QLCDNumber * lcdstep=  ui->stackedLights->widget(arg1)->findChild<QLCDNumber *>("stepValue");

        dialstep->setValue(gTriosLights[index].lights.step);
        lcdstep->display(gTriosLights[index].lights.step);

        QLabel * name =  ui->stackedLights->widget(arg1)->findChild<QLabel *>("lightName");
        name->setText(gpCLightNames[index]);

    }

}


void MainWindow::on_lightValue_sliderReleased()
{
    QDial * dialvalue =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QDial *>("lightValue");
    QLCDNumber * lcdvalue =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("lcdValue");

    lcdvalue->display(dialvalue->value());
    gTriosLights[index].lights.value = dialvalue->value();
    TriosSendPostBuffer();
}

void MainWindow::on_lightMin_sliderReleased()
{
    QDial * dialmin =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QDial *>("lightMin");
    QLCDNumber * lcdmin =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("minValue");

    lcdmin->display(dialmin->value());
    gTriosLights[index].lights.minimum = dialmin->value();
    TriosSendPostBuffer();
}

void MainWindow::on_lightMax_sliderReleased()
{
    QDial * dialmax =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QDial *>("lightMax");
    QLCDNumber * lcdmax=  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("maxValue");

    lcdmax->display(dialmax->value());
    gTriosLights[index].lights.maximum = dialmax->value();
    TriosSendPostBuffer();
}

void MainWindow::on_lightStep_sliderReleased()
{
    QDial * dialstep =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QDial *>("lightStep");
    QLCDNumber * lcdstep=  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("stepValue");

    lcdstep->display(dialstep->value());
    gTriosLights[index].lights.step = dialstep->value();
    TriosSendPostBuffer();
}

void MainWindow::on_back_clicked()
{
   ui->stackedLights->setCurrentIndex(0);
}


void MainWindow::on_statusButton_clicked()
{
    ui->stackedLights->setCurrentIndex(2);

    TriosSendGetBuffer();

    QLCDNumber * temp1 =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("temp1");
    temp1->display(gTriosCortexes.cortexes[0].temperature);

    QLCDNumber * temp2 =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("temp2");
    temp2->display(gTriosCortexes.cortexes[1].temperature);

    QLCDNumber * temp3 =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("temp3");
    temp3->display(gTriosCortexes.cortexes[2].temperature);

    QLCDNumber * temp4 =  ui->stackedLights->widget(ui->stackedLights->currentIndex())->findChild<QLCDNumber *>("temp4");
    temp4->display(gTriosCortexes.cortexes[3].temperature);
}
