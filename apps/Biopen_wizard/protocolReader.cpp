/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolReader.h"
#include <QMessageBox>
#include <QDateTime>


Labonatip_protocolReader::Labonatip_protocolReader(QTreeWidget * _tree, 
	QMainWindow *parent ) :
	m_cmd_idx_c(0), m_cmd_command_c(1), m_cmd_range_c(2),
	m_cmd_value_c(3), m_cmd_msg_c(4), m_cmd_level_c(5)
{
	m_tree = _tree;
	m_current_protocol_file_name = "";
	m_v_7_minor_backward_compatibility = false;
}



void Labonatip_protocolReader::initCustomStrings()
{
	m_str_warning = tr("Warning");
	m_str_file_not_found = tr("File not found");
	m_str_negative_level = tr("Negative level, file corrupted");
}

void Labonatip_protocolReader::switchLanguage(QString _translation_file)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocolWriter::switchLanguage " << endl;

	qApp->removeTranslator(&m_translator_reader);

	if (m_translator_reader.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_reader);

		//ui_tools->retranslateUi(this);

		initCustomStrings();

		cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_protocolWriter::switchLanguage   installTranslator" << endl;
	}

}

bool Labonatip_protocolReader::readProtocol(QString _filename)
{
	QFile protocol_file(_filename);
	m_current_protocol_file_name = _filename;

	// open the file and check its existance
	if (protocol_file.exists() &&
		protocol_file.open(QIODevice::ReadWrite))
	{
		// this is che protocol file content
		QByteArray content = protocol_file.readLine();

		// browse the entire file content
		while (!content.isEmpty())
		{
			// for each line in the file one item is created
			protocolTreeWidgetItem *new_item =
				new protocolTreeWidgetItem();

			// the parent is only needed if we have it
			protocolTreeWidgetItem *parent =
				new protocolTreeWidgetItem();

			// the decodeProtocol will fill the new_item with the actual content
			// it will return false for any error and the line is discarded
			if (decodeProtocolCommand(content, *new_item)) {

				// if we are at top level
				if (getLevel(*new_item) == 0)
				{
					// add the item at the top level
					m_tree->addTopLevelItem(new_item);
				}
				if (getLevel(*new_item) > 0)  // we are at the first level
				{
					// we need to know who is the parent 
					QTreeWidgetItemIterator it(m_tree);
					while (*it) {
						parent = dynamic_cast<protocolTreeWidgetItem *> ((*it));
						++it;
					}

					// create the child item
					protocolTreeWidgetItem *child_item =
						new protocolTreeWidgetItem();

					QTreeWidgetItemIterator it2(m_tree);
					while (*it2) { // this will just get the last node
						child_item = dynamic_cast<protocolTreeWidgetItem * > (*it2);
						++it2;
					}

					int current_item_level = getLevel(*child_item);
					int new_item_level = getLevel(*new_item); // this is the parent item

					if (current_item_level == -1) current_item_level = 0;

					// same tree level
					if (current_item_level == new_item_level) {
						child_item->QTreeWidgetItem::parent()->addChild(new_item);
					}

					// increase in level
					if (new_item_level == current_item_level + 1) {
						child_item->addChild(new_item);
					}

					// decrease in level
					if (new_item_level < current_item_level) {
						protocolTreeWidgetItem *parent_item = child_item;
						for (int i = 0; i < current_item_level - new_item_level; i++) {
							parent_item = dynamic_cast<protocolTreeWidgetItem *> (
								parent_item->QTreeWidgetItem::parent());
						}
						parent_item->QTreeWidgetItem::parent()->addChild(new_item);
					}

					m_tree->update();
				}
				else { // there is something wrong !! 
					if (getLevel(*new_item) != 0)
						QMessageBox::warning(this, m_str_warning,
							m_str_negative_level);
				}
			}
			// we are ready to read the new line
			content = protocol_file.readLine();

			//this will just give the focus to the last element
			m_tree->setCurrentItem(new_item, m_cmd_value_c, QItemSelectionModel::SelectionFlag::Rows);
		}
		m_tree->setItemSelected(
			m_tree->topLevelItem(
				m_tree->topLevelItemCount() - 1), true);
	}
	else {
		QMessageBox::warning(this, m_str_warning,
			m_str_file_not_found + "<br>" + _filename);
		return false;

	}
	return true;
}

