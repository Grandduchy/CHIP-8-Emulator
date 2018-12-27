#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    Game game;
    game.setFile("/home/challenger/Development/CHIP-8-Emulator/INVADERS");
    game.show();
    return a.exec();
}
