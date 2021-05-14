#pragma once

class FilePathWrapper;

class NECEXP_CORE FilePath
{
	friend class FilePathWrapper;
public:
    enum DialogType
    {
        kOpenFileType,
        kSaveFileType,
        kOpenPathType,
        kImpoFileType,
        kRefeFileType,
        kPLOT3DFileType
    };

    FilePath();
    FilePath(const FilePath& other);
    FilePath(const QString& Path);
    FilePath(const QString& Path, const QString& Filter);
    FilePath(DialogType dtype);

	QString path() const;
	void setPath(QString val);

	QString filter() const;
	void setFilter(QString val);

    FilePath::DialogType dialogType() const;
    void setDialogType(FilePath::DialogType val);

	operator QVariant() const;
	operator QString() const;
	bool operator ==(const FilePath& a) const;

private:
	QString m_Path;
	QString m_Filter;
    FilePath::DialogType m_DialogType;

};


Q_DECLARE_METATYPE(FilePath)


class FilePathWrapper : public QObject
{
	Q_OBJECT

		public slots:
			FilePath* new_FilePath(const QString& path)
			{
				return new FilePath(path);
			}
			QString path(FilePath* o) { return o->m_Path; }

			//QString py_toString(FilePath* o)
			//{
			//	QString result;
			//	QDebug d(&result);
			//	d << *o;
			//	return result;
			//}
};



