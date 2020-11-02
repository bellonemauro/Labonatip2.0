/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip.h"
#include <QDesktopServices>

void Labonatip_GUI::readProtocolFolder(QString _path)
{
	ui->treeWidget_protocol_folder->clear();
	_path = QDir::cleanPath(_path);
	ui->lineEdit_protocolPath->setText(_path);

	QStringList filters;
	filters << "*.prt";

	QDir protocol_path;
	protocol_path.setPath(_path);
	
	QStringList protocol_list = protocol_path.entryList(filters);
	protocol_path.setFilter( QDir::Dirs | QDir::NoDot );
	QStringList folder_list = protocol_path.entryList();

	
	for (int i = 0; i < folder_list.size(); i++) // starting from 2 it will not add ./ and ../
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, folder_list.at(i));
		item->setIcon(0, QIcon(":/icons/open_off.png"));
		//item->setBackgroundColor(0, QColor::fromRgb(220, 220, 220));
		item->setBackground(0, QColor::fromRgb(220, 220, 220));
		ui->treeWidget_protocol_folder->addTopLevelItem(item);
	}

	for (int i = 0; i < protocol_list.size(); i++) // starting from 2 it will not add ./ and ../
	{
		QTreeWidgetItem* item = new QTreeWidgetItem();
		item->setText(0, protocol_list.at(i));
		item->setIcon(0, QIcon(":/icons/dummy.png"));
		ui->treeWidget_protocol_folder->addTopLevelItem(item);
	}
}

void Labonatip_GUI::openProtocolFolder()
{
	QDir path = QFileDialog::getExistingDirectory(this, m_str_select_folder, m_protocol_path);
	QString pp = path.path();
	if (pp != ".") // this prevent cancel to delete the old path
		setProtocolUserPath(pp);
}

