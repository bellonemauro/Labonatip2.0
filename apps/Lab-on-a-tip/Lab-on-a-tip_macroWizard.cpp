/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */




#include "Lab-on-a-tip_macroWizard.h"

Labonatip_macroWizard::Labonatip_macroWizard(QWidget *parent)
    : ui_wizard(new Ui::Labonatip_macroWizard),
	QDialog(parent)
{
	ui_wizard->setupUi(this);

	ui_wizard->stackedWidget->setCurrentIndex(0);

	connect(ui_wizard->pushButton_next1,
		SIGNAL(clicked()), this, SLOT(next()));

	connect(ui_wizard->pushButton_next2,
		SIGNAL(clicked()), this, SLOT(next()));

	connect(ui_wizard->pushButton_back2,
		SIGNAL(clicked()), this, SLOT(back()));
	
	connect(ui_wizard->pushButton_back1,
		SIGNAL(clicked()), this, SLOT(back()));
}

void Labonatip_macroWizard::next()
{
	ui_wizard->stackedWidget->setCurrentIndex(ui_wizard->stackedWidget->currentIndex()+1);
}

void Labonatip_macroWizard::back()
{
	ui_wizard->stackedWidget->setCurrentIndex(ui_wizard->stackedWidget->currentIndex()-1);
}
