#include "NEPreferencesDialog.h"
#include "ui_NEPreferencesDialog.h"
#include "Managers/NEGUIManager.h"
#include "Base/NESceneManager.h"
#include "Manager/NESimulationManager.h"
#include "Manager/NESolidManager.h"
#include "Rendering/Manager/NERenderNEOManager.h"
#include "Windows/PropertyManager/NEObjectController.h"
#include "Manager/NEGeometryManager.h"
#include "Base/NENodeFactory.h"
#include "Dialogs/PluginInfo/NEPluginInfoDialog.h"
#include "Windows/PluginManager/NEPluginsTreeWidget.h"
#include "NENode.h"
#include "NEShelfItemEditor.h"
#include "Widgets/ShelfBar/NEShelfBar.h"
#include "Windows/Main/NEMainWindow.h"

NEPreferencesDialog::NEPreferencesDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::NEPreferencesDialog)
{
	ui->setupUi(this);
	m_pPluginInfoDialog = new NEPluginInfoDialog(this);

	connect(ui->saveButton, &QPushButton::clicked, this, &NEPreferencesDialog::saveUserPreferences);
	connect(ui->closeButton, &QPushButton::clicked, this, &NEPreferencesDialog::close);
	connect(ui->restoreButton, &QPushButton::clicked, this, &NEPreferencesDialog::restorePageDefaults);
	connect(ui->treeWidget, &QTreeWidget::clicked, this, &NEPreferencesDialog::changePrefPage);
	connect(ui->pluginsTree, &QTreeWidget::clicked, ui->pluginsTree, &QTreeWidget::activated);
	connect(ui->pluginsTree, &NEPluginsTreeWidget::itemCheckStateChanged, this, &NEPreferencesDialog::reactToPluginCheck);
	connect(ui->pluginsTree, &QTreeWidget::doubleClicked, this, &NEPreferencesDialog::showPluginInfo);
	connect(ui->shelfTree, &QTreeWidget::doubleClicked, this, &NEPreferencesDialog::editShelfItem);
	connect(ui->shelfAddButton, &QPushButton::clicked, this, &NEPreferencesDialog::addShelfItem);
	connect(ui->shelfRemoveButton, &QPushButton::clicked, this, &NEPreferencesDialog::removeShelfItem);
	connect(ui->shelfEditButton, &QPushButton::clicked, this, &NEPreferencesDialog::editShelfItem);

	m_pSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Neutrino Dynamics", "Neutrino");
	ui->interfacePage->setObject(NEGUI.NEExternalProperties::P());
	if(NESCENE.initialized())
	{
		populatePluginsPage();
		loadUserPreferences();
	}
}

NEPreferencesDialog::~NEPreferencesDialog()
{
	delete ui;
}

void NEPreferencesDialog::saveObjectProperties(const QObject& o)
{
	const QMetaObject* m = o.metaObject();
	for(int i = 0; i < m->propertyCount(); ++i)
	{
		const QMetaProperty& mp = m->property(i);
		if(mp.isWritable())
			m_pSettings->setValue(mp.name(), mp.read(&o));
	}

}

void NEPreferencesDialog::loadObjectProperties(QObject& o, QString group)
{
	const QMetaObject* m = o.metaObject();
	for(int i = 1; i < m->propertyCount(); ++i)
	{
		QMetaProperty mp = m->property(i);
		if(mp.isWritable())
		{
			mp.write(&o, m_pSettings->value(group + '/' + mp.name()));
		}
	}
}

