/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef PROTOCOL_EDITOR_DELEGATES_H_
#define PROTOCOL_EDITOR_DELEGATES_H_

// standard libraries
#include <iostream>
#include <string>

#include "ui_protocolEditor.h"
#include "protocolWizard.h"

#include <QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QTreeWidget>
#include <QObject>
#include <QItemDelegate>

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>

#include "dataStructures.h"

using namespace std;


class QModelIndex;
class QWidget;
class QVariant;

class ComboBoxDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	ComboBoxDelegate(QObject *parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:


	std::vector<std::string> Items;
};

class NoEditDelegate : public QStyledItemDelegate {
public:
	NoEditDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}
	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
		return 0;
	}
};

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
