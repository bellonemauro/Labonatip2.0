/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */



#include "protocolWizard.h"

Labonatip_macroWizard::Labonatip_macroWizard(QWidget *parent)
    : ui_wizard(new Ui::Labonatip_macroWizard),
	QDialog(parent),
	m_protocol_name("")
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::Labonatip_macroWizard    " << endl;

	ui_wizard->setupUi(this);

	m_solutionParams = new solutionsParams();
	m_pr_params = new pr_params();

	m_macro_path = "";

	ui_wizard->stackedWidget->setCurrentIndex(0);

	// page 1
	connect(ui_wizard->pushButton_next1,
		SIGNAL(clicked()), this, SLOT(next_page1to2()));

	connect(ui_wizard->pushButton_cancel,
		SIGNAL(clicked()), this, SLOT(exit()));

	// page 2

	connect(ui_wizard->pushButton_next2,
		SIGNAL(clicked()), this, SLOT(next_page2to()));

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
	connect(ui_wizard->pushButton_next4,
		SIGNAL(clicked()), this, SLOT(on_next4_clicked()));

	connect(ui_wizard->pushButton_back4,
		SIGNAL(clicked()), this, SLOT(back()));

	connect(ui_wizard->pushButton_cancel4,
		SIGNAL(clicked()), this, SLOT(exit()));

	//page 5
	connect(ui_wizard->pushButton_next5,
		SIGNAL(clicked()), this, SLOT(next()));

	connect(ui_wizard->pushButton_back5,
		SIGNAL(clicked()), this, SLOT(back()));

	connect(ui_wizard->pushButton_cancel5,
		SIGNAL(clicked()), this, SLOT(exit()));


	//page 6
	connect(ui_wizard->pushButton_next6,
		SIGNAL(clicked()), this, SLOT(next()));

	connect(ui_wizard->pushButton_back6,
		SIGNAL(clicked()), this, SLOT(back()));

	connect(ui_wizard->pushButton_cancel6,
		SIGNAL(clicked()), this, SLOT(exit()));

	//page 7
	connect(ui_wizard->pushButton_back7,
		SIGNAL(clicked()), this, SLOT(back()));

	connect(ui_wizard->pushButton_save,
		SIGNAL(clicked()), this, SLOT(save()));

	connect(ui_wizard->pushButton_cancel7,
		SIGNAL(clicked()), this, SLOT(exit()));

	// last page
	connect(ui_wizard->pushButton_exit,
		SIGNAL(clicked()), this, SLOT(exit()));
	
}

void Labonatip_macroWizard::next_page1to2()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::next_page1to2    " << endl;

	m_protocol_name = ui_wizard->lineEdit_protocol_name->text();
    
	int current_idx = ui_wizard->stackedWidget->currentIndex() + 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);

}

void Labonatip_macroWizard::next_page2to()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::next_page2to    " << endl;

	if (ui_wizard->radioButton_loadSetting_1->isChecked())
	{

		int current_idx = ui_wizard->stackedWidget->currentIndex() + 1;
		ui_wizard->stackedWidget->setCurrentIndex(current_idx);
		setBanner(current_idx);

		updateSolPrFields();
		return;
	}

	if (ui_wizard->radioButton_loadSetting_2->isChecked())
	{
		
		// load the file
		emit loadSettings();

		int current_idx = ui_wizard->stackedWidget->currentIndex() + 1;
		ui_wizard->stackedWidget->setCurrentIndex(current_idx);
		setBanner(current_idx);


		return;
	}

}

void Labonatip_macroWizard::on_next4_clicked()
{

	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::on_next4_clicked    " << endl;


	if (ui_wizard->radioButton_init_stand->isChecked()) {
		loadStdProtocol();
		this->next();
	}
	if (ui_wizard->radioButton_init_oper->isChecked()) {
		loadOptProtocol();
		this->next();
	}
	if (ui_wizard->radioButton_init_custom->isChecked()) {
		loadCustomProtocol();
		this->next();
	}




}



void Labonatip_macroWizard::next()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::next    " << endl;

	int current_idx = ui_wizard->stackedWidget->currentIndex() + 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);

}

void Labonatip_macroWizard::back()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::back    " << endl;

	int current_idx = ui_wizard->stackedWidget->currentIndex() - 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);
}



bool Labonatip_macroWizard::save()
{
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::save    " << endl;

	QApplication::setOverrideCursor(Qt::WaitCursor);    //transform the cursor for waiting mode

	QString fileName = QFileDialog::getSaveFileName(this,
		tr("Save something"), m_macro_path,  // dialog to open files
		"Lab-on-a-tip macro File (*.macro);; Data (*.dat);; All Files(*.*)", 0);

	//if (!saveMacro(fileName)) 
	{
		QApplication::restoreOverrideCursor();    //close transform the cursor for waiting mode
		QMessageBox::warning(this, "Warning ", "File not saved ! <br>" + fileName);

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
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::exit    " << endl;

	//emit ok();  // emit the signal ??
	this->close();
}

void Labonatip_macroWizard::closeEvent(QCloseEvent *event) {
	
	cout << QDate::currentDate().toString().toStdString() << "  "
		<< QTime::currentTime().toString().toStdString() << "  "
		<< "Labonatip_macroWizard::closeEvent    " << endl;

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
		ui_wizard->pushButton_saved->setChecked(false);
		ui_wizard->pushButton_finished->setChecked(false);
		break;
	}
	case 5:
	{		
		ui_wizard->pushButton_home->setChecked(true);
		ui_wizard->pushButton_second->setChecked(true);
		ui_wizard->pushButton_4->setChecked(true);
		ui_wizard->pushButton_saved->setChecked(true);
		ui_wizard->pushButton_finished->setChecked(false);
		break;

	}
	case 6: 
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

void Labonatip_macroWizard::updateSolPrFields()
{
	ui_wizard->lineEdit_solName1->setText(m_solutionParams->sol1);
	ui_wizard->lineEdit_solName2->setText(m_solutionParams->sol2);
	ui_wizard->lineEdit_solName3->setText(m_solutionParams->sol3);
	ui_wizard->lineEdit_solName4->setText(m_solutionParams->sol4);

	ui_wizard->spinBox_pon_def->setValue(m_pr_params->p_on_default);
	ui_wizard->spinBox_poff_def->setValue(m_pr_params->p_off_default);
	ui_wizard->spinBox_v_r_def->setValue(m_pr_params->v_recirc_default);
	ui_wizard->spinBox_v_s_def->setValue(m_pr_params->v_switch_default);
}
