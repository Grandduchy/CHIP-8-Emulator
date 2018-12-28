#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPushButton>
#include <QFileInfo>
#include <QString>
#include <QMessageBox>
#include <game.hpp>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), game() {
    ui->setupUi(this);
    connect(ui->fileButton, &QPushButton::pressed, this, &MainWindow::fileButtonPressed);
    connect(ui->playButton, &QPushButton::pressed, this, &MainWindow::playButtonPressed);
    connect(ui->InvadersButton, &QPushButton::pressed, this, &MainWindow::preInstalledButton);
    connect(ui->PongButton, &QPushButton::pressed, this, &MainWindow::preInstalledButton);
    connect(ui->TetrisButton, &QPushButton::pressed, this, &MainWindow::preInstalledButton);
    connect(ui->UFOButton, &QPushButton::pressed, this, &MainWindow::preInstalledButton);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::fileButtonPressed() {
    // Get the location of the file
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.exec();
    QStringList files = dialog.selectedFiles();
    // Only consider the first file selected.
    QString file;
    if (!files.isEmpty())
        file = files[0];

    // Change the string in the text edit
    ui->lineEdit->setText(file);

}

void MainWindow::playButtonPressed() {
    QString fileName = ui->lineEdit->text();
    QFileInfo fileInfo(fileName);
    if (fileInfo.exists() && fileInfo.isFile()) {

        if (game == nullptr)
            game = new Game();
        game->resetgame();
        game->show();
        game->setFile(fileName);
    }
    else {
        QMessageBox* box = new QMessageBox(this);
        box->setText("File does not exist or is not a file.");
        box->exec();
    }
}

void MainWindow::preInstalledButton() {
    QString fName;
    if (ui->InvadersButton->isDown())
        fName = ":/ROMs/ROMs/INVADERS";
    if (ui->PongButton->isDown())
        fName = ":/ROMs/ROMs/PONG";
    if (ui->TetrisButton->isDown())
        fName = ":/ROMs/ROMs/TETRIS";
    if (ui->UFOButton->isDown())
        fName = ":/ROMs/ROMs/UFO";

    if (game == nullptr)
        game = new Game();
    game->resetgame();
    game->show();
    game->setFile(fName);

}
