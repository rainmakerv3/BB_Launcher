
#include <QDebug>
#include "QmlObject.h"

MyCppObject::MyCppObject(QObject* parent) : QObject(parent) {}

void MyCppObject::handleQmlSignal(const QString& message) {
    qDebug() << "Received signal from QML with message:" << message;
}
