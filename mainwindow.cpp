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

    OpenButton->setStyleSheet("QToolButton {"
                               "background-color: #212121;"
                               "border: 1px solid #505050;"
                               "border-color: #505050;"
                               "color: #FFFFFF;"
                               "text-align:center;"
                               "padding: 2px 4px;"
                               "vertical-align: middle;"
                               "text-decoration:none;}"

                              "QToolButton:hover {"
                               "border-color: rgba(82, 168, 236, 0.8);"
                               "background-color: #303030;"
                               "color: #52a8ec;"
                               "outline: 0 none;}");

    SaveButton->setStyleSheet("QToolButton {"
                              "background-color: #212121;"
                              "border: 1px solid #505050;"
                              "border-color: #505050;"
                              "color: #FFFFFF;"
                              "text-align:center;"
                              "padding: 2px 4px;"
                              "vertical-align: middle;"
                              "text-decoration:none;}"

                             "QToolButton:hover {"
                              "border-color: rgba(82, 168, 236, 0.8);"
                              "background-color: #303030;"
                              "color: #52a8ec;"
                              "outline: 0 none;}");

    ToolBar->addWidget(OpenButton);
    ToolBar->addWidget(SaveButton);
    ToolBar->setStyleSheet("QToolBar {"
                           "background-color: #191919;"
                           "border: 1px solid #141414;"
                           "padding: 2px;"
                           "font-weight: bold;}");

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
    if(image) delete image;
    image = new QImage(fileName);

    if(stg) delete stg;
    stg = new Steganography();

    QString mSize = stg->GetSize(*image);
    for(int i = mSize.length() - 3; i > 0; i -= 3)
    {
        mSize.insert(i, ' ');
    }
    ui->statusBar->showMessage("Количество символов: " + mSize + "/" +
                               QString::number(stg->GetMaxSize(*image)));

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
    ui->textEdit->setText(stg->decode(*image));
}

void MainWindow::on_textEdit_textChanged()
{
    int maxSize = stg->GetMaxSize(*image);

    QString mSize = ui->textEdit->toPlainText();
    int size = mSize.length();
    if(size > maxSize)
    {
        QMessageBox::warning(this, "Warning", "Максимальное колчество символов");
        QString message = ui->textEdit->toPlainText().remove(maxSize-1, size - maxSize);
        ui->textEdit->clear();
        ui->textEdit->setText(message);
        size = maxSize;
    }
    for(int i = mSize.length() - 3; i > 0; i -= 3)
    {
        mSize.insert(i, ' ');
    }
    ui->statusBar->showMessage("Количество символов: " + QString::number(size) + "/" +
                               QString::number(stg->GetMaxSize(*image)));
}

void MainWindow::on_CancelButton_clicked()
{
    ui->textEdit->setVisible(false);
    ui->CancelButton->setVisible(false);

    ui->TextButton->setVisible(true);
    ui->labelImage->setVisible(true);
    ui->ReadButton->setVisible(true);
}
