#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void fileButtonPressed();
    void playButtonPressed();
signals:
    void playGame(const QString&);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HPP
