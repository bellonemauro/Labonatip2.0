/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
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
	for (int i = 0; i < protocolCommands::END; i++)
		Items.push_back(protocolCommands::asString(i)); // populate the combo with all the commands
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


std::string ComboBoxDelegate::getElementAt(int _idx)
{
	std::string elementAsText = this->Items.at(_idx);
	return elementAsText;
}


int ComboBoxDelegate::getElementIndex(std::string _name)
{
	for (int i = 0; i < this->Items.size(); i++)
	{
		if (this->Items.at(i).c_str() == _name)
			return i;
	}
	return 0;
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
