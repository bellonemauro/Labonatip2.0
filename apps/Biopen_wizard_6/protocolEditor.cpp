/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "Lab-on-a-tip.h"


void Labonatip_GUI::readProtocolFolder(QString _path)
{
	ui->treeWidget_protocol_folder->clear();

	ui->lineEdit_protocolPath->setText(_path);

	QStringList filters;
	filters << "*.prt";

	QDir protocol_path;
	protocol_path.setPath(_path);
	QStringList list = protocol_path.entryList(filters);
	
	for (int i = 0; i < list.size(); i++) // starting from 2 it will not add ./ and ../
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, list.at(i));
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
	QString file = item->text(0);
	m_current_protocol_file_name = file;
	// append the path
	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append(file);

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
		m_reader->readProtocol(ui->treeWidget_macroTable, protocol_path);
		addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);
		m_current_protocol_file_name = protocol_path;
		QApplication::restoreOverrideCursor();   
	}
	if (resBtn == QMessageBox::No)
	{
		// clear the current protocol and load the clicked protocol instead
		QApplication::setOverrideCursor(Qt::WaitCursor);
		clearAllCommands(); 
		m_reader->readProtocol(ui->treeWidget_macroTable, protocol_path);
		addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);
		m_current_protocol_file_name = protocol_path;
		QApplication::restoreOverrideCursor();
	}
	if (resBtn == QMessageBox::Cancel)
	{
		//do nothing
	}

	
	//
}

void Labonatip_GUI::addAllCommandsToProtocol(QTreeWidget* _tree,
                                             std::vector<fluicell::PPC1api6dataStructures::command>* _protocol)
{
	/////////////////////////////////////////////////////////////////////////////////
	//TODO THIS IS WEIRD
	//QList<QStringList> protocol_last = visitTree(ui_p_editor->treeWidget_macroTable);
	/////////////////////////////////////////////////////////////////////////////////


	// all the items 
	std::vector<protocolTreeWidgetItem*> commands_vector;
	//TODO: this is not the best way, 
	//      every time it will re-build the whole macro 
	//      instead of update (waste of time)
	_protocol->clear();

	// push all the items in the macro table into the command vector
	for (int i = 0;
		i < _tree->topLevelItemCount();
		++i) {

		// get the current item
		protocolTreeWidgetItem *item =
			dynamic_cast<protocolTreeWidgetItem * > (
				_tree->topLevelItem(i));
		_tree->blockSignals(true);
		item->setText(editorParams::c_idx, QString::number(i + 1));
		_tree->blockSignals(false);
		//item->blockSignals(true);
		//item->checkValidity(m_cmd_value_c);
		//item->blockSignals(false);


		if (item->childCount() < 1) { // if no children, just add the line 
			std::string a = _tree->topLevelItem(i)->text(
				editorParams::c_command).toStdString();

			commands_vector.push_back(
				dynamic_cast<protocolTreeWidgetItem *> (
					_tree->topLevelItem(i)));

		}
		else
		{// otherwise we need to traverse the subtree

		 //commands_vector.push_back(ui_p_editor->treeWidget_macroTable->topLevelItem(i)); 
		 //TODO: the actual item is the loop and it should not be added, 
		 //      loops are not supported in the API, they are a high-level feature
		 //TODO: here there is a bug, there is no check that the upper level is actually a loop! 
			for (int loop = 0; loop < item->text(editorParams::c_value).toInt(); loop++) {
				// we need to check how many times we need to run the operations
				// and add the widget to the list
				for (int childrenCount = 0; childrenCount < item->childCount(); childrenCount++) {
					commands_vector.push_back(
						dynamic_cast<protocolTreeWidgetItem *> (
							item->child(childrenCount)));
					_tree->blockSignals(true);
					protocolTreeWidgetItem *item_child =
						dynamic_cast<protocolTreeWidgetItem *> (
							_tree->topLevelItem(i)->child(childrenCount));//
					item_child->setText(
						editorParams::c_idx, QString::number(childrenCount + 1));
					_tree->blockSignals(false);
					//item_child->blockSignals(true);
					//item_child->checkValidity(m_cmd_value_c);
					//item_child->blockSignals(false);
				}
			}
		}
	}

	for (size_t i = 0; i < commands_vector.size(); ++i)
	{

		fluicell::PPC1api6dataStructures::command new_command;

		std::string a = commands_vector.at(i)->text(editorParams::c_command).toStdString();

        //TODO: here it would be possible in principle to use a different interpretation from
        //      tree structure to actual protocol command, this would allow to have more complex command made of
        //      a series of commands, example: ramp pressure can be seen as a series of different pressures and wait commands
        //      hence it is possible to separate elementary commands and complex commands that can be destructurated
        //      into a set of elementary commands
		new_command.setInstruction(static_cast<pCmd>(
			commands_vector.at(i)->text(editorParams::c_command).toInt()));

		new_command.setValue(commands_vector.at(i)->text(editorParams::c_value).toInt());
		new_command.setStatusMessage(commands_vector.at(i)->text(editorParams::c_msg).toStdString());

		_protocol->push_back(new_command);
	}

	// add the protocol to the stack


	// update duration
	double duration = m_ppc1->protocolDuration(*m_protocol);
	ui->treeWidget_params->topLevelItem(10)->setText(1, QString::number(duration));
	int remaining_time_sec = duration;
	QString s;
	s.append(m_str_protocol_duration);
	int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
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
	ui->label_protocolDuration->setText(s);
}

void Labonatip_GUI::protocolsMenu(const QPoint & _pos)
{
	std::cout << HERE << std::endl;

	m_triggered_protocol_item = //a class member is used to pass a data between functions
		ui->treeWidget_protocol_folder->indexAt(_pos).row();


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
	menu.addAction(delete_protocol);
	menu.addAction(help);

	QPoint pt(_pos);
	menu.exec(ui->treeWidget_protocol_folder->mapToGlobal(_pos));

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
	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);
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
	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);  //TODO: this is not really nice, better to append (much faster)

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
	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);  //TODO: this is not really nice, better to append (much faster)

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
	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);  // TODO: this is not really nice, better to append (much faster)

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

		if (parent->QTreeWidgetItem::parent()) {
			QMessageBox::warning(this, m_str_warning,
				QString("Currently only one loop level is supported"));
			return; //TODO: fix this --- translate string
		}

		if (parent) {

			// set the current parent to be a loop
			parent->setText(editorParams::c_command, QString::number(pCmd::loop));

			// add the new line as a child
			cmd = new addProtocolCommand(
				ui->treeWidget_macroTable, 0, parent);
		}

	}

	// add the new command in the undo stack
	m_undo_stack->push(cmd);

	// update the macro command
	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);  //TODO this is not really nice, better to append (much faster)
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

	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);
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

	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);
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
		editorParams::c_command, QString::number(pCmd::loop));// "Loop"); // 

	this->plusIndent();
	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);
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
	addAllCommandsToProtocol(ui->treeWidget_macroTable, m_protocol);

}

void Labonatip_GUI::redo()
{
	std::cout << HERE << std::endl;

	ui->treeWidget_macroTable->blockSignals(true);
	m_undo_stack->redo();
	ui->treeWidget_macroTable->blockSignals(false);
}
