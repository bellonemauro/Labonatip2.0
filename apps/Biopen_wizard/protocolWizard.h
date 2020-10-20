/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef Labonatip_macroWizard_H_
#define Labonatip_macroWizard_H_

// standard libraries
#include <iostream>

// autogenerated form header
#include "ui_protocolWizard.h"

// Qt
#include <QtWidgets>
#include <QDateTime>

#include "dataStructures.h"

class Labonatip_macroWizard : public QDialog
{
    Q_OBJECT

public:
	explicit Labonatip_macroWizard(QWidget *parent = 0);

	~Labonatip_macroWizard();

	void setMacroPath(QString _path) { m_macro_path = _path; }

	void setSolParams(solutionsParams _params) {
		*m_solutionParams = _params; 
		updateSolPrFields();
	}

	void setPrParams(pr_params _params) { 
		*m_pr_params = _params; 
	updateSolPrFields();
	}

	void switchLanguage(QString _translation_file);

signals:
	void loadSettings();  // generated when loadsettings is pressed
	void loadStdProtocol();
	void loadOptProtocol();
	void loadCustomProtocol();
	void loadSleepProtocol();
	void loadAllOffProtocol();
	void saveProtocol();

private slots:
	
	void next_page1to2();

	void next_page2to();

	void on_next4_clicked();

	void on_next5_clicked();

	void next();

	void back();

	bool save();

	void exit();
	
	void closeEvent(QCloseEvent *event);


private:

	QString m_macro_path;

	void setBanner(int _current_index);

	QString m_protocol_name;

	void updateSolPrFields();

	solutionsParams *m_solutionParams;
	pr_params *m_pr_params;
	QTranslator m_translator_wizard;

protected:
	Ui::Labonatip_macroWizard *ui_wizard;    //!<  the user interface
};

#endif /* Labonatip_macroWizard_H_ */