bool Labonatip_protocolReader::decodeProtocolCommand(
	QByteArray &_command, protocolTreeWidgetItem &_out_item)
{
	QChar prohibited_char_1 = QChar::fromLatin1(*"#");
	QChar prohibited_char_2 = QChar::fromLatin1(*"\n");

	QStringList data_string;
	if (_command.at(0) == *"%") {
		// it is the header, do nothing, just discard the line
		QByteArray headerVersionLine = "%% Protocol Header V. 0.7 \n";
		if (_command.contains("0.6") || _command.contains("0.5") || _command.contains("0.4") ||
			_command.contains("0.3") || _command.contains("0.2") || _command.contains("0.1"))
		{
			QMessageBox::warning(this, m_str_warning,
				"ATTENTION: this file has been saved using an older version");
			m_v_7_minor_backward_compatibility = true;
			return false;
		}

		return false;
	}
	else
	{
		// the endline has 2 end characters, # for the string and § for the line
		for (int i = 0; i < _command.size() - 2; i++)
		{
			QString data = "";
			while (_command.at(i) != prohibited_char_1)
			{
				data.append(_command.at(i));
				i++;
				if (_command.at(i) == prohibited_char_2) break; // endline
			}
			data_string.push_back(data);
		}
	}

	if (data_string.size() < 4) {
		cerr << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString()
			<< "Labonatip_protocolReader::decodeProtocolCommand ::: data_string size error - size = "
			<< data_string.size()
			<< " command = " << _command.toStdString() << endl;
		// something went wrong, discard the line, but a message is out 
		return false;
	}


	// fill the qtreewidget item
	if (m_v_7_minor_backward_compatibility == true)
	{
		
		QString command_data;

		command_data = remapForBackwardCompatibility(data_string.at(0));

		_out_item.setText(m_cmd_command_c, command_data);
	}
	else {
		_out_item.setText(m_cmd_command_c, data_string.at(0));
	}


	_out_item.setText(m_cmd_value_c, data_string.at(1));

	if (data_string.at(m_cmd_value_c) == "2") { // 2 is the string for true
												//_out_item.setCheckState(m_cmd_msg_c, Qt::CheckState::Checked); 
	}
	else {
		//_out_item.setCheckState(m_cmd_msg_c, Qt::CheckState::Unchecked); 
	}
	_out_item.setText(m_cmd_msg_c, data_string.at(3));
	_out_item.setText(m_cmd_level_c, data_string.at(4));

	_out_item.setFlags(_out_item.flags() | (Qt::ItemIsEditable));

	//setRangeColumn(&_out_item, data_string.at(0).toInt());
	bool success = _out_item.checkValidity(m_cmd_value_c);

	_out_item.setText(m_cmd_range_c,
		_out_item.getRangeColumn(
			_out_item.text(m_cmd_command_c).toInt()));

	return true;
}

int Labonatip_protocolReader::getLevel(QTreeWidgetItem _item)
{
	int level;

	// for our item structure the level is on the column number 4
	bool success = false;
	level = _item.text(m_cmd_level_c).toInt(&success);
	if (success) return level;
	else return -1;

}

QString Labonatip_protocolReader::remapForBackwardCompatibility(QString _old_data)
{
	//setPon = 0,      zoneSize = 0,
	if (_old_data == "0") return "9";
	//setPoff = 1,     flowSpeed = 1,
	if (_old_data == "1") return "10";
	//setVswitch = 2,  vacuum = 2,
	if (_old_data == "2") return "12";
	//setVrecirc = 3,  wait = 3,
	if (_old_data == "3") return "11";
	//solution1 = 4,   allOff = 4,
	if (_old_data == "4") return "5";
	//solution2 = 5,   solution1 = 5,
	if (_old_data == "5") return "6";
	//solution3 = 6,   solution2 = 6,
	if (_old_data == "6") return "7";
	//solution4 = 7,   solution3 = 7,
	if (_old_data == "7") return "8";
	//wait = 8,        solution4 = 8,
	if (_old_data == "8") return "3";
	//ask_msg = 9,     setPon = 9,
	if (_old_data == "9") return "13";
	//allOff = 10,     setPoff = 10,
	if (_old_data == "10") return "4";
	//pumpsOff = 11,   setVrecirc = 11,
	if (_old_data == "11") return "14";
	//waitSync = 12,   setVswitch = 12,
	if (_old_data == "12") return "15";
	//syncOut = 13,	   ask_msg = 13,
	if (_old_data == "13") return "16";
	//zoneSize = 14,   pumpsOff = 14,
	if (_old_data == "14") return "0";
	//flowSpeed = 15,  waitSync = 15,
	if (_old_data == "15") return "1";
	//vacuum = 16,     syncOut = 16,
	if (_old_data == "16") return "2";
	//loop = 17        loop = 17 
	if (_old_data == "17") return "17";
	//TODO remap as Zone size, Flow speed, Vacuum, Wait, Alloff, Solution 1-4, Pon, Poff, Vrecirc, V switch, all the rest.

	return QString();
}
