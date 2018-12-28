#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "game.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void fileButtonPressed();
    void playButtonPressed();
    void preInstalledButton();
private:
    Ui::MainWindow *ui;
    Game* game;
};

#endif // MAINWINDOW_HPP
