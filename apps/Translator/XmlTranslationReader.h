/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef XML_READER_H
#define XML_READER_H
 
#include <QXmlStreamReader>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
	class QTreeWidget;
QT_END_NAMESPACE

class XmlTranslationReader
{

public:

	XmlTranslationReader(QTreeWidget *treeWidget);
    
	bool read(QIODevice *device);
    
	QString errorString() const;
    
	static inline QString versionAttribute() { return QStringLiteral("version"); }

	QString getSourceLanguage() {
		return source_language;
	}

	QString getTranslationLanguage() {
		return translation_language;
	}

private:
    
	void readMessageElement(QTreeWidgetItem *_parent);

	void readContexElement();
    
	void readSource(QTreeWidgetItem *_item);
    
	void readTranslation(QTreeWidgetItem *_item);

	void readName(QTreeWidgetItem *_item);

	QTreeWidgetItem* createChildItem(QTreeWidgetItem *item);

	QString source_language;
	QString translation_language;

	QXmlStreamReader xml;
   	QTreeWidget *treeWidget;

};

#endif
