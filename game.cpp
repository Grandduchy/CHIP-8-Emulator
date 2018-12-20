#include "game.hpp"
#include "ui_game.h"
#include <Chip8.hpp>
#include <iostream>
#include <QPainter>
#include <thread>
#include <QTextStream>
#include <QKeyEvent>


Game::Game(QWidget *parent) :
    QWidget(parent), ui(new Ui::Game) {
    ui->setupUi(this);
    this->timer = new QTimer(this);
    setFixedSize(QSize(WIDTH * 10, HEIGHT * 10));
    connect(timer, &QTimer::timeout, this, &Game::runCycle);
    timer->start(0);


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
    resetKeys();
    if (emulator.isDrawFlag()) {
        repaint();
        emulator.removeDrawFlag();
    }

}


void Game::runGame(const QString& fileName) {
    Q_UNUSED(fileName);
}

void Game::resetKeys() {
    std::fill(emulator.keys.begin(), emulator.keys.end(), 0);
}


void Game::keyPressEvent(QKeyEvent* key) {
    if (key->key() == Qt::Key_1)
        emulator.keys[0] = 1;
    if (key->key() == Qt::Key_2)
        emulator.keys[1] = 1;
    if (key->key() == Qt::Key_3)
        emulator.keys[2] = 1;
    if (key->key() == Qt::Key_4)
        emulator.keys[3] = 1;
    if (key->key() == Qt::Key_Q)
        emulator.keys[4] = 1;
    if (key->key() == Qt::Key_W)
        emulator.keys[5] = 1;
    if (key->key() == Qt::Key_E)
        emulator.keys[6] = 1;
    if (key->key() == Qt::Key_R)
        emulator.keys[7] = 1;
    if (key->key() == Qt::Key_A)
        emulator.keys[8] = 1;
    if (key->key() == Qt::Key_S)
        emulator.keys[9] = 1;
    if (key->key() == Qt::Key_D)
        emulator.keys[10] = 1;
    if (key->key() == Qt::Key_F)
        emulator.keys[11] = 1;
    if (key->key() == Qt::Key_Z)
        emulator.keys[12] = 1;
    if (key->key() == Qt::Key_X)
        emulator.keys[13] = 1;
    if (key->key() == Qt::Key_C)
        emulator.keys[14] = 1;
    if (key->key() == Qt::Key_V)
        emulator.keys[15] = 1;
}

void Game::paint() {
    QTextStream cout(stdout);
    QPainter painter(this);
    QColor color;
    const std::array<std::array<uint8_t, WIDTH>, HEIGHT>& pixels = emulator.pixels;
    painter.setPen(Qt::white);
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, WIDTH * 10, HEIGHT * 10);
    painter.setPen(Qt::black);
    painter.setBrush(Qt::black);
    int enlargement = 10;
    for(auto yIter = pixels.cbegin(); yIter != pixels.cend(); yIter++) {
        for (auto xIter = yIter->cbegin(); xIter != yIter->cend(); xIter++) {
            auto y = HEIGHT - static_cast<int>(std::distance(yIter, pixels.cend()));
            auto x = WIDTH - static_cast<int>(std::distance(xIter, yIter->cend()));
            uint8_t type = *xIter; // 0 -> white, 1 -> black
            if (type == 0) {
                painter.drawRect(x * enlargement, y * enlargement, enlargement, enlargement);
            }
        }
    }

}


void Game::resetgame() {

}
