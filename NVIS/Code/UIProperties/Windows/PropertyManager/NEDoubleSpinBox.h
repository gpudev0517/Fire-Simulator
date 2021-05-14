#pragma once


class NECEXP_NEUIPROPERTIES NEDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

public:
	explicit NEDoubleSpinBox(QWidget* parent = 0) : QDoubleSpinBox(parent) { }

	QString textFromValue(double val) const
	{
		//return QDoubleSpinBox::textFromValue(val);
		////tmpStr
		//return tmpStr;
		////return QString::number(val);
		//return str;

		QString str = locale().toString(val, 'g', 15);
		if (qAbs(val) >= 1000.0) {
			str.remove(locale().groupSeparator());
		}
		return str;
	}
};
