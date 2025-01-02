#include "ModManager.h"
#include "ui_ModManager.h"

ModManager::ModManager(QWidget* parent) : QDialog(parent), ui(new Ui::ModManager) {
    ui->setupUi(this);
}

ModManager::~ModManager() {
    delete ui;
}
