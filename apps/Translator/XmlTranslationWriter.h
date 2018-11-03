/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef XML_WRITER_H
#define XML_WRITER_H

#include <QXmlStreamWriter>

QT_BEGIN_NAMESPACE
  class QTreeWidget;
  class QTreeWidgetItem;
QT_END_NAMESPACE


class XmlTranslationWriter
{

public:
    explicit XmlTranslationWriter(const QTreeWidget *treeWidget);

    bool writeFile(QIODevice *device);

 	static inline QString message() { return QStringLiteral("Message"); }
	static inline QString source() { return QStringLiteral("source"); }
	static inline QString translation() { return QStringLiteral("translation"); }
	static inline QString versionAttribute() { return QStringLiteral("version"); }

private:
    
	void writeItem(const QTreeWidgetItem *item);
	
	QXmlStreamWriter xml;
    const QTreeWidget *treeWidget;
};
 #endif