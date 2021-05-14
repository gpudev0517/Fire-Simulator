#pragma once

class QXmlStreamWriter;
class QXmlStreamReader;

class  NECEXP_CORE NEColorRamp
{
public:
    NEColorRamp();

    QVector<QPair<qreal, QColor>> getColorRamp() const;
    QVector<QPair<qreal, QColor>>& getColorRampRef();
    QVector<QPair<qreal, QColor>>* getColorRampPtr();
    void setColorRamp(QVector<QPair<qreal, QColor>> val);
    bool saveToFile(QString fileName, QString folderName, QXmlStreamWriter* xmlWriter);
    bool loadFromFile(QString fileName, QString folderName, QXmlStreamReader* xmlReader);
    bool saveToPreset();
    bool loadFromPreset();
    void setInitialColorRampPreset(bool isOverwrite = false);
    bool saveToPresetList(QMap<QString, NEColorRamp*> *colorMap);
    bool loadFromPresetList(QMap<QString, NEColorRamp*> *colorMap);

    operator QVariant() const;
    bool operator==(const NEColorRamp& other) const;


private:
	//!
    QVector<QPair<qreal, QColor>> m_ColorRamp;
};

Q_DECLARE_METATYPE(NEColorRamp)
