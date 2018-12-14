#ifndef GAME_HPP
#define GAME_HPP

#include <QWidget>
#include <QBasicTimer>
#include <Chip8.hpp>

namespace Ui {
class Game;
}

class Game : public QWidget {
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game() override;

    void setFile(QString& str);
    void resetgame();
protected:
    void paintEvent(QPaintEvent*) override;
    void timerEvent(QTimerEvent*) override;
    void paint();

private slots:
    void runGame(const QString&);
private:
    Ui::Game *ui;
    QBasicTimer timer;
    QString filepath;
    Chip8 emulator;
};

#endif // GAME_HPP
