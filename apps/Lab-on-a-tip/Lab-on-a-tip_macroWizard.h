/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_macroWizard_H_
#define Labonatip_macroWizard_H_

#include "Lab-on-a-tip.h"

#include <QtWidgets>

using namespace std;



class Labonatip_macroWizard : public QDialog
{
    Q_OBJECT

public:
	explicit Labonatip_macroWizard(QWidget *parent = 0);

	~Labonatip_macroWizard() {}

	void setMacroPath(QString _path) { m_macro_path = _path; }

private slots:
	
	void next();

	void back();

	bool save();

	void exit();
	
	void closeEvent(QCloseEvent *event);


private:

	QString m_macro_path;

	void setBanner(int _current_index);

protected:
	Ui::Labonatip_macroWizard *ui_wizard;    //!<  the user interface
};



#endif /* Labonatip_macroWizard_H_ */
