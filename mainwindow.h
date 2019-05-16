#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QToolBar>
#include <QToolButton>

#include "steganography.h"

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

     QToolBar *ToolBar;
     QToolButton *OpenButton, *SaveButton;

private slots:
    void on_OpenButton_clicked();
    void on_SaveButton_clicked();

    void on_AddTextButton_clicked();
    void on_ReadButton_clicked();

    void on_CancelButton_clicked();

    void text_edit_visible(bool visible);

    void on_textEdit_textChanged();

private:
    Ui::MainWindow *ui;
    virtual void resizeEvent(QResizeEvent *event);
};
#endif // MAINWINDOW_H
