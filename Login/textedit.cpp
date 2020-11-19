#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QFontComboBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QMenu>
#include <QMenuBar>
#include <QTextCodec>
#include <QTextEdit>
#include <QStatusBar>
#include <QToolBar>
#include <QTextCursor>
#include <QTextDocumentWriter>
#include <QTextList>
#include <QTextDocumentFragment>
#include <QtDebug>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMimeData>
#include <QMimeDatabase>
#include <QtPrintSupport/qtprintsupportglobal.h>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>

#include <QPainter>
#include <QLabel>
#include <QDebug>
#include <QSaveFile>
#include <QThread>

#include "presence.h"
#include "textedit.h"
#include "client.h"

#ifdef Q_OS_MACOS
const QString rsrcPath = ":/images/mac";
#else
const QString rsrcPath = ":/images/win";
#endif
int global_var=0;

TextEdit::TextEdit(QWidget *parent,struct subscriber_t* subscriber,QList<subscriber_t>* listUsers)
    : QMainWindow(parent),subscriber(subscriber),listUsers(listUsers),ready(false),deletion(false)
{

    setWindowTitle(QCoreApplication::applicationName());

    textEdit = new QTextEdit(parent);
    const QSignalBlocker blocker(textEdit->document());

    connect(textEdit, &QTextEdit::currentCharFormatChanged,this, &TextEdit::currentCharFormatChanged);
    connect(textEdit, &QTextEdit::cursorPositionChanged,this, &TextEdit::cursorPositionChanged);
    setCentralWidget(textEdit);

    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setupFileActions();
    setupEditActions();
    setupTextActions();
    setupClientOnline();

    {
        QMenu *helpMenu = menuBar()->addMenu(tr("Help"));
        helpMenu->addAction(tr("About"), this, &TextEdit::about);
        //helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
    }

    QFont textFont("Helvetica");
    textFont.setStyleHint(QFont::SansSerif);
    textEdit->setFont(textFont);
    fontChanged(textEdit->font());
    colorChanged(textEdit->textColor());
    alignmentChanged(textEdit->alignment());

    connect(textEdit->document(), &QTextDocument::modificationChanged,actionSave, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::modificationChanged,this, &QWidget::setWindowModified);
    connect(textEdit->document(), &QTextDocument::undoAvailable,actionUndo, &QAction::setEnabled);
    connect(textEdit->document(), &QTextDocument::redoAvailable,actionRedo, &QAction::setEnabled);

    setWindowModified(textEdit->document()->isModified());
    actionSave->setEnabled(textEdit->document()->isModified());
    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

    actionCut->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCut, &QAction::setEnabled);

    actionCopy->setEnabled(false);
    connect(textEdit, &QTextEdit::copyAvailable, actionCopy, &QAction::setEnabled);

    //Cipboard: temporal storage of data to be copied, pasted...
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &TextEdit::clipboardDataChanged);

    textEdit->setFocus();
    setCurrentFileName(QString());

    int j=1;
    for(auto i=this->listUsers->begin(); i!=this->listUsers->end(); i++){
        qDebug()<<"TEXTEDIT USERNAME FROM LIST"<<i->username;
        QImage img;
        if(i->serializedImage==nullptr){
            img.load(":/icons_pack/avatar_default.png");
        }else{
            img.loadFromData(i->serializedImage);
        }


        newPresence(j,i->username,img);
        j++;
    }
    drawAllCursor();
    textEdit->installEventFilter(this);

    connect(textEdit->document(),&QTextDocument::contentsChange,this,&TextEdit::onContentsChanged);

    qRegisterMetaType<QTextCharFormat>("QTextCharFormat");

}

void TextEdit::closeEvent(QCloseEvent *e)
{
    qDebug() << QFileInfo(fileName).fileName();
    emit removeClientWorkspace(QFileInfo(fileName).fileName());
    listUsers->clear();
}

