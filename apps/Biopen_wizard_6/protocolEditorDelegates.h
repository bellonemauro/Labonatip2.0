/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef PROTOCOL_EDITOR_DELEGATES_H_
#define PROTOCOL_EDITOR_DELEGATES_H_

// standard libraries
#include <iostream>
#include <string>

//#include "ui_protocolEditor.h"
//#include "protocolWizard.h"

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QTreeWidget>
#include <QObject>
#include <QItemDelegate>
#include <QStyledItemDelegate>

// PPC1api 
#include <fluicell/ppc1api6/ppc1api6.h>
#include <serial/serial.h>

#include "dataStructures.h"


class QModelIndex;
class QWidget;
class QVariant;

// this allows to add a combobox in a column of the QTreeWidget
// inspired from here http://programmingexamples.net/wiki/Qt/Delegates/ComboBoxDelegate
class ComboBoxDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	ComboBoxDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setEditorData(QWidget *editor, 
		const QModelIndex &index) const;

	void setModelData(QWidget *editor, 
		QAbstractItemModel *model, const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void paint(QPainter *painter, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const;

	std::string getElementAt(int _idx);
	int getElementIndex(std::string _name);


private:

	std::vector<std::string> Items;
};

// this allows to add to a QTreeWidget a non-modifiable column
// see this
// https://stackoverflow.com/questions/2801959/making-only-one-column-of-a-qtreewidgetitem-editable
class NoEditDelegate : public QStyledItemDelegate {
public:
	NoEditDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}
	virtual QWidget* createEditor(QWidget *parent, 
		const QStyleOptionViewItem &option, const QModelIndex &index) const {
		return 0;
	}
};

// this allows to add a spinbox in a column of the QTreeWidget
// see this http://doc.qt.io/qt-5/qtwidgets-itemviews-spinboxdelegate-example.html
class SpinBoxDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	SpinBoxDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	void setEditorData(QWidget *editor, const QModelIndex &index) const override;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const override;

	void updateEditorGeometry(QWidget *editor,
		const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


#endif /* PROTOCOL_EDITOR_DELEGATES_H_ */
