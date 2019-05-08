#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPixmap>

#include <QBitArray>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    image = nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString directory = "C:\\Users\\ASUS\\OneDrive\\C++\\bmp.BMP";
    QString message = "smth";
    int height, width;

    image = new QImage(directory);
    height = image->height();
    width = image->width();

    ui->Size->setText(QString::number(height * width));

    int pixWidth  = ui->label->width();
    int pixHeight = ui->label->height();
    ui->label->setPixmap(QPixmap::fromImage(*image).scaled(pixWidth, pixHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));


    int count = 0;
    QRgb pixel;

    QByteArray byte = message.toUtf8();
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            pixel = image->pixel(x, y);

            QBitArray bits;
            bits.resize(8);

            int gpos = 0;
            //foreach(char ch, )
        }
    }
}
