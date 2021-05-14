#include "NECurveDialog.h"
#include "Dialogs/Curve/NECurveScene.h"
#include "Dialogs/Curve/NEEnumCurveScene.h"
#include "Manager/NECurveManager.h"
#include "NENode.h"
#include "Dialogs/Abstract/NEAbstractDialog.h"
#include "Utility/Curves/NECurve.h"
#include "Dialogs//Curve/NEKey.h"

NECurveDialog::NECurveDialog(NENode* node, QString propertyName, QWidget *parent, Qt::WindowFlags flags) : QDialog(parent, flags)
{
        setupUi(this);
        m_node = node;
        scene = NULL;

        // set up properties table
        QStringList label;
        propertiesTable->verticalHeader()->hide();
        propertiesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        propertiesTable->setShowGrid(false);

        QList<QString> keyedProperties = node->getKeyedProperties();
        QTableWidgetItem* item;
        QSet<QString> propertyNames;

        // iterate keyed properties and save propertyNames in
        // QSet to avoid duplicates
        for(int i=0; i<keyedProperties.size(); ++i)
        {
                QString prop = keyedProperties[i];
                QList<uint> keyframes = m_node->getKeyframesForProperty(&prop);
                QVariant val = m_node->getKeyframeValue(&prop, keyframes.first());
//		QVariant::Type type = val.type();

                // we currently do not show curves for composite types.
                // e.g. for vec3f the components are shown separately
                if(val.type() == (unsigned)val.userType())
                        propertyNames.insert(prop);
        }

        // write propertyNames to propertiesTable
        QList<QString> tmp = propertyNames.toList();
        qSort(tmp);
        for(int row = 0; row < tmp.size(); ++row)
        {
                item = new QTableWidgetItem(tmp[row]);
                item->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                propertiesTable->insertRow(row);
                propertiesTable->setItem(row, 0, item);
        }

        // set up combobox
        typeComboBox->addItem("NELinearCurve");
        typeComboBox->addItem("NEQuadraticCurve");
        typeComboBox->addItem("NECubicCurve");
        typeComboBox->addItem("NESinusoidalCurve");
        typeComboBox->addItem("NESinusoidalIncreaseCurve");
        typeComboBox->addItem("NESinusoidalDecreaseCurve");
//    typeComboBox->addItem("NETangentCurve");
        typeComboBox->addItem("NEStepCurve");

        //typeComboBox->addItem("Spline");
        typeComboBox->setEnabled(false);
        typeComboBox->setCurrentIndex(-1);

        // set up scene
        if(propertiesTable->findItems(propertyName, Qt::MatchStartsWith).size() > 0)
        {
                QTableWidgetItem* currentItem = propertiesTable->findItems(propertyName, Qt::MatchStartsWith)[0];

                selectedProperty = currentItem->text();
                propertiesTable->setCurrentItem(currentItem);
                initializeScene(selectedProperty);
        }

    this->setWindowTitle(tr("Curve Dialog"));

        // set connections for mode buttons
        connect(this->addKeyframeButton, SIGNAL(toggled(bool)), this, SLOT(toggleKeyframeMode()));
        connect(this->deleteKeyframeButton, SIGNAL(toggled(bool)), this, SLOT(toggleKeyframeMode()));
        connect(this->moveKeyframeButton, SIGNAL(toggled(bool)), this, SLOT(toggleKeyframeMode()));
        connect(this->graphicsView, SIGNAL(scenePosChanged(QPointF)), this, SLOT(onScenePosChanged(QPointF)));
        connect(propertiesTable, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(selectedPropertyChanged(QTableWidgetItem*)));
        connect(this->x_keyframe, SIGNAL(editingFinished()), this, SLOT(keyValueChanged()));
        connect(this->y_keyframe, SIGNAL(editingFinished()), this, SLOT(keyValueChanged()));
}

