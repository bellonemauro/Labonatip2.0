/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef XMLPROTOCOLREADER_H
#define XMLPROTOCOLREADER_H

#include <QXmlStreamReader>
#include "protocolTreeWidgetItem.h"

QT_BEGIN_NAMESPACE
class QTreeWidget;
QT_END_NAMESPACE


class XmlProtocolReader
{
public:
	XmlProtocolReader(QTreeWidget *treeWidget);

    bool read(QIODevice *device, protocolTreeWidgetItem* after_item);

    QString errorString() const;

    static inline QString versionAttribute() { return QStringLiteral("version"); }

private:
    void readPRT();
    void readProtocolItem(protocolTreeWidgetItem* parent_item);

	protocolTreeWidgetItem *createChildItem(protocolTreeWidgetItem *parent_item);

    QXmlStreamReader xml;
    QTreeWidget *treeWidget;
    protocolTreeWidgetItem* m_after_item;
    int m_row;
};

#endif
