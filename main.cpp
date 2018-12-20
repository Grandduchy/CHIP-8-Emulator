#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    Game game;
    game.setFile("E:\\Development\\CHIP-8-Emulator\\PONG");
    game.show();
    return a.exec();
}
