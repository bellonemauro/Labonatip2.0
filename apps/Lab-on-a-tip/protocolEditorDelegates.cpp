/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolEditorDelegates.h"
#include <QSpinBox>


ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
	:QItemDelegate(parent)
{
	Items.push_back("Pressure ON");
	Items.push_back("Pressure OFF");
	Items.push_back("Vacuum Switch");
	Items.push_back("Vacuum Recirculation");
	Items.push_back("Solution 1");
	Items.push_back("Solution 2");
	Items.push_back("Solution 3");
	Items.push_back("Solution 4");
	Items.push_back("Wait");
	Items.push_back("Ask");
	Items.push_back("All Off");
	Items.push_back("Pumps Off");
	Items.push_back("Wait sync");
	Items.push_back("Sync out");
	Items.push_back("Zone size");
	Items.push_back("Flow speed");
	Items.push_back("Vacuum"); //16
	Items.push_back("Loop");

}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
{
	QComboBox* editor = new QComboBox(parent);
	for (unsigned int i = 0; i < Items.size(); ++i)
	{
		editor->addItem(Items[i].c_str());
	}
	return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	int value = index.model()->data(index, Qt::EditRole).toUInt();
	comboBox->setCurrentIndex(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	model->setData(index, comboBox->currentIndex(), Qt::EditRole);

}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem myOption = option;

	// set colors thanks https://stackoverflow.com/questions/43035378/qtreeview-item-hover-selected-background-color-based-on-current-color
	//background
	QColor bgColor;

	//color logic
	bgColor = QColor(Qt::transparent);//default is transparent to retain alternate row colors



	if (option.state & QStyle::State_Selected)//check if it is hovered AND selected
	{
		bgColor = 0xFFFFFF;// qRgba(200, 200, 200, 255);

		if (option.state & QStyle::State_Active)//check if item is selected
		{
			bgColor = 0xDDFFAA;// qRgba(200, 255, 255, 255);

							   //background color won't show on selected items unless you do this
			myOption.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
		}
		//background color won't show on selected items unless you do this
		myOption.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
	}

	if (option.state & QStyle::State_MouseOver)//check if item is hovered
	{
		//background: qlineargradient(x1 : 0, y1 : 0, x2 : 0, y2 : 1, stop : 0 #f6f7fa, stop: 1 #F0FFF0);
		//more color logic
		bgColor = 0xDDFFAA;//qRgba(bgColor.red() - 25, bgColor.green() - 25, bgColor.blue() - 25, 255);

		if (option.state & QStyle::State_Selected)//check if it is hovered AND selected
		{
			bgColor = 0xDDFFAA;// qRgba(148, 200, 234, 255);

							   //background color won't show on selected items unless you do this
			myOption.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
		}
	}

	//set the backgroundBrush to our color. This affects unselected items.
	myOption.backgroundBrush = QBrush(bgColor);
	//draw the item background
	option.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &myOption, painter);


	//text
	QString text = Items[index.data().toInt()].c_str();
	myOption.text = text;
	QRect textRect = option.rect;
	textRect.setLeft(textRect.left() + 25);//offset it a bit to the right
										   //draw in text, this can be grabbed from Qt::DisplayRole
	option.widget->style()->drawItemText(painter,
		textRect, Qt::AlignLeft | Qt::AlignVCenter, option.palette, true, text);// index.data(Qt::DisplayRole).toString());

																				//QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);

}




SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent,
	const QStyleOptionViewItem &/* option */,
	const QModelIndex &/* index */) const
{
	QSpinBox *editor = new QSpinBox(parent);
	editor->setFrame(false);
	editor->setMinimum(-300);
	editor->setMaximum(400);

	return editor;
}

void SpinBoxDelegate::setEditorData(QWidget *editor,
	const QModelIndex &index) const
{
	int value = index.model()->data(index, Qt::EditRole).toInt();

	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
	const QModelIndex &index) const
{
	QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
	spinBox->interpretText();
	int value = spinBox->value();

	model->setData(index, value, Qt::EditRole);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}