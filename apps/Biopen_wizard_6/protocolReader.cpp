/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolReader.h"


void protocolReader::initCustomStrings()
{
	m_str_warning = tr("Warning");
	m_str_file_not_found = tr("File not found");
	m_str_negative_level = tr("Negative level, file corrupted");
}

void protocolReader::switchLanguage(QString _translation_file)
{
	std::cout << HERE << std::endl;

	qApp->removeTranslator(&m_translator_reader);

	if (m_translator_reader.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_reader);

		//ui_tools->retranslateUi(this);

		initCustomStrings();

		std::cout << HERE << " installTranslator" << std::endl;
	}
}



//TODO: the way the protocol is read is not very clever, this must be fixed.
//      what happens is that the protocol file is read into the qtreewidget
//      and then passed into the data structure m_protocol, this is not correct
bool protocolReader::readProtocol(QTreeWidget *_out_tree, 
	const QString _filename)
{
	QFile protocol_file(_filename);

	// there is no real check if _out_tree is inizialized, 
	if (_out_tree == NULL) return false;

	// open the file and check its existence
	if (protocol_file.exists() &&
		protocol_file.open(QIODevice::ReadWrite))
	{
		// this is the protocol file content
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
					_out_tree->addTopLevelItem(new_item);
				}
				if (getLevel(*new_item) > 0)  // we are at the first level
				{
					// we need to know who is the parent 
					QTreeWidgetItemIterator it(_out_tree);
					while (*it) {
						parent = dynamic_cast<protocolTreeWidgetItem *> ((*it));
						++it;
					}

					// create the child item
					protocolTreeWidgetItem *child_item =
						new protocolTreeWidgetItem();

					QTreeWidgetItemIterator it2(_out_tree);
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

					_out_tree->update();
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
			_out_tree->setCurrentItem(new_item, editorParams::c_value,
				QItemSelectionModel::SelectionFlag::Rows);
		}
		_out_tree->topLevelItem(
			_out_tree->topLevelItemCount() - 1)->setSelected(true);
		//_out_tree->setItemSelected(
		//	_out_tree->topLevelItem(
		//		_out_tree->topLevelItemCount() - 1), true);
	}
	else {
		QMessageBox::warning(this, m_str_warning,
			m_str_file_not_found + "<br>" + _filename);
		return false;

	}
	int cou = _out_tree->topLevelItemCount();
	std::cout << HERE << " number of items in the tree " 
		<< cou << std::endl;
	return true;
}

