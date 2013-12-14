#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QString>
#include <QFileDialog>
#include <QFile>
#include <QVector2D>

QVector<QVector2D> _controlPoints;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    buffer = QImage(800,600, QImage::Format_ARGB32);
    QPalette Pal(palette());
    // set black background
    Pal.setColor(QPalette::Background, Qt::white);
    this->setAutoFillBackground(true);
    this->setPalette(Pal);


    interfaceUpdate();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::interfaceUpdate(void)
{
    ui->labelPrincipal->setPixmap(QPixmap::fromImage(buffer));
    update();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    event->accept();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

QPoint MainWindow::transformaParaQPoint(QVector2D vec2d)
{
    return QPoint(vec2d.x(),vec2d.y());
}

void MainWindow::geraBezier()
{
    QPainter p(&buffer);

    int steps = 10000;

    QVector2D delta01 = (_controlPoints[1] - _controlPoints[0]) / steps,
              delta12 = (_controlPoints[2] - _controlPoints[1]) / steps,
              delta23 = (_controlPoints[3] - _controlPoints[2]) / steps,
              delta03, delta02, delta13,
              p01, p12, p23, p13, p02, p02anterior, p13anterior,
              p03, p03anterior;

    p02anterior = _controlPoints[0];
    p13anterior = _controlPoints[1];
    p03anterior = _controlPoints[0];

    for (int i = 1; i <= steps; i++){
        p01 = _controlPoints[0]+delta01*i;
        p12 = _controlPoints[1]+delta12*i;
        p23 = _controlPoints[2]+delta23*i;

        delta02 = (p12 - p01)/steps;
        delta13 = (p23 - p12)/steps;

        p02=p01 + delta02*i;
        p13=p12 + delta13*i;

        delta03 = (p13 - p02)/steps;

        p03=p02 + delta03*i;

//        p.drawLine(QPoint(p02anterior.x(), p02anterior.y()) , QPoint(p02.x(), p02.y()));

//        p.drawLine(QPoint(p13anterior.x(), p13anterior.y()) ,QPoint(p13.x() , p13.y()));

        p.drawLine(QPoint(p03anterior.x(),p03anterior.y())  ,QPoint(p03.x() , p03.y()));

        p02anterior = p02;
        p13anterior = p13;
        p03anterior = p03;
    }

   // Plotar retas de bezier
   // p.drawLine( transformaParaQPoint(_controlPoints[0]) , transformaParaQPoint(_controlPoints[1]));
   // p.drawLine(transformaParaQPoint(_controlPoints[1]),transformaParaQPoint(_controlPoints[2]));
   // p.drawLine(transformaParaQPoint(_controlPoints[2]),transformaParaQPoint(_controlPoints[3]));

    return;
}

/**
 * Função utilizada para consumir as 4 linhas seguintes de um arquivo.
 * Estas linhas devem ser os pontos de controle de uma curva de Bezier.
 * @brief MainWindow::consomeCurvaBezier
 * @param QFile file
 */
bool MainWindow::consomeCurvaBezier(QFile *file)
{
    for (int i = 0 ; i < 4 ; i++)
    {
        QString linha = file->readLine().constData();

        QStringList splitter = linha.split(" ");

        if (splitter.length() > 2) {
            return false;
        }

        _controlPoints.append(QVector2D(splitter.first().toInt() , splitter.last().toInt() ));

        qDebug() << linha;
    }

    geraBezier();
    _controlPoints.clear();
    return true;
}

/**
 * Função que é utilizada para abrir e consumir um arquivo.
 * O arquivo deve conter o formato necessário para ser consumido.
 * @brief MainWindow::abrirArquivo
 */
void MainWindow::abrirArquivo()
{
    bool arquivoCorreto = true;

    /*
     *Caso você esteja no windows use "C:/".
     *Caso você esteja no linux use "/home/"
    */
    QString arquivo = QFileDialog::getOpenFileName(this
                                                   ,tr("Abrir Arquivo")
                                                   ,"C:/"
                                                   ,tr("Arquivo Texto (*.txt)")
                                                   );
    if (arquivo.isNull())
    {
        qDebug() << "Não foi especificado um arquivo de entrada.";
        return;
    }

    QFile file(arquivo);
    file.open(QIODevice::ReadOnly);

    while (!file.atEnd())
    {
        // Pula linha por causa do "B" ou "H"

        QString variacao = file.readLine().constData();

        if(variacao.compare("B"))
        {
            arquivoCorreto = consomeCurvaBezier(&file);
        }
        if (variacao.compare("F")) {
            //arquivoCorreto = consomeCurvaHermit(file);
        }
        if (variacao.isEmpty() || variacao.isNull() || !arquivoCorreto)
        {
            qDebug() << "Não foi especificado um arquivo no formato válido.";
            return;
        }
    }
    file.close();
    interfaceUpdate();
}