void TextEdit::setupFileActions()
{
    QToolBar *tb = addToolBar(tr("File Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&File"));

    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(rsrcPath + "/filenew.png"));
    QAction *a = menu->addAction(newIcon,  tr("&New"), this, &TextEdit::fileNew);
    //tb->addAction(a);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::New);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(rsrcPath + "/fileopen.png"));
    a = menu->addAction(openIcon, tr("&Open..."), this, &TextEdit::fileOpen);
    a->setShortcut(QKeySequence::Open);
    //tb->addAction(a);

    menu->addSeparator();

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(rsrcPath + "/filesave.png"));
    actionSave = menu->addAction(saveIcon, tr("&Save"), this, &TextEdit::fileSave);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(false);
    //tb->addAction(actionSave);

    a = menu->addAction(tr("Save &As..."), this, &TextEdit::fileSaveAs);
    a->setPriority(QAction::LowPriority);
    menu->addSeparator();

    const QIcon printIcon = QIcon::fromTheme("documfileNameent-print", QIcon(rsrcPath + "/fileprint.png"));
    a = menu->addAction(printIcon, tr("&Print..."), this, &TextEdit::filePrint);
    tb->addAction(a);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::Print);

    const QIcon filePrintIcon = QIcon::fromTheme("fileprint", QIcon(rsrcPath + "/fileprint.png"));
    menu->addAction(filePrintIcon, tr("Print Preview..."), this, &TextEdit::filePrintPreview);

    const QIcon exportPdfIcon = QIcon::fromTheme("exportpdf", QIcon(rsrcPath + "/exportpdf.png"));
    a = menu->addAction(exportPdfIcon, tr("&Export PDF..."), this, &TextEdit::filePrintPdf);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    tb->addAction(a);

    menu->addSeparator();

    a = menu->addAction(tr("&Quit"), this, &QWidget::close);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
}

void TextEdit::setupEditActions()
{
    QToolBar *tb = addToolBar(tr("Edit Actions"));
    QMenu *menu = menuBar()->addMenu(tr("&Edit"));

    const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon(rsrcPath + "/editundo.png"));
    actionUndo = menu->addAction(undoIcon, tr("&Undo"), textEdit, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon(rsrcPath + "/editredo.png"));
    actionRedo = menu->addAction(redoIcon, tr("&Redo"), textEdit, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    menu->addSeparator();

    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(rsrcPath + "/editcut.png"));
    actionCut = menu->addAction(cutIcon, tr("Cu&t"), textEdit, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(rsrcPath + "/editcopy.png"));
    actionCopy = menu->addAction(copyIcon, tr("&Copy"), textEdit, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(rsrcPath + "/editpaste.png"));
    actionPaste = menu->addAction(pasteIcon, tr("&Paste"), textEdit, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);

    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(!md->hasImage());
}

