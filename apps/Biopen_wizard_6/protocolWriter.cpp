/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolWriter.h"
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QInputDialog>

void protocolWriter::initCustomStrings()
{
	m_str_warning = tr("Warning");
	m_str_information = tr("Information");
	m_str_check_validity_protocol = tr("Check validity failed during protocol saving");
	m_str_check_validity_protocol_try_again = tr("Please check your settings and try again");
	m_str_file_not_saved = tr("File not saved");
	m_ask_password = tr("This is for expert users only, a password is required");
	m_wrong_password = tr("Wrong password, file not saved");
	m_correct_password = tr("Correct password, file saved");
}

void protocolWriter::switchLanguage(QString _translation_file)
{
	std::cout << HERE << std::endl;

    qApp->removeTranslator(&m_translator_writer);

	if (m_translator_writer.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_writer);

		//ui_tools->retranslateUi(this);

		initCustomStrings();

		std::cout << HERE << "installTranslator" << std::endl;
	}

}

bool protocolWriter::saveProtocol(const QTreeWidget *_tree, QString _file_name)
{

	if (_file_name.isEmpty()) {

		return false;
	}

	if (_file_name.contains("stopSolution", Qt::CaseSensitive) ||
		_file_name.contains("pumpSolution", Qt::CaseSensitive))
	{
		// Ask for the password
		bool ok; 
		QString text = QInputDialog::getText(0, m_str_warning,
			m_ask_password, QLineEdit::Password,
			"", &ok);
		if (ok && !text.isEmpty()) {
			QString password = text;
			QString password_check = "FluicellGrowth2018";
			if (!password.compare(password_check))
			{
				QMessageBox::information(this, m_str_information, m_correct_password);
				// continue and go out of this statement to save the file
			}
			else
			{
				QMessageBox::warning(this, m_str_information, m_wrong_password);
				return false;
			}
		}
	}


	if (!_file_name.endsWith(".prt", Qt::CaseSensitive)) {
		_file_name.append(".prt");
	}

	QFile macroFile(_file_name);

	if (macroFile.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&macroFile);
		QString header = createHeader();
        stream << header << Qt::endl;

		QList<QStringList> result = visitTree(_tree);

		for (int i = 0; i < result.size(); i++) {
			for (int j = 0; j < result.at(i).size(); j++)
			{
				stream << result.at(i).at(j) << "#";
			}
            stream << Qt::endl;
		}
	}
	else {
		QMessageBox::warning(this, m_str_warning,
			m_str_file_not_saved + "<br>" + _file_name);
		return false;
	}

	return true;
}

void protocolWriter::visitTree(QList<QStringList> &_list,
	const QTreeWidget *_tree, QTreeWidgetItem *_item) {

	QStringList _string_list;

	int depth = 0;
	QTreeWidgetItem *parent = _item->parent();
	while (parent != 0) {
		depth++;
		parent = parent->parent();
	}

	int idx = _item->text(editorParams::c_command).toInt();

	_string_list.push_back(QString::number(idx));
	_string_list.push_back(_item->text(editorParams::c_value));
	_string_list.push_back(QString::number(0));// _item->checkState(m_editor_params->m_cmd_msg_c)));
	_string_list.push_back(_item->text(editorParams::c_msg));
	_string_list.push_back(QString::number(depth)); // push the depth of the command as last

	_list.push_back(_string_list);

	for (int i = 0; i<_item->childCount(); ++i)
		visitTree(_list, _tree, _item->child(i));
}

QList<QStringList> protocolWriter::visitTree(const QTreeWidget *_tree) {
	QList<QStringList> list;
	for (int i = 0; i < _tree->topLevelItemCount(); ++i) {

		protocolTreeWidgetItem * ii = dynamic_cast<protocolTreeWidgetItem *>(_tree->topLevelItem(i));
		if (!ii->checkValidity(editorParams::c_command)) {
			QMessageBox::information(this, m_str_warning,
				QString(m_str_check_validity_protocol + "\n" + m_str_check_validity_protocol_try_again));
			list.clear();
			return list;
		}

		visitTree(list, _tree, _tree->topLevelItem(i));
	}
	return list;
}

