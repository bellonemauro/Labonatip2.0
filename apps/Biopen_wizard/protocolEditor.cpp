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
		//TODO: here there is a possible memory leak, the pointer *item will never be removed anymore outside the cycle
		//      however, ->addTopLevelItem only accepts pointers 
		//      this means that if one dereferentiate *item that will be removed automatically from the list
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
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString()
		<< "Labonatip_GUI::onProtocolClicked " << endl;

	QString file = item->text(0);
	m_current_protocol_file_name = file;
	QString protocol_path = m_protocol_path;
	protocol_path.append("/");
	protocol_path.append(file);

	// TODO: the wait cursor does not work if called after the message !
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode


	QMessageBox::StandardButton resBtn = QMessageBox::question(this, m_str_warning,
		m_str_add_protocol_bottom + "<br>" + m_str_add_protocol_bottom_guide,
		QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
		QMessageBox::Yes);


	if (resBtn == QMessageBox::Yes) {
		m_reader->readProtocol(protocol_path);
		addAllCommandsToProtocol();
		m_current_protocol_file_name = protocol_path;
	}
	if (resBtn == QMessageBox::No)
	{
		clearAllCommands(); 
		m_reader->readProtocol(protocol_path);
		addAllCommandsToProtocol();
		m_current_protocol_file_name = protocol_path;

	}
	if (resBtn == QMessageBox::Cancel)
	{
		//do nothing
	}
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
}

void Labonatip_GUI::addAllCommandsToProtocol()
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
	m_protocol->clear();

	// push all the items in the macro table into the command vector
	for (int i = 0;
		i < ui->treeWidget_macroTable->topLevelItemCount();
		++i) {

		// get the current item
		protocolTreeWidgetItem *item =
			dynamic_cast<protocolTreeWidgetItem * > (
				ui->treeWidget_macroTable->topLevelItem(i));
		ui->treeWidget_macroTable->blockSignals(true);
		item->setText(m_editor_params->m_cmd_idx_c, QString::number(i + 1));//TODO
		ui->treeWidget_macroTable->blockSignals(false);
		//item->blockSignals(true);
		//item->checkValidity(m_cmd_value_c);
		//item->blockSignals(false);


		if (item->childCount() < 1) { // if no children, just add the line 
			string a = ui->treeWidget_macroTable->topLevelItem(i)->text(
				m_editor_params->m_cmd_command_c).toStdString();

			commands_vector.push_back(
				dynamic_cast<protocolTreeWidgetItem *> (
					ui->treeWidget_macroTable->topLevelItem(i)));

		}
		else
		{// otherwise we need to traverse the subtree

		 //commands_vector.push_back(ui_p_editor->treeWidget_macroTable->topLevelItem(i)); 
		 //TODO: the actual item is the loop and it should not be added, 
		 //      loops are not supported in the API, they are a high-level feature
		 //TODO: here there is a bug, there is no check that the upper level is actually a loop! 
			for (int loop = 0; loop < item->text(m_editor_params->m_cmd_value_c).toInt(); loop++) {
				// we need to check how many times we need to run the operations
				// and add the widget to the list
				for (int childrenCount = 0; childrenCount < item->childCount(); childrenCount++) {
					commands_vector.push_back(
						dynamic_cast<protocolTreeWidgetItem *> (
							item->child(childrenCount)));
					ui->treeWidget_macroTable->blockSignals(true);
					protocolTreeWidgetItem *item_child =
						dynamic_cast<protocolTreeWidgetItem *> (
							ui->treeWidget_macroTable->topLevelItem(i)->child(childrenCount));//
					item_child->setText(
						m_editor_params->m_cmd_idx_c, QString::number(childrenCount + 1));
					ui->treeWidget_macroTable->blockSignals(false);
					//item_child->blockSignals(true);
					//item_child->checkValidity(m_cmd_value_c);
					//item_child->blockSignals(false);
				}
			}
		}
	}

	for (size_t i = 0; i < commands_vector.size(); ++i)
	{

		fluicell::PPC1api::command new_command;

		string a = commands_vector.at(i)->text(m_editor_params->m_cmd_command_c).toStdString();

		new_command.setInstruction(static_cast<pCmd>(
			commands_vector.at(i)->text(m_editor_params->m_cmd_command_c).toInt()));

		new_command.setValue(commands_vector.at(i)->text(m_editor_params->m_cmd_value_c).toInt());
		//new_command.setVisualizeStatus( commands_vector.at(i)->checkState(m_cmd_msg_c)); //TODO clean checkState
		new_command.setStatusMessage(commands_vector.at(i)->text(m_editor_params->m_cmd_msg_c).toStdString());

		m_protocol->push_back(new_command);
	}

	// add the protocol to the stack


	// update duration
	double duration = protocolDuration(*m_protocol);
	ui->treeWidget_params->topLevelItem(8)->setText(1, QString::number(duration));
	int remaining_time_sec = duration;
	QString s;
	s.append(m_str_protocol_duration);
	int remaining_hours = floor(remaining_time_sec / 3600); // 3600 sec in a hour
	int remaining_mins = floor((remaining_time_sec % 3600) / 60); // 60 minutes in a hour
	int remaining_secs = remaining_time_sec - remaining_hours * 3600 - remaining_mins * 60; // 60 minutes in a hour
	s.append(QString::number(remaining_hours));
	s.append(" h,   ");
	s.append(QString::number(remaining_mins));
	s.append(" min,   ");
	s.append(QString::number(remaining_secs));
	s.append(" sec   ");
	ui->label_protocolDuration->setText(s);
}

