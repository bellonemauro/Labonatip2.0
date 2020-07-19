/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Biopen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "protocolEditorDelegates.h"
#include <QSpinBox>
#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
	:QItemDelegate(parent)
{
	Items.push_back("Set zone size");
	Items.push_back("Change zone size by");
	Items.push_back("Set flow speed");
	Items.push_back("Change flow speed by");
	Items.push_back("Set vacuum");
	Items.push_back("Change vacuum by");
	Items.push_back("Wait");
	Items.push_back("All Off");
	Items.push_back("Solution 1");
	Items.push_back("Solution 2");
	Items.push_back("Solution 3");
	Items.push_back("Solution 4");
	Items.push_back("Solution 5");
	Items.push_back("Solution 6");
	Items.push_back("Pressure ON");
	Items.push_back("Pressure OFF");
	Items.push_back("Vacuum Recirculation");
	Items.push_back("Vacuum Switch");
	Items.push_back("Ask");
	Items.push_back("Pumps Off");
	Items.push_back("Wait sync");
	Items.push_back("Sync out");
	Items.push_back("Loop");
}


QWidget *ComboBoxDelegate::createEditor(QWidget *parent, 
	const QStyleOptionViewItem &/* option */, const QModelIndex &/* index */) const
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


void ComboBoxDelegate::setModelData(QWidget *editor,
	QAbstractItemModel *model, const QModelIndex &index) const
{
	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	model->setData(index, comboBox->currentIndex(), Qt::EditRole);
}


void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, 
	const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}


void ComboBoxDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem myOption = option;

	// set colors thanks
    // https://stackoverflow.com/questions/43035378/qtreeview-item-hover-selected-background-color-based-on-current-color
	// background
	QColor bgColor;
	//default is transparent to retain alternate row colors
	bgColor = QColor(Qt::transparent);
	
	if (option.state & QStyle::State_MouseOver)//check if item is hovered
	{
		bgColor = 0xC0C0C0;//qRgba(bgColor.red() - 25, bgColor.green() - 25, bgColor.blue() - 25, 255);
		myOption.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
	}
	if (option.state & QStyle::State_Selected)//check if it is hovered AND selected
	{
		//bgColor = 0xDDDDDD;
		if (option.state & QStyle::State_Active)//check if item is selected
		{
			int grey = 239;  //TODO: what the fuck is this shit !
			bgColor = qRgba(grey, grey-34, grey-62, 255);

			//background color won't show on selected items unless you do this
			//myOption.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
		}
		//background color won't show on selected items unless you do this
		//myOption.palette.setBrush(QPalette::Highlight, QBrush(bgColor));
	}

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
	editor->setMinimum(MIN_CHAN_A);
	editor->setMaximum(MAX_CHAN_C);

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
