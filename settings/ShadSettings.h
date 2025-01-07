#include <QDialog>

namespace Ui {
class ShadSettings;
}

class ShadSettings : public QDialog {
    Q_OBJECT
public:
    explicit ShadSettings(QWidget* parent = nullptr);
    ~ShadSettings();

    bool eventFilter(QObject* obj, QEvent* event) override;
    void updateNoteTextEdit(const QString& groupName);

private:
    void LoadValuesFromConfig();
    void OnCursorStateChanged(int index);
    void SaveSettings();
    void SetDefaults();
    void UpdateShad();
    void InstallUpdate();
    void DownloadUpdate(const QString& downloadUrl);
    void UpdateDialog();

    std::unique_ptr<Ui::ShadSettings> ui;

    std::map<std::string, int> languages;
    QString defaultTextEdit;
    int initialHeight;
};

void PathToQString(QString& result, const std::filesystem::path& path);

const QVector<int> languageIndexes = {21, 23, 14, 6, 18, 1, 12, 22, 2, 4,  25, 24, 29, 5,  0, 9,
                                      15, 16, 17, 7, 26, 8, 11, 20, 3, 13, 27, 10, 19, 30, 28};

const QStringList languageNames = {"Arabic",
                                   "Czech",
                                   "Danish",
                                   "Dutch",
                                   "English (United Kingdom)",
                                   "English (United States)",
                                   "Finnish",
                                   "French (Canada)",
                                   "French (France)",
                                   "German",
                                   "Greek",
                                   "Hungarian",
                                   "Indonesian",
                                   "Italian",
                                   "Japanese",
                                   "Korean",
                                   "Norwegian (Bokmaal)",
                                   "Polish",
                                   "Portuguese (Brazil)",
                                   "Portuguese (Portugal)",
                                   "Romanian",
                                   "Russian",
                                   "Simplified Chinese",
                                   "Spanish (Latin America)",
                                   "Spanish (Spain)",
                                   "Swedish",
                                   "Thai",
                                   "Traditional Chinese",
                                   "Turkish",
                                   "Ukrainian",
                                   "Vietnamese"};

// TODO: map for settings descriptions
