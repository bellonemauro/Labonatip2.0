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
#include "XmlTranslationReader.h"
#include "XmlTranslationWriter.h"


class Translator_GUI;

/**  
*  
* \note
*/
class Translator_GUI : public  QMainWindow
{
	Q_OBJECT


public:
	explicit Translator_GUI(QWidget *parent = nullptr);

	~Translator_GUI();

	void exit();
	
private slots:

	bool saveXml();
	
	bool openXml();


private:
	//bool local;

	QString source_language;
	QString translation_language;

protected:
	Ui::MainWindow *ui;    //--> the user interface
};

#endif /* TRANSLATOR_GUI_H_ */