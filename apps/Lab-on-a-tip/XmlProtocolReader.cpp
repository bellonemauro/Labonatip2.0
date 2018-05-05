/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include <QtWidgets>

#include "XmlProtocolReader.h"

XmlProtocolReader::XmlProtocolReader(QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{

}

bool XmlProtocolReader::read(QIODevice *device)
{
    xml.setDevice(device);

    if (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("Protocol")
            && xml.attributes().value(versionAttribute()) == QLatin1String("1.0")) {
            readPRT();
        } else {
            xml.raiseError(QObject::tr("The file is not an PRT version 1.0 file."));
        }
    }

    return !xml.error();
}

QString XmlProtocolReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}

void XmlProtocolReader::readPRT()
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("Protocol"));

	while (xml.readNextStartElement()) {
		protocolTreeWidgetItem *item;

		while (xml.readNextStartElement()){
			if (xml.name() == QLatin1String("Instruction")) {
				item = createChildItem(0);
				readInstruction(item);
		}
		else if (xml.name() == QLatin1String("Value"))
			readValue(item);
		else if (xml.name() == QLatin1String("Message"))
			readMessage(item);
		else
			xml.skipCurrentElement();
	}
	}
}

void XmlProtocolReader::readInstruction(protocolTreeWidgetItem *item)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("Instruction"));

    QString Instruction = xml.readElementText();

	
		item = createChildItem(0);
		item->setText(1, Instruction);
	

}

void XmlProtocolReader::readValue(protocolTreeWidgetItem *item)
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("Value"));
	QString value = xml.readElementText();
	item->setText(3, value);
}

void XmlProtocolReader::readMessage(protocolTreeWidgetItem *item)
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("Message"));

	QString value = xml.readElementText();
	item->setText(4, value);

	if (item->text(1) == "17")
	{
		protocolTreeWidgetItem *in_loop_item;
		while (xml.readNextStartElement()) {
			{
				if (xml.name() == QLatin1String("Instruction")) {
					in_loop_item = createChildItem(item);
					readInstruction(in_loop_item);
				}
				else if (xml.name() == QLatin1String("Value"))
					readValue(in_loop_item);
				else if (xml.name() == QLatin1String("Message"))
					readMessage(in_loop_item);
				else
					xml.skipCurrentElement();
			}
		}
	}
	else
	{
	}

}

protocolTreeWidgetItem *XmlProtocolReader::createChildItem(protocolTreeWidgetItem *item)
{
	protocolTreeWidgetItem *childItem;
    if (item) {
        childItem = new protocolTreeWidgetItem(item);
    } else {
		childItem = new protocolTreeWidgetItem();
		treeWidget->addTopLevelItem(childItem);
    }
    childItem->setData(0, Qt::UserRole, xml.name().toString());
    return childItem;
}
