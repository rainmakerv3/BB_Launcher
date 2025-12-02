#pragma once

#include <QObject>
#include <QPushButton>

class QRightClickButton : public QPushButton {
    Q_OBJECT

public:
    explicit QRightClickButton(QWidget *parent = 0);

public slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void rightClicked();
    void leftClicked();
};
