/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */



#ifndef MYQTLED_H_
#define MYQTLED_H_

// standard libraries
#include <iostream>
#include <string>


// Qt
#include <QMainWindow>
#include <QTranslator>
#include <QDateTime>
#include <QTimer>

//#include <QWhatsthis>
#include <qwhatsthis.h>
#include <QException>
#include <QUndoView>
#include <QProgressDialog>
#include <QDir>
#include <QFileDialog>
#include <QMenu>
#include <QTextStream>
#include <QInputDialog>

// QT for graphics
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>



struct QFled {

	enum ColorType {
		nocolor = 0,
		green = 1,
		orange = 2,
		red = 3,
		grey = 4
	};

	QLabel* led_label;
	ColorType color_type;


	QFled(QLabel* _led_label, ColorType _color_type) {
		led_label = _led_label;
		color_type = nocolor;// _color_type;

		led_green = new QPixmap(QSize(20, 20));
		led_orange = new QPixmap(QSize(20, 20));
		led_red = new QPixmap(QSize(20, 20));
		led_grey = new QPixmap(QSize(20, 20));

		led_green->fill(Qt::transparent);
		painter_led_green = new QPainter(led_green);
		QRadialGradient radialGradient_green(8, 8, 12);
		radialGradient_green.setColorAt(0.0, 0xF0F0F0);
		radialGradient_green.setColorAt(0.5, 0x30D030);
		radialGradient_green.setColorAt(1.0, Qt::transparent);
		painter_led_green->setBackground(Qt::blue);
		painter_led_green->setBrush(radialGradient_green);
		painter_led_green->setPen(Qt::gray);
		painter_led_green->drawEllipse(2, 2, 16, 16);

		led_orange->fill(Qt::transparent);
		painter_led_orange = new QPainter(led_orange);
		QRadialGradient radialGradient_orange(8, 8, 12);
		radialGradient_orange.setColorAt(0.0, 0xF0F0F0);
		radialGradient_orange.setColorAt(0.5, 0xFF7213);
		radialGradient_orange.setColorAt(1.0, Qt::transparent);
		painter_led_orange->setBackground(Qt::blue);
		painter_led_orange->setBrush(radialGradient_orange);
		painter_led_orange->setPen(Qt::gray);
		painter_led_orange->drawEllipse(2, 2, 16, 16);

		led_red->fill(Qt::transparent);
		painter_led_red = new QPainter(led_red);
		QRadialGradient radialGradient_red(8, 8, 12);
		radialGradient_red.setColorAt(0.0, 0xF0F0F0);
		radialGradient_red.setColorAt(0.5, 0xFF5050);
		radialGradient_red.setColorAt(1.0, Qt::transparent);
		painter_led_red->setBackground(Qt::blue);
		//painter_led_red->setBrush(Qt::red);
		painter_led_red->setBrush(radialGradient_red);
		painter_led_red->setPen(Qt::gray);
		painter_led_red->drawEllipse(2, 2, 16, 16);

		led_grey->fill(Qt::transparent);
		painter_led_grey = new QPainter(led_grey);
		QRadialGradient radialGradient_grey(8, 8, 12);
		radialGradient_grey.setColorAt(0.0, 0xF0F0F0);
		radialGradient_grey.setColorAt(0.5, 0x909090);
		radialGradient_grey.setColorAt(1.0, Qt::transparent);
		painter_led_grey->setBackground(Qt::blue);
		//painter_led_grey->setBrush(Qt::red);
		painter_led_grey->setBrush(radialGradient_grey);
		painter_led_grey->setPen(Qt::gray);
		painter_led_grey->drawEllipse(2, 2, 16, 16);

		this->setColor(_color_type);
	}

	void setColor(ColorType _color_type)
	{
		if (_color_type != this->color_type)
		{
			switch (_color_type)
			{
			case nocolor: break;
			case green:
			{
				this->color_type = green;
				this->led_label->setPixmap(*led_green);
				break;
			}
			case orange: {
				this->color_type = orange;
				this->led_label->setPixmap(*led_orange);
				break;
			}
			case red: {
				this->color_type = red;
				this->led_label->setPixmap(*led_red);
				break;
			}
			case grey: {
				this->color_type = grey;
				this->led_label->setPixmap(*led_grey);
				break;
			}
			}
		}
	}

private:
	QPixmap* led_green;
	QPixmap* led_orange;
	QPixmap* led_red;
	QPixmap* led_grey;
	QPainter* painter_led_green;
	QPainter* painter_led_orange;
	QPainter* painter_led_red;
	QPainter* painter_led_grey;
};


#endif /* MYQTLED_H_ */
