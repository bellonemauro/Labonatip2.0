/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include <QtWidgets>

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
    xml.writeStartElement(QStringLiteral("Protocol"));
    xml.writeAttribute(versionAttribute(), QStringLiteral("1.0"));
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
        writeItem(treeWidget->topLevelItem(i));

    xml.writeEndDocument();
    return true;
}

void XmlProtocolWriter::writeItem(const QTreeWidgetItem *item)
{
	QString tagName = "ProtocolItem";//item->data(0, Qt::UserRole).toString();

	xml.writeStartElement(tagName);
	xml.writeTextElement(index(), item->text(0));
	xml.writeTextElement(command_str(), getCommandAsString(item->text(1).toInt()));
	xml.writeTextElement(command_element(), item->text(1));
	xml.writeTextElement(value(), item->text(3));
	xml.writeTextElement(message(), item->text(4));
	for (int i = 0; i < item->childCount(); ++i)
		writeItem(item->child(i));
	xml.writeEndElement();

}

QString XmlProtocolWriter::getCommandAsString(int _instruction)
{
	static const char* const text[] =
	{ "setPon", "setPoff", "setVswitch", "setVrecirc",
		"solution1", "solution2","solution3","solution4",
		"wait", "ask_msg", "allOff", "pumpsOff",
		"waitSync", "syncOut",
		"zoneSize", "flowSpeed", "vacuum", "loop" };
	return  text[int(_instruction)]; // cast to integer
}
