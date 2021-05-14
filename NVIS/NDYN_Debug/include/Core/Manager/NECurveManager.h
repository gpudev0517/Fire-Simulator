#pragma once

class NECurve;

class NECEXP_CORE NECurveManager final : public QObject
{
	Q_OBJECT
	friend class NEBase;
    friend class NEBasePrivate;
public:

    static NECurveManager& Instance();
	//! Adds a give curve for the given object, property and frame
	void addCurve(QObject* obj, const QString propertyName, uint frameBegin, NECurve* curve);
	//! Adds a curve of given cuveType name for the given object, property and frame
	void addCurve(QObject* obj, const QString propertyName, uint frameBegin, QString curveType);
	//! Adds a curve of given type for the given object, property and frame
	void addCurve(QObject* obj, const QString propertyName, uint frameBegin, NE::NECurveType type);
	//! Removes a curve for the given frame, propertyname and Object
	void removeCurve(QObject* obj, const QString propertyName, uint frameBegin);
	//! Returns the curve for the given object, propertyName and frame
	NECurve* getCurve(QObject* obj, const QString propertyName, uint frameBegin);
	//! Checks, if a curve is existing for the given object, propertyName and frame
	bool curveExists(QObject* obj, const QString propertyName, uint frameBegin);
	//! Returns the string representation of the queried curve (used for saving)
	QString curveName(QObject* obj, const QString propertyName, uint frameBegin);
	//! Factory method. Creates a curve for the given string representation (or Linear if none)
	NECurve* createCurve(QString curveType = "NELinearCurve");

private:
    void init();
	QString getKey(QObject* obj, const QString propertyName, uint frameBegin);
    QMap<QString, NECurve*>* m_keyToCurve; /**< Pointer from keys to curves */
};

#define NECURVE NECurveManager::Instance()
