#ifndef GAME_HPP
#define GAME_HPP

#include <QWidget>
#include <QTimer>
#include <Chip8.hpp>

namespace Ui {
class Game;
}

class Game : public QWidget {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game() override;

    void setFile(const QString& str);
    void resetgame();
protected:
    void paintEvent(QPaintEvent*) override;
    void timerEvent(QTimerEvent*) override;
    void paint();
    void keyPressEvent(QKeyEvent* key) override;

private slots:
    void runGame(const QString&);
private:
    void runCycle();
    void resetKeys();
    Ui::Game *ui;
    QTimer* timer;
    QString filepath;
    Chip8 emulator;
};

#endif // GAME_HPP
