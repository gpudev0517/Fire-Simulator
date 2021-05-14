#include "NEShelfBar.h"

NEShelfBar::NEShelfBar(QWidget* parent) : QToolBar(parent)
{
    populateShelf();
}

uint NEShelfBar::addShelfButton(QString name, QString script, QString help, QString icon)
{
    NEShelfData d = { script, help, icon };
    m_ShelfData[name] = d;
    createAction(name, icon);
    return NE::kReturnSuccess;
}

uint NEShelfBar::addShelfButton(QString name, const NEShelfData& data)
{
    m_ShelfData[name] = data;
    createAction(name, data.icon);
    return NE::kReturnSuccess;
}

uint NEShelfBar::removeShelfButton(QString name)
{
    m_ActionToShelfData.remove(m_ShelfDataToAction[&m_ShelfData[name]]);
    QAction* a = m_ShelfDataToAction.take(&m_ShelfData[name]);
    removeAction(a);
    delete a;
    m_ShelfData.remove(name);
    return NE::kReturnSuccess;
}

QMap<QString, NEShelfData>& NEShelfBar::shelfData()
{
    return m_ShelfData;
}

void NEShelfBar::processAction()
{
    QAction* a = static_cast<QAction*>(sender());
    getPyMain().evalScript(m_ShelfData[a->text()].script);
}

uint NEShelfBar::createAction(QString name, QString icon)
{
    QIcon ic;
    ic.addFile(icon);
    QAction* a = addAction(ic, name, this, &NEShelfBar::processAction);
    a->setToolTip(name);
    m_ActionToShelfData[a] = &m_ShelfData[name];
    m_ShelfDataToAction[&m_ShelfData[name]] = a;
    NEShelfData& sd = m_ShelfData[name];
    a->setStatusTip(sd.help);
    a->setWhatsThis(sd.help);
    return NE::kReturnSuccess;
}

void NEShelfBar::populateShelf()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Neutrino Dynamics", "Neutrino");
    settings.beginGroup("ShelfStates");
    QStringList l1 = settings.childGroups();
    foreach(const QString& s, l1)
    {
        settings.beginGroup(s);
        bool enabled = settings.value("enabled", true).toBool();
        QString name = settings.value("name").toString();
        QString help = settings.value("help").toString();
        QString script = settings.value("script").toString();
        QString icon = settings.value("icon").toString();
        NEShelfData sd = { script, help, icon };
        removeShelfButton(name);
        if(enabled)
            addShelfButton(name, sd);
        settings.endGroup();
    }
    settings.endGroup();
    settings.sync();
}
