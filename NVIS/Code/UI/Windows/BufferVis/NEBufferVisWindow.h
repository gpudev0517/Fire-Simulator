#pragma once


class NEGPUBufferVis;

namespace Ui {
class NEBufferVisWindow;
}

class NEBufferVisWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit NEBufferVisWindow(QWidget *parent = 0);
    ~NEBufferVisWindow();

    NEGPUBufferVis* bufferVis();



private:
    Ui::NEBufferVisWindow *ui;
};