bool protocolReader::decodeProtocolCommand(
	QByteArray &_command, protocolTreeWidgetItem &_out_item)
{
	QChar prohibited_char_1 = QChar::fromLatin1(*"#");
	QChar prohibited_char_2 = QChar::fromLatin1(*"\n");

	QStringList data_string;
	if (_command.at(0) == *"%") {
		// it is the header, do nothing, just discard the line
		//QByteArray headerVersionLine = "%% Protocol Header V. 0.7 \n";
		if (_command.contains("%% Protocol Header ")) {
			m_protocol_version = this->checkProtocolVersion(_command);
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
		std::cerr << HERE << " data_string size error - size = " << data_string.size()
			<< " command = " << _command.toStdString() << std::endl;
		// something went wrong, discard the line, but a message is out 
		return false;
	}

	QString command_data;
	command_data = remapForBackwardCompatibility(m_protocol_version, data_string.at(0));
	_out_item.setText(editorParams::c_command, command_data);
	_out_item.setText(editorParams::c_value, data_string.at(1));

	if (data_string.at(editorParams::c_value) == "2") { // 2 is the string for true
												//_out_item.setCheckState(m_cmd_msg_c, Qt::CheckState::Checked); 
	}
	else {
		//_out_item.setCheckState(m_cmd_msg_c, Qt::CheckState::Unchecked); 
	}
	_out_item.setText(editorParams::c_msg, data_string.at(3));
	_out_item.setText(editorParams::c_level, data_string.at(4));

	_out_item.setFlags(_out_item.flags() | (Qt::ItemIsEditable));

	//setRangeColumn(&_out_item, data_string.at(0).toInt());
	bool success = _out_item.checkValidity(editorParams::c_value);

	_out_item.setText(editorParams::c_range,
		_out_item.getRangeColumn(
			_out_item.text(editorParams::c_command).toInt()));

	return true;
}

int protocolReader::getLevel(QTreeWidgetItem _item)
{
	int level;

	// for our item structure the level is on the column number 4
	bool success = false;
	level = _item.text(editorParams::c_level).toInt(&success);
	if (success) 
		return level;
	else 
		return -1;
}

int protocolReader::checkProtocolVersion(QByteArray _command)
{
	if (_command.contains("%% Protocol Header ")) {
	
	// headerVersionLine = "%% Protocol Header V. 0.___ \n";
	if (_command.contains("0.9")) { return 9; }
	if (_command.contains("0.8")) { return 8; }
	if (_command.contains("0.7")) { return 7; }
	if (_command.contains("0.6")) { return 6; }
	if (_command.contains("0.5")) { return 5; }
	if (_command.contains("0.4")) { return 4; }
	if (_command.contains("0.3")) { return 3; }
	if (_command.contains("0.2")) { return 2; }
	if (_command.contains("0.1")) { return 1; }
	}

	// if 0 is returned, something is wrong with the line
	return 0; 
}

QString protocolReader::remapForBackwardCompatibility(int _version, QString _old_data)
{
	if (_version <= 6)
	{
		QMessageBox::warning(this, m_str_warning,
			"THE PROTOCOL VERSION IS TOO OLD " + _version);

	}
	if (_version == 7)
	{
		QMessageBox::warning(this, m_str_warning,
			"THE PROTOCOL VERSION IS TOO OLD "+ _version);
		
	}
	if (_version == 8)
	{
		// setZoneSize = 0,     setZoneSize = 0,
		if (_old_data == "0") return "0";
		// changeZoneSizeBy = 1,    changeZoneSizeBy = 1,
		if (_old_data == "1") return "1";
		// setFlowSpeed = 2,       setFlowSpeed = 2,
		if (_old_data == "2") return "2";
		// changeFlowSpeedBy = 3,         changeFlowSpeedBy = 3,
		if (_old_data == "3") return "3";
		// setVacuum = 4,       setVacuum = 4,
		if (_old_data == "4") return "4";
		// changeVacuumBy = 5,    changeVacuumBy = 5,
		if (_old_data == "5") return "5";
		// wait = 6,    wait = 6,
		if (_old_data == "6") return "6";
		// allOff = 7,    allOff = 7,
		if (_old_data == "7") return "7";
		// solution1 = 8,    solution1 = 8,
		if (_old_data == "8") return "8";
		// solution2 = 9,       solution2 = 9,
		if (_old_data == "9") return "9";
		// solution3 = 10,     solution3 = 10,
		if (_old_data == "10") return "10";
		// solution4 = 11,  solution4 = 11,
		if (_old_data == "11") return "11";

		// setPon = 12,  solution5 = 12,
		if (_old_data == "12") return "14";
		// setPoff = 13,     solution5 = 13,
		if (_old_data == "13") return "15";
		// setVrecirc = 14,    setPon = 14,
		if (_old_data == "14") return "16";
		// setVswitch = 15,    setVswitch = 15,
		if (_old_data == "15") return "17";
		// ask_msg = 16,     ask_msg = 16,
		if (_old_data == "16") return "18";
		// pumpsOff = 17         pumpsOff = 17,
		if (_old_data == "17") return "19";

		// if we are here the command was not found so we return the same data
		return _old_data;
		//					 waitSync = 18,
		//if (_old_data == "18") return "6";
		//				 	 syncOut = 19,
		//if (_old_data == "19") return "6";
		//					 loop = 20,
		//if (_old_data == "20") return "6";
	}
	
	return _old_data;
}