void NEPreferencesDialog::populatePluginsPage()
{
	ui->pluginsTree->clear();
    insertManager(NESCENE.geometryManager().get(), &m_pGeoMgrGroup, "Geometry Plugins");
    insertManager((NEManager*)NESIMMGR.collisionManager().get(), &m_pCOLMgrGroup, "Collision Detection Plugins");
    insertManager((NEManager*)&NESIMMGR, &m_pSIMMgrGroup, "Deamon Plugins");
    insertManager((NEManager*)NESIMMGR.solidManager()->rigidManager().get(), &m_pRGDMgrGroup, "Rigid-Body Plugins");
    insertManager((NEManager*)NESIMMGR.sphLiquidManager().get(), &m_pSPHMgrGroup, "Solver Plugins");
    insertManager((NEManager*)NESCENE.dataFieldManager().get(), &m_pDFMgrGroup, "Data Field Plugins");
    insertManager((NEManager*)NESCENE.postProcessorManager().get(), &m_pPPMgrGroup, "Post-Processor Plugins");
#if NE_GRIDFLUID
	insertManager((NEManager*)NESIMMGR.gridFluidManager(), &m_pGFMgrGroup, "Grid-Based Solver Plugins");
#endif
    //insertManager((NEManager*)NESIMMGR.MSSManager(), &m_pMSSMgrGroup, "Mass-Spring Solver Plugins");
#if NE_DEFORMABLE
    insertManager((NEManager*)NESIMMGR.solidManager()->deformableManager().get(), &m_pDEFMgrGroup, "Deformable-Body Plugins");
#endif
    insertManager((NEManager*)&NERENDERMAN, &m_pRENMgrGroup, "Renderer Plugins");
}

void NEPreferencesDialog::insertManager(NEManager* m, QTreeWidgetItem** item2, QString name)
{
	if(!m->dynaFactories().isEmpty())
	{
		QStringList sl;
		sl.append(name);
		QTreeWidgetItem* item = new QTreeWidgetItem(sl);
		*item2 = item;
		ui->pluginsTree->addTopLevelItem(item);
		loadManagerPlugins(m, item);
	}
}

void NEPreferencesDialog::loadManagerPlugins(NEManager* m, QTreeWidgetItem* pi)
{
	if(m->dynaFactories().isEmpty())
		return;

	QMap<QString, NEDynaFactoryInfo>::iterator i = m->dynaFactories().begin();
	for(; i != m->dynaFactories().end(); ++i)
	{
        QStringList sl;
		sl.append(i->nf->nodeName());
		sl.append(i->filename);
		NEPrefTreeWidgetItem* item = new NEPrefTreeWidgetItem(sl);

        NENodeIcon icon = *i->nf->nodeIcon();
        QIcon qicon;
        for(int i = 0; i < icon.size(); i++)
        {
            qicon.addFile(icon[i].iconPath, icon[i].iconSize);
        }

        item->setIcon(0, qicon);
		item->setDynaFactory(&*i);
		pi->addChild(item);

	}
}

void NEPreferencesDialog::savePluginStates()
{
	savePluginStates(m_pSIMMgrGroup);
	savePluginStates(m_pGeoMgrGroup);
	savePluginStates(m_pDFMgrGroup);
	savePluginStates(m_pPPMgrGroup);
	savePluginStates(m_pSPHMgrGroup);
	savePluginStates(m_pGFMgrGroup);
    //savePluginStates(m_p3MgrGroup);
	savePluginStates(m_pCOLMgrGroup);
	savePluginStates(m_pDEFMgrGroup);
	savePluginStates(m_pRGDMgrGroup);
	savePluginStates(m_pRENMgrGroup);
}

void NEPreferencesDialog::savePluginStates(QTreeWidgetItem* item)
{
	if(!item)
		return;

	for(int i = 0; i < item->childCount(); ++i)
	{
		NEPrefTreeWidgetItem* it = static_cast<NEPrefTreeWidgetItem*>(item->child(i));

		m_pSettings->setValue(it->dynaFactory()->filename, (bool)(it->checkState(0) >> 1));
	}

}

void NEPreferencesDialog::loadPluginStates()
{
	loadPluginStates(m_pSIMMgrGroup);
	loadPluginStates(m_pGeoMgrGroup);
	loadPluginStates(m_pDFMgrGroup);
	loadPluginStates(m_pPPMgrGroup);
	loadPluginStates(m_pSPHMgrGroup);
	loadPluginStates(m_pGFMgrGroup);
    //loadPluginStates(m_pMSSMgrGroup);
	loadPluginStates(m_pCOLMgrGroup);
	loadPluginStates(m_pDEFMgrGroup);
	loadPluginStates(m_pRGDMgrGroup);
	loadPluginStates(m_pRENMgrGroup);
}

