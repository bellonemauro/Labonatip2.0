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

XmlProtocolReader::XmlProtocolReader(QTreeWidget* treeWidget)
	: treeWidget(treeWidget),
	m_after_item(NULL)
{
	m_row = 0;
	m_str_notPRTvfile = QObject::tr("The file is not an PRT version 1.0 file.");
}

bool XmlProtocolReader::read(QIODevice *device, protocolTreeWidgetItem* after_item)
{
    xml.setDevice(device);
	m_after_item = after_item;
	m_row = treeWidget->currentIndex().row();
    if (xml.readNextStartElement()) {
        if (xml.name() == swProtocolAttribute()
            && xml.attributes().value(versionAttribute()) == QLatin1String("1.0")) {
            readPRT();
        } else {
            xml.raiseError(m_str_notPRTvfile);
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
	Q_ASSERT(xml.isStartElement() && xml.name() == swProtocolAttribute());

	while (xml.readNextStartElement()) {
		if (m_after_item)
			readProtocolItem(m_after_item);
		else
			readProtocolItem(0);
	}
}

void XmlProtocolReader::readProtocolItem(protocolTreeWidgetItem* parent_item)
{
	protocolTreeWidgetItem* child_item;
	
	std::string command = xml.name().toString().toStdString();
	child_item = createChildItem(parent_item);
	ComboBoxDelegate virtual_combo;
	child_item->setText(1, QString::number(virtual_combo.getElementIndex(command)));

	for (int i = 0; i < xml.attributes().count(); i++)
	{
		if (xml.attributes().at(i).name() == valueAttribute())
			child_item->setText(3, xml.attributes().at(i).value().toString());
		if (xml.attributes().at(i).name() == messageAttribute())
			child_item->setText(4, xml.attributes().at(i).value().toString());
	}

	while (xml.readNextStartElement()) {
			readProtocolItem(child_item);
	}
}


protocolTreeWidgetItem *XmlProtocolReader::createChildItem(protocolTreeWidgetItem *parent_item)
{
	protocolTreeWidgetItem *childItem;
#pragma message("TODO: it looks that here there is a problem with the parent items, protocols are not loaded in the loop")
    if (parent_item) {
        childItem = new protocolTreeWidgetItem(parent_item);
		parent_item->addChild(childItem);
    } else {
		childItem = new protocolTreeWidgetItem();
		m_row++;
		treeWidget->insertTopLevelItem(m_row, childItem);
    }
    childItem->setData(0, Qt::UserRole, xml.name().toString());
    return childItem;
}
