#include "Widgets/Menu/FDYNMenu.h"

FDYNMenu::FDYNMenu(QWidget *parent): QMenu(parent)
{
    setWindowTitle(this->title());
}

void FDYNMenu::setTitle(const QString &title)
{
    QMenu::setTitle(title);
    setWindowTitle(title);

}
