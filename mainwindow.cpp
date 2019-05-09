#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stg = nullptr;
    image = nullptr;
    ui->textEdit->setVisible(false);
    ui->CancelButton->setVisible(false);

    ToolBar = new QToolBar("Toolbar");
    OpenButton = new QToolButton(this);
    SaveButton = new QToolButton(this);

    OpenButton->setText("Открыть");
    SaveButton->setText("Сохранить");

    ToolBar->addWidget(OpenButton);
    ToolBar->addWidget(SaveButton);
    addToolBar(Qt::TopToolBarArea, ToolBar);

    connect(OpenButton, SIGNAL(clicked()), this, SLOT (on_OpenButton_clicked()));
    connect(SaveButton, SIGNAL(clicked()), this, SLOT (on_SaveButton_clicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(image)
    {
        int pixWidth  = ui->labelImage->width();
        int pixHeight = ui->labelImage->height();
        ui->labelImage->setPixmap(QPixmap::fromImage(*image).scaled(pixWidth, pixHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void MainWindow::on_OpenButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8("Открыть файл"),
                                                          QDir::currentPath(),
                                                          "Images (*.png *.bmp *.jpg);");

    if(fileName == "") return;
    image = new QImage(fileName);

    if(stg) delete stg;
    stg = new Steganography();

    ui->statusBar->showMessage("Количество символов: " + stg->GetSize(*image) + "/" +
                               QString::number(image->height() * image->width() - image->height()-1));

    int pixWidth  = ui->labelImage->width();
    int pixHeight = ui->labelImage->height();
    ui->labelImage->setPixmap(QPixmap::fromImage(*image).scaled(pixWidth, pixHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::on_SaveButton_clicked()
{
    if(!image)
    {
        QMessageBox::warning(this, "Warning", "Выберите изображение");
        return;
    }
    stg->encode(ui->textEdit->toPlainText(), *image);

    ui->textEdit->setVisible(false);
    ui->CancelButton->setVisible(false);

    ui->TextButton->setVisible(true);
    ui->labelImage->setVisible(true);
    ui->ReadButton->setVisible(true);
    image->save(QFileDialog::getSaveFileName(this, QString::fromUtf8("Сохранить файл"),
                                                   QDir::currentPath(),
                                                   "Images (*.png);\n"
                                                   "Images (*.bmp);"));
}

void MainWindow::on_TextButton_clicked()
{
    if(!image)
    {
        QMessageBox::warning(this, "", "Выберите изображение");
        return;
    }
    ui->labelImage->setVisible(false);
    ui->TextButton->setVisible(false);
    ui->ReadButton->setVisible(false);

    ui->CancelButton->setVisible(true);
    ui->textEdit->setVisible(true);
}

void MainWindow::on_ReadButton_clicked()
{
    if(!image)
    {
        QMessageBox::warning(this, "Warning", "Нет изображения для чтения");
        return;
    }
    int mSize = stg->GetSize(*image).toInt();
    if(!mSize)
    {
        QMessageBox::warning(this, "Warning", "Изображение не содержить текст");
        return;
    }

    ui->labelImage->setVisible(false);
    ui->TextButton->setVisible(false);
    ui->ReadButton->setVisible(false);

    ui->CancelButton->setVisible(true);
    ui->textEdit->setVisible(true);

    ui->textEdit->clear();
    ui->textEdit->setText(stg->decode(*image, mSize));
}

void MainWindow::on_textEdit_textChanged()
{
    int mSize = ui->textEdit->toPlainText().length();
    int maxSize = image->height() * image->width() - image->height()-1;

    if(mSize > maxSize)
    {
        QMessageBox::warning(this, "Warning", "Максимальное колчество символов");
        QString message = ui->textEdit->toPlainText().remove(maxSize-1, mSize - maxSize);
        ui->textEdit->clear();
        ui->textEdit->setText(message);
        mSize = maxSize;
    }
    ui->statusBar->showMessage("Количество символов: " + QString::number(mSize) + "/" +
                               QString::number(image->height() * image->width() - image->height()-1));
}

void MainWindow::on_CancelButton_clicked()
{
    ui->textEdit->setVisible(false);
    ui->CancelButton->setVisible(false);

    ui->TextButton->setVisible(true);
    ui->labelImage->setVisible(true);
    ui->ReadButton->setVisible(true);
}