void NEPreferencesDialog::loadPluginStates(QTreeWidgetItem* item)
{
	if(!item)
		return;

	for(int i = 0; i < item->childCount(); ++i)
	{
		NEPrefTreeWidgetItem* it = static_cast<NEPrefTreeWidgetItem*>(item->child(i));
		bool b = m_pSettings->value("PluginStates/" + it->dynaFactory()->filename).toBool();

		it->setCheckState(0, (Qt::CheckState)(int(b) << 1));
	}
}

void NEPreferencesDialog::saveShelfData()
{
	NEShelfItemEditor* e = new NEShelfItemEditor(this);
	NEShelfBar* b = NEGUI.mainWindow()->shelfBar();

    for(int i = 0; i < ui->shelfTree->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = ui->shelfTree->topLevelItem(i);
        QString name = item->text(0);
        b->removeShelfButton(name);
        NEShelfData sd = { item->text(2), item->text(1), item->text(3) };
        if(item->checkState(0)>>1)
            b->addShelfButton(name, sd);

//        b->shelfData()[name] = sd;
        m_pSettings->beginGroup(name);
        m_pSettings->setValue("enabled", item->checkState(0)>>1);
        m_pSettings->setValue("name", name);
        m_pSettings->setValue("help", sd.help);
        m_pSettings->setValue("script", sd.script);
        m_pSettings->setValue("icon", sd.icon);
        m_pSettings->endGroup();
    }
}

void NEPreferencesDialog::loadShelfData()
{
    ui->shelfTree->clear();

    m_pSettings->beginGroup("ShelfStates");
    QStringList l1 = m_pSettings->childGroups();
    foreach(const QString& s, l1)
    {
        m_pSettings->beginGroup(s);
        bool enabled = m_pSettings->value("enabled", true).toBool();
        QString name = m_pSettings->value("name").toString();
        QString help = m_pSettings->value("help").toString();
        QString script = m_pSettings->value("script").toString();
        QString icon = m_pSettings->value("icon").toString();
        QTreeWidgetItem* twi = new QTreeWidgetItem();
        twi->setFlags(twi->flags() | Qt::ItemIsUserCheckable);
        twi->setCheckState(0, (Qt::CheckState)(enabled<<1));

        twi->setText(0, name);
        twi->setText(1, help);
        twi->setText(2, script);
        twi->setText(3, icon);
        QIcon ic;
        ic.addFile(icon);
        twi->setIcon(3, ic);
        ui->shelfTree->addTopLevelItem(twi);
        m_pSettings->endGroup();
    }
    m_pSettings->endGroup();
    m_pSettings->sync();
}

void NEPreferencesDialog::populateShelfPage()
{

}

void NEPreferencesDialog::showEvent(QShowEvent* e)
{
	populatePluginsPage();
	loadUserPreferences();
}

void NEPreferencesDialog::saveUserPreferences()
{
	m_pSettings->clear();
	m_pSettings->beginGroup("UIProperties");
	saveObjectProperties(*NEGUI.NEExternalProperties::P());
	m_pSettings->endGroup();
	m_pSettings->beginGroup("PluginStates");
	savePluginStates();
	m_pSettings->endGroup();
	m_pSettings->beginGroup("ShelfStates");
	saveShelfData();
	m_pSettings->endGroup();
	m_pSettings->sync();
}

void NEPreferencesDialog::loadUserPreferences()
{
	loadObjectProperties(*NEGUI.NEExternalProperties::P(), "UIProperties");
	ui->interfacePage->updateProperties();
	loadPluginStates();
	loadShelfData();

	m_pSettings->sync();
}

void NEPreferencesDialog::changePrefPage(const QModelIndex& mi)
{
	ui->stackedWidget->setCurrentIndex(mi.row());
}

void NEPreferencesDialog::restorePageDefaults()
{

}

void NEPreferencesDialog::showPluginInfo()
{
	NEPrefTreeWidgetItem* item = static_cast<NEPrefTreeWidgetItem*>(ui->pluginsTree->currentItem());
	if(item->parent() == ui->pluginsTree->invisibleRootItem() || item->parent() == 0)
		return;

	NEDynaFactoryInfo* f = item->dynaFactory();
	m_pPluginInfoDialog->setName(f->nf->nodeName());
	m_pPluginInfoDialog->setHelp(f->nf->nodeHelp());
	m_pPluginInfoDialog->setVendor(f->nf->nodeVendor());
	m_pPluginInfoDialog->setVersion(f->nf->nodeVersion());
	m_pPluginInfoDialog->setFilename(f->filename);
	m_pPluginInfoDialog->setPath(f->path);
	m_pPluginInfoDialog->show();
}

