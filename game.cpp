#include "game.hpp"
#include "ui_game.h"
#include <Chip8.hpp>
#include <iostream>
#include <QPainter>
#include <QTextStream>


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
    try {
        emulator.loadGame(file.toStdString());
    } catch(const std::exception& e) {
        std::cerr << "Error loading game into emulator, Error : " << e.what() << std::endl;
    }
}

void Game::paintEvent(QPaintEvent *) {
    paint();

}

void Game::timerEvent(QTimerEvent *) {
    runCycle();
}

void Game::runCycle() {
    emulator.emulateCycle();
   // if (emulator.isDrawFlag())
        repaint();
}


void Game::runGame(const QString& fileName) {
    Q_UNUSED(fileName);
}

void Game::resetgame() {

}

void Game::paint() {
    QTextStream cout(stdout);
    const std::array<std::array<uint8_t, WIDTH>, HEIGHT>& pixels = emulator.pixels;
    for(auto yIter = pixels.cbegin(); yIter != pixels.cend(); yIter++) {
        for (auto xIter = yIter->cbegin(); xIter != yIter->cend(); xIter++) {
            auto y = static_cast<int>(std::distance(yIter, pixels.cend()));
            auto x = static_cast<int>(std::distance(xIter, yIter->cend()));
            uint8_t type = *xIter; // 0 -> white, 1 -> black

            QPainter painter(this);
            QColor color = type == 1 ? Qt::white : Qt::black;
            painter.setBrush(color);
            painter.drawRect(x, y, 10, 10);

        }
    }
}
