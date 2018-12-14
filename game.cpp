#include "game.hpp"
#include "ui_game.h"
#include <Chip8.hpp>


Game::Game(QWidget *parent) :
    QWidget(parent), ui(new Ui::Game) {
    ui->setupUi(this);
    this->setFixedSize(QSize(WIDTH * 10, HEIGHT * 10));
    timer.start(100, this);
}

Game::~Game() {
    delete ui;
}

void Game::setFile(QString& str) {
    this->filepath = str;

}

void Game::paintEvent(QPaintEvent *) {
    paint();
}

void Game::timerEvent(QTimerEvent *) {
    repaint();
}

void Game::runGame(const QString&) {

}

void Game::resetgame() {

}

void Game::paint() {

}
