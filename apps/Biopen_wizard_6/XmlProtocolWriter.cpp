/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include <QtWidgets>
#include <QTextStream>
#include "protocolEditorDelegates.h"
#include "XmlProtocolWriter.h"
//#include "XmlProtocolReader.h"


XmlProtocolWriter::XmlProtocolWriter(const QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    xml.setAutoFormatting(true);
}

bool XmlProtocolWriter::writeFile(QIODevice *device)
{
    xml.setDevice(device);

    xml.writeStartDocument();
    xml.writeDTD(QStringLiteral("<!DOCTYPE prt>"));
    xml.writeComment("Fluicell protocol xml");
#pragma message ("TODO: write a full header")
    xml.writeStartElement(QStringLiteral("Protocol"));
    xml.writeAttribute(versionAttribute(), QStringLiteral("1.0"));
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
        writeItem(treeWidget->topLevelItem(i));

    xml.writeEndDocument();
    return true;
}

void XmlProtocolWriter::writeItem(const QTreeWidgetItem *item)
{
	xml.writeStartElement(getCommandAsString(item->text(1).toInt()));
	xml.writeAttribute("value", item->text(3)); 
#pragma message ("TODO: the attributes should be coded into variables")
	xml.writeAttribute("message", item->text(4));
	for (int i = 0; i < item->childCount(); ++i)
		writeItem(item->child(i));
	xml.writeEndElement();
}

QString XmlProtocolWriter::getCommandAsString(int _instruction)
{
	ComboBoxDelegate* tmp_combo = new ComboBoxDelegate();
	QString text = QString::fromStdString(tmp_combo->getElementAt(_instruction));
	return text;
}
