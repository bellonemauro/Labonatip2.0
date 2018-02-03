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

	inline void setMacroPath(QString _path) { m_protocol_path = _path; 
	readProtocolFolder(m_protocol_path);
	}

	void setMacroPrt(f_protocol *_protocol) { m_macro = _protocol; };

	void switchLanguage(QString _translation_file);

private slots:


	/** emit ok signal, save the setting, send the current macro to the main
	*   and close the window
	* \note
	*/
	void okPressed();

	/** Emit cancel signal and close the window
	* \note
	*/
	void cancelPressed();

	/** emit apply signal, save the setting, and send the current macro to the main
	*   
	*/
	void applyPressed();

	void emitLoadSettings() { emit loadSettingsRequest(); }

	/** new macro widard
	*
	*/
	void newProtocolWizard();


	/** Load a macro fron file, only one type of macro is currently supported
	*
	*/
	bool loadMacro( );

	/** Save the macro to file
	*
	*/
	bool saveMacro( );

	/** Clear all the macro commands
	*
	*/
	void clearAllCommands();

	void updateChartProtocol(f_protocol * _macro);

	/** Add a new macro command
	*
	*/
	void addMacroCommand();

	/** remove a macro command
	*
	*/
	void removeMacroCommand();

	/** The selected element will become a child for the preceding element
	*
	*/
	void becomeChild();

	/** The selected element will become parent 
	*
	*/
	void becomeParent();
	
	void moveUp();

	void moveDown();

	void plusIndent();

	void duplicateItem();

	bool checkValidity(QTreeWidgetItem *_item, int _column);

	void commandChanged(int _idx);

	void openProtocolFolder();

	void showChartsPanel();

	void showParamsPanel();

	void loadStdP();

	void loadOptP();

	void loadCustomP();

	void loadSleepP();

	void loadAlloffP();

	void on_protocol_clicked(QTreeWidgetItem *item, int column);

	/** Put all the commands in the macro editor to the macro structure for running
	*
	* \note
	*/
	void addAllCommandsToMacro();

	void about();

private:


	void createNewCommand(QTreeWidgetItem &_command, macroCombobox &_combo_box);
	
	/** overload to allow creating the combobox only without the item
	*
	*/
	void createNewCommand(macroCombobox &_combo_box) {
		QTreeWidgetItem item;
		createNewCommand(item, _combo_box);
	}
		
	QString createHeader();
	
	bool loadMacro(const QString _file_name);
	
	bool saveMacro(QString _file_name);
	
	void getLastNode(QTreeWidget *_tree, QTreeWidgetItem *_item);

	int getLevel(QTreeWidgetItem _item);

	//void visitTree(QTreeWidgetItem *_item);

	void visitTree(QList<QStringList> &_list, QTreeWidget *_tree, QTreeWidgetItem *_item);

	QList<QStringList> visitTree(QTreeWidget *_tree);

	bool decodeMacroCommand(QByteArray &_command, QTreeWidgetItem &_out_item);

	void setGUIcharts();

	void initCustomStrings();

	void readProtocolFolder(QString _path);

	f_protocol *m_macro;

	double protocolDuration(std::vector<fluicell::PPC1api::command> _macro);

	QString m_current_protocol_file_name;
	QString m_protocol_path;

	Labonatip_macroWizard * m_protocolWizard;

	solutionsParams *m_solutionParams;
	pr_params *m_pr_params;


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

	double max_pon;// = 450;
	double max_poff;// = 450;
	double max_v_recirc;// = 300;
	double max_v_switch;// = 300;

	QTranslator m_translator_editor;

	//custom translatable stringss
	QString m_str_warning;
	QString m_str_save_protocol;
	QString m_str_load_protocol;
	QString m_str_file_not_found;
	QString m_str_file_not_saved;
	QString m_str_protocol_duration;
	QString m_str_check_validity_msg1;
	QString m_str_check_validity_msg2;
	QString m_str_check_validity_msg3;
	QString m_str_check_validity_msg4;
	QString m_str_check_validity_msg5;
	QString m_str_check_validity_msg6;
	QString m_str_check_validity_msg7;
	QString m_str_check_validity_msg8;
	QString m_str_check_validity_msg9;
	QString m_str_check_validity_msg10;
	QString m_str_check_validity_msg11;
	QString m_str_check_validity_msg12;
	QString m_str_check_validity_msg13;
	QString m_str_check_validity_msg14;
	QString m_str_check_validity_msg15;
	QString m_str_check_validity_msg16;
	QString m_str_check_validity_msg17;
	QString m_str_check_validity_protocol;

protected:
	Ui::Labonatip_protocol_editor *ui_p_editor;    //!<  the user interface
};


#endif /* Labonatip_PROTOCOL_EDITOR_H_ */
