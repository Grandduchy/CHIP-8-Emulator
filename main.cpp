#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    Game game;
    game.setFile("INVADERS");
    game.show();
    return a.exec();
}
