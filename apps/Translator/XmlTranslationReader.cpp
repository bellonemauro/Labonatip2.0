
/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include <QtWidgets>
#include "XmlTranslationReader.h"

XmlTranslationReader::XmlTranslationReader(QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
}

bool XmlTranslationReader::read(QIODevice *device)
{
    xml.setDevice(device);
     if (xml.readNextStartElement()) {
        //if (xml.name() == QLatin1String("File Name") &&
         //    xml.attributes().value(versionAttribute()) == QLatin1String("1.0")) {
			readXmlFile();
        //} else {
        //    xml.raiseError(QObject::tr("The file is not a translation version 1.0 file."));
        //}
    }
     return !xml.error();
}

QString XmlTranslationReader::errorString() const
{
    return QObject::tr("%1\nLine %2, column %3")
            .arg(xml.errorString())
            .arg(xml.lineNumber())
            .arg(xml.columnNumber());
}

void XmlTranslationReader::readXmlFile()
{
	//Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("context"));
 	while (xml.readNextStartElement()) {
		QTreeWidgetItem *item;
		item = createChildItem(0);
		while (xml.readNextStartElement()){
			if (xml.name() == QLatin1String("message")) {
				//QString value = xml.readElementText();
				item->setText(0, "ciao");
		}
		else if (xml.name() == QLatin1String("source"))
			readSource(item);
		else if (xml.name() == QLatin1String("translation"))
			readTranslation(item);
		else
			xml.skipCurrentElement();
	}
	}
}


void XmlTranslationReader::readSource(QTreeWidgetItem *item)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("source"));
     QString value = xml.readElementText();
	 //item = createChildItem(0);
	item->setText(1, value);
	
}

void XmlTranslationReader::readTranslation(QTreeWidgetItem *item)
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("translation"));
	QString value = xml.readElementText();
	item->setText(2, value);
}
/*

void XmlTranslationReader::readInstruction(protocolTreeWidgetItem *item)
{
Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("Instruction"));
QString Instruction = xml.readElementText();

item = createChildItem(0);
item->setText(1, Instruction);

}


void XmlTranslationReader::readMessage(protocolTreeWidgetItem *item)
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
}*/
 
QTreeWidgetItem *XmlTranslationReader::createChildItem(QTreeWidgetItem *item)
{
	QTreeWidgetItem *childItem;
    if (item) {
        childItem = new QTreeWidgetItem(item);
    } else {
		childItem = new QTreeWidgetItem();
		treeWidget->addTopLevelItem(childItem);
    }
    childItem->setData(0, Qt::UserRole, xml.name().toString());
    return childItem;
}