void Labonatip_GUI::onProtocolClicked(QTreeWidgetItem *item, int column)
{
	std::cout << HERE << std::endl;

	// retrieve the clicked file name
	QString textOfItem = item->text(0);
	int found = textOfItem.indexOf(".prt");

	if (found == -1)
	{
		// a folder was clicked
		m_protocol_path.append("/");
		m_protocol_path.append(textOfItem);
		m_protocol_path = QDir::cleanPath(m_protocol_path);
		readProtocolFolder(m_protocol_path);
		return;
	}
	else
	{
	  // a protocol was clicked
	m_current_protocol_file_name = textOfItem;
	// append the path
	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append(textOfItem);

	QMessageBox::StandardButton resBtn = QMessageBox::Yes;

	if (m_protocol->size() > 0)
	{
		resBtn = QMessageBox::question(this, m_str_warning,
			m_str_add_protocol_bottom + "<br>" + m_str_add_protocol_bottom_guide,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	}
	
	if (resBtn == QMessageBox::Yes) {
		// read the clicked protocol and add it to the current 
		QApplication::setOverrideCursor(Qt::WaitCursor);   
		//m_reader->readProtocol(ui->treeWidget_macroTable, protocol_path);
		this->openXml(protocol_path, ui->treeWidget_macroTable);
		updateTreeView(ui->treeWidget_macroTable);
		m_current_protocol_file_name = protocol_path;
		QApplication::restoreOverrideCursor();   
	}
	if (resBtn == QMessageBox::No)
	{
		// clear the current protocol and load the clicked protocol instead
		QApplication::setOverrideCursor(Qt::WaitCursor);
		clearAllCommands(); 
		//m_reader->readProtocol(ui->treeWidget_macroTable, protocol_path);
		this->openXml(protocol_path, ui->treeWidget_macroTable); 
		updateTreeView(ui->treeWidget_macroTable);
		m_current_protocol_file_name = protocol_path;
		QApplication::restoreOverrideCursor();
	}
	if (resBtn == QMessageBox::Cancel)
	{
		//do nothing
	}
	}
	
}

void Labonatip_GUI::updateTreeView(QTreeWidget* _tree)
{
	for (int i = 0;
		i < _tree->topLevelItemCount();
		++i) {
		// get the current item
		protocolTreeWidgetItem* item =
			dynamic_cast<protocolTreeWidgetItem*> (
				_tree->topLevelItem(i));
		// update the command numbering
		_tree->blockSignals(true);
		item->setText(editorParams::c_idx, QString::number(i + 1));
		item->setText(editorParams::c_range, item->getRangeColumn(item->text(editorParams::c_command).toInt()));
		item->checkValidity(editorParams::c_value); 
		_tree->blockSignals(false);
		

		if (item->childCount() > 0)
		{
			for (int childrenCount = 0; childrenCount < item->childCount(); childrenCount++) {
				_tree->blockSignals(true);
				protocolTreeWidgetItem* item_child =
					dynamic_cast<protocolTreeWidgetItem*> (
						item->child(childrenCount));
				item_child->setText(
					editorParams::c_idx, QString::number(childrenCount + 1));
				item_child->setText(editorParams::c_range, item_child->getRangeColumn(item_child->text(editorParams::c_command).toInt()));
				item_child->checkValidity(editorParams::c_value);

				if (item_child->childCount() > 0)
					this->updateChildrenView(item_child);

				_tree->blockSignals(false);
			}
		}
		
	}

}

void Labonatip_GUI::updateChildrenView(protocolTreeWidgetItem* _parent)
{
	if (_parent->childCount() > 0)
	{
		for (int childrenCount = 0; childrenCount < _parent->childCount(); childrenCount++) {
			protocolTreeWidgetItem* item_child =
				dynamic_cast<protocolTreeWidgetItem*> (
					_parent->child(childrenCount));
			item_child->setText(
				editorParams::c_idx, QString::number(childrenCount + 1));
			item_child->checkValidity(editorParams::c_value);

			if (item_child->childCount() > 0)
				this->updateChildrenView(item_child);
		}
	}
}

void Labonatip_GUI::addAllCommandsToPPC1Protocol(QTreeWidget* _tree,
	std::vector<fluicell::PPC1api6dataStructures::command>* _protocol)
{
	std::cout << HERE << std::endl;
	// this should be done only in a few specific conditions
	// 1. to update the chart
	// 2. to run the protocol

	// clear the old protocol
	_protocol->clear();

	// container for all the items from the protocol commands
	std::vector<protocolTreeWidgetItem*> command_vector;
	// the rational here is that all the commands from the high level are translated into
	// elementary commands for the low level, hence loops becomes series of commands
	// functions are substituted with their content and 
	// complex commands are substituted with their rispective list of elements

	// push all the items in the macro table into the command vector
	fromTreeToItemVector(_tree, &command_vector);
	
	// then from the command vector are pushed to the protocol
	fromItemVectorToProtocol(&command_vector, _protocol);

	// update duration
	double duration = m_ppc1->protocolDuration(*_protocol);
	ui->treeWidget_params->topLevelItem(10)->setText(1, QString::number(duration));
	int remaining_time_sec = duration;
	QString s;
	s.append(m_str_protocol_duration);
	s.append(generateDurationString(remaining_time_sec));
	ui->label_protocolDuration->setText(s);
}

QString Labonatip_GUI::generateDurationString(int _time)
{
	QString s;
	int remaining_hours = floor(_time / 3600); // 3600 sec in a hour
	int remaining_mins = floor((_time % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = _time - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	s.append(" ");
	s.append(m_str_h);
	s.append(",   ");
	s.append(QString::number(remaining_mins));
	s.append(" ");
	s.append(m_str_min);
	s.append(",   ");
	s.append(QString::number(remaining_secs));
	s.append(" ");
	s.append(m_str_sec);
	s.append("   ");
	return s;
}

void Labonatip_GUI::fromTreeToItemVector(QTreeWidget* _tree,
	std::vector<protocolTreeWidgetItem*>* _command_vector)
{
	for (int i = 0;
		i < _tree->topLevelItemCount();
		++i) {

		// get the current item
		protocolTreeWidgetItem* item =
			dynamic_cast<protocolTreeWidgetItem*> (_tree->topLevelItem(i));

		if (item->childCount() < 1) { // if no children, just add the line 
			interpreter(item, _command_vector);
		}
		else
		{
			// otherwise we need to traverse the subtree
			for (int loop = 0; loop < item->text(editorParams::c_value).toInt(); loop++) {

				traverseChildren(item, _command_vector);
			}
		}
	}
}

void Labonatip_GUI::fromItemVectorToProtocol(std::vector<protocolTreeWidgetItem*>* _command_vector,
	std::vector<fluicell::PPC1api6dataStructures::command>* _protocol)
{
	// now all the items are inside the vector and we can interpret them as protocol commands
	for (int i = 0; i < _command_vector->size(); i++)
	{
		fluicell::PPC1api6dataStructures::command new_command;

		std::string a = _command_vector->at(i)->text(editorParams::c_command).toStdString();

		//TODO: here it would be possible in principle to use a different interpretation from
		//      tree structure to actual protocol command, this would allow to have more complex command made of
		//      a series of commands, example: ramp pressure can be seen as a series of different pressures and wait commands
		//      hence it is possible to separate elementary commands and complex commands that can be destructurated
		//      into a set of elementary commands
#pragma message (" TODO: ----- setInstruction  static_cast<pCmd> ")
		new_command.setInstruction(static_cast<ppc1Cmd>(
			_command_vector->at(i)->text(editorParams::c_command).toInt()));

		new_command.setValue(_command_vector->at(i)->text(editorParams::c_value).toInt());
		new_command.setStatusMessage(_command_vector->at(i)->text(editorParams::c_msg).toStdString());

		_protocol->push_back(new_command);
	}
}

void Labonatip_GUI::interpreter(protocolTreeWidgetItem* _item,
	std::vector<protocolTreeWidgetItem*>* _command_vector )
{
	if (_item->childCount() > 1)
		// error this cannot be done
		return;

	int command_idx = _item->text(editorParams::c_command).toInt();

	//TODO: this should be done once and for all at the very beginning
	QString preset_protocols_path = QDir::homePath();
	preset_protocols_path.append("/Documents/Biopen6/presetProtocols/internal/");
	QDir preset_protocols_dir;
	if (!preset_protocols_dir.exists(preset_protocols_path)) {
		// TODO: define what to do here, re-install message?
		return;
	}

	switch (command_idx)
	{
	case protocolCommands::allOff:
	case protocolCommands::solution1:
	case protocolCommands::solution2:
	case protocolCommands::solution3:
	case protocolCommands::solution4:
	case protocolCommands::solution5:
	case protocolCommands::solution6:
	case protocolCommands::setPon:
	case protocolCommands::setPoff:
	case protocolCommands::setVrecirc:
	case protocolCommands::setVswitch:
	case protocolCommands::waitSync:
	case protocolCommands::syncOut:
	case protocolCommands::wait:
	case protocolCommands::ask:
	case protocolCommands::pumpsOff: // TODO: check pump off as this is also a set of commands
	{
		_command_vector->push_back(
		dynamic_cast<protocolTreeWidgetItem*> (_item)); 
		return;
	}
	case protocolCommands::loop: // this should never happen as there is a child for that
	{
		return;
	}
	case protocolCommands::function: // this should never happen as there is a child for that
	{
		return;
	}
	case protocolCommands::comment: // this is just a comment so nothing happens no command added
	{
		return;
	}
	case protocolCommands::button1: 
	{ 
		// load the protocol for pushSolution1 or stopSolution1
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		if (_item->text(editorParams::c_value).toInt() == 1)
			openXml(QString (preset_protocols_path + "/pumpSolution1.prt"), virtual_tree_widget);
		else if (_item->text(editorParams::c_value).toInt() == 0)
			openXml(QString(preset_protocols_path + "/stopSolution1.prt"), virtual_tree_widget);
		else // this should never happen 
			return;

		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::button2:
	{
		// load the protocol for pushSolution2 or stopSolution2
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		if (_item->text(editorParams::c_value).toInt() == 1)
			openXml(QString(preset_protocols_path + "/pumpSolution2.prt"), virtual_tree_widget);
		else if (_item->text(editorParams::c_value).toInt() == 0)
			openXml(QString(preset_protocols_path + "/stopSolution2.prt"), virtual_tree_widget);
		else // this should never happen 
			return;

		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::button3:
	{
		// load the protocol for pushSolution3 or stopSolution3
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		if (_item->text(editorParams::c_value).toInt() == 1)
			openXml(QString(preset_protocols_path + "/pumpSolution3.prt"), virtual_tree_widget);
		else if (_item->text(editorParams::c_value).toInt() == 0)
			openXml(QString(preset_protocols_path + "/stopSolution3.prt"), virtual_tree_widget);
		else // this should never happen 
			return;

		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::button4:
	{
		// load the protocol for pushSolution4 or stopSolution4
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		if (_item->text(editorParams::c_value).toInt() == 1)
			openXml(QString(preset_protocols_path + "/pumpSolution4.prt"), virtual_tree_widget);
		else if (_item->text(editorParams::c_value).toInt() == 0)
			openXml(QString(preset_protocols_path + "/stopSolution4.prt"), virtual_tree_widget);
		else // this should never happen 
			return;

		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::button5:
	{
		// load the protocol for pushSolution5 or stopSolution5
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		if (_item->text(editorParams::c_value).toInt() == 1)
			openXml(QString(preset_protocols_path + "/pumpSolution5.prt"), virtual_tree_widget);
		else if (_item->text(editorParams::c_value).toInt() == 0)
			openXml(QString(preset_protocols_path + "/stopSolution5.prt"), virtual_tree_widget);
		else // this should never happen 
			return;

		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::button6:
	{
		// load the protocol for pushSolution6 or stopSolution6
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		if (_item->text(editorParams::c_value).toInt() == 1)
			openXml(QString(preset_protocols_path + "/pumpSolution6.prt"), virtual_tree_widget);
		else if (_item->text(editorParams::c_value).toInt() == 0)
			openXml(QString(preset_protocols_path + "/stopSolution1.prt"), virtual_tree_widget);
		else // this should never happen 
			return;

		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::ramp:
	{
		// load the protocol for ramp
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/rampPon.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::operational:
	{
		// load the protocol for operational
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/operational.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::initialize:
	{
		// load the protocol for newtip
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/initialize.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::standby:
	{
		// load the protocol for standby
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/standby.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::standardAndSlow: {
		// load the protocol for standby
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/StandardAndSlow.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::standardAndRegular: {
		// load the protocol for standby
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/StandardAndRegular.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::largeAndSlow: {
		// load the protocol for standby
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/LargeAndSlow.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	case protocolCommands::largeAndRegular: {
		// load the protocol for standby
		QTreeWidget* virtual_tree_widget = new QTreeWidget();
		openXml(QString(preset_protocols_path + "/LargeAndRegular.prt"), virtual_tree_widget);
		fromTreeToItemVector(virtual_tree_widget, _command_vector);
		return;
	}
	default:
		break;
	}
}

void Labonatip_GUI::traverseChildren(protocolTreeWidgetItem* _parent, 
	std::vector<protocolTreeWidgetItem*>* _command_vector)
{
	std::cout << HERE << std::endl;
	
	for (int i = 0; i < _parent->childCount(); i++)
	{
		protocolTreeWidgetItem* child =
			dynamic_cast<protocolTreeWidgetItem*> (_parent->child(i));

		// if the item is a loop or a function we need to traverse the subtree
		if (child->childCount() < 1) { // if no children, just add the line 
			interpreter(child, _command_vector);
		}
		else
		{
			// basically the subtree is always the same for loops or function, 
			// just the function will be traversed only once
			for (int loop = 0; loop < child->text(editorParams::c_value).toInt(); loop++) {
				traverseChildren(child, _command_vector);
			}
		}
	}

}

void Labonatip_GUI::protocolsMenu(const QPoint & _pos)
{
	std::cout << HERE << std::endl;

	m_triggered_protocol_item = //a class member is used to pass a data between functions
		ui->treeWidget_protocol_folder->indexAt(_pos).row();


	QAction* show_in_folder = new QAction(
		QIcon(":/icons/open_off.png"), tr("&Show in folder"), this);
	show_in_folder->setStatusTip(tr("new sth"));
	connect(show_in_folder, SIGNAL(triggered()),
		this, SLOT(OnShowInFolderClicked())); 
	
	QAction *delete_protocol = new QAction(
		QIcon(":/icons/delete.png"), tr("&Delete"), this);
	delete_protocol->setStatusTip(tr("new sth"));
	connect(delete_protocol, SIGNAL(triggered()),
		this, SLOT(deleteProtocol()));

	QAction *help = new QAction(
		QIcon(":/icons/about.png"), tr("&Help"), this);
	help->setStatusTip(tr("new sth"));
	connect(help, SIGNAL(triggered()), this, SLOT(helpTriggered()));

	QMenu menu(this);
	menu.addAction(show_in_folder);
	menu.addAction(delete_protocol);
	menu.addAction(help);

	QPoint pt(_pos);
	menu.exec(ui->treeWidget_protocol_folder->mapToGlobal(_pos));

}

void Labonatip_GUI::onTabEditorChanged(int _idx)
{
	std::cout << HERE << std::endl;

	// update code from tree to xml to machine code and vice versa
	int current_index = ui->tabWidget_editor->currentIndex();
	switch (current_index)
	{
	case 0:
	{// we are now in the tree view/editor

		QString save_tmp_file = QDir::tempPath();
		save_tmp_file.append("/tmp_biopen_xml.prt");
		{
			QFile file(save_tmp_file);
			if (!file.open(QFile::WriteOnly | QFile::Text)) {
				std::cerr << HERE <<
					" impossible to open the temporary file for the protocol from the commander " << std::endl;
				return;
			}

			QString fileContent = ui->textBrowser_XMLcode->toPlainText();
			file.write(fileContent.toUtf8());
		}

		ui->treeWidget_macroTable->clear();
		openXml(save_tmp_file, ui->treeWidget_macroTable);
		updateTreeView(ui->treeWidget_macroTable);
		QFile f(save_tmp_file);
		f.remove();
		return;
	}
	case 1:
	{// we are now in the XML code view/editor

#pragma message ("TODO: this is not correct, we are first writing to a file and then reloading it")
		QString save_tmp_file = QDir::tempPath();
		save_tmp_file.append("/tmp_biopen_xml.prt");
		{// Do not remove this parentesis, the file is written on destruction which is automatically done when it goes out of scope
			QFile file(save_tmp_file);
			if (!file.open(QFile::ReadWrite | QFile::Text)) {
				std::cerr << HERE <<
					" impossible to open the temporary file for the protocol from the commander " << std::endl;
				return;
			}
			XmlProtocolWriter writer(ui->treeWidget_macroTable);
			if (!writer.writeFile(&file))
			{
				std::cerr << HERE <<
					" impossible to write the temporary file for the protocol from the commander" << std::endl;
				return;
			}
	    }

		QFile readfile(save_tmp_file);
		readfile.open(QIODevice::ReadOnly);
		QString plainXMLcode(QString::fromUtf8(readfile.readAll()));
		ui->textBrowser_XMLcode->setPlainText(plainXMLcode);
		return;
	}
	case 2:
	{
		
#pragma message ("TODO: this is not correct, we are first writing to a file and then reloading it")
		QString save_tmp_file = QDir::tempPath();
		save_tmp_file.append("/tmp_biopen_xml.prt");
		{// Do not remove this parentesis, the file is written on destruction which is automatically done when it goes out of scope
			QFile file(save_tmp_file);
			if (!file.open(QFile::ReadWrite | QFile::Text)) {
				std::cerr << HERE <<
					" impossible to open the temporary file for the protocol from the commander " << std::endl;
				return;
			}
			XmlProtocolWriter writer(ui->treeWidget_macroTable);
			if (!writer.writeFile(&file))
			{
				std::cerr << HERE <<
					" impossible to write the temporary file for the protocol from the commander" << std::endl;
				return;
			}
		}

		QFile readfile(save_tmp_file);
		readfile.open(QIODevice::ReadOnly);
		QString plainXMLcode(QString::fromUtf8(readfile.readAll()));
		ui->textBrowser_XMLcode->setPlainText(plainXMLcode);

#pragma message ("TODO: code repetition")

		ui->textBrowser_machineCode->clear();
		// we are now in the machine code editor
		addAllCommandsToPPC1Protocol(ui->treeWidget_macroTable, 
			m_protocol);

		for (auto element = m_protocol->begin(); element < m_protocol->end(); element++)
		{
			QString new_line = QString::fromStdString(element->getCommandAsString());
			new_line.append("  (");
			if (element->getInstruction() == ppc1Cmd::ask)
				new_line.append(QString::fromStdString(element->getStatusMessage()));
			else
				new_line.append(QString::number(element->getValue()));
			new_line.append(")  ");
			ui->textBrowser_machineCode->append(new_line);
		}


		return;
	}
	default:
	{
		//something weird happened
		return;
	}
	}
	

}


void Labonatip_GUI::OnShowInFolderClicked()
{
	std::cout << HERE << std::endl;

	QString file_path = m_protocol_path;
	QDesktopServices::openUrl(QUrl::fromLocalFile(m_protocol_path));
}

void Labonatip_GUI::helpTriggered() {

	std::cout << HERE << std::endl;	
	this->about();
}

void Labonatip_GUI::deleteProtocol()
{
	std::cout << HERE << std::endl;

	// TODO: this is not safe as the member could be modified somewhere else
	int row = m_triggered_protocol_item;

	QString file_path = m_protocol_path;
	file_path.append(
		ui->treeWidget_protocol_folder->topLevelItem(row)->text(0));

	QMessageBox::StandardButton resBtn = 
		QMessageBox::question(this, m_str_warning, m_str_remove_file,
			QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn == QMessageBox::Yes) {
		// continue with file removal 

		QFile f(file_path);
		if (f.exists())
		{
			// delete file
			f.remove();
			// update the folder
			readProtocolFolder(m_protocol_path);
		}
		else
		{
			// the file does not exists
			return;
		}
	}
	else {
		// the choice was no, nothing happens
		return;
	}
}

void Labonatip_GUI::addCommand()
{
	std::cout << HERE << std::endl;

	// create the command
	addProtocolCommand *new_command;
	int row = 0;
	//if we are at the top level with no element or no selection 
	// the element is added at the last position
	if (!ui->treeWidget_macroTable->currentIndex().isValid())
	{
		row = ui->treeWidget_macroTable->topLevelItemCount();
		new_command = new addProtocolCommand(
			ui->treeWidget_macroTable, row);
	}
	else { 	//else we add the item at the selected row
		row = ui->treeWidget_macroTable->currentIndex().row() + 1;

		// get the parent
		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem * >(
				ui->treeWidget_macroTable->currentItem()->parent());
		new_command = new addProtocolCommand(
			ui->treeWidget_macroTable, row, parent);
	}

	// add the new command in the undo stack
	m_undo_stack->push(new_command);

	// focus is give to the new added element
	ui->treeWidget_macroTable->setCurrentItem(
		new_command->item(), editorParams::c_value,
		QItemSelectionModel::SelectionFlag::Rows);

	// every time we add a new command we update all macro commands
	// this is not really nice, better to append (much faster)
	updateTreeView(ui->treeWidget_macroTable);
}

void Labonatip_GUI::removeCommand()
{
	std::cout << HERE << std::endl;

	// avoid crash is no elements in the table or no selection
	if (ui->treeWidget_macroTable->currentItem() &&
		ui->treeWidget_macroTable->topLevelItemCount() > 0) {
		removeProtocolCommand *cmd;

		// get the current row
		int row = ui->treeWidget_macroTable->currentIndex().row();

		protocolTreeWidgetItem * parent = dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem()->parent());
		cmd = new removeProtocolCommand(
			ui->treeWidget_macroTable, row, parent);
		m_undo_stack->push(cmd);
	}

	// every time we remove a command we update the macro command
	updateTreeView(ui->treeWidget_macroTable);
}


void Labonatip_GUI::moveUp()
{
	std::cout << HERE << std::endl;

	// get the current selected item
	protocolTreeWidgetItem *move_item =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());
	int row = ui->treeWidget_macroTable->currentIndex().row();

	// if the selection is valid and we are not at the first row
	if (move_item && row > 0)
	{
		moveUpCommand *cmd;

		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem *> (
				ui->treeWidget_macroTable->currentItem()->parent());

		// if we are not at the fist level, so the item has a parent
		if (parent) {
			cmd = new moveUpCommand(ui->treeWidget_macroTable,
				parent);
		}
		else {
			// if we are on the top level, just take the item 
			// and add the selected item one row before
			cmd = new moveUpCommand(ui->treeWidget_macroTable);
		}
		m_undo_stack->push(cmd);
		ui->treeWidget_macroTable->setCurrentItem(move_item);
		ui->treeWidget_macroTable->setCurrentItem(
			move_item, editorParams::c_value, QItemSelectionModel::SelectionFlag::Rows);
	}

	// update the macro command
	updateTreeView(ui->treeWidget_macroTable);
}

void Labonatip_GUI::moveDown()
{
	std::cout << HERE << std::endl;

	// get the current selected item
	protocolTreeWidgetItem *move_item =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());
	int row = ui->treeWidget_macroTable->currentIndex().row();
	int number_of_items = ui->treeWidget_macroTable->topLevelItemCount();

	if (move_item && row >= 0 && row < number_of_items - 1)
	{
		moveDownCommand *cmd;
		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem *> (
				ui->treeWidget_macroTable->currentItem()->parent());

		// if the item has a parent
		if (parent) {
			cmd = new moveDownCommand(ui->treeWidget_macroTable,
				parent);
		}
		else {
			cmd = new moveDownCommand(ui->treeWidget_macroTable);
		}

		m_undo_stack->push(cmd);
		ui->treeWidget_macroTable->setCurrentItem(move_item);
		ui->treeWidget_macroTable->setCurrentItem(
			move_item, editorParams::c_value, QItemSelectionModel::SelectionFlag::Rows);
	}

	// update the macro command
	updateTreeView(ui->treeWidget_macroTable);
}

void Labonatip_GUI::plusIndent()
{
	std::cout << HERE << std::endl;

	// create the command
	addProtocolCommand *cmd;

	// if no item selected, add to the top level
	if (!ui->treeWidget_macroTable->currentIndex().isValid())
	{
		cmd = new addProtocolCommand(ui->treeWidget_macroTable, 0);
	}
	else { // otherwise add the item as a child
		protocolTreeWidgetItem *parent =
			dynamic_cast<protocolTreeWidgetItem *> (
				ui->treeWidget_macroTable->currentItem());

		//if (parent->QTreeWidgetItem::parent()) {
		//	QMessageBox::warning(this, m_str_warning,
		//		QString("Currently only one loop level is supported"));
		//	return; //TODO: fix this --- translate string
		//}
#pragma message (" TODO: ----- support more loops and functions ")

		if (parent) {

			// set the current parent to be a loop
#pragma message (" TODO: ----- QString::number(pCmd::loop) ")
			//parent->setText(editorParams::c_command, QString::number(ppc1Cmd::loop));
			// TODO: what should happen here is that only loop or functions can be added 

			// add the new line as a child
			cmd = new addProtocolCommand(
				ui->treeWidget_macroTable, 0, parent);
		}

	}

	// add the new command in the undo stack
	m_undo_stack->push(cmd);
#pragma message ("TODO:  UNDO REDO do not work with multiple loops" )

	// update the macro command
	updateTreeView(ui->treeWidget_macroTable);
}

bool Labonatip_GUI::itemChanged(QTreeWidgetItem *_item, int _column)
{
	if (_column == editorParams::c_idx ||
		_column == editorParams::c_range)
	{
		dynamic_cast<protocolTreeWidgetItem *>(_item)->checkValidity(_column);
		return true;
	}
	else
	{
		// if the changed element has a parent 
		if (_item->parent())
		{
			changedProtocolCommand * cmd =
				new changedProtocolCommand(ui->treeWidget_macroTable,
					dynamic_cast<protocolTreeWidgetItem *>(_item), _column,
					dynamic_cast<protocolTreeWidgetItem *>(_item->parent()));

			// push the command into the stack
			m_undo_stack->push(cmd);
		}
		else
		{
			// the command is called with null pointer on the parent
			changedProtocolCommand * cmd =
				new changedProtocolCommand(
					ui->treeWidget_macroTable,
					dynamic_cast<protocolTreeWidgetItem *>(_item),
					_column);

			// push the command into the stack
			m_undo_stack->push(cmd);
		}
		dynamic_cast<protocolTreeWidgetItem *>(_item)->checkValidity(_column);
	}

	//addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);
	updateTreeView(ui->treeWidget_macroTable);
	return true;
}


void Labonatip_GUI::duplicateItem()
{
	std::cout << HERE << std::endl;

	// avoid crash if no selection
	if (!ui->treeWidget_macroTable->currentItem()) return;

	// get the current item to clone
	protocolTreeWidgetItem *to_clone =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());

	int command_idx = to_clone->text(editorParams::c_command).toInt();
	int value = to_clone->text(editorParams::c_value).toInt();
	//Qt::CheckState show_msg = to_clone->checkState(m_cmd_msg_c);
	QString msg = to_clone->text(editorParams::c_msg);

	this->addCommand();

	// get the clone, I am aware that the function give the new focus to the added item
	protocolTreeWidgetItem *clone =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());


	clone->setText(editorParams::c_command, QString::number(command_idx));
	clone->setText(editorParams::c_value, QString::number(value));
	//clone->setCheckState(m_cmd_command_c, show_msg);
	clone->setText(editorParams::c_msg, msg);

	updateTreeView(ui->treeWidget_macroTable);
	return;
}


void Labonatip_GUI::createNewLoop()
{
	this->createNewLoop(2);  // set 2 loops as minimum value ?
}

void Labonatip_GUI::createNewLoop(int _loops)
{
	std::cout << HERE << std::endl;

	this->addCommand();
	ui->treeWidget_macroTable->currentItem()->setText(
		editorParams::c_command, QString::number(ppc1Cmd::loop));// "Loop"); // 
#pragma message (" TODO: ----- QString::number(pCmd::loop) ")

	this->plusIndent();
	updateTreeView(ui->treeWidget_macroTable);
	return;
}


void Labonatip_GUI::createNewFunction()
{
	std::cout << HERE << std::endl;

	this->addCommand();
	ui->treeWidget_macroTable->currentItem()->setText(
		editorParams::c_command, "28");// "function"); // 
#pragma message (" TODO: ----- QString::number(function) ")

	this->plusIndent();
	updateTreeView(ui->treeWidget_macroTable);
	return;
}

void Labonatip_GUI::clearAllCommandsRequest()
{
	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, m_str_information,
			m_str_clear_commands + "<br>" + m_str_areyousure,
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		// do nothing
	}
	else {
		clearAllCommands();
	}
}


void Labonatip_GUI::clearAllCommands() {
	std::cout << HERE << std::endl;

	ui->treeWidget_macroTable->clear();
	m_protocol->clear();
	m_undo_stack->clear();
}



void Labonatip_GUI::showUndoStack()
{
	std::cout << HERE << std::endl;
	m_undo_view->show();
}

void Labonatip_GUI::undo()
{
	std::cout << HERE << std::endl;

	ui->treeWidget_macroTable->blockSignals(true);
	m_undo_stack->undo();
	ui->treeWidget_macroTable->blockSignals(false);
	updateTreeView(ui->treeWidget_macroTable);
}

void Labonatip_GUI::redo()
{
	std::cout << HERE << std::endl;

	ui->treeWidget_macroTable->blockSignals(true);
	m_undo_stack->redo();
	ui->treeWidget_macroTable->blockSignals(false);
}
