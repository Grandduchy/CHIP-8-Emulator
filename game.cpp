#include "game.hpp"
#include "ui_game.h"
#include <Chip8.hpp>


Game::Game(QWidget *parent) :
    QWidget(parent), ui(new Ui::Game) {
    ui->setupUi(this);
    this->setFixedSize(QSize(WIDTH * 10, HEIGHT * 10));
    timer.start(100, this);
    emulator.initalize();
}

Game::~Game() {
    delete ui;
}

void Game::setFile(const QString& file) {
    this->filepath = file;
    emulator.loadGame(file.toStdString());
}

void Game::paintEvent(QPaintEvent *) {
    paint();

}

void Game::timerEvent(QTimerEvent *) {
    runCycle();
}

void Game::runCycle() {
    emulator.emulateCycle();
    if (emulator.isDrawFlag())
        repaint();
}


void Game::runGame(const QString& fileName) {
    Q_UNUSED(fileName);
}

void Game::resetgame() {

}

void Game::paint() {

}
