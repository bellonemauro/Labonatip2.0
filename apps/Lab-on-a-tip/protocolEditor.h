/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_PROTOCOL_EDITOR_H_
#define Labonatip_PROTOCOL_EDITOR_H_

// standard libraries
#include <iostream>
#include <string>

#include "ui_protocolEditor.h"
#include "protocolWizard.h"

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QTreeWidget>

#include "protocolTreeWidgetItem.h"
#include "protocolCommands.h"

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>

#include "dataStructures.h"

using namespace std;


class Labonatip_protocol_editor : public  QMainWindow
{
	Q_OBJECT

	typedef std::vector<fluicell::PPC1api::command> f_protocol; // define a type for fluicell protocol

/** Create signals to be passed to the main app
* 
*/
signals :
	void ok();  // generated when ok is pressed
	void apply(); // generated when apply is pressed
	void cancel(); // generated when cancel is pressed
	void loadSettingsRequest();  // forward the signal to GUI

public:

	explicit Labonatip_protocol_editor(QWidget *parent = 0);

	~Labonatip_protocol_editor();

	void setSolParams(solutionsParams _params) {
		*m_solutionParams = _params; 
		ui_p_editor->treeWidget_params->topLevelItem(0)->setText(1, m_solutionParams->sol1);
		ui_p_editor->treeWidget_params->topLevelItem(1)->setText(1, m_solutionParams->sol2);
		ui_p_editor->treeWidget_params->topLevelItem(2)->setText(1, m_solutionParams->sol3);
		ui_p_editor->treeWidget_params->topLevelItem(3)->setText(1, m_solutionParams->sol4);
		m_protocolWizard->setSolParams(*m_solutionParams);
	}

	void setPrParams(pr_params _params) { 
		*m_pr_params = _params; 
		ui_p_editor->treeWidget_params->topLevelItem(4)->setText(1, QString::number(m_pr_params->p_on_default));
		ui_p_editor->treeWidget_params->topLevelItem(5)->setText(1, QString::number(m_pr_params->p_off_default));
		ui_p_editor->treeWidget_params->topLevelItem(6)->setText(1, QString::number(m_pr_params->v_recirc_default));
		ui_p_editor->treeWidget_params->topLevelItem(7)->setText(1, QString::number(m_pr_params->v_switch_default));
		m_protocolWizard->setPrParams(*m_pr_params);
	}
	
	
	QString getProtocolPath() { return m_current_protocol_file_name; }

	QString getProtocolName() {
		QFileInfo fi(m_current_protocol_file_name);
		return fi.fileName();
	}

	inline void setProtocolPath(QString _path) { m_protocol_path = _path; 
	readProtocolFolder(m_protocol_path);
	}

	//void setProtocolPrt(f_protocol *_protocol) { m_protocol = _protocol; };

	f_protocol getProtocol() { return *m_protocol; }

	void switchLanguage(QString _translation_file);

	/**  Set the version of the software from the main
	*
	*/
	void setVersion(QString _version) { m_version = _version; }

private slots:

	/** emit ok signal, save the setting, send the current protocol to the main
	*   and close the window
	* \note
	*/
	void okPressed();

	/** Emit cancel signal and close the window
	* \note
	*/
	void cancelPressed();

	/** emit apply signal, save the setting, and send the current protocol to the main
	*   
	*/
	void applyPressed();

	void emitLoadSettings() { emit loadSettingsRequest(); }

	/** new protocol wizard
	*
	*/
	void newProtocolWizard();


	/** Load a protocol from file, only one type of protocol is currently supported
	*
	*/
	bool loadProtocol( );

	/** Save the protocol to file
	*
	*/
	bool saveProtocol( );

	bool saveProtocolAs();


	bool saveXml();
	bool openXml();

	/** Clear all the protocol commands
	*
	*/
	void clearAllCommandsRequest();

	void updateChartProtocol(f_protocol * _protocol);

	/** Show undo stack
	*
	*/
	void showUndoStack();

	void undo();

	void redo();

	/** Add a new protocol command
	*
	*/
	void addCommand();

	/** remove a protocol command
	*
	*/
	void removeCommand();
	
	void moveUp();

	void moveDown();

	void plusIndent();

	void duplicateItem();

	bool itemChanged(QTreeWidgetItem *_item, int _column);

	void openProtocolFolder();

	void showChartsPanel();

	void showParamsPanel();

	// load standard protocol
	void loadStdP();

	void loadOptP();

	void loadCustomP();

	void loadSleepP();

	void loadAlloffP();

