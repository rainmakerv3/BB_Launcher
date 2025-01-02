#include <QDialog>

namespace Ui {
class ModManager;
}

class ModManager : public QDialog {
    Q_OBJECT

public:
    explicit ModManager(QWidget* parent = nullptr);
    ~ModManager();

private:
    Ui::ModManager* ui;
};
