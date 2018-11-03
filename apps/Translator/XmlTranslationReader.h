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

private:
    
	void readXmlFile();
    
	void readSource(QTreeWidgetItem *item);
    
	void readTranslation(QTreeWidgetItem *item);

	QTreeWidgetItem *XmlTranslationReader::createChildItem(QTreeWidgetItem *item);

	QXmlStreamReader xml;
    QTreeWidget *treeWidget;

};

#endif