/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
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
	std::cout << HERE << std::endl;

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
	std::cout << HERE << std::endl;

	m_protocol_name = ui_wizard->lineEdit_protocol_name->text();
    
	int current_idx = ui_wizard->stackedWidget->currentIndex() + 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);

}

void Labonatip_macroWizard::next_page2to()
{
	std::cout << HERE << std::endl;

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
	std::cout << HERE << std::endl;

	if (ui_wizard->radioButton_init_stand->isChecked()) {
		emit loadStdProtocol();
		this->next();
		return;
	}
	if (ui_wizard->radioButton_init_oper->isChecked()) {
		emit loadOptProtocol();
		this->next();
		return;
	}
	if (ui_wizard->radioButton_init_custom->isChecked()) {
		emit loadCustomProtocol();
		this->next();
		return;
	}

	this->next();
	return;

}


void Labonatip_macroWizard::on_next5_clicked()
{
	std::cout << HERE << std::endl;

}


void Labonatip_macroWizard::next()
{
	std::cout << HERE << std::endl;

	int current_idx = ui_wizard->stackedWidget->currentIndex() + 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);

}

void Labonatip_macroWizard::back()
{
	std::cout << HERE << std::endl;

	int current_idx = ui_wizard->stackedWidget->currentIndex() - 1;
	ui_wizard->stackedWidget->setCurrentIndex(current_idx);
	setBanner(current_idx);
}



bool Labonatip_macroWizard::save()
{
	std::cout << HERE << std::endl;

	if (ui_wizard->radioButton_end_sleep->isChecked()) {
		emit loadSleepProtocol();
	}
	if (ui_wizard->radioButton_end_oper->isChecked()) {
		emit loadOptProtocol();
	}
	if (ui_wizard->radioButton_end_alloff->isChecked()) {
		emit loadAllOffProtocol();
	}
	if (ui_wizard->radioButton_end_custom->isChecked()) {
		emit loadCustomProtocol();
	}
	if (ui_wizard->radioButton_end_none->isChecked()) {
		//do nothing so far
	}


	emit saveProtocol();

	ui_wizard->stackedWidget->setCurrentIndex(ui_wizard->stackedWidget->count() - 1);
	setBanner(5);
	return true;
}


void Labonatip_macroWizard::exit()
{
	std::cout << HERE << std::endl;

	//emit ok();  // emit the signal ??
	this->close();
}

void Labonatip_macroWizard::closeEvent(QCloseEvent *event) {

	std::cout << HERE << std::endl;

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

void Labonatip_macroWizard::switchLanguage(QString _translation_file)
{
	std::cout << HERE << std::endl;

	qApp->removeTranslator(&m_translator_wizard);

	if (m_translator_wizard.load(_translation_file)) {
		//m_translator_tool.translate("Labonatip_tool", "english");
		qApp->installTranslator(&m_translator_wizard);

		ui_wizard->retranslateUi(this);

		//m_str_warning = QApplication::translate("Labonatip_GUI", qPrintable(m_str_warning));
		

		std::cout << HERE << "  installTranslator" << std::endl;
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

Labonatip_macroWizard::~Labonatip_macroWizard() {

	//TODO: add delete class members
	delete m_solutionParams;
	delete m_pr_params;
	delete ui_wizard;
}