#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <stack>

QT_BEGIN_NAMESPACE
namespace Ui { class TextEditor; }
QT_END_NAMESPACE

class TextEditor : public QMainWindow {
    Q_OBJECT

public:
    explicit TextEditor(QWidget *parent = nullptr);
    ~TextEditor();

private slots:
    void undo();
    void redo();
    void search();
    void replace();
    void openFile();
    void saveFile();
    void printFile();
    void autosave();
    void showFind();
    void highlightAllOccurrences(const QString &searchText);
    void clearHighlights();
    void increaseFontSize();
    void decreaseFontSize();
    void resetFontSize();

private:
    Ui::TextEditor *ui;
    QTimer *autosaveTimer;
    QString currentFilePath;
    std::stack<QString> undoStack;
    std::stack<QString> redoStack;

    void saveState();
    QString encryptText(const QString &text, int shift);
    QString decryptText(const QString &encryptedText, int shift);
};

#endif // TEXTEDITOR_H
