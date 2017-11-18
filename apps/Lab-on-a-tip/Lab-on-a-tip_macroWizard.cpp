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

	m_macro_path = "";

	ui_wizard->stackedWidget->setCurrentIndex(0);

	// page 1
	connect(ui_wizard->pushButton_next1,
		SIGNAL(clicked()), this, SLOT(next()));

	connect(ui_wizard->pushButton_cancel,
		SIGNAL(clicked()), this, SLOT(exit()));

	// page 2
	connect(ui_wizard->pushButton_next2,
		SIGNAL(clicked()), this, SLOT(next()));

	connect(ui_wizard->pushButton_back2,
		SIGNAL(clicked()), this, SLOT(back()));

	connect(ui_wizard->pushButton_cancel2,
		SIGNAL(clicked()), this, SLOT(exit()));

	// page 3
	connect(ui_wizard->pushButton_next3,
		SIGNAL(clicked()), this, SLOT(next()));
	
	connect(ui_wizard->pushButton_back3,
		SIGNAL(clicked()), this, SLOT(back()));

	connect(ui_wizard->pushButton_cancel3,
		SIGNAL(clicked()), this, SLOT(exit()));

	// page 4

	connect(ui_wizard->pushButton_back4,
		SIGNAL(clicked()), this, SLOT(back()));

	connect(ui_wizard->pushButton_save,
		SIGNAL(clicked()), this, SLOT(save()));

	connect(ui_wizard->pushButton_cancel4,
		SIGNAL(clicked()), this, SLOT(exit()));

	// last page
	connect(ui_wizard->pushButton_exit,
		SIGNAL(clicked()), this, SLOT(exit()));
	
}

void Labonatip_macroWizard::next()
{
	int current_idx = ui_wizard->stackedWidget->currentIndex() + 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);

}

void Labonatip_macroWizard::back()
{
	int current_idx = ui_wizard->stackedWidget->currentIndex() - 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);
}


bool Labonatip_macroWizard::save()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save something"), m_macro_path,  // dialog to open files
		"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);

	//if (!saveMacro(fileName)) 
	{
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning !", "File not saved ! <br>" + fileName);

		ui_wizard->stackedWidget->setCurrentIndex(ui_wizard->stackedWidget->count() -1 );
		setBanner(5);
		return false;
	}
	QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode

	ui_wizard->stackedWidget->setCurrentIndex(ui_wizard->stackedWidget->count() - 1);
	setBanner(5);
	return true;
}


void Labonatip_macroWizard::exit()
{
	//emit ok();  // emit the signal ??
	this->close();
}

void Labonatip_macroWizard::closeEvent(QCloseEvent *event) {

	QMessageBox::StandardButton resBtn =
		QMessageBox::question(this, "Lab-on-a-tip",
			tr("Are you sure?\n"),
			QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
			QMessageBox::Yes);
	if (resBtn != QMessageBox::Yes) {
		event->ignore();
	}
	else {

		event->accept();
		ui_wizard->stackedWidget->setCurrentIndex(0);
	}
}

void Labonatip_macroWizard::setBanner(int _current_index)
{

	switch (_current_index)
	{
	case 0:
	{
		ui_wizard->pushButton_home->setChecked(false);
		ui_wizard->pushButton_second->setChecked(false);
		ui_wizard->pushButton_4->setChecked(false);
		ui_wizard->pushButton_saved->setChecked(false);
		ui_wizard->pushButton_finished->setChecked(false);
		break;
	}
	case 1:
	{
		ui_wizard->pushButton_home->setChecked(true);
		ui_wizard->pushButton_second->setChecked(false);
		ui_wizard->pushButton_4->setChecked(false);
		ui_wizard->pushButton_saved->setChecked(false);
		ui_wizard->pushButton_finished->setChecked(false);
		break;
	}
	case 2:
	{
		ui_wizard->pushButton_home->setChecked(true);
		ui_wizard->pushButton_second->setChecked(true);
		ui_wizard->pushButton_4->setChecked(false);
		ui_wizard->pushButton_saved->setChecked(false);
		ui_wizard->pushButton_finished->setChecked(false);
		break;
	}
	case 3:
	{
		ui_wizard->pushButton_home->setChecked(true);
		ui_wizard->pushButton_second->setChecked(true);
		ui_wizard->pushButton_4->setChecked(true);
		ui_wizard->pushButton_saved->setChecked(false);
		ui_wizard->pushButton_finished->setChecked(false);
		break;
	}
	case 4:
	{		
		ui_wizard->pushButton_home->setChecked(true);
		ui_wizard->pushButton_second->setChecked(true);
		ui_wizard->pushButton_4->setChecked(true);
		ui_wizard->pushButton_saved->setChecked(true);
		ui_wizard->pushButton_finished->setChecked(false);
		break;

	}
	case 5: 
	{		
		ui_wizard->pushButton_home->setChecked(true);
		ui_wizard->pushButton_second->setChecked(true);
		ui_wizard->pushButton_4->setChecked(true);
		ui_wizard->pushButton_saved->setChecked(true);
		ui_wizard->pushButton_finished->setChecked(true);
		break;
	}
	default:
	{
		ui_wizard->pushButton_home->setChecked(false);
		ui_wizard->pushButton_second->setChecked(false);
		ui_wizard->pushButton_4->setChecked(true);
		ui_wizard->pushButton_saved->setChecked(false);
		ui_wizard->pushButton_finished->setChecked(false); 
		break;
	}
	}

}
