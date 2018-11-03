/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include "Translator_GUI.h"
#include "QFileDialog"
#include "QMessageBox"


Translator_GUI::Translator_GUI(QWidget *parent ):
									 QDialog (parent),
                                     ui (new Ui::Translator_GUI)
{
  ui->setupUi(this );

  // what I want to do here is to read files into the 
  // languages folder of biopen_wizard
  // and generate translation also including fix sentences and 
  // making it easier to translate new development of the app

  connect(ui->pushButton_open, SIGNAL(clicked()), this, SLOT(openXml()));

}

bool Translator_GUI::saveXml()
{
	QString fileName =
		QFileDialog::getSaveFileName(this, tr("Save Protocol File"),
			QDir::currentPath(),
			tr("XBEL Files (*.prt *.xml *.ts *.*)"));
	if (fileName.isEmpty())
		return false;
	QFile file(fileName);
	if (!file.open(QFile::WriteOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
			tr("Cannot write file %1:\n%2.")
			.arg(QDir::toNativeSeparators(fileName),
				file.errorString()));
		return false;
	}
	XmlTranslationWriter writer(ui->treeWidget_2);
	if (writer.writeFile(&file))
	{
		return true;
	}
	return false;
}

bool Translator_GUI::openXml()
{
	QString fileName =
		QFileDialog::getOpenFileName(this, tr("Open  File"),
			QDir::currentPath(),
			tr("prt Files (*.prt *.xml *ts *.*)"));
	if (fileName.isEmpty())
		return false;
	ui->treeWidget_2->clear();
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
			tr("Cannot read file %1:\n%2.")
			.arg(QDir::toNativeSeparators(fileName),
				file.errorString()));
		return false;
	}
	XmlTranslationReader reader(ui->treeWidget_2);
	if (!reader.read(&file)) {
		QMessageBox::warning(this, tr("QXmlStream Bookmarks"),
			tr("Parse error in file %1:\n\n%2")
			.arg(QDir::toNativeSeparators(fileName),
				reader.errorString()));
	}
	else {
		return true;
	}
	return false;
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
