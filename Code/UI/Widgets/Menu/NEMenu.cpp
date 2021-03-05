#include "Widgets/Menu/NEMenu.h"

NEMenu::NEMenu(QWidget *parent): QMenu(parent)
{
    setWindowTitle(this->title());
}

void NEMenu::setTitle(const QString &title)
{
    QMenu::setTitle(title);
    setWindowTitle(title);

}
