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
#include <iostream>

XmlTranslationWriter::XmlTranslationWriter(const QTreeWidget *treeWidget)
    : treeWidget(treeWidget)
{
    xml.setAutoFormatting(true);
	QString source_language = "en_GB";
	QString translation_language = "en_GB";
}

 bool XmlTranslationWriter::writeFile(QIODevice *device)
{
    xml.setDevice(device);
     xml.writeStartDocument();
	// <TS version = "2.1" language = "en_GB" sourcelanguage = "en_GB">
    xml.writeDTD(QStringLiteral("<!DOCTYPE TS>"));
    xml.writeStartElement(QStringLiteral("TS"));
	xml.writeAttribute(versionAttribute(), QStringLiteral("2.1"));

	xml.writeAttribute(languageAttribute(), translation_language);
	xml.writeAttribute(sourceLanguageAttribute(), source_language);

    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
        writeItem(treeWidget->topLevelItem(i));
    
	xml.writeEndDocument();
    return true;
}

 void XmlTranslationWriter::writeItem(const QTreeWidgetItem *item)
{
	QString tagName = "context";//item->data(0, Qt::UserRole).toString();
 	xml.writeStartElement(tagName);
	xml.writeTextElement(name(), item->text(0));

	for (int i = 0; i < item->childCount(); ++i)
		writeMessageItem(item->child(i));
	xml.writeEndElement();
}

 void XmlTranslationWriter::writeMessageItem(const QTreeWidgetItem *item)
 {
	 QString tagName = "message";//item->data(0, Qt::UserRole).toString();
	 xml.writeStartElement(tagName);
	 xml.writeTextElement(source(), item->text(1));
	 xml.writeTextElement(translation(), item->text(2));

	 xml.writeEndElement();
 }
