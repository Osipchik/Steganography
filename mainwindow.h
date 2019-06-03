#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

#include "steganography.h"
#include <QGraphicsDropShadowEffect>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

     QImage *image;
     Steganography *stg;

private slots:
    void on_AddTextButton_clicked();
    void on_ReadButton_clicked();

    void on_CancelButton_clicked();
    void on_textEdit_textChanged();

    void widget_visible(bool visible);
    QString number(QString str);

    void on_open_triggered();
    void on_save_as_triggered();

    void on_open_doc_triggered();
    void on_save_a_doc_triggered();

private:
    Ui::MainWindow *ui;
    virtual void resizeEvent(QResizeEvent *event);
};
#endif // MAINWINDOW_H
