/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Translator_GUI.h"



Translator_GUI::Translator_GUI(QWidget *parent ):
									 QDialog (parent),
                                     ui (new Ui::Translator_GUI)
{
  ui->setupUi(this );

  // what I want to do here is to read files into the 
  // languages folder of biopen_wizard
  // and generate translation also including fix sentences and 
  // making it easier to translate new development of the app

}


void Translator_GUI::exit()
{
    close();
    qApp->quit();
    delete ui;
}

Translator_GUI::~Translator_GUI()
{
  delete ui;
}
