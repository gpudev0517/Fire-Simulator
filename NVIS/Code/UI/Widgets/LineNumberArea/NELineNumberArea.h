#pragma once

template <class T>
class NELineNumberArea : public QWidget
{
public:
	NELineNumberArea(T *editor) : QWidget(editor)
	{
		codeEditor = editor;
	}

	QSize sizeHint() const
	{
		return QSize(codeEditor->lineNumberAreaWidth(), 0);
	}

protected:
	void paintEvent(QPaintEvent *event)
	{
		codeEditor->lineNumberAreaPaintEvent(event);
	}

private:
	T *codeEditor;
};

