#include <QStyle>
#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent, CharMapReader *r) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    reader(r),
    romArray(nullptr),
    buffer(nullptr),
    filePath("."),
    romHandle(nullptr),
    buttonDict(new QHash<QString, QString>{std::pair<QString, QString>("pushButton_Up", "[方向上]"),
        std::pair<QString, QString>("pushButton_Down", "[方向下]"),
        std::pair<QString, QString>("pushButton_Left", "[方向左]"),
        std::pair<QString, QString>("pushButton_Right", "[方向右]"),
        std::pair<QString, QString>("pushButton_LeftRight", "[方向左右]"),
        std::pair<QString, QString>("pushButton_UpDown", "[方向上下]"),
        std::pair<QString, QString>("pushButton_Start", "[键Start]"),
        std::pair<QString, QString>("pushButton_Select", "[键Sel]"),
        std::pair<QString, QString>("pushButton_L", "[键L]"),
        std::pair<QString, QString>("pushButton_R", "[键R]"),
        std::pair<QString, QString>("pushButton_A", "[键A]"),
        std::pair<QString, QString>("pushButton_B", "[键B]")}),
    openAction(nullptr),
    saveAction(nullptr),
    quitAction(nullptr),
    aboutAction(nullptr),
    aboutQtAction(nullptr),
    showCharMapAction(nullptr),
    licenseAction(nullptr),
    fontAction(nullptr),
    colorAction(nullptr),
    themeAction1(nullptr),
    themeAction2(nullptr),
    loaded(false)
{
    QCoreApplication::setApplicationName("textmasterjp");
    QApplication::setApplicationDisplayName("Pokemon Text Master JPN");
    QApplication::setWindowIcon(QIcon(":/icons/togepi.ico"));

    ui->setupUi(this);
    setWindowTitle("TextMasterJpn");
    openAction = new QAction(QIcon::fromTheme("document-open", QApplication::style()->standardIcon(QStyle::SP_DirIcon)), QString("打开ROM"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(QString("选择要加载的ROM文件"));
    connect(openAction, &QAction::triggered, this, &MainWindow::open);

    saveAction = new QAction(QIcon::fromTheme("document-save", QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)), QString("保存ROM"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(QString("将修改内容写入ROM"));
    connect(saveAction, &QAction::triggered, this, &MainWindow::save);

    quitAction = new QAction(QString("退出"), this);
    quitAction->setShortcuts(QKeySequence::Quit);
    quitAction->setStatusTip(QString("退出程序"));
    connect(quitAction, &QAction::triggered, this, &MainWindow::quit);

    aboutAction = new QAction(QString("关于"), this);
    aboutAction->setStatusTip(QString("显示程序信息"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);

    aboutQtAction = new QAction(QString("关于Qt"), this);
    aboutQtAction->setStatusTip(QString("显示Qt信息"));
    connect(aboutQtAction, &QAction::triggered, this, &MainWindow::aboutQt);

    showCharMapAction = new QAction(QIcon::fromTheme("text-html", QApplication::style()->standardIcon(QStyle::SP_FileIcon)), QString("查看码表"), this);
    showCharMapAction->setStatusTip(QString("查看当前加载的码表"));
    connect(showCharMapAction, &QAction::triggered, this, &MainWindow::showCharMap);

    licenseAction = new QAction(QString("许可证"), this);
    licenseAction->setStatusTip(QString("显示程序所用的License"));
    connect(licenseAction, &QAction::triggered, this, &MainWindow::license);

    fontAction = new QAction(QString("字体..."), this);
    fontAction->setStatusTip(QString("更改输入文本的显示字体"));
    connect(fontAction, &QAction::triggered, this, &MainWindow::fontConf);

    colorAction = new QAction(QString("颜色..."), this);
    colorAction->setStatusTip(QString("更改输入文本的颜色"));
    connect(colorAction, &QAction::triggered, this, &MainWindow::colorConf);

    themeAction1 = new QAction(QString("默认"), this);
    themeAction1->setStatusTip(QString("设置为Qt默认主题"));
    themeAction1->setCheckable(true);
    themeAction1->setChecked(true);
    connect(themeAction1, &QAction::triggered, this, &MainWindow::themeConf1);

    themeAction2 = new QAction(QString("黑色"), this);
    themeAction2->setStatusTip(QString("设置为黑色主题"));
    themeAction2->setCheckable(true);
    themeAction2->setChecked(false);
    connect(themeAction2, &QAction::triggered, this, &MainWindow::themeConf2);

    QMenu *file = menuBar()->addMenu(QString("文件"));
    file->addAction(openAction);
    file->addAction(saveAction);
    file->addSeparator();
    file->addAction(quitAction);

    QMenu *config = menuBar()->addMenu(QString("设置"));
    QMenu *theme = config->addMenu("主题...");
    theme->addAction(themeAction1);
    theme->addAction(themeAction2);
    config->addSeparator();
    config->addAction(fontAction);
    config->addAction(colorAction);

    QMenu *help = menuBar()->addMenu(QString("帮助"));
    help->addAction(showCharMapAction);
    help->addSeparator();
    help->addAction(aboutAction);
    help->addAction(licenseAction);
    help->addAction(aboutQtAction);

    ui->mainToolBar->addAction(openAction);
    ui->mainToolBar->addAction(saveAction);
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(showCharMapAction);

    ui->plainTextEdit_UserInput->setAcceptDrops(false);
    this->setAcceptDrops(true);

    statusBar();

    // various buttons
    connect(ui->pushButton_CtrlChar, &QPushButton::clicked, this, &MainWindow::pushButton_CtrlChar);
    connect(ui->pushButton_Buffer, &QPushButton::clicked, this, &MainWindow::pushButton_Buffer);
    connect(ui->pushButton_PlayerName, &QPushButton::clicked, this, &MainWindow::pushButton_PlayerName);
    connect(ui->pushButton_RivalName, &QPushButton::clicked, this, &MainWindow::pushButton_RivalName);

    QList<QPushButton *> allButtons = ui->groupBox_4->findChildren<QPushButton *>();
    for (int i = 0; i < allButtons.size(); ++i)
        connect(allButtons[i], &QPushButton::clicked, this, &MainWindow::pushButton_Button);
    connect(ui->pushButton_TextColor, &QPushButton::clicked, this, &MainWindow::pushButton_TextColor);
    connect(ui->pushButton_BgColor, &QPushButton::clicked, this, &MainWindow::pushButton_BgColor);
    connect(ui->pushButton_FgColor, &QPushButton::clicked, this, &MainWindow::pushButton_FgColor);
    connect(ui->pushButton_Font, &QPushButton::clicked, this, &MainWindow::pushButton_Font);

    connect(ui->pushButton_Pause, &QPushButton::clicked, this, &MainWindow::pushButton_Pause);
    connect(ui->pushButton_Indent, &QPushButton::clicked, this, &MainWindow::pushButton_Indent);
    connect(ui->pushButton_Bgm, &QPushButton::clicked, this, &MainWindow::pushButton_Bgm);
    connect(ui->pushButton_PauseBgm, &QPushButton::clicked, this, &MainWindow::pushButton_PauseBgm);
    connect(ui->pushButton_ContBgm, &QPushButton::clicked, this, &MainWindow::pushButton_ContBgm);

    connect(ui->pushButton_StrToHex, &QPushButton::clicked, this, &MainWindow::pushButton_StrToHex);
    connect(ui->pushButton_HexToStr, &QPushButton::clicked, this, &MainWindow::pushButton_HexToStr);

    connect(ui->pushButton_ReadFromOffset, &QPushButton::clicked, this, &MainWindow::pushButton_ReadFromOffset);
    connect(ui->pushButton_WriteText, &QPushButton::clicked, this, &MainWindow::pushButton_WriteText);
    connect(ui->pushButton_WriteHex, &QPushButton::clicked, this, &MainWindow::pushButton_WriteHex);
    connect(ui->pushButton_SearchText, &QPushButton::clicked, this, &MainWindow::pushButton_SearchText);
}

MainWindow::~MainWindow()
{
    if (romArray)
        delete romArray;
    if (ui)
        delete ui;
    if (romHandle)
    {
        romHandle->close();
        delete romHandle;
    }
    if (reader)
        delete reader;
    delete buttonDict;
    // the QAction objects will be deleted automatically because of parent param. 
}

void MainWindow::OpenInternal()
{
    if (!filePath.isEmpty())
    {
        romHandle = new QFile(filePath);
        if (!romHandle->open(QIODevice::ReadOnly) || romHandle->size() > 0x2000000)
        {
            loaded = false;
            if (romArray)
                delete romArray;
            QMessageBox::warning(this, "打开",
                tr("无法打开文件:\n%1").arg(filePath));
            return;
        }
        romArray = new QByteArray{romHandle->readAll()};
        romHandle->close();
        delete romHandle;
        romHandle = nullptr;
        loaded = true;
    }
    else
    {
        loaded = false;
        if (romArray)
            delete romArray;
        QMessageBox::warning(this, "提示", "请选择ROM文件！");
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->urls().isEmpty())
    {
        filePath = event->mimeData()->urls().first().toLocalFile();
        if (!filePath.isNull() && !filePath.isEmpty())
        {
            OpenInternal();
            return;
        }
    }
    loaded = false;
    if (romArray)
        delete romArray;
    QMessageBox::warning(this, "提示", "请选择ROM文件！");
}

void MainWindow::open()
{
    filePath = QFileDialog::getOpenFileName(this,
        "打开文件...",
        filePath,
        "GBA ROM(*.gba);;二进制文件(*.bin)");
    OpenInternal();
}

void MainWindow::save()
{
    if (loaded)
    {
        QSaveFile *romHandle = new QSaveFile(filePath);
        if (!romHandle->open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(this, "保存",
                tr("无法保存文件:\n%1").arg(filePath));
            return;
        }
        romHandle->write(*romArray);
        romHandle->commit();
    }
    else
    {
        QMessageBox::warning(this, "提示", "未加载ROM！");
    }
}

void MainWindow::quit()
{
    QApplication::quit();
}

void MainWindow::about()
{
    QMessageBox::about(this, QString{"关于"}, QString{"<p>Pokemon TextMaster Jpn - 2019 JZW</p><br />\
        <a href='https://github.com/jiangzhengwenjz/TextMaster'>https://github.com/jiangzhengwenjz/TextMaster</a>"});
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this, QString{"关于Qt"});
}

void MainWindow::showCharMap()
{
    QDialog d{this};
    d.setWindowTitle(QString{"码表"});
    int size = reader->charMapDictRev->size();
    
    QHBoxLayout *layout = new QHBoxLayout{&d};

    QTableWidget *tableWidget = new QTableWidget{&d};
    layout->addWidget(tableWidget);

    tableWidget->setColumnCount(2);
    tableWidget->setRowCount(size);

    QStringList headers;
    headers << "编码" << "文字";
    tableWidget->setHorizontalHeaderLabels(headers);

    QMap<QString, QString> *map = new QMap<QString, QString>{};

    for (auto i = reader->charMapDictRev->cbegin(); i != reader->charMapDictRev->cend(); ++i)
    {
        QString hexString{i.key()};
        for (int j = 2; j < hexString.size(); j += 3)
            hexString.insert(j, QChar(' '));
        map->insert(hexString, i.value());
    }

    int row = 0, col = 0;
    for (auto i = map->cbegin(); i != map->cend(); ++i)
    {
        QTableWidgetItem *str = new QTableWidgetItem(i.value());
        QTableWidgetItem *hex = new QTableWidgetItem(i.key());

        hex->setFlags(hex->flags() ^ Qt::ItemIsEditable);
        str->setFlags(str->flags() ^ Qt::ItemIsEditable);
        tableWidget->setItem(row, col++, hex);
        tableWidget->setItem(row++, col--, str);
    }
    delete map;
    tableWidget->show();
    d.exec();
}

void MainWindow::license()
{
    QMessageBox msgbox{this};

    msgbox.setWindowTitle(QString{"LGPL v3"});
    msgbox.setText(QString{"<b>GNU LESSER GENERAL PUBLIC LICENSE</b>"});
    msgbox.setInformativeText("Copyright 2019 JZW\n\n\
        Pokemon TextMaster Jpn is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. Pokemon TextMaster Jpn is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU Lesser General Public License along with Pokemon TextMaster Jpn. If not, see <https://www.gnu.org/licenses/>.");
    msgbox.setStandardButtons(QMessageBox::Ok);
    msgbox.setIconPixmap(QPixmap(":/images/lgplv3.png"));
    msgbox.exec();
}

void MainWindow::fontConf()
{
    ui->plainTextEdit_UserInput->setFont(QFontDialog::getFont(nullptr, ui->plainTextEdit_UserInput->font()));
}

void MainWindow::colorConf()
{
    QColor color = QColorDialog::getColor(Qt::black, this);

    QPalette p = ui->plainTextEdit_UserInput->palette();
    p.setColor(QPalette::Base, Qt::white);
    p.setColor(QPalette::Text, color);
    ui->plainTextEdit_UserInput->setPalette(p);
}

void MainWindow::themeConf1()
{
    themeAction2->setChecked(false);
    this->setStyleSheet("");

}

void MainWindow::themeConf2()
{
    themeAction1->setChecked(false);
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())
    {
        qDebug() << ("找不到style sheet。");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        this->setStyleSheet(ts.readAll());
    }
}

void MainWindow::pushButton_CtrlChar()
{
    QButtonGroup group;
    QList<QRadioButton *> allButtons = ui->groupBox->findChildren<QRadioButton *>();

    for (int i = 0; i < allButtons.size(); ++i)
        group.addButton(allButtons[i],i);

    switch (group.checkedId())
    {
    case 0:
        ui->plainTextEdit_UserInput->insertPlainText("\\n");
        break;
    case 1:
        ui->plainTextEdit_UserInput->insertPlainText("\\p");
        break;
    case 2:
        ui->plainTextEdit_UserInput->insertPlainText("\\l");
        break;
    case 3:
        ui->plainTextEdit_UserInput->insertPlainText("\\c");
        break;
    case 4:
        ui->plainTextEdit_UserInput->insertPlainText("\\v");
        break;
    default: // -1
        break;
    }
}

void MainWindow::pushButton_Buffer()
{
    QHash<int, QString> dict{std::pair<int, QString>(0, "[buffer1]"),
        std::pair<int, QString>(1, "[buffer2]"),
        std::pair<int, QString>(2, "[buffer3]")};

    ui->plainTextEdit_UserInput->insertPlainText(dict[ui->comboBox_Buffer->currentIndex()]);
}

void MainWindow::pushButton_PlayerName()
{
    ui->plainTextEdit_UserInput->insertPlainText("[玩家]");
}

void MainWindow::pushButton_RivalName()
{
    ui->plainTextEdit_UserInput->insertPlainText("[对手]");
}

void MainWindow::pushButton_Button()
{
    ui->plainTextEdit_UserInput->insertPlainText(buttonDict->value(sender()->objectName()));
}

void MainWindow::pushButton_TextColor()
{
    ui->plainTextEdit_UserInput->insertPlainText(QString("[文本色_%1]").arg(ui->comboBox_TextColor->currentIndex(), 2, 10, QChar('0')));
}

void MainWindow::pushButton_BgColor()
{
    ui->plainTextEdit_UserInput->insertPlainText(QString("[背景色_%1]").arg(ui->comboBox_BgColor->currentIndex(), 2, 10, QChar('0')));
}

void MainWindow::pushButton_FgColor()
{
    ui->plainTextEdit_UserInput->insertPlainText(QString("[阴影色_%1]").arg(ui->comboBox_FgColor->currentIndex(), 2, 10, QChar('0')));
}

void MainWindow::pushButton_Font()
{
    QButtonGroup group;
    QList<QRadioButton *> allButtons = ui->widget_5->findChildren<QRadioButton *>();

    for (int i = 0; i < allButtons.size(); ++i)
        group.addButton(allButtons[i],i);

    if (group.checkedId() == 0)
        ui->plainTextEdit_UserInput->insertPlainText(QString{"[字号]\\h00"});
    else if (group.checkedId() == 1)
        ui->plainTextEdit_UserInput->insertPlainText(QString{"[字号]\\h01"});
}

void MainWindow::pushButton_Pause()
{
    ui->plainTextEdit_UserInput->insertPlainText(QString("[延时]\\h") + QString("%1").arg(ui->spinBox_Pause->value(),2, 16, QChar('0')).toUpper());
}

void MainWindow::pushButton_Indent()
{
    ui->plainTextEdit_UserInput->insertPlainText(QString("[缩进]\\h") + QString("%1").arg(ui->spinBox_Indent->value(),2, 16, QChar('0')).toUpper());
}

void MainWindow::pushButton_Bgm()
{
    quint32 val = ui->spinBox_Bgm->value();

    ui->plainTextEdit_UserInput->insertPlainText(QString("[音乐]\\h") + QString("%1").arg(val & 0xFF, 2, 16, QChar('0')).toUpper() + QString("\\h" + QString("%1").arg(val >> 8, 2, 16, QChar('0')).toUpper()));
}

void MainWindow::pushButton_PauseBgm()
{
    ui->plainTextEdit_UserInput->insertPlainText("[暂停音乐]");
}

void MainWindow::pushButton_ContBgm()
{
    ui->plainTextEdit_UserInput->insertPlainText("[继续音乐]");
}

void MainWindow::CopyToClipBoard(QStrResult &res)
{
    if (res.State())
    {
        QDialog d{this};
        QVBoxLayout *layout = new QVBoxLayout{&d};
        QPlainTextEdit *area = new QPlainTextEdit{&d};
        
        area->setReadOnly(true);
        area->setUndoRedoEnabled(false);
        area->setPlainText(res);
        layout->addWidget(area);

        QPushButton *copyButton = new QPushButton{&d};
        copyButton->setText("复制到剪贴板");
        layout->addWidget(copyButton);
        connect(copyButton, &QPushButton::clicked, [&res]() {
            QGuiApplication::clipboard()->setText(res);
        });

        d.exec();
    }
    else
    {
        QMessageBox::warning(this, "提示", res);
    }
}

void MainWindow::pushButton_StrToHex()
{
    FDecoder dec{reader};
    QString input = ui->plainTextEdit_UserInput->toPlainText();
    QStrResult res = dec.StrToHex(input);
    CopyToClipBoard(res);

}

void MainWindow::pushButton_HexToStr()
{
    FDecoder dec{reader};
    QString input = ui->plainTextEdit_UserInput->toPlainText();
    QStrResult res = dec.HexToStr(input);
    CopyToClipBoard(res);
}

int MainWindow::GetOffset() const
{
    QString offStr = ui->lineEdit_Offset->text();

    if (offStr.startsWith("0x"))
        offStr = offStr.mid(2);

    if (!offStr.size()
     || offStr.size() > 7)
        return -1;

    for (QChar c : offStr)
        if (!c.isLetterOrNumber())
            return -1;

    bool ok;
    int result = offStr.toUInt(&ok, 16);
    return ok ? result : -1;

}

void MainWindow::pushButton_ReadFromOffset()
{
    if (!loaded)
    {
        QMessageBox::warning(this, "提示", "未加载ROM！");
        return;
    }

    int offset = GetOffset();
    if (offset == -1 || offset >= romArray->size())
    {
        QMessageBox::warning(this, "提示", "无效的地址");
    }
    else
    {
        FDecoder dec{reader};
        QString hexStr{};

        for (int i = offset; i < romArray->size(); ++i)
        {
            unsigned char cur = (*romArray)[i];
            if (cur == 0xFF)
            {
                QStrResult res = dec.HexToStr(hexStr); // fuck me. too lazy to write a better func
                if (res.State())
                    ui->plainTextEdit_UserInput->setPlainText(res);
                else
                    QMessageBox::warning(this, "提示", res);
                return;
            }
            else
            {
                QString h = QString::number((uint)cur, 16).toUpper();
                if (h.size() == 1)
                    h = QString("0") + h;
                hexStr.append(QString("%1").arg(h));
            }
        }
        QMessageBox::warning(this, "提示", "未能找到文本终止符");
    }
}

inline int MainWindow::IsValidHexStr(QString &hex)
{
    int size = 0;
    buffer = new QByteArray{};
    if (!hex.size())
        return -1;
    for (int i = 0; i < hex.size(); ++i)
    {
        if (hex[i] != QChar(' ') && hex[i] != QChar('\t') && hex[i] != ('\n'))
        {
            if (i < hex.size() - 1 && hex[i].isLetterOrNumber() && hex[i + 1].isLetterOrNumber())
            {
                bool ok;
                uchar h = hex.mid(i, 2).toUInt(&ok, 16);
                
                if (ok)
                {
                    buffer->append(h);
                    ++size;
                    ++i;
                }
                else
                {
                    return -1;
                }
            }
            else
            {
                return -1; // we always want 2 valid chars to be adjacent
            }
        }
        else
        {
            hex.remove(i--, 1); // remove but do not go ahead
        }
    }
    return size;

}

void MainWindow::pushButton_WriteText()
{
    if (!loaded)
    {
        QMessageBox::warning(this, "提示", "未加载ROM！");
        return;
    }

    int offset = GetOffset();
    if (offset == -1 || offset >= romArray->size())
    {
        QMessageBox::warning(this, "提示", "无效的地址");
    }
    else
    {
        FDecoder dec{};
        QString str{ui->plainTextEdit_UserInput->toPlainText()};
        QStrResult res = dec.StrToHex(str);
        if (!res.State())
        {
            QMessageBox::warning(this, "提示", res);
            return;
        }
        QString hexStr{res};

        // fuck my laziness -> copy and paste
        if (IsValidHexStr(hexStr) > 0)
        {
            // case 0: the string is so long that will exceed the ROM's end
            if (buffer->size() >= (romArray->size() - offset)) // == is also included because we need an additional \hFF
            {
                QMessageBox::warning(this, "提示", "写入该文本将超出ROM大小范围");
            }
            else
            {
                int x00 = 0, xff = 0;
                for (int i = 0; i < buffer->size() + 1; ++i)
                {
                    uchar cur = (*romArray)[offset + i];

                    if (cur == 0)
                        ++x00;
                    if (cur == 0xff)
                        ++xff;
                    if (cur != 0 && cur != 0xFF)
                        break;
                }
                // case 1: is free space
                if (x00 == buffer->size() + 1 || xff == buffer->size() + 1)
                {
                NORMAL_WRITE:
                    for (int i = 0; i < buffer->size(); ++i)
                        (*romArray)[offset + i] = (*buffer)[i];
                    (*romArray)[offset + buffer->size()] = 0xFF;
                    delete buffer;
                    return;
                }
                else
                {
                    int ogStrLen = 0;
                    for (int i = offset; i < romArray->size(); ++i)
                    {
                        uchar cur = (*romArray)[i];

                        if (cur == 0xFF)
                            break;
                        else
                            ++ogStrLen;
                    }
                    if (buffer->size() <= ogStrLen)
                    {
                        goto NORMAL_WRITE;
                    }
                    else
                    {
                        if (QMessageBox::Yes == QMessageBox::question(this,
                            "提示",
                            "要写入的文本似乎比原文本长。继续写入吗？",
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes))
                            goto NORMAL_WRITE;
                    }
                }
            }
        }
        else
        {
            QMessageBox::warning(this, "提示", "码表中似乎存在无效数据");
        }
        delete buffer;
    }
}

void MainWindow::pushButton_WriteHex()
{
    if (!loaded)
    {
        QMessageBox::warning(this, "提示", "未加载ROM！");
        return;
    }

    int offset = GetOffset();
    if (offset == -1 || offset >= romArray->size())
    {
        QMessageBox::warning(this, "提示", "无效的地址");
    }
    else
    {
        QString hexStr{ui->plainTextEdit_UserInput->toPlainText()};
        // no strict check here because the user might have used \hxx to hardcode something
        if (IsValidHexStr(hexStr) > 0)
        {
            // case 0: the string is so long that will exceed the ROM's end
            if (buffer->size() >= (romArray->size() - offset)) // == is also included because we need an additional \hFF
            {
                QMessageBox::warning(this, "提示", "写入该hex数据串将超出ROM大小范围");
            }
            else
            {
                int x00 = 0, xff = 0;
                for (int i = 0; i < buffer->size() + 1; ++i)
                {
                    uchar cur = (*romArray)[offset + i];

                    if (cur == 0)
                        ++x00;
                    if (cur == 0xff)
                        ++xff;
                    if (cur != 0 && cur != 0xFF)
                        break;
                }
                // case 1: is free space
                if (x00 == buffer->size() + 1 || xff == buffer->size() + 1)
                {
                NORMAL_WRITE:
                    for (int i = 0; i < buffer->size(); ++i)
                        (*romArray)[offset + i] = (*buffer)[i];
                    (*romArray)[offset + buffer->size()] = 0xFF;
                    delete buffer;
                    return;
                }
                else
                {
                    int ogStrLen = 0;
                    for (int i = offset; i < romArray->size(); ++i)
                    {
                        uchar cur = (*romArray)[i];

                        if (cur == 0xFF)
                            break;
                        else
                            ++ogStrLen;
                    }
                    if (buffer->size() <= ogStrLen)
                    {
                        goto NORMAL_WRITE;
                    }
                    else
                    {
                        if (QMessageBox::Yes == QMessageBox::question(this,
                            "提示",
                            "要写入的数据似乎比原文本长。继续写入吗？",
                            QMessageBox::Yes | QMessageBox::No,
                            QMessageBox::Yes))
                            goto NORMAL_WRITE;
                    }
                }
            }
        }
        else
        {
            QMessageBox::warning(this, "提示", "该hex数据串无效");
        }
        delete buffer;
    }
}

void MainWindow::pushButton_SearchText()
{
    if (!loaded)
    {
        QMessageBox::warning(this, "提示", "未加载ROM！");
        return;
    }
    FDecoder dec{};
    QString str{ui->plainTextEdit_UserInput->toPlainText()};
    QStrResult res = dec.StrToHex(str);
    
    if (!res.State())
    {
        QMessageBox::warning(this, "提示", res);
        return;
    }
    QString hexStr{res};
    if (IsValidHexStr(hexStr) > 0)
    {
        QList<QList<int> > tree{};
        int idx = romArray->indexOf(*buffer);
        
        if (idx == -1)
        {
            QMessageBox::warning(this, "提示", "ROM中找不到指定文本");
            delete buffer;
            return;
        }
        do
        {
            tree.append(QList<int>{idx});
            // search for pointers to the string
            char p[4];

            // narrowing -_-
            p[0] = idx & 0xFF;
            p[1] = (idx & 0xFF00) >> 8;
            p[2] = (idx & 0xFF0000) >> 16;
            p[3] = (((uint)(idx & 0xFF000000)) >> 24) + 8;

            QByteArray ba(p, 4);

            int pIdx = -1;
            while ((pIdx = romArray->indexOf(ba, pIdx + 1)) != -1)
                tree.last().append(pIdx);
        }
        while ((idx = romArray->indexOf(*buffer, idx + 1)) != -1);

        QDialog d{this};
        d.setWindowTitle("搜索结果");

        QHBoxLayout *layout = new QHBoxLayout{&d};
        QTreeWidget treeWidget{&d};
        layout->addWidget(&treeWidget);

        QStringList header{"O = 文本位置, P = 指针位置"};
        treeWidget.setHeaderLabels(header);

        for (auto &l : tree)
        {
            QTreeWidgetItem *treeItem = new QTreeWidgetItem{&treeWidget};
            treeItem->setText(0, QString{"0x%1"}.arg(QString::number(l.front(), 16).toUpper(), 7, '0'));
            treeItem->setIcon(0,QIcon(":/icons/black_o.ico"));

            for (auto li = l.cbegin() + 1; li != l.cend(); ++li)
            {
                QTreeWidgetItem *treeItem_ = new QTreeWidgetItem{treeItem};
                treeItem_->setText(0, QString{"0x%1"}.arg(QString::number(*li, 16).toUpper(), 7, '0'));
                treeItem_->setIcon(0,QIcon(":/icons/black_p.ico"));
                
                treeItem->addChild(treeItem_);
            }
        }

        treeWidget.show();
        d.exec();

    }
    else
    {
        QMessageBox::warning(this, "提示", "码表中似乎存在无效数据");
    }
    delete buffer;
}