void NEPreferencesDialog::reloadPlugin(NEPrefTreeWidgetItem* item)
{
	if(!item)
		item = static_cast<NEPrefTreeWidgetItem*>(ui->pluginsTree->currentItem());

	if(!item)
		return;
	NEDynaFactoryInfo* f = item->dynaFactory();
//	f->nf->manager()->unregisterDynaNode(f);
	f->nf->manager()->registerDynaNode(f->nf, f->path + "/" + f->filename);
	recreateNodes(f);
}

void NEPreferencesDialog::unloadPlugin(NEPrefTreeWidgetItem* item)
{
	if(!item)
		item = static_cast<NEPrefTreeWidgetItem*>(ui->pluginsTree->currentItem());
	if(!item)
		return;

	NEDynaFactoryInfo* f = item->dynaFactory();
	deleteNodes(f);
	f->nf->manager()->unregisterDynaNode(f);

//	static_cast<QWidget*>(sender())->close();
	//	populatePluginsPage();
}

void NEPreferencesDialog::reactToPluginCheck(NEPrefTreeWidgetItem* item, bool b)
{
	saveUserPreferences();
	if(!b)
	{
		unloadPlugin(item);
	}
	else
	{
		reloadPlugin(item);
	}
}

void NEPreferencesDialog::deleteNodes(NEDynaFactoryInfo* f)
{
	bool deleted = false;
	foreach(QObject* o, NESCENE.children())
	{
		if(NENode* n = qobject_cast<NENode*>(o))
		{
			if(f->nf == &n->factory())
			{
				NESCENE.deleteObject(n, false);
				deleted = true;
			}

		}
	}
	if(deleted)
		NEGUI.updateSceneInspector();
}

NEDynaFactoryInfo* NEPrefTreeWidgetItem::dynaFactory() const
{
	return m_pDFactoryInfo;
}

void NEPrefTreeWidgetItem::setDynaFactory(NEDynaFactoryInfo* val)
{
	m_pDFactoryInfo = val;
}

void NEPreferencesDialog::recreateNodes(NEDynaFactoryInfo* f)
{
	QList<NENode*> nl;
	bool deleted = false;
	foreach(QObject* o, NESCENE.children())
	{
		if(NENode* n = qobject_cast<NENode*>(o))
		{
			if(f->nf == &n->factory())
			{
				nl.push_back(n);
				deleted = true;
			}
		}
	}

	foreach(NENode* n, nl)
	{
		NENode* c = NESCENE.createObject(n->factory().nodeName(), n->objectName() + "_TMP_", &NESCENE);
		QString name = n->objectName();
		c->cloneNode(n);
		NESCENE.deleteObject(n, false);
		c->setObjectName(name);
	}

	if(deleted)
		NEGUI.updateSceneInspector();
}

void NEPreferencesDialog::editShelfItem()
{
	if(!ui->shelfTree->currentItem())
		return;
	NEShelfItemEditor* e = new NEShelfItemEditor(this);
	NEShelfBar* b = NEGUI.mainWindow()->shelfBar();
	QString name = ui->shelfTree->currentItem()->text(0);
	QString help = ui->shelfTree->currentItem()->text(1);
	QString script = ui->shelfTree->currentItem()->text(2);
	QString icon = ui->shelfTree->currentItem()->text(3);

	e->populateEditor(name, help, script, icon, ui->shelfTree->currentItem());
	e->show();
}

void NEPreferencesDialog::addShelfItem()
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
	item->setCheckState(0, (Qt::CheckState)1);
	ui->shelfTree->addTopLevelItem(item);
	item->setSelected(true);
	editShelfItem();
}

void NEPreferencesDialog::removeShelfItem()
{
	if(!ui->shelfTree->currentItem())
		return;
	delete ui->shelfTree->currentItem();
}