void TextEdit::setupTextActions()
{
    QToolBar *tb = addToolBar(tr("Format Actions"));
    QMenu *menu = menuBar()->addMenu(tr("F&ormat"));

    tb->setFixedHeight(40);
    tb->setIconSize(QSize(40,40));

    const QIcon boldIcon = QIcon::fromTheme("format-text-bold", QIcon(rsrcPath + "/textbold.png"));
    actionTextBold = menu->addAction(boldIcon, tr("&Bold"), this, &TextEdit::textBold);
    actionTextBold->setShortcut(Qt::CTRL + Qt::Key_B);
    actionTextBold->setPriority(QAction::LowPriority);
    QFont bold;
    bold.setBold(true);
    actionTextBold->setFont(bold);
    tb->addAction(actionTextBold);
    actionTextBold->setCheckable(true);

    const QIcon italicIcon = QIcon::fromTheme("format-text-italic", QIcon(rsrcPath + "/textitalic.png"));
    actionTextItalic = menu->addAction(italicIcon, tr("&Italic"), this, &TextEdit::textItalic);
    actionTextItalic->setPriority(QAction::LowPriority);
    actionTextItalic->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italic;
    italic.setItalic(true);
    actionTextItalic->setFont(italic);
    tb->addAction(actionTextItalic);
    actionTextItalic->setCheckable(true);

    const QIcon underlineIcon = QIcon::fromTheme("format-text-underline", QIcon(rsrcPath + "/textunder.png"));
    actionTextUnderline = menu->addAction(underlineIcon, tr("&Underline"), this, &TextEdit::textUnderline);
    actionTextUnderline->setShortcut(Qt::CTRL + Qt::Key_U);
    actionTextUnderline->setPriority(QAction::LowPriority);
    QFont underline;
    underline.setUnderline(true);
    actionTextUnderline->setFont(underline);
    tb->addAction(actionTextUnderline);
    actionTextUnderline->setCheckable(true);

    menu->addSeparator();

    const QIcon leftIcon = QIcon::fromTheme("format-justify-left", QIcon(rsrcPath + "/textleft.png"));
    actionAlignLeft = new QAction(leftIcon, tr("&Left"), this);
    actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
    actionAlignLeft->setCheckable(true);
    actionAlignLeft->setPriority(QAction::LowPriority);
    const QIcon centerIcon = QIcon::fromTheme("format-justify-center", QIcon(rsrcPath + "/textcenter.png"));
    actionAlignCenter = new QAction(centerIcon, tr("C&enter"), this);
    actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
    actionAlignCenter->setCheckable(true);
    actionAlignCenter->setPriority(QAction::LowPriority);
    const QIcon rightIcon = QIcon::fromTheme("format-justify-right", QIcon(rsrcPath + "/textright.png"));
    actionAlignRight = new QAction(rightIcon, tr("&Right"), this);
    actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
    actionAlignRight->setCheckable(true);
    actionAlignRight->setPriority(QAction::LowPriority);
    const QIcon fillIcon = QIcon::fromTheme("format-justify-fill", QIcon(rsrcPath + "/textjustify.png"));
    actionAlignJustify = new QAction(fillIcon, tr("&Justify"), this);
    actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
    actionAlignJustify->setCheckable(true);
    actionAlignJustify->setPriority(QAction::LowPriority);
/*
    const QIcon indentMoreIcon = QIcon::fromTheme("format-indent-more", QIcon(rsrcPath + "/format-indent-more.png"));
    actionIndentMore = menu->addAction(indentMoreIcon, tr("&Indent"), this, &TextEdit::indent);
    actionIndentMore->setShortcut(Qt::CTRL + Qt::Key_BracketRight);
    actionIndentMore->setPriority(QAction::LowPriority);
    const QIcon indentLessIcon = QIcon::fromTheme("format-indent-less", QIcon(rsrcPath + "/format-indent-less.png"));
    actionIndentLess = menu->addAction(indentLessIcon, tr("&Unindent"), this, &TextEdit::unindent);
    actionIndentLess->setShortcut(Qt::CTRL + Qt::Key_BracketLeft);
    actionIndentLess->setPriority(QAction::LowPriority);
*/

    actionTextSubscript = tb->addAction(QIcon(rsrcPath + "/textpedix.png"), tr("Su&bscript"),
        this, &TextEdit::textSubscript);
    actionTextSubscript->setCheckable(true);

    // Superscript
    actionTextSuperscript = tb->addAction(QIcon(rsrcPath + "/textapix.png"), tr("Su&perscript"),
        this, &TextEdit::textSuperscript);
    actionTextSuperscript->setCheckable(true);


    // Make sure the alignLeft  is always left of the alignRight
    QActionGroup *alignGroup = new QActionGroup(this);
    connect(alignGroup, &QActionGroup::triggered, this, &TextEdit::textAlign);

    if (QApplication::isLeftToRight()) {
        alignGroup->addAction(actionAlignLeft);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignRight);
    } else {
        alignGroup->addAction(actionAlignRight);
        alignGroup->addAction(actionAlignCenter);
        alignGroup->addAction(actionAlignLeft);
    }
    alignGroup->addAction(actionAlignJustify);

    tb->addActions(alignGroup->actions());
    menu->addActions(alignGroup->actions());
    //tb->addAction(actionIndentMore);
    //tb->addAction(actionIndentLess);
    //menu->addAction(actionIndentMore);
    //menu->addAction(actionIndentLess);

    menu->addSeparator();

    QPixmap pix(16, 16);
    pix.fill(Qt::black);
    actionTextColor = menu->addAction(pix, tr("&Color..."), this, &TextEdit::textColor);
    tb->addAction(actionTextColor);

    menu->addSeparator();

    /*
    const QIcon checkboxIcon = QIcon::fromTheme("status-checkbox-checked", QIcon(rsrcPath + "/checkbox-checked.png"));
    actionToggleCheckState = menu->addAction(checkboxIcon, tr("Chec&ked"), this, &TextEdit::setChecked);
    actionToggleCheckState->setShortcut(Qt::CTRL + Qt::Key_K);
    actionToggleCheckState->setCheckable(true);
    actionToggleCheckState->setPriority(QAction::LowPriority);
    tb->addAction(actionToggleCheckState);
    */


    tb = addToolBar(tr("Format Actions"));
    tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    addToolBarBreak(Qt::TopToolBarArea);
    addToolBar(tb);

    comboStyle = new QComboBox(tb);
    tb->addWidget(comboStyle);
    comboStyle->addItem("Standard");
    /*
    comboStyle->addItem("Heading 1");
    comboStyle->addItem("Heading 2");
    comboStyle->addItem("Heading 3");
    comboStyle->addItem("Heading 4");
    comboStyle->addItem("Heading 5");
    comboStyle->addItem("Heading 6");*/

    connect(comboStyle, QOverload<int>::of(&QComboBox::activated), this, &TextEdit::textStyle);

    comboFont = new QFontComboBox(tb);
    tb->addWidget(comboFont);
    connect(comboFont, &QComboBox::textActivated, this, &TextEdit::textFamily);

    comboSize = new QComboBox(tb);
    comboSize->setObjectName("comboSize");
    tb->addWidget(comboSize);
    comboSize->setEditable(true);

    const QList<int> standardSizes = QFontDatabase::standardSizes();
    for (int size : standardSizes)
        comboSize->addItem(QString::number(size));
    comboSize->setCurrentIndex(standardSizes.indexOf(QApplication::font().pointSize()));

    connect(comboSize, &QComboBox::textActivated, this, &TextEdit::textSize);
}