void NECurveDialog::fitInView()
{
        if(scene != NULL)
        {
                QList<NEKey *> keylist = scene->getKeylist().keys();

                // we do no fitting if the list is empty. could happen if the user
                // deleted all keys in the editor.
                if(keylist.empty())
                        return;

                qSort(keylist.begin(), keylist.end(), NEKey::greaterThan);
                int x_min = keylist.first()->x();
                int x_max = keylist.last()->x();

                QList<double> values;
                foreach(NEKey* key, keylist)
                        values << key->y();
                qSort(values);
                double y_min = values.first();
                double y_max = values.last();

                double offset = (y_max - y_min + 20.0) * 0.1;
                double min_val = y_min - offset;
                double max_val = y_max + offset;

                scene->setSceneRect(x_min - ((x_max - x_min) * 0.1), min_val , (x_max - x_min) + ((x_max - x_min) * 0.2), max_val - min_val);
                graphicsView->fitInView(scene->sceneRect());
        }
}


void NECurveDialog::initializeScene(QString propertyName)
{

        if(scene != NULL)
                delete(scene);

        // set up different curve scene depending of type of property
        uint index = m_node->metaObject()->indexOfProperty(propertyName.toUtf8());
        QMetaProperty metaProp = m_node->metaObject()->property(index);

        // compute xmin and xmax for scene rect
        QList<uint> keyframes = m_node->getKeyframesForProperty(&propertyName);
        if(keyframes.size() < 1)
        {
                return;
        }
        int x_min = keyframes.first();
        int x_max = keyframes.last();

        // catch all values for value range determination and key mapping
        QMap<int, QVariant> keys;
        QList<double> valueRange;

        foreach(int key, keyframes)
        {
                QVariant val = m_node->getKeyframeValue(&propertyName, key);
                keys.insert(key, val);
                valueRange.append(val.toFloat());
        }

        if(metaProp.isEnumType())
        {
                QMap<int, QString> enumVals;
                for(int x = 0; x <= metaProp.enumerator().keyCount(); ++x)
                {
                        QString key = metaProp.enumerator().key(x);
                        if(key != "")
                        {
                                int value = metaProp.enumerator().keyToValue(key.toUtf8());
                                enumVals.insert(value, key);
                        }
                }
                scene = new NEEnumCurveScene(graphicsView, enumVals);
                QList<int> enumRange = enumVals.keys();

                qSort(enumRange);

                double min_val = enumRange.first() - 1.0;
                double max_val = enumRange.last() + 1.0;

                scene->setSceneRect(x_min - ((x_max - x_min) * 0.1), min_val , (x_max - x_min) + ((x_max - x_min) * 0.2) + 10.0, max_val - min_val);

        }
        else
        {
                scene = new NECurveScene(graphicsView);

                // compute min and max of the range of this properties values
                qSort(valueRange);
                double offset = (valueRange.last() - valueRange.first() + 20.0) * 0.1;
                double min_val = valueRange.first() - offset;
                double max_val = valueRange.last() + offset;

                scene->setSceneRect(x_min - ((x_max - x_min) * 0.1), min_val , (x_max - x_min) + ((x_max - x_min) * 0.2) + 10.0, max_val - min_val);
        }

        // a gray background
        scene->setBackgroundBrush(QColor(100, 100, 100, 255));

        // add splines and curves
        for(int c = 0; c < keys.size(); ++c)
        {
                scene->addKey(new NEKey(QPointF(keys.keys()[c], keys.values()[c].toFloat()), keys.values()[c].type(), scene), NECURVE.getCurve(m_node, propertyName, keys.keys()[c]));
        }

        graphicsView->setScene(scene);
        graphicsView->show();
        graphicsView->fitInView(scene->sceneRect());

        connect(scene, SIGNAL(selectedKeyframeChanged(NEKey*, NECurve *)), this, SLOT(setCurveType(NEKey*, NECurve*)));
        connect(this->typeComboBox, SIGNAL(currentIndexChanged(const QString)), scene, SLOT(changeCurveType(const QString)));
        connect(scene, SIGNAL(selectedKeyframeMoved(NEKey*)), this, SLOT(currentKeyMoved(NEKey*)));
}

void NECurveDialog::selectedPropertyChanged(QTableWidgetItem* item)
{
        selectedProperty = item->text();
        this->moveKeyframeButton->setChecked(true);
        initializeScene(item->text());
}


NECurveDialog::~NECurveDialog()
{

}

QGraphicsScene *NECurveDialog::getScene() { return scene; }

