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

Labonatip_protocolWriter::Labonatip_protocolWriter(QTreeWidget * _tree, 
	QMainWindow *parent ) :
	m_cmd_idx_c(0), m_cmd_command_c(1), m_cmd_range_c(2),
	m_cmd_value_c(3), m_cmd_msg_c(4), m_cmd_level_c(5)
{
	m_tree = _tree;
}

void Labonatip_protocolWriter::initCustomStrings()
{
	m_str_warning = tr("Warning");
	m_str_check_validity_protocol = tr("Check validity failed during protocol saving");
	m_str_check_validity_protocol_try_again = tr("Please check your settings and try again");
	m_str_file_not_saved = tr("File not saved");
}

void Labonatip_protocolWriter::switchLanguage(QString _translation_file)
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_protocolWriter::switchLanguage " << endl;

	qApp->removeTranslator(&m_translator_writer);

	if (m_translator_writer.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_writer);

		//ui_tools->retranslateUi(this);

		initCustomStrings();

		cout << QDate::currentDate().toString().toStdString() << "  "
			<< QTime::currentTime().toString().toStdString() << "  "
			<< "Labonatip_protocolWriter::switchLanguage   installTranslator" << endl;
	}

}

bool Labonatip_protocolWriter::saveProtocol(QString _file_name)
{

	if (_file_name.isEmpty()) {

		return false;
	}

	if (!_file_name.endsWith(".prt", Qt::CaseSensitive)) {
		_file_name.append(".prt");
	}

	QFile macroFile(_file_name);

	if (macroFile.open(QIODevice::WriteOnly))
	{
		QTextStream stream(&macroFile);
		QString header = createHeader();
		stream << header << endl;

		QList<QStringList> result = visitTree(m_tree);

		//cout << QDate::currentDate().toString().toStdString() << "  " 
		//	 << QTime::currentTime().toString().toStdString() << "  "
		//	 << "Labonatip_tools::saveProtocol :::: result size " << result.size() << endl;

		for (int i = 0; i < result.size(); i++) {
			for (int j = 0; j < result.at(i).size(); j++)
			{

				//cout << QDate::currentDate().toString().toStdString() << "  " 
				//     << QTime::currentTime().toString().toStdString() << "  "
				//     << " element " << i << " is " << result.at(i).at(j).toStdString() << endl;
				stream << result.at(i).at(j) << "#";
			}
			stream << endl;
		}
	}
	else {
		QMessageBox::warning(this, m_str_warning,
			m_str_file_not_saved + "<br>" + _file_name);
		return false;
	}

	return true;
}

void Labonatip_protocolWriter::visitTree(QList<QStringList> &_list,
	QTreeWidget *_tree, QTreeWidgetItem *_item) {

	QStringList _string_list;

	int depth = 0;
	QTreeWidgetItem *parent = _item->parent();
	while (parent != 0) {
		depth++;
		parent = parent->parent();
	}

	int idx = _item->text(m_cmd_command_c).toInt();

	_string_list.push_back(QString::number(idx));
	_string_list.push_back(_item->text(m_cmd_value_c));
	_string_list.push_back(QString::number(0));// _item->checkState(m_cmd_msg_c)));
	_string_list.push_back(_item->text(m_cmd_msg_c));
	_string_list.push_back(QString::number(depth)); // push the depth of the command as last

	_list.push_back(_string_list);

	for (int i = 0; i<_item->childCount(); ++i)
		visitTree(_list, _tree, _item->child(i));
}

QList<QStringList> Labonatip_protocolWriter::visitTree(QTreeWidget *_tree) {
	QList<QStringList> list;
	for (int i = 0; i < _tree->topLevelItemCount(); ++i) {

		protocolTreeWidgetItem * ii = dynamic_cast<protocolTreeWidgetItem *>(_tree->topLevelItem(i));
		if (!ii->checkValidity(m_cmd_command_c)) {
			QMessageBox::information(this, m_str_warning,
				QString(m_str_check_validity_protocol + "\n" + m_str_check_validity_protocol_try_again));
			list.clear();
			return list;
		}

		visitTree(list, _tree, _tree->topLevelItem(i));
	}
	return list;
}



QString Labonatip_protocolWriter::createHeader()
{

	QString header;
	header.append(tr("%% +---------------------------------------------------------------------------+\n"));
	header.append(tr("%% |                           FLUICELL LAB-ON-A-TIP                           |\n"));
	header.append(tr("%% |                                                                           |\n"));
	header.append(tr("%% |                                                                           |\n"));
	header.append(tr("%% |                          http://www.fluicell.com/                         |\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+\n"));
	header.append(tr("%% \n"));
	header.append(tr("%% Protocol Header V. 0.6 \n"));
	header.append(tr("%% file created on dd/mm/yyyy - "));
	header.append(QDate::currentDate().toString());
	header.append(" ");
	header.append(QTime::currentTime().toString());
	header.append(tr("%% \n"));
	header.append(tr("%% Fluicell Lab-on-a-tip protocol file description \n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% List of parameters: \n"));
	header.append(tr("%%  - Command :  \n"));
	header.append(tr("%%  -       0:  Pressure ON (mbar)           :  Value in mbar\n"));
	header.append(tr("%%  -       1:  Pressure OFF (mbar)          :  Value in mbar\n"));
	header.append(tr("%%  -       2:  Vacuum Switch (mbar)         :  Value in mbar \n"));
	header.append(tr("%%  -       3:  Vacuum Recirculation (mbar)  :  Value in mbar \n"));
	header.append(tr("%%  -       4:  Solution 1 (open/close)      :  \n"));
	header.append(tr("%%  -       5:  Solution 2 (open/close)      :  \n"));
	header.append(tr("%%  -       6:  Solution 3 (open/close)      :  \n"));
	header.append(tr("%%  -       7:  Solution 4 (open/close)      :  \n"));
	header.append(tr("%%  -       8:  Sleep (s)                    :  Wait in seconds \n"));
	header.append(tr("%%  -       9:  Ask (string)                 :  Ask a message at the end of some operation \n"));
	header.append(tr("%%  -       10: All Off                      :  All the valves 1 - 4 will be closed \n"));
	header.append(tr("%%  -       11: Pumps Off                    :  Set the all the pumps to zero (pressure and vacuum) \n"));
	header.append(tr("%%  -       12: Valve state (HEX)            :  Set the valve state \n"));
	header.append(tr("%%  -       13: Wait sync                    :  Wait a sync signal \n"));
	header.append(tr("%%  -       14: Sync out                     :  Sync with external trigger \n"));
	header.append(tr("%%  -       15: Zone size (%)                :  Set the droplet size in % respect to the default values \n"));
	header.append(tr("%%  -       16: Flow speed (%)               :  Set the flow speed in % respect to the default values\n"));
	header.append(tr("%%  -       17: Vacuum (%)                   :  Set the vacuum in % respect to the default values\n"));
	header.append(tr("%%  -       18: Loop (num)                   :  All the commands inside the loop will run cyclically \n"));
	header.append(tr("%%  - \n"));
	header.append(tr("%%  - value (mbar, %, s) - value to be applied to the command\n"));
	header.append(tr("%%  - status_message (string) \n"));
	header.append(tr("%%  - depth : depth in the tree of the command, all the commands at different layers will run in loop\n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%%  - \n"));
	header.append(tr("%% +---------------------------------------------------------------------------+ \n"));
	header.append(tr("%% Command Value status_message depth\n"));
	header.append(tr("%% Follows a line example\n"));
	header.append(tr("%% 13#1#2#message#0#§ \n"));
	header.append(tr("%% Command Value status_message depth\n%"));
	return header;
}
