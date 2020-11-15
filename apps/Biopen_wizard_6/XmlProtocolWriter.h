/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef XMLPROTOCOLRITER_H
#define XMLPROTOCOLRITER_H

#include <QXmlStreamWriter>

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class XmlProtocolWriter
{
public:
    explicit XmlProtocolWriter(const QTreeWidget *treeWidget);
    bool writeFile(QIODevice *device);

	static inline QString valueAttribute() { return QStringLiteral("Value"); }
	static inline QString messageAttribute() { return QStringLiteral("Message"); }
	static inline QString versionAttribute() { return QStringLiteral("version"); }

private:
    void writeItem(const QTreeWidgetItem *item);
	QString getCommandAsString(int _instruction);
    QXmlStreamWriter xml;
    const QTreeWidget *treeWidget;
};

#endif
