
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
	QString source_language = "en_GB";
	QString translation_language = "en_GB";
}

bool XmlTranslationReader::read(QIODevice *device)
{
    xml.setDevice(device);


     while (xml.readNextStartElement()) {
		 if (xml.name() == QLatin1String("TS")) {
			 QXmlStreamAttributes attributes = xml.attributes();

			 if (attributes.at(0).name() == QLatin1String("version"))
			 {
				 //ok
			 }
			 if (attributes.at(1).name() == QLatin1String("language"))
			 {
				 translation_language = attributes.at(1).value().toString();
			 }
			 if (attributes.at(2).name() == QLatin1String("sourcelanguage"))
			 {
				 source_language = attributes.at(2).value().toString();
			 }
		}
		else if (xml.name() == QLatin1String("context")) {
			 //QString value = xml.readElementText();
			 //xml.skipCurrentElement();
			 readContexElement();
		 }
		 //xml.readNextStartElement();
		 
		 
		 //if (xml.name() == QLatin1String("File Name") &&
         //    xml.attributes().value(versionAttribute()) == QLatin1String("1.0")) {
			
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

void XmlTranslationReader::readContexElement()
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("context"));
	QTreeWidgetItem *item = new QTreeWidgetItem();
	item = createChildItem(0);
	item->setExpanded(true);
	while (xml.readNextStartElement()) {
		if (xml.name() == QLatin1String("name")) {
			//QString value = xml.readElementText();
			//QTreeWidgetItem *item;
			//item = createChildItem(0);
			readName(item);
			//item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);

		}
		else if (xml.name() == QLatin1String("message")) {
			//QString value = xml.readElementText();
			//item->setText(0, " ");

			readMessageElement(item);
		}
		else
			xml.skipCurrentElement();
	}
}


void XmlTranslationReader::readMessageElement(QTreeWidgetItem *_parent)
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("message"));
		//while (xml.readNextStartElement()) {
			QTreeWidgetItem *item = new QTreeWidgetItem();
			//item = createChildItem(0);
			_parent->addChild(item);
			while (xml.readNextStartElement()) {
				if (xml.name() == QLatin1String("source")) {
					readSource(item);
					item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
				}
				else if (xml.name() == QLatin1String("translation"))
				{
					readTranslation(item);
					item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
				}
				else
					xml.skipCurrentElement();
			}
		//}
}


void XmlTranslationReader::readSource(QTreeWidgetItem *_item)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("source"));
     QString value = xml.readElementText();
	 //item = createChildItem(0);
	_item->setText(1, value);
	
}

void XmlTranslationReader::readTranslation(QTreeWidgetItem *_item)
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("translation"));
	QString value = xml.readElementText();
	_item->setText(2, value);
}

void XmlTranslationReader::readName(QTreeWidgetItem *_item)
{
	Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("name"));
	QString value = xml.readElementText();
	_item->setText(0, value);
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