	void createNewLoop();

	void createNewLoop(int _loops);

	void onProtocolClicked(QTreeWidgetItem *item, int column);

	void editorMenu(const QPoint & _pos);

	void protocolsMenu(const QPoint & _pos);

	void deleteProtocol();

	/** Put all the commands in the protocol editor to the protocol structure for running
	*
	* \note
	*/
	void addAllCommandsToProtocol(); 

	void helpTriggered();

	void about();

private:

	
	QString createHeader();
	
	bool loadProtocol(const QString _file_name);

	bool saveProtocol(QString _file_name);
	
	int getLevel(QTreeWidgetItem _item);

	void visitTree(QList<QStringList> &_list, QTreeWidget *_tree, QTreeWidgetItem *_item);

	QList<QStringList> visitTree(QTreeWidget *_tree);

	/** \brief Decode a protocol command from file  
	*
	*  The following line type is expected: 
	*  
	*  field0#field1#field2#field3#field4£§
	*
	*  int#int#bool#string#int#§
	*
	*  where: 
	*    field 0 (int)     is the command index --- see fluicell::fluicell::PPC1api::command for details
	*    field 1 (int)     is the value to be applied at the command in 0
	*    field 2 (bool)    show or not the string in field 3
	*    field 3 (string)  comprehensible explanation of the command in field 1
	*    field 4 (int)     level in the execution tree (used for the loop implementation)
	*
	*  example:
	*   3#-45#0#setVrecirc(-45)#0#§
	*
	*	
	*/
	bool decodeProtocolCommand(QByteArray &_command, protocolTreeWidgetItem &_out_item);

	void setGUIcharts();

	void initCustomStrings();

	void readProtocolFolder(QString _path);

	double protocolDuration(std::vector<fluicell::PPC1api::command> _protocol);

	void clearAllCommands();

	f_protocol *m_protocol;

	QString m_current_protocol_file_name;
	QString m_protocol_path;
	QString m_version;

	Labonatip_macroWizard * m_protocolWizard;

	solutionsParams *m_solutionParams;
	pr_params *m_pr_params;
	ComboBoxDelegate * m_combo_delegate;
	NoEditDelegate * m_no_edit_delegate;
	NoEditDelegate * m_no_edit_delegate2;
	SpinBoxDelegate * m_spinbox_delegate;

	// line series for the chart
	QtCharts::QLineSeries *m_series_Pon;
	QtCharts::QChart *m_chart_p_on;
	QtCharts::QChartView *m_chartView_p_on;

	QtCharts::QLineSeries *m_series_Poff;
	QtCharts::QChart *m_chart_p_off;
	QtCharts::QChartView *m_chartView_p_off;

	QtCharts::QLineSeries *m_series_v_s;
	QtCharts::QChart *m_chart_v_s;
	QtCharts::QChartView *m_chartView_v_s;

	QtCharts::QLineSeries *m_series_v_r;
	QtCharts::QChart *m_chart_v_r;
	QtCharts::QChartView *m_chartView_v_r;

	int m_cmd_idx_c;       // index of the column for command index
	int m_cmd_command_c;   // index of the column for the command
	int m_cmd_range_c;     // index of the column for the range
	int m_cmd_value_c;     // index of the column for the value
	int m_cmd_msg_c;       // index of the column for the command status message
	int m_cmd_level_c;     // index of the column for the level in the tree

	// for undo
	QUndoStack *m_undo_stack;
	QUndoView *m_undo_view;

	QTranslator m_translator_editor;

	//TODO: this cannot be a class member, it is used only to pass
	//      a parameter from the menu to the delete_protocol function
	int m_triggered_protocol_item;

	//custom translatable strings
	QString m_str_warning;
	QString m_str_save_protocol;
	QString m_str_load_protocol;
	QString m_str_file_not_found;
	QString m_str_file_not_saved;
	QString m_str_select_folder;
	QString m_str_protocol_duration;
	QString m_str_check_validity_protocol;
	QString m_str_check_validity_protocol_try_again;
	QString m_str_negative_level;
	QString m_str_remove_file;
	QString m_str_current_prot_name;
	QString m_str_question_override;
	QString m_str_override_guide;
	QString m_str_add_protocol_bottom;
	QString m_str_add_protocol_bottom_guide;
	QString m_str_information;
	QString m_str_areyousure;

protected:
	Ui::Labonatip_protocol_editor *ui_p_editor;    //!<  the user interface
};


#endif /* Labonatip_PROTOCOL_EDITOR_H_ */
