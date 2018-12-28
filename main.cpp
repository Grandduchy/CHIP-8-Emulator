#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    Game game;
    game.setFile(":/ROMs/ROMs/PONG");
    game.show();
    return a.exec();
}
