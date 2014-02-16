#include <QApplication>
#include "mainwindow.hpp"
#include <QGraphicsScene>
#include <QGraphicsView>
#include "pong.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow main;
    main.show();
    
    return a.exec();
}
