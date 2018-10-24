/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef TRANSLATOR_GUI_H_
#define TRANSLATOR_GUI_H_

#include "ui_Translator_GUI.h"



class Translator_GUI;

/**  
*  
* \note
*/
class Translator_GUI : public  QDialog
{
	Q_OBJECT


public:
	explicit Translator_GUI(QWidget *parent = nullptr);

	~Translator_GUI();

	void exit();
	
private slots:

	void slot1(){}
	
private:
	bool local;


protected:
	Ui::Translator_GUI *ui;    //--> the user interface
};

#endif /* TRANSLATOR_GUI_H_ */