void NECurveDialog::showEvent(QShowEvent* event)
{
    // this is done to make sure, the scene
        // is scaled correctly when the dialog is initialized
        if(scene != NULL)
                graphicsView->fitInView(scene->sceneRect());
}

void NECurveDialog::onScenePosChanged(QPointF point)
{
        if(scene != NULL)
        {
                this->x_ScenePos->setText(QString::number(point.x()));
                this->y_ScenePos->setText(QString::number(point.y()));
                double fx = scene->getValueForX(point.x());
                this->fx_ScenePos->setText(QString::number(fx));
        }
}

void NECurveDialog::accept()
{
        if(scene != NULL)
        {
                // we remove all keyframes
                m_node->removeKeyframesForProperty(selectedProperty);

                QMap<NEKey*, NECurve*> keylist = scene->getKeylist();

                // and add the keyframes from the editor
                for( int count = 0; count < keylist.size(); ++count)
                {
                        m_node->addKeyframe(keylist.keys()[count]->x(), selectedProperty, keylist.keys()[count]->getValue(), NE::kLinear, keylist.values()[count]);
                }
        }

        QDialog::accept();
}

void NECurveDialog::apply()
{
        if(scene != NULL)
        {
                // we remove all keyframes
                m_node->removeKeyframesForProperty(selectedProperty);

                QMap<NEKey*, NECurve*> keylist = scene->getKeylist();

                // and add the keyframes from the editor
                for( int count = 0; count < keylist.size(); ++count)
                {
                        m_node->addKeyframe(keylist.keys()[count]->x(), selectedProperty, keylist.keys()[count]->getValue(), NE::kLinear, keylist.values()[count]);
                }
        }
}


void NECurveDialog::toggleKeyframeMode()
{
        if(scene != NULL)
        {
                if(this->addKeyframeButton->isChecked())
                        this->scene->setMode(NECurveScene::selectionMode::addKeyframe);
                else if(this->deleteKeyframeButton->isChecked())
                        this->scene->setMode(NECurveScene::selectionMode::deleteKeyframe);
                else if(this->moveKeyframeButton->isChecked())
                        this->scene->setMode(NECurveScene::selectionMode::moveKeyframe);
        }

}

void NECurveDialog::setCurveType(NEKey* key, NECurve* curve)
{
        if(key)
        {
                m_currentKey = key;
                this->typeComboBox->setEnabled(true);
                this->typeComboBox->setCurrentIndex(this->typeComboBox->findText(curve->name()));
                this->selectedKeyframeBox->setEnabled(true);
                this->x_keyframe->setText(QString::number(key->x()));
                this->y_keyframe->setText(QString::number(key->y()));
        }
        else
        {
                m_currentKey = NULL;
                this->typeComboBox->setEnabled(false);
                this->typeComboBox->setCurrentIndex(-1);
                this->selectedKeyframeBox->setEnabled(false);
                this->x_keyframe->setText("");
                this->y_keyframe->setText("");
        }
}

void NECurveDialog::currentKeyMoved(NEKey *key)
{
        this->x_keyframe->setText(QString::number(key->x()));
        this->y_keyframe->setText(QString::number(key->y()));
}

void NECurveDialog::keyValueChanged()
{
        if(m_currentKey)
        {
                // validate x value and set value or reset value according to validity
                QIntValidator* xValidate = new QIntValidator();
        int pos = 0;

                xValidate->setBottom(0);

        QString ktext1 = this->x_keyframe->text();

        if(xValidate->validate(ktext1, pos) == QValidator::Acceptable)
                {
                        m_currentKey->setX(this->x_keyframe->text().toDouble());
                }
                else
                {
                        this->x_keyframe->setText(QString::number(m_currentKey->x()));
                }

                // validate y value and set value or reset value according to validity
                QDoubleValidator* yValidate = new QDoubleValidator();
                this->y_keyframe->setValidator(yValidate);
        QString ktext2 = this->y_keyframe->text();
        int dpos = (double)pos;
        if(yValidate->validate(ktext2, dpos) == QValidator::Acceptable)
                {
                        m_currentKey->setY(this->y_keyframe->text().toDouble());
                }
                else
                {
                        this->y_keyframe->setText(QString::number(m_currentKey->y()));
                }

                scene->moveKey(m_currentKey);
        }
}