void TextEdit::setupClientOnline()
{
    onlineUsers = addToolBar(tr("Client Online"));

    onlineUsers->setIconSize(QSize(40,40));

    addToolBar(Qt::BottomToolBarArea,onlineUsers);
    onlineUsers->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea | Qt::LeftToolBarArea | Qt::RightToolBarArea);

}


bool TextEdit::load(const QString &fileName, SharedFile* file)
{
    {
        const QSignalBlocker blocker(textEdit->document());
        textEdit->textCursor().setPosition(QTextCursor::Start);
        for(Symbol s: file->getSymbols()){
            textEdit->textCursor().insertText(s.getCar(),s.getFmt());
            QTextBlockFormat textBlockFormat=textEdit->textCursor().blockFormat();
            switch (s.getFmt().properties().find(QTextFormat::BlockAlignment).value().toUInt()) {
                case Qt::AlignLeading  | Qt::AlignAbsolute:         textBlockFormat.setAlignment(Qt::AlignLeading  | Qt::AlignAbsolute); break;
                case Qt::AlignTrailing | Qt::AlignAbsolute:         textBlockFormat.setAlignment(Qt::AlignTrailing | Qt::AlignAbsolute); break;
                case Qt::AlignHCenter:                              textBlockFormat.setAlignment(Qt::AlignHCenter); break;
                case Qt::AlignJustify:                              textBlockFormat.setAlignment(Qt::AlignJustify); break;
                default:                                            textBlockFormat.setAlignment(Qt::AlignLeading  | Qt::AlignAbsolute); break;
            }
            textEdit->textCursor().setBlockFormat(textBlockFormat);
        }
    }
    textEdit->textCursor().setPosition(QTextCursor::Start,QTextCursor::MoveAnchor);
    Presence p=onlineUsers_map.find(this->subscriber->username).value();
    drawCursor(p);
    setCurrentFileName(fileName);
    drawAllCursor();
    return true;
}

void TextEdit::drawAllCursor()
{
    foreach(const Presence& p, onlineUsers_map.values()) {
        if (p.name() != this->subscriber->username)
            drawCursor(p);
    }
}

void TextEdit::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName, QCoreApplication::applicationName()));
    setWindowModified(false);
}

void TextEdit::fileNew()
{

    textEdit->clear();
    setCurrentFileName(QString());

}

void TextEdit::fileOpen()
{
}

bool TextEdit::fileSave()
{
    emit saveFile(fileName);
    return true;
}

bool TextEdit::fileSaveAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    QStringList mimeTypes;
    mimeTypes << "text/plain"
#if QT_CONFIG(textodfwriter)
              << "application/vnd.oasis.opendocument.text"
#endif
#if QT_CONFIG(textmarkdownwriter)
              << "text/markdown"
#endif
              << "text/html";
    fileDialog.setMimeTypeFilters(mimeTypes);
#if QT_CONFIG(textodfwriter)
    fileDialog.setDefaultSuffix("odt");
#endif
    if (fileDialog.exec() != QDialog::Accepted)
        return false;
    const QString fn = fileDialog.selectedFiles().first();
    setCurrentFileName(fn);
    return fileSave();
}

void TextEdit::filePrint()
{
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted)
        textEdit->print(&printer);
    delete dlg;
}

void TextEdit::filePrintPreview()
{
#if QT_CONFIG(printpreviewdialog)
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &TextEdit::printPreview);
    preview.exec();
#endif
}

void TextEdit::printPreview(QPrinter *printer)
{
    textEdit->print(printer);
}


void TextEdit::filePrintPdf()
{

    QFileDialog fileDialog(this, tr("Export PDF"));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setMimeTypeFilters(QStringList("application/pdf"));
    fileDialog.setDefaultSuffix("pdf");
    if (fileDialog.exec() != QDialog::Accepted)
        return;
    QString fileName = fileDialog.selectedFiles().first();
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit->document()->print(&printer);
    statusBar()->showMessage(tr("Exported \"%1\"").arg(QDir::toNativeSeparators(fileName)));

}