void Labonatip_GUI::protocolsMenu(const QPoint & _pos)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString()
		<< "Labonatip_tools::protocolsMenu " << endl;

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

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::helpTriggered   " << endl;
	if (1)
	{
		//this->showUndoStack(); //TODO
		return;

	}
	else
	{
		this->about();
	}


}

void Labonatip_GUI::deleteProtocol()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString()
		<< "Labonatip_tools::deleteProtocol " << endl;

	// TODO: this is not safe as the member could be modified somewhere else
	int row = m_triggered_protocol_item;

	QString file_path = m_protocol_path;
	file_path.append(
		ui->treeWidget_protocol_folder->topLevelItem(row)->text(0));

	QMessageBox::StandardButton resBtn = //TODO: translation
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
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::addMacroCommand " << endl;

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
		new_command->item(), m_editor_params->m_cmd_value_c,
		QItemSelectionModel::SelectionFlag::Rows);

	// every time we add a new command we update all macro commands
	// this is not really nice, better to append (much faster)
	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
}

void Labonatip_GUI::removeCommand()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::removeMacroCommand    " << endl;

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
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
								 //updateChartProtocol(m_protocol); //TODO
}


void Labonatip_GUI::moveUp()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::moveUp    " << endl;

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
			move_item, m_editor_params->m_cmd_value_c, QItemSelectionModel::SelectionFlag::Rows);
	}

	// update the macro command
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
								 //updateChartProtocol(m_protocol); //TODO
}

void Labonatip_GUI::moveDown()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::moveDown    " << endl;

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
			move_item, m_editor_params->m_cmd_value_c, QItemSelectionModel::SelectionFlag::Rows);
	}

	// update the macro command
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
								 //updateChartProtocol(m_protocol); //TODO
}

void Labonatip_GUI::plusIndent()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::plusIndent    " << endl;

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
			parent->setText(m_editor_params->m_cmd_command_c, QString::number(17));

			// add the new line as a child
			cmd = new addProtocolCommand(
				ui->treeWidget_macroTable, 0, parent);
		}

	}

	// add the new command in the undo stack
	m_undo_stack->push(cmd);

	// update the macro command
	addAllCommandsToProtocol();  // this is not really nice, better to append (much faster)
								 //updateChartProtocol(m_protocol); //TODO
}

bool Labonatip_GUI::itemChanged(QTreeWidgetItem *_item, int _column)
{
	if (_column == m_editor_params->m_cmd_idx_c || 
		_column == m_editor_params->m_cmd_range_c)
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

	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
	return true;
}


void Labonatip_GUI::duplicateItem()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::duplicateItem    " << endl;

	// avoid crash if no selection
	if (!ui->treeWidget_macroTable->currentItem()) return;

	// get the current item to clone
	protocolTreeWidgetItem *to_clone =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());

	int command_idx = to_clone->text(m_editor_params->m_cmd_command_c).toInt();
	int value = to_clone->text(m_editor_params->m_cmd_value_c).toInt();
	//Qt::CheckState show_msg = to_clone->checkState(m_cmd_msg_c);
	QString msg = to_clone->text(m_editor_params->m_cmd_msg_c);

	this->addCommand();

	// get the clone, I am aware that the function give the new focus to the added item
	protocolTreeWidgetItem *clone =
		dynamic_cast<protocolTreeWidgetItem *> (
			ui->treeWidget_macroTable->currentItem());


	clone->setText(m_editor_params->m_cmd_command_c, QString::number(command_idx));
	clone->setText(m_editor_params->m_cmd_value_c, QString::number(value));
	//clone->setCheckState(m_cmd_command_c, show_msg);
	clone->setText(m_editor_params->m_cmd_msg_c, msg);

	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
	return;
}


void Labonatip_GUI::createNewLoop()
{
	this->createNewLoop(2);  // set 2 loops as minimum value ?
}

void Labonatip_GUI::createNewLoop(int _loops)
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::createNewLoop    " << endl;

	this->addCommand();
	ui->treeWidget_macroTable->currentItem()->setText(
		m_editor_params->m_cmd_command_c, QString::number(17));// "Loop"); // 

	this->plusIndent();
	addAllCommandsToProtocol();
	//updateChartProtocol(m_protocol); //TODO
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
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::clearAllCommands    " << endl;

	ui->treeWidget_macroTable->clear();
	m_protocol->clear();
	m_undo_stack->clear();
}



void Labonatip_GUI::showUndoStack()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::showUndoStack " << endl;
	//if (ui->actionShowStack->isChecked())
	//{
	m_undo_view->show();
	//}
	//else
	//{
	//	m_undo_view->hide();
	//}
}

void Labonatip_GUI::undo()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::undo " << endl;

	ui->treeWidget_macroTable->blockSignals(true);
	m_undo_stack->undo();
	ui->treeWidget_macroTable->blockSignals(false);
	addAllCommandsToProtocol();

}

void Labonatip_GUI::redo()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_GUI::redo " << endl;

	ui->treeWidget_macroTable->blockSignals(true);
	m_undo_stack->redo();
	ui->treeWidget_macroTable->blockSignals(false);
}
