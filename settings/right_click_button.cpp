#include <QMouseEvent>

#include "right_click_button.h"

QRightClickButton::QRightClickButton(QWidget *parent) :
    QPushButton(parent) {
}

void QRightClickButton::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton) {
        emit rightClicked();
    } else {
        QPushButton::mousePressEvent(e);
    }
}