void TextEdit::textBold()
{
    //const QSignalBlocker blocker(textEdit->document());

    QTextCharFormat fmt;
    fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textUnderline()
{
    //const QSignalBlocker blocker(textEdit->document());

    QTextCharFormat fmt;
    fmt.setFontUnderline(actionTextUnderline->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textItalic()
{
    //const QSignalBlocker blocker(textEdit->document());

    QTextCharFormat fmt;
    fmt.setFontItalic(actionTextItalic->isChecked());
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textFamily(const QString &f)
{
    //const QSignalBlocker blocker(textEdit->document());

    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSize(const QString &p)
{
    //const QSignalBlocker blocker(textEdit->document());

    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}

void TextEdit::textStyle(int styleIndex)
{
    //const QSignalBlocker blocker(textEdit->document());

    QTextCursor cursor = textEdit->textCursor();
    QTextListFormat::Style style = QTextListFormat::ListStyleUndefined;

    //For case 4 and 5, is possible to add a box crossed or not in order to rapresent a task that is
    //already done or todo (like a checklist). Inside the box there is the symbol choosen to
    //rapresent every entry in the list (upperAlpha, loweAlpha, Roman...)
    QTextBlockFormat::MarkerType marker = QTextBlockFormat::MarkerType::NoMarker;

    switch (styleIndex) {
    case 1:
        style = QTextListFormat::ListDisc;
        break;
    case 2:
        style = QTextListFormat::ListCircle;
        break;
    case 3:
        style = QTextListFormat::ListSquare;
        break;
    case 4:
        if (cursor.currentList())
            style = cursor.currentList()->format().style();
        else
            style = QTextListFormat::ListDisc;
        marker = QTextBlockFormat::MarkerType::Unchecked;
        break;
    case 5:
        if (cursor.currentList())
            style = cursor.currentList()->format().style();
        else
            style = QTextListFormat::ListDisc;
        marker = QTextBlockFormat::MarkerType::Checked;
        break;
    case 6:
        style = QTextListFormat::ListDecimal;
        break;
    case 7:
        style = QTextListFormat::ListLowerAlpha;
        break;
    case 8:
        style = QTextListFormat::ListUpperAlpha;
        break;
    case 9:
        style = QTextListFormat::ListLowerRoman;
        break;
    case 10:
        style = QTextListFormat::ListUpperRoman;
        break;
    default:
        break;
    }

    cursor.beginEditBlock();

    //Structure holding all the format changes to apply
    QTextBlockFormat blockFmt = cursor.blockFormat();

    if (style == QTextListFormat::ListStyleUndefined) {
        blockFmt.setObjectIndex(-1);
        int headingLevel = styleIndex >= 11 ? styleIndex - 11 + 1 : 0; // H1 to H6, or Standard
        blockFmt.setHeadingLevel(headingLevel);
        cursor.setBlockFormat(blockFmt);

        int sizeAdjustment = headingLevel ? 4 - headingLevel : 0; // H1 to H6: +3 to -2
        QTextCharFormat fmt;
        fmt.setFontWeight(headingLevel ? QFont::Bold : QFont::Normal);
        fmt.setProperty(QTextFormat::FontSizeAdjustment, sizeAdjustment);
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fmt);

        //Merges the properties specified in modifier into the current character format by calling QTextCursor::mergeCharFormat on the editor's cursor.
        //If the editor has a selection then the properties of modifier are directly applied to the selection
        //Testato, se ho testo highlighted, se clicco su bolded, esempio, verrà modificato quella porzione di testo soltanto
        textEdit->mergeCurrentCharFormat(fmt);
    } else {
        blockFmt.setMarker(marker);
        cursor.setBlockFormat(blockFmt);
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
        } else {
            listFmt.setIndent(blockFmt.indent() + 1);
            blockFmt.setIndent(0);
            cursor.setBlockFormat(blockFmt);
        }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
    }

    cursor.endEditBlock();
}

void TextEdit::textColor()
{
    //const QSignalBlocker blocker(textEdit->document());
    //Presence p=onlineUsers_map.find(this->subscriber.username).value()
    qDebug()<<"COLOR CALLED";
    QColor col = QColorDialog::getColor(textEdit->textColor(), this);
    if (!col.isValid())
        return;
    QTextCharFormat fmt;
    fmt.setForeground(col);
    mergeFormatOnWordOrSelection(fmt);
    colorChanged(col);
}

void TextEdit::textAlign(QAction *a)
{

    if (a == actionAlignLeft)
        textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (a == actionAlignCenter)
        textEdit->setAlignment(Qt::AlignHCenter);
    else if (a == actionAlignRight)
        textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (a == actionAlignJustify)
        textEdit->setAlignment(Qt::AlignJustify);

    Presence& p=onlineUsers_map.find(this->subscriber->username).value();
    drawCursor(p);
    drawAllCursor();
}

void TextEdit::setChecked(bool checked)
{
    textStyle(checked ? 5 : 4);
}

void TextEdit::indent()
{
    modifyIndentation(1);
}

void TextEdit::unindent()
{
    modifyIndentation(-1);
}

void TextEdit::modifyIndentation(int amount)
{
    //const QSignalBlocker blocker(textEdit->document());

    QTextCursor cursor = textEdit->textCursor();
    cursor.beginEditBlock();
    if (cursor.currentList()) {
        QTextListFormat listFmt = cursor.currentList()->format();
        // See whether the line above is the list we want to move this item into,
        // or whether we need a new list.
        QTextCursor above(cursor);
        above.movePosition(QTextCursor::Up);
        if (above.currentList() && listFmt.indent() + amount == above.currentList()->format().indent()) {
            above.currentList()->add(cursor.block());
        } else {
            listFmt.setIndent(listFmt.indent() + amount);
            cursor.createList(listFmt);
        }
    } else {
        QTextBlockFormat blockFmt = cursor.blockFormat();
        blockFmt.setIndent(blockFmt.indent() + amount);
        cursor.setBlockFormat(blockFmt);
    }
    cursor.endEditBlock();
}

void TextEdit::currentCharFormatChanged(const QTextCharFormat &format)
{
    //const QSignalBlocker blocker(textEdit->document());
    fontChanged(format.font());
    colorChanged(format.foreground().color());
    scriptChanged(format.verticalAlignment());
}

void TextEdit::cursorPositionChanged()
{

    alignmentChanged(textEdit->alignment());
    QTextList *list = textEdit->textCursor().currentList();

    if (list) {
        switch (list->format().style()) {
        case QTextListFormat::ListDisc:
            comboStyle->setCurrentIndex(1);
            break;
        case QTextListFormat::ListCircle:
            comboStyle->setCurrentIndex(2);
            break;
        case QTextListFormat::ListSquare:
            comboStyle->setCurrentIndex(3);
            break;
        case QTextListFormat::ListDecimal:
            comboStyle->setCurrentIndex(6);
            break;
        case QTextListFormat::ListLowerAlpha:
            comboStyle->setCurrentIndex(7);
            break;
        case QTextListFormat::ListUpperAlpha:
            comboStyle->setCurrentIndex(8);
            break;
        case QTextListFormat::ListLowerRoman:
            comboStyle->setCurrentIndex(9);
            break;
        case QTextListFormat::ListUpperRoman:
            comboStyle->setCurrentIndex(10);
            break;
        default:
            comboStyle->setCurrentIndex(-1);
            break;
        }
        switch (textEdit->textCursor().block().blockFormat().marker()) {
        case QTextBlockFormat::MarkerType::NoMarker:
            actionToggleCheckState->setChecked(false);
            break;
        case QTextBlockFormat::MarkerType::Unchecked:
            comboStyle->setCurrentIndex(4);
            actionToggleCheckState->setChecked(false);
            break;
        case QTextBlockFormat::MarkerType::Checked:
            comboStyle->setCurrentIndex(5);
            actionToggleCheckState->setChecked(true);
            break;
        }
    } else {
        int headingLevel = textEdit->textCursor().blockFormat().headingLevel();
        comboStyle->setCurrentIndex(headingLevel ? headingLevel + 10 : 0);
    }


    QTextCursor cursor = textEdit->textCursor();
    int pre= cursor.position();
    Presence p=onlineUsers_map.find(this->subscriber->username).value();
    p.cursor()->setPosition(pre,QTextCursor::MoveAnchor);
    drawCursor(p);
    p.cursor()->clearSelection();
    //qDebug()<<"MY CURSORO: "<<p.cursor()->position()<<" pre: "<<pre;
    emit changeCursorPositionSignal(pre, this->subscriber->username);
    ////////////////////////////////

}

void TextEdit::fromServerNewCursorPosition(int pos, QString user){
    QTextCursor cursor = textEdit->textCursor();
    cursor.setPosition(pos);
    Presence p=onlineUsers_map.find(user).value();
    p.cursor()->setPosition(cursor.position(),QTextCursor::MoveAnchor);
    drawCursor(p);
}

void TextEdit::fromServerInsert(QString c, int pos,QString user,QTextCharFormat nee_format){
    qDebug()<< "<TESTING **********>Sono nello slot di inserimento da server con c="<<c<<" e pos="<<pos<< " e di "<<user;
    const QSignalBlocker blocker(textEdit->document());

    Presence p=onlineUsers_map.find(user).value();
    p.cursor()->clearSelection();
    p.cursor()->setPosition(pos);
    p.cursor()->insertText(c,nee_format);

    // code to set the allignment of the text block
    //qDebug()<<"Align form server "<<nee_format.properties().values();
    int numberblock=p.cursor()->blockNumber();
    //qDebug()<<"BLOCK NUMBER"<<numberblock;
    QTextBlockFormat textBlockFormat = p.cursor()->blockFormat();
    switch (nee_format.properties().find(QTextFormat::BlockAlignment).value().toUInt()) {
        case Qt::AlignLeading  | Qt::AlignAbsolute:         textBlockFormat.setAlignment(Qt::AlignLeading  | Qt::AlignAbsolute); break;
        case Qt::AlignTrailing | Qt::AlignAbsolute:         textBlockFormat.setAlignment(Qt::AlignTrailing | Qt::AlignAbsolute); break;
        case Qt::AlignHCenter:                              textBlockFormat.setAlignment(Qt::AlignHCenter); break;
        case Qt::AlignJustify:                              textBlockFormat.setAlignment(Qt::AlignJustify); break;
        default:                                            textBlockFormat.setAlignment(Qt::AlignLeading  | Qt::AlignAbsolute); break;
    }
    p.cursor()->setBlockFormat(textBlockFormat);
    // end code
    p.cursor()->clearSelection();

    drawAllCursor();
    //emit cursorPositionChanged();


    QTextBlock curblock=textEdit->textCursor().block();
    if(curblock.blockNumber()==numberblock){
        //qDebug()<<"SONO NEL IF DI CONFRONTO "<<numberblock;
        textEdit->textCursor().setBlockFormat(textBlockFormat);
        emit cursorPositionChanged();
    }

    //qDebug()<<"BLOCK FIND"<<bl.blockNumber();
}

void TextEdit::fromServerDelete(int pos,QString user){
    const QSignalBlocker blocker(textEdit->document());
    textEdit->textCursor().clearSelection();
    //qDebug()<<" REMOVE POS:"<<pos<<" user: "<<user;
    Presence p=onlineUsers_map.find(user).value();
    p.cursor()->clearSelection();
    p.cursor()->setPosition(pos+1,QTextCursor::MoveAnchor);
    p.cursor()->setPosition(pos,QTextCursor::KeepAnchor);
    //qDebug()<<"SELECTED TEXT TO REMOVE"<<p.cursor()->selectedText();
    p.cursor()->removeSelectedText();
    p.cursor()->clearSelection();
    drawAllCursor();
    Presence pp=onlineUsers_map.find(this->subscriber->username).value();
    drawCursor(pp);
}

void TextEdit::onUpdateListUsersConnected(int id, QString username, QImage img)
{
    qDebug()<<"TEXT EDIT - signal on update user";
    newPresence(id,username,img);
    drawAllCursor();
}

void TextEdit::onContentsChanged(int position, int charsRemoved, int charsAdded)
{
    //const QSignalBlocker blocker(textEdit->document());
    for (int i = 0; i < charsRemoved; ++i){
        emit localDeletionSignal(position);
    }
    QTextCursor cur;
    for (int i = position, pos = position; i < position + charsAdded ; i++)
    {
        //qDebug()<<"STO INSERENDO NEL FOR";
        cur = textEdit->textCursor();
        cur.setPosition(i+1);
        QTextCharFormat fmt = cur.charFormat();


        fmt.setProperty(QTextFormat::BlockAlignment,QVariant(cur.blockFormat().alignment()));
        //qDebug()<<fmt.properties().keys()<<"conunt"<<fmt.properties().count();
        //qDebug()<<fmt.properties().values()<<"conunt"<<fmt.properties().count();

        QChar ch = textEdit->document()->characterAt(i);
        //qDebug()<<"INSERT i="<<i<<" char="<<ch<<" italic="<<fmt.fontItalic()<<" underline="<<fmt.fontUnderline()<<" bold="<<fmt.fontWeight();
        if (ch != QChar::Null){
            if(ch==QChar::ParagraphSeparator)
                emit localInsertionSignal(QChar::ParagraphSeparator,i,fmt);
            else
                emit localInsertionSignal(ch,i,fmt);
        }
        //qDebug()<<"Align: "<<cur.blockFormat().alignment();
    }
    //Presence p=onlineUsers_map.find(this->subscriber->username).value();
    //drawCursor(p);
    //p.cursor()->setPosition(textEdit->textCursor().position(),QTextCursor::MoveAnchor);
    emit cursorPositionChanged();
    drawAllCursor();


}
void TextEdit::clipboardDataChanged()
{
    if (const QMimeData *md = QApplication::clipboard()->mimeData())
        actionPaste->setEnabled(!md->hasImage());
}

void TextEdit::about()
{
    QMessageBox::about(this, tr("About"), tr("This project demonstrates Qt's "
        "rich text editing facilities in action, providing an example "
        "document for you to experiment with. SAVERIO MASSANO ; MANISI COSIMO ; SILVIA FOIS"));
}

void TextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{

    //const QSignalBlocker blocker(textEdit->document());

    QTextCursor cursor = textEdit->textCursor();
    if (cursor.hasSelection()){
        qDebug()<<"MERGE SELECTION** "<<cursor.position();
        cursor.select(QTextCursor::WordUnderCursor);
    }
    textEdit->textCursor().mergeCharFormat(format);
    textEdit->mergeCurrentCharFormat(format);

}


void TextEdit::fontChanged(const QFont &f)
{
    comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
    comboSize->setCurrentIndex(comboSize->findText(QString::number(f.pointSize())));
    actionTextBold->setChecked(f.bold());
    actionTextItalic->setChecked(f.italic());
    actionTextUnderline->setChecked(f.underline());
}

void TextEdit::colorChanged(const QColor &c)
{
    QPixmap pix(16, 16);
    pix.fill(c);
    actionTextColor->setIcon(pix);
}

void TextEdit::alignmentChanged(Qt::Alignment a)
{
    if (a & Qt::AlignLeft)
        actionAlignLeft->setChecked(true);
    else if (a & Qt::AlignHCenter)
        actionAlignCenter->setChecked(true);
    else if (a & Qt::AlignRight)
        actionAlignRight->setChecked(true);
    else if (a & Qt::AlignJustify)
        actionAlignJustify->setChecked(true);
}

void TextEdit::drawCursor(const Presence &p)
{
    //Getting Presence's cursor and QLabel (to draw the graphic cursor)
    QLabel* userCursorLabel = p.label();
    userCursorLabel->close();
    const QRect qRect = textEdit->cursorRect(*p.cursor());

    //Draw the cursor (Pixmap)
    QPixmap pix(qRect.width() * 2.5, qRect.height());
    pix.fill(p.color());

    //Set the graphic cursor to the label
    userCursorLabel->setPixmap(pix);
    userCursorLabel->setVisible(true);
    userCursorLabel->move(qRect.left(), qRect.top());

    //Show the moved label
    userCursorLabel->update();
}

void TextEdit::newPresence(qint32 userId, QString username, QImage image)
{
    int randomNumber = 7 + (userId-1) % 11;

    //Pick a color
    QColor color = (Qt::GlobalColor) (randomNumber);
    Presence p(userId, username, color, image, textEdit);

    //Add user icon on user toolbar
    QAction* onlineAction = new QAction(QIcon(p.profilePicture()), username, onlineUsers);
    connect(onlineAction, &QAction::triggered, this, [p](){

        QMessageBox msgBox(QMessageBox::NoIcon,
                "",
                "I'm ' "+p.name()+ " ' and my ID is "+QString::number(p.id()),
                QMessageBox::Close);
        msgBox.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::WindowCloseButtonHint);
        msgBox.setIconPixmap(QPixmap(p.profilePicture()).scaled(60,60,Qt::KeepAspectRatio,Qt::SmoothTransformation));
        msgBox.exec();
    });

    onlineAction->setChecked(true);
    p.setAction(onlineAction);
    onlineUsers->addAction(onlineAction);
    onlineUsers_map.insert(p.name(), p);

    qDebug() << "users" << onlineUsers_map.size();

    p.cursor()->movePosition(QTextCursor::End,QTextCursor::MoveAnchor);
    qDebug()<< "NEW PRESENCE pos:"<<p.cursor()->position();
    p.cursor()->clearSelection();
}

void TextEdit::removePresence(QString userId)
{
    if (onlineUsers_map.contains(userId))
     {
         Presence& p = onlineUsers_map.find(userId).value();
         //Hide user cursor
         p.label()->clear();
         //Remove user icon from users toolbar
         onlineUsers->removeAction(p.actionHighlightText());
         //Remove from editor
         onlineUsers_map.remove(userId);
     }
}

void TextEdit::textSubscript()
{
    //Uncheck text Superscription
    actionTextSuperscript->setChecked(false);

    //Set Subscript format according to button
    QTextCharFormat fmt;
    fmt.setVerticalAlignment(actionTextSubscript->isChecked() ?
        QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal);

    //Apply format
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::textSuperscript()
{
    //Uncheck text Subscription
    actionTextSubscript->setChecked(false);

    //Set Superscript format according to button
    QTextCharFormat fmt;
    fmt.setVerticalAlignment(actionTextSuperscript->isChecked() ?
        QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal);

    //Apply format
    mergeFormatOnWordOrSelection(fmt);
}

void TextEdit::scriptChanged(QTextCharFormat::VerticalAlignment a)
{
    //Checks SubScript/SuperScript button according to the text's vertical alignment
    if (a == QTextCharFormat::AlignSubScript)
    {
        actionTextSubscript->setChecked(true);
        actionTextSuperscript->setChecked(false);
    }
    else if (a == QTextCharFormat::AlignSuperScript) {
        actionTextSubscript->setChecked(false);
        actionTextSuperscript->setChecked(true);
    }
    else {
        actionTextSubscript->setChecked(false);
        actionTextSuperscript->setChecked(false);
    }
}

