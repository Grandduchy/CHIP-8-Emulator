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
        //game->setFile(fileName);
        game->resetgame();
        game->show();
    }
    else {
        QMessageBox* box = new QMessageBox(this);
        box->setText("File does not exist or is not a file.");
        box->exec();
    }
}
