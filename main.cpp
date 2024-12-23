#include "texteditor.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    TextEditor editor;
    editor.setWindowTitle("Text Editor");
    editor.show();

    return app.exec();
}
