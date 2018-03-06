#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->fileButton, &QPushButton::pressed, this, &MainWindow::fileButtonPressed);
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
    ui->textEdit->setText(file);

}

void MainWindow::playButtonPressed() {

}
