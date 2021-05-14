#include "NEExpressionEditor.h"
#include "Widgets/LineNumberArea/NELineNumberArea.h"
#include "Managers/NEGUIManager.h"


NEExpressionEditor::NEExpressionEditor(QWidget* parent)
    : QPlainTextEdit(parent) {

    _defaultTextCharacterFormat = currentCharFormat();
    m_HistoryPosition            = 0;
    m_HadError                   = false;

    m_Completer = new QCompleter(this);
    m_Completer->setWidget(this);
    m_ClearAfterExecute = false;

    m_LineNumberArea = new NELineNumberArea<NEExpressionEditor>(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();


    QObject::connect(m_Completer, SIGNAL(activated(const QString&)), this, SLOT(insertCompletion(const QString&)));
}

void NEExpressionEditor::init(QPlainTextEdit* outputWin)
{
    outputWindow = outputWin;

    this->setTabStopWidth(20);
    outputWindow->setTabStopWidth(20);

    clear();
}

void NEExpressionEditor::connectPythonStdOutErr()
{
    //	connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this, SLOT(stdOut(const QString&)));
    //	connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this, SLOT(stdErr(const QString&)));
}

//-----------------------------------------------------------------------------

void NEExpressionEditor::stdOut(const QString& s)
{
    m_StdOut += s;
    int idx;
    while ((idx = m_StdOut.indexOf('\n'))!=-1)
    {
        consoleMessage(m_StdOut.left(idx));
        std::cout << m_StdOut.left(idx).toLatin1().data() << std::endl;
        m_StdOut = m_StdOut.mid(idx+1);
    }
}

void NEExpressionEditor::stdErr(const QString& s)
{
    m_HadError = true;
    m_StdErr += s;
    int idx;
    while ((idx = m_StdErr.indexOf('\n'))!=-1)
    {
        consoleMessage(m_StdErr.left(idx));
        std::cerr << m_StdErr.left(idx).toLatin1().data() << std::endl;
        m_StdErr = m_StdErr.mid(idx+1);
    }
}

void NEExpressionEditor::flushStdOut()
{
    if (!m_StdOut.isEmpty())
        stdOut("\n");

    if (!m_StdErr.isEmpty())
        stdErr("\n");
}

//-----------------------------------------------------------------------------

NEExpressionEditor::~NEExpressionEditor()
{
}



//-----------------------------------------------------------------------------

void NEExpressionEditor::clear()
{
    //outputWindow->clear();
    QPlainTextEdit::clear();
}


void NEExpressionEditor::executeCode(const QString& code)
{
    getPyMain().evalScript(code);

    if(UpdateUI())
    {
        outputWindow->appendHtml("<font color=#003A00>" + code + "</font>");
        flushStdOut();
        NEGUI.updateSceneInspector();
        NEGUI.updateNodeGraphEditor();
        NEGUI.updateSystemInspector();
        NEGUI.updatePropertyEditor();
        NEGUI.updateGLWindows();
    }
}

//-----------------------------------------------------------------------------

void NEExpressionEditor::setCurrentFont(const QColor& color, bool bold) {

    QTextCharFormat charFormat(_defaultTextCharacterFormat);

    QFont font(charFormat.font());
    font.setBold(bold);
    charFormat.setFont(font);

    QBrush brush(charFormat.foreground());
    brush.setColor(color);
    charFormat.setForeground(brush);

    setCurrentCharFormat(charFormat);
}



//-----------------------------------------------------------------------------

int NEExpressionEditor::commandPromptPosition()
{
    QTextCursor textCursor(this->textCursor());
    textCursor.movePosition(QTextCursor::End);

    return textCursor.block().position();
}



//-----------------------------------------------------------------------------

void NEExpressionEditor::insertCompletion(const QString& completion)
{
    QTextCursor tc = textCursor();
    tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
    if (tc.selectedText()==".")
        tc.insertText(QString(".") + completion);
    else
    {
        tc = textCursor();
        tc.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
        tc.movePosition(QTextCursor::EndOfWord, QTextCursor::KeepAnchor);
        tc.insertText(completion);
        setTextCursor(tc);
    }
}

//-----------------------------------------------------------------------------
void NEExpressionEditor::handleTabCompletion()
{
    QTextCursor textCursor   = this->textCursor();
    int pos = textCursor.position();
    textCursor.setPosition(commandPromptPosition());
    textCursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    int startPos = textCursor.selectionStart();

    int offset = pos-startPos;
    QString text = textCursor.selectedText();

    QString textToComplete;
    int cur = offset;
    if(!text.isEmpty())
    {
        while (cur--)
        {
            QChar c = text.at(cur);
            if (c.isLetterOrNumber() || c == '.' || c == '_') {
                textToComplete.prepend(c);
            } else {
                break;
            }
        }
    }



    QString lookup;
    QString compareText = textToComplete;
    int dot = compareText.lastIndexOf('.');
    if (dot!=-1) {
        lookup = compareText.mid(0, dot);
        compareText = compareText.mid(dot+1, offset);
    }
    if (!lookup.isEmpty() || !compareText.isEmpty()) {
        compareText = compareText.toLower();
        QStringList found;
        QStringList variables = PythonQt::self()->introspection(getPyMain(), lookup, PythonQt::Anything);
        //QStringList functions = PythonQt::self()->introspection(m_Context, lookup, PythonQt::Function);
        //QStringList classes = PythonQt::self()->introspection(m_Context, lookup, PythonQt::Class);
        //QStringList modules = PythonQt::self()->introspection(m_Context, lookup, PythonQt::Module);
        //QStringList calloverloads = PythonQt::self()->introspection(m_Context, lookup, PythonQt::CallOverloads);
        foreach (QString n, variables) {
            if (n.toLower().startsWith(compareText)) {
                found << n;
            }
        }

        if (!found.isEmpty()) {
            m_Completer->setCompletionPrefix(compareText);
            m_Completer->setCompletionMode(QCompleter::PopupCompletion);
            m_Completer->setModel(new QStringListModel(found, m_Completer));
            //QStringListModel::
            m_Completer->setCaseSensitivity(Qt::CaseInsensitive);
            QTextCursor c = this->textCursor();
            c.movePosition(QTextCursor::StartOfWord);
            QRect cr = cursorRect(c);
            cr.setWidth(m_Completer->popup()->sizeHintForColumn(0)
                        + m_Completer->popup()->verticalScrollBar()->sizeHint().width());
            cr.translate(0,8);
            m_Completer->complete(cr);
        } else {
            m_Completer->popup()->hide();
        }
    } else {
        m_Completer->popup()->hide();
    }
}

void NEExpressionEditor::keyPressEvent(QKeyEvent* event) {

    if (m_Completer && m_Completer->popup()->isVisible())
    {
        // The following keys are forwarded by the completer to the widget
        switch (event->key())
        {
        case Qt::Key_Return:
            if (!m_Completer->popup()->currentIndex().isValid())
            {
                insertCompletion(m_Completer->currentCompletion());
                m_Completer->popup()->hide();
                event->accept();
            }
            event->ignore();
            return;
            break;
        case Qt::Key_Enter:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:

            event->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }
    bool        eventHandled = false;
    QTextCursor textCursor   = this->textCursor();

    int key = event->key();
    switch (key) {

    case Qt::Key_Return:

        if(event->modifiers() == Qt::ControlModifier)
        {
            executeCode(this->toPlainText());
            if(m_ClearAfterExecute)
                this->clear();
            eventHandled = true;
            break;
        }
    default:
        break;
    }

    if (eventHandled)
    {
        m_Completer->popup()->hide();
        event->accept();
    }
    else
    {
        QPlainTextEdit::keyPressEvent(event);
        QString text = event->text();
        if (!text.isEmpty())
            handleTabCompletion();
        else
            m_Completer->popup()->hide();
        eventHandled = true;
    }
}


//-----------------------------------------------------------------------------

void NEExpressionEditor::changeHistory()
{

    // Select the text after the last command prompt ...
    QTextCursor textCursor = this->textCursor();
    textCursor.movePosition(QTextCursor::End);
    textCursor.setPosition(commandPromptPosition(), QTextCursor::KeepAnchor);

    // ... and replace it with the history text.
    textCursor.insertText(m_History.value(m_HistoryPosition));

    textCursor.movePosition(QTextCursor::End);
    setTextCursor(textCursor);
}



//-----------------------------------------------------------------------------

void NEExpressionEditor::consoleMessage(const QString & message)
{
    outputWindow->moveCursor(QTextCursor::End);
    outputWindow->appendPlainText(QString());
    outputWindow->insertPlainText(message);
}
bool NEExpressionEditor::clearAfterExecute() const
{
    return m_ClearAfterExecute;
}

void NEExpressionEditor::setClearAfterExecute(bool val)
{
    m_ClearAfterExecute = val;
}


void NEExpressionEditor::lineNumberAreaPaintEvent( QPaintEvent *event )
{

    QPainter painter(m_LineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    //![extraAreaPaintEvent_0]

    //![extraAreaPaintEvent_1]
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    //![extraAreaPaintEvent_1]

    //![extraAreaPaintEvent_2]
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, m_LineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int NEExpressionEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void NEExpressionEditor::resizeEvent( QResizeEvent* event )
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    m_LineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void NEExpressionEditor::updateLineNumberAreaWidth( int newBlockCount )
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void NEExpressionEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QGuiApplication::palette().color(QPalette::Window);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);

}

void NEExpressionEditor::updateLineNumberArea( const QRect& rect, int dy)
{
    if (dy)
        m_LineNumberArea->scroll(0, dy);
    else
        m_LineNumberArea->update(0, rect.y(), m_LineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);

}