QString protocolWriter::createHeader()
{
	QString header;
	header.append(tr("%% +---------------------------------------------------------------------------+\n"));
	header.append(tr("%% |                           FLUICELL LAB-ON-A-TIP                           |\n"));
	header.append(tr("%% |                                                                           |\n"));
	header.append(tr("%% |                                                                           |\n"));
	header.append(tr("%% |                          http://www.fluicell.com/                         |\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+\n"));
	header.append(tr("%% \n"));
	header.append(tr("%% Protocol Header V. 0.9 \n"));
	header.append(tr("%% file created on dd/mm/yyyy - "));
	header.append(QDate::currentDate().toString());
	header.append(" ");
	header.append(QTime::currentTime().toString());
	header.append(tr("%% \n"));
	header.append(tr("%% Fluicell Lab-on-a-tip protocol file description \n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% List of parameters: \n"));
	header.append(tr("%%  - Command :  \n")); 
	header.append(tr("%%  -       0:  Set Zone size (%)             :  Set the droplet size percentage in the range [50, 200]  \n"));
	header.append(tr("%%  -       1:  Change Zone size by (%)       :  Change the droplet size in % respect to the current values, accepted range [-40, +40] \n"));
	header.append(tr("%%  -       2:  Set Flow speed (%)            :  Set the flow speed percentage in the range [50, 220] \n"));
	header.append(tr("%%  -       3:  Change Flow speed by (%)      :  Change the flow speed in % respect to the current values, accepted range [-40, +40] \n"));
	header.append(tr("%%  -       4:  Set Vacuum (%)                :  Set the vacuum percentage in the range [50, 250] \n"));
	header.append(tr("%%  -       5:  Change Vacuum by (%)          :  Change the vacuum in % respect to the current values, accepted range [-40, +40] \n"));
	header.append(tr("%%  -       6:  Wait (s)                      :  Wait in seconds \n"));
	header.append(tr("%%  -       7:  All Off                       :  All the valves 1 - 4 will be closed \n"));
	header.append(tr("%%  -       8:  Solution 1 (open/close)       :  \n"));
	header.append(tr("%%  -       9:  Solution 2 (open/close)       :  \n"));
	header.append(tr("%%  -       10: Solution 3 (open/close)       :  \n"));
	header.append(tr("%%  -       11: Solution 4 (open/close)       :  \n"));
	header.append(tr("%%  -       12: Solution 5 (open/close)       :  \n"));
	header.append(tr("%%  -       13: Solution 6 (open/close)       :  \n"));
	header.append(tr("%%  -       14: Pressure ON (mbar)            :  Value in mbar\n"));
	header.append(tr("%%  -       15: Pressure OFF (mbar)           :  Value in mbar\n"));
	header.append(tr("%%  -       16: Vacuum Recirculation (mbar)   :  Value in mbar \n"));
	header.append(tr("%%  -       17: Vacuum Switch (mbar)          :  Value in mbar \n"));
	header.append(tr("%%  -       18: Ask (string)                  :  Ask a message at the end of some operation \n"));
	header.append(tr("%%  -       19: Pumps Off                     :  Set the all the pumps to zero (pressure and vacuum) \n"));
	header.append(tr("%%  -       20: Wait sync                     :  Wait a sync signal \n"));
	header.append(tr("%%  -       21: Sync out                      :  Sync with external trigger \n"));
	header.append(tr("%%  -       22: Loop (num)                    :  All the commands inside the loop will run cyclically \n"));
	header.append(tr("%%  - \n"));
	header.append(tr("%%  - value (mbar, %, s) - value to be applied to the command\n"));
	header.append(tr("%%  - status_message (string) \n"));
	header.append(tr("%%  - depth : depth in the tree of the command, all the commands at different layers will run in loop\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%%  - \n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% Command Value status_message depth\n"));
	header.append(tr("%% Follows a line example\n"));
	header.append(tr("%% 13#1#message#0#§ \n"));
	header.append(tr("%% Command Value status_message depth\n%"));
	return header;
}
