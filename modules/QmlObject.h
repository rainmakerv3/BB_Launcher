// mycppobject.h
#include <QObject>

class MyCppObject : public QObject {
    Q_OBJECT
public:
    explicit MyCppObject(QObject* parent = nullptr);

public slots:
    void handleQmlSignal(const QString& message); // The slot to receive the QML signal
};
