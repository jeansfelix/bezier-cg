#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QFile>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool consomeCurvaBezier(QFile *file);
    void interfaceUpdate();
    void geraBezier();
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    QPoint transformaParaQPoint(QVector2D vec2d);


    Ui::MainWindow *ui;
    QImage buffer;

public slots:
    void abrirArquivo();
};

#endif // MAINWINDOW_H
