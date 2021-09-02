#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customfont.h"

#include <QMainWindow>
#include <QString>
#include <QTimer>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

    void readCheckBoxes(char* data);
    void writeCheckBoxes(char* data);

public slots:

    void selectCustomFontFile();
    void loadCustomFont(QString f);
    void save();
    void saveAs();
    void store();
    void refreshDump();

    void selectFont();
    void pixelizeFont();

    void ComputeUTF8Char();
    void charInc();
    void charDec();

    void moveX(int);
    void moveY(int);
    void zoom(int);

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    void updatePreview();
    void search();

    void autoCharFromUpdated();
    void autoCharToUpdated();
    void autoRun();
    void autoIterate();


private:
    Ui::MainWindow *ui;

    QString _FontPath;
    QString _CustomFontPath;
    QString _FontFamily ;
    QString _Str;
    QTimer _IteratorTimer;
    int _W=8;
    int _H=8.;

    CustomFont _CustomFont;
    long long int _AutoIt;
};

#endif // MAINWINDOW_H
