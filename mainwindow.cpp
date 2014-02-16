#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <iostream>
#include <QDebug>
#include <QResizeEvent>

#define WIDTH 640
#define HEIGHT 480

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->mScene = new Pong(QRect(0, 0, WIDTH, HEIGHT), this);
    this->mView = new QGraphicsView(this->mScene);
    this->setCentralWidget(this->mView);

    this->adjustSize();

    connect(ui->actionSet_Background, SIGNAL(triggered()), mScene, SLOT(slotBGChange()));
    connect(ui->actionChange_Ball, SIGNAL(triggered()), mScene, SLOT(slotBallChange()));
    connect(ui->actionNew, SIGNAL(triggered()), mScene, SLOT(slotNewGame()));
    connect(ui->actionStart, SIGNAL(triggered()), mScene, SLOT(slotStartGame()));
    connect(ui->actionStop, SIGNAL(triggered()), mScene, SLOT(slotStopGame()));
    connect(ui->actionSingle, SIGNAL(triggered()), mScene, SLOT(slotSingleMode()));
    connect(ui->actionSerial, SIGNAL(triggered()), mScene, SLOT(slotSerialMode()));
    connect(ui->actionSettings, SIGNAL(triggered()), mScene, SLOT(slotSettings()));
}

void MainWindow::resizeEvent(QResizeEvent * e)
{
    QMainWindow::resizeEvent(e);

    if (e->oldSize().width() == -1) return;

    mView->setSceneRect(0, 0, this->size().width(), this->size().height());

    mScene->resize(this->size().width(), this->size().height());

    this->mView->fitInView(0, 0, this->size().width(), this->size().height());
}

MainWindow::~MainWindow()
{
    delete ui;
}
