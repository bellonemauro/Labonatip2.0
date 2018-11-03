/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */
 
#include <QtWidgets>
#include "XmlTranslationWriter.h"


XmlTranslationWriter::XmlTranslationWriter(const QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    xml.setAutoFormatting(true);
}

 bool XmlTranslationWriter::writeFile(QIODevice *device)
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

 void XmlTranslationWriter::writeItem(const QTreeWidgetItem *item)
{
	QString tagName = "Name";//item->data(0, Qt::UserRole).toString();
 	xml.writeStartElement(tagName);
	xml.writeTextElement(message(), item->text(0));
	xml.writeTextElement(source(), item->text(1));
	xml.writeTextElement(translation(), item->text(3));

	for (int i = 0; i < item->childCount(); ++i)
		writeItem(item->child(i));
	xml.writeEndElement();
}

