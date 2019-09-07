#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QIcon>
#include <QPixmap>
#include <QColor>
#include <QVector>
#include <QMap>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDataStream>
#include <QSaveFile>
#include <QStringList>
#include <QRadioButton>
#include <QButtonGroup>
#include <QMimeData>
#include <QClipboard>
#include <QTreeWidget>
#include "core/read_charmap.h"
#include "core/f_decoder.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR, CharMapReader *r = Q_NULLPTR);
    virtual ~MainWindow();
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

// QT macro
public slots:
    void open();
    void save();
    void quit();
    void about();
    void aboutQt();
    void showCharMap();
    void license();
    void fontConf();
    void colorConf();
    void themeConf1();
    void themeConf2();
    void pushButton_CtrlChar();
    void pushButton_Buffer();
    void pushButton_PlayerName();
    void pushButton_RivalName();
    void pushButton_Button();
    void pushButton_TextColor();
    void pushButton_BgColor();
    void pushButton_FgColor();
    void pushButton_Font();
    void pushButton_Pause();
    void pushButton_Indent();
    void pushButton_Bgm();
    void pushButton_PauseBgm();
    void pushButton_ContBgm();
    void pushButton_StrToHex();
    void pushButton_HexToStr();
    void pushButton_ReadFromOffset();
    void pushButton_WriteText();
    void pushButton_WriteHex();
    void pushButton_SearchText();

private:
    Ui::MainWindow *ui;
    CharMapReader *reader;
    QByteArray *romArray;
    QByteArray *buffer;
    QString filePath;
    QFile *romHandle;
    const QHash<QString, QString> *buttonDict;
    QAction *openAction;
    QAction *saveAction;
    QAction *quitAction;
    QAction *aboutAction;
    QAction *aboutQtAction;
    QAction *showCharMapAction;
    QAction *licenseAction;
    QAction *fontAction;
    QAction *colorAction;
    QAction *themeAction1;
    QAction *themeAction2;
    bool loaded;
    void OpenInternal();
    void CopyToClipBoard(QStrResult &res);
    int GetOffset() const;
    inline int IsValidHexStr(QString &str);
};

#endif // MAIN_WINDOW_H
