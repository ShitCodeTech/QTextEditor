#include "texteditor.h"
#include "ui_texteditor.h"
#include <QFileDialog>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QInputDialog>
#include <QFile>
#include <QTextStream>
#include <qshortcut.h>

TextEditor::TextEditor(QWidget *parent)
: QMainWindow(parent), ui(new Ui::TextEditor) {
    ui->setupUi(this);

    // Set up keyboard shortcuts
    ui->actionFind->setShortcut(QKeySequence("Ctrl+F"));
    ui->actionOpen->setShortcut(QKeySequence("Ctrl+O"));
    ui->actionSave->setShortcut(QKeySequence("Ctrl+S"));
    ui->actionPrint->setShortcut(QKeySequence("Ctrl+P"));
    ui->actionUndo->setShortcut(QKeySequence("Ctrl+Z"));
    ui->actionRedo->setShortcut(QKeySequence("Ctrl+Y"));
    QShortcut *increaseShortcut = new QShortcut(QKeySequence("Ctrl++"), this);
    QShortcut *decreaseShortcut = new QShortcut(QKeySequence("Ctrl+-"), this);
    QShortcut *resetShortcut = new QShortcut(QKeySequence("Ctrl+0"), this);

    // Connect menu actions to slots
    connect(ui->actionOpen, &QAction::triggered, this, &TextEditor::openFile);
    connect(ui->actionSave, &QAction::triggered, this, &TextEditor::saveFile);
    connect(ui->actionPrint, &QAction::triggered, this, &TextEditor::printFile);
    connect(ui->actionExit, &QAction::triggered, this, &TextEditor::close);

    connect(ui->actionUndo, &QAction::triggered, this, &TextEditor::undo);
    connect(ui->actionRedo, &QAction::triggered, this, &TextEditor::redo);
    connect(ui->actionFind, &QAction::triggered, this, &TextEditor::showFind);

    connect(ui->searchButton, &QPushButton::clicked, this, &TextEditor::search);
    connect(ui->replaceButton, &QPushButton::clicked, this, &TextEditor::replace);

    connect(increaseShortcut, &QShortcut::activated, this, &TextEditor::increaseFontSize);
    connect(decreaseShortcut, &QShortcut::activated, this, &TextEditor::decreaseFontSize);
    connect(resetShortcut, &QShortcut::activated, this, &TextEditor::resetFontSize);

    // Initially hide the search/replace layout
    ui->searchReplaceWidget->hide();
}

TextEditor::~TextEditor() {
    delete ui;
}

void TextEditor::saveState() {
    undoStack.push(ui->textEdit->toPlainText());
    while (!redoStack.empty()) redoStack.pop();
}

void TextEditor::showFind() {
    bool isVisible = ui->searchReplaceWidget->isVisible();
    ui->searchReplaceWidget->setVisible(!isVisible);
}

void TextEditor::undo() {
    if (!undoStack.empty()) {
        redoStack.push(ui->textEdit->toPlainText());
        ui->textEdit->setPlainText(undoStack.top());
        undoStack.pop();
    }
}

void TextEditor::redo() {
    if (!redoStack.empty()) {
        undoStack.push(ui->textEdit->toPlainText());
        ui->textEdit->setPlainText(redoStack.top());
        redoStack.pop();
    }
}

void TextEditor::search() {
    QString searchText = ui->searchInput->text();

    if (searchText.isEmpty()) {
        return;
    }

    // bug fixed)))
    // highlightAllOccurrences(searchText);

    // Move to the first occurrence (optional)
    QTextCursor cursor = ui->textEdit->document()->find(searchText);
    if (!cursor.isNull()) {
        ui->textEdit->setTextCursor(cursor);
    } else {
        statusBar()->showMessage("No matches found", 2000);
    }
}

void TextEditor::replace() {
    saveState();

    QString searchTerm = ui->searchInput->text();
    QString replacement = ui->replaceInput->text();
    QString content = ui->textEdit->toPlainText();

    ui->textEdit->setPlainText(content.replace(searchTerm, replacement));
}

void TextEditor::openFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            ui->textEdit->setPlainText(in.readAll());
            file.close();
            currentFilePath = filePath;
        } else {
            QMessageBox::warning(this, "Error", "Could not open file.");
        }
    }
}

void TextEditor::saveFile() {
    if (currentFilePath.isEmpty()) {
        currentFilePath = QFileDialog::getSaveFileName(this, "Save File", "", "Text Files (*.txt);;All Files (*)");
    }

    if (!currentFilePath.isEmpty()) {
        QFile file(currentFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->textEdit->toPlainText();
            file.close();
        } else {
            QMessageBox::warning(this, "Error", "Could not save file.");
        }
    }
}

void TextEditor::printFile() {
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        ui->textEdit->print(&printer);
    }
}

void TextEditor::autosave() {
    if (!currentFilePath.isEmpty()) {
        QFile file(currentFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->textEdit->toPlainText();
            file.close();
        }
    }
}

void TextEditor::highlightAllOccurrences(const QString &searchText) {
    if (searchText.isEmpty()) {
        return;
    }

    // Clear previous highlights
    clearHighlights();

    QTextDocument *document = ui->textEdit->document();
    QTextCursor cursor(document);
    QTextCharFormat highlightFormat;
    highlightFormat.setBackground(Qt::yellow); // Highlight color

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = document->find(searchText, cursor); // Find the next occurrence
        if (!cursor.isNull()) {
            cursor.mergeCharFormat(highlightFormat); // Highlight the found text
        }
    }
}

void TextEditor::clearHighlights() {
    QTextDocument *document = ui->textEdit->document();
    QTextCursor cursor(document);
    QTextCharFormat defaultFormat;

    cursor.select(QTextCursor::Document); // Select the entire document
    cursor.setCharFormat(defaultFormat);  // Reset the formatting
}

void TextEditor::increaseFontSize() {
    QFont font = ui->textEdit->font();
    font.setPointSize(font.pointSize() + 1); // Увеличить размер шрифта
    ui->textEdit->setFont(font);
}

void TextEditor::decreaseFontSize() {
    QFont font = ui->textEdit->font();
    font.setPointSize(qMax(font.pointSize() - 1, 1)); // Уменьшить размер шрифта, но не ниже 1
    ui->textEdit->setFont(font);
}

void TextEditor::resetFontSize() {
    QFont font = ui->textEdit->font();
    font.setPointSize(12); // Установите размер шрифта по умолчанию
    ui->textEdit->setFont(font);
}
