/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_PROTOCOL_EDITOR_H_
#define Labonatip_PROTOCOL_EDITOR_H_

// standard libraries
#include <iostream>
#include <string>

#include "ui_protocolEditor.h"
#include "macroWizard.h"

#include <QDialog>

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>


using namespace std;


//custom combo behavior
class macroCombobox_2 :public QComboBox {
	Q_OBJECT

public:
	explicit macroCombobox_2(QWidget* parent = 0) : QComboBox(parent) {}

	void wheelEvent(QWheelEvent *e)
	{
		if (hasFocus())
			QComboBox::wheelEvent(e);
	}
};


class Labonatip_protocol_editor : public  QDialog
{
	Q_OBJECT

/** Create signals to be passed to the main app
* 
*/
signals :
	void ok();  // generated when ok is pressed
	void apply(); // generated when apply is pressed
	void cancel(); // generated when cancel is pressed


public:

	explicit Labonatip_protocol_editor(QWidget *parent = 0);

	~Labonatip_protocol_editor();



	QString getMacroPath() { return m_current_macro_file_name; };

	inline void setMacroPath(QString _path) { m_macro_path = _path; }

	void setMacroPrt(std::vector<fluicell::PPC1api::command> *_macro) { m_macro = _macro; };

private slots:


	/** emit ok signal, save the setting, send the current macro to the main
	*   and close the window
	* \note
	*/
	void okPressed();

	/** Emit cancel signal and close the window
	* \note
	*/
	void cancelPressed();

	/** emit apply signal, save the setting, and send the current macro to the main
	*   
	*/
	void applyPressed();

	/** new macro widard
	*
	*/
	void newMacroWizard();


	/** Load a macro fron file, only one type of macro is currently supported
	*
	*/
	bool loadMacro( );

	/** Save the macro to file
	*
	*/
	bool saveMacro( );

	/** Clear all the macro commands
	*
	*/
	void clearAllCommands();

	/** Add a new macro command
	*
	*/
	void addMacroCommand();

	/** remove a macro command
	*
	*/
	void removeMacroCommand();

	/** The selected element will become a child for the preceding element
	*
	*/
	void becomeChild();

	/** The selected element will become parent 
	*
	*/
	void becomeParent();
	
	void moveUp();

	void moveDown();

	void plusIndent();

	void duplicateItem();

	bool checkValidity(QTreeWidgetItem *_item, int _column);

	void commandChanged(int _idx);


	/** Put all the commands in the macro editor to the macro structure for running
	*
	* \note
	*/
	void addAllCommandsToMacro();


private:


	void createNewCommand(QTreeWidgetItem &_command, macroCombobox_2 &_combo_box);
	
	/** overload to allow creating the combobox only without the item
	*
	*/
	void createNewCommand(macroCombobox_2 &_combo_box) {
		QTreeWidgetItem item;
		createNewCommand(item, _combo_box);
	}
		
	QString createHeader();
	
	bool loadMacro(const QString _file_name);
	
	bool saveMacro(QString _file_name);
	
	void getLastNode(QTreeWidget *_tree, QTreeWidgetItem *_item);

	int getLevel(QTreeWidgetItem _item);

	//void visitTree(QTreeWidgetItem *_item);

	void visitTree(QList<QStringList> &_list, QTreeWidget *_tree, QTreeWidgetItem *_item);

	QList<QStringList> visitTree(QTreeWidget *_tree);

	int interpreteLanguage(QString _language);

	bool decodeMacroCommand(QByteArray &_command, QTreeWidgetItem &_out_item);



	std::vector<fluicell::PPC1api::command> *m_macro;


	QString m_current_macro_file_name;
	QString m_macro_path;

	int p_on_min;
	int p_on_max;
	int p_off_min;
	int p_off_max;
	int v_switch_min;
	int v_switch_max;
	int v_recirc_min;
	int v_recirc_max;

	Labonatip_macroWizard * macroWizard;

protected:
	Ui::Labonatip_protocol_editor *ui_p_editor;    //!<  the user interface
};


#endif /* Labonatip_PROTOCOL_EDITOR_H_ */
