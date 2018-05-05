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

    bool read(QIODevice *device);

    QString errorString() const;

    static inline QString versionAttribute() { return QStringLiteral("version"); }

private:
    void readPRT();
    void readInstruction(protocolTreeWidgetItem *item);
    void readValue(protocolTreeWidgetItem *item);
    void readMessage(protocolTreeWidgetItem *item);

	protocolTreeWidgetItem *createChildItem(protocolTreeWidgetItem *item);

    QXmlStreamReader xml;
    QTreeWidget *treeWidget;
};

#endif
