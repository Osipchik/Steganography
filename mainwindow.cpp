#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    stg = nullptr;
    image = nullptr;

    ui->textEdit->setVisible(false);
    ui->CancelButton->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete stg;
    delete ui;
}


void MainWindow::resizeEvent(QResizeEvent*)
{
    if (image)
    {
        ui->labelImage->setPixmap(QPixmap::fromImage(*image).scaled(ui->labelImage->width(),
                                                                    ui->labelImage->height(),
                                                                    Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}


//метод для отображения или скрытия элементов интерфейса,
//чтобы не дублировать код
void MainWindow::widget_visible(bool visible)
{
    if (visible)
    {
        ui->labelImage->setVisible(false);
        ui->AddTextButton->setVisible(false);
        ui->ReadButton->setVisible(false);

        ui->CancelButton->setVisible(true);
        ui->textEdit->setVisible(true);
    }
    else
    {
        ui->textEdit->setVisible(false);
        ui->CancelButton->setVisible(false);

        ui->AddTextButton->setVisible(true);
        ui->labelImage->setVisible(true);
        ui->ReadButton->setVisible(true);
    }
}

//метод для разделения разрядов числа
QString MainWindow::number(QString str)
{
    for (int i = str.length() - 3; i > 0; i -= 3)
    {
        str.insert(i, ' ');
    }
    return str;
}

void MainWindow::on_AddTextButton_clicked()
{
    if (!image)
    {
        QMessageBox::warning(this, "", "Выберите изображение");
        return;
    }
    widget_visible(true);
    ui->textEdit->clear();
}

void MainWindow::on_ReadButton_clicked()
{
    if (!image)
    {
        QMessageBox::warning(this, "Warning", "Нет изображения для чтения");
        return;
    }
    int mSize = stg->GetSize().toInt();
    if (!mSize)
    {
        QMessageBox::warning(this, "Warning", "Изображение не содержить текст");
        return;
    }

    widget_visible(true);
    ui->textEdit->clear();
    ui->textEdit->setText(stg->decode());
}

void MainWindow::on_textEdit_textChanged()
{
    int maxSize = stg->GetMaxSize();

    QString mSize = ui->textEdit->toPlainText();
    int size = mSize.length();
    if (size > maxSize)
    {
        QMessageBox::warning(this, "Warning", "Максимальное колчество символов");
        QString message = ui->textEdit->toPlainText().remove(maxSize-1, size - maxSize);
        ui->textEdit->clear();
        ui->textEdit->setText(message);
        size = maxSize;
    }
    ui->statusBar->showMessage("Количество символов: " + number(QString::number(size)) + "/" +
                               number(QString::number(stg->GetMaxSize())));
}

void MainWindow::on_CancelButton_clicked()
{
    widget_visible(false);
}

void MainWindow::on_open_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8("Открыть файл"),
                                                          QDir::currentPath(),
                                                          "(*.png *.bmp *.jpg *.dng *.raw);");

    if (fileName == "") return;

    if (image) delete image;
    image = new QImage(fileName);

    if (stg) delete stg;
    stg = new Steganography(*image);

    QString mSize = stg->GetSize();
    ui->statusBar->showMessage("Количество символов: " + number(mSize) + "/" +
                               number(QString::number(stg->GetMaxSize())));
    widget_visible(false);
    int pixWidth  = ui->labelImage->width();
    int pixHeight = ui->labelImage->height();
    ui->labelImage->setPixmap(QPixmap::fromImage(*image).scaled(pixWidth, pixHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::on_save_as_triggered()
{
    if (!image)
    {
        QMessageBox::warning(this, "Warning", "Выберите изображение");
        return;
    }
    stg->encode(ui->textEdit->toPlainText());

    widget_visible(false);
    image->save(QFileDialog::getSaveFileName(this, QString::fromUtf8("Сохранить файл"),
                                                   QDir::currentPath(),
                                                   "(*.png);\n"
                                                   "(*.bmp);"));
}

void MainWindow::on_open_doc_triggered()
{
    if(!image)
    {
        QMessageBox::warning(this, "warning", "Чтобы открыть документ выберите изображение");
        return;
    }
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8("Сохранить файл"),
                                                    QDir::currentPath(),
                                                    "(*.txt *.doc);");
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        widget_visible(true);
        ui->textEdit->setText(in.readAll());
        file.close();
    }
}

void MainWindow::on_save_a_doc_triggered()
{
    if (!image)
    {
        QMessageBox::warning(this, "warning", "Чтобы открыть документ выберите изображение");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, QString::fromUtf8("Сохранить файл"),
                                                    QDir::currentPath(),
                                                    "(*.txt)\n"
                                                    "(*.doc);");
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream << ui->textEdit->toPlainText();
        file.close();
    }
}


