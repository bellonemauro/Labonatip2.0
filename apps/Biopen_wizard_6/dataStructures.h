/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | BioPen wizard 2.1                                                         |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef DATASTRUCTURES_H_
#define DATASTRUCTURES_H_

// standard libraries
#include <iostream>
#include <string>

#include <QSettings>
#include <QColor>
#include <QtWidgets/QLabel>
// QT for graphics
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsView>

// PPC1api 
#include <fluicell/ppc1api6/ppc1api6.h>
#include <serial/serial.h>

// Global definitions
#define MIN_WARNING_VOLUME 6    // in ml
#define MAX_VOLUME_IN_WELL 30   // in ml
#define MAX_WASTE_VOLUME 35     // value in ml
#define MAX_WASTE_WARNING_VOLUME 27    // in ml
#define HERE std::string(__FUNCTION__ )//+ std::string(" at line ") + std::to_string(__LINE__))

// just re-definition of a protocol command to get a shorter name
//typedef fluicell::PPC1api6dataStructures::command::instructions pCmd;
typedef fluicell::PPC1api6dataStructures::command::instructions ppc1Cmd;

//TODO IMPORTANT: check duplicated structures between dataStructure.h and ppc1api_data_structure.h 

/**  \brief Structure to handle COM parameters.
*
*  @param set_point is the closed loop PID controller input value (in mbar)
*  @param sensor_reading shows the actual current pressure value (in mbar)
*  @param PID_out_DC PID output duty cycle is the output value of closed loop PID controller.
*                    Values 0 to 50000 mark the on time period in microseconds.
*                    PWM frequency is 20Hz
*  @param state shows error flags
*               0 means no errors
*               1 is set point timeout error, which occurs when the channel has not reached in
*                 the range of + -5mbar of the set point within 30 seconds.
*                 In this case the output and set point will be set to 0.
*                 The error flag clears when a new set point is set.
*
**/
struct COMSettings {

public:
	explicit COMSettings() {   // default values
		this->name = "COM1";
		this->baudRate = 115200;
		this->dataBits = serial::eightbits;
		this->parity = serial::parity_none;
		this->stopBits = serial::stopbits_one;
		this->flowControl = serial::flowcontrol_none;
	}

	void setName(std::string _name) { name = _name; }
	std::string getName() { return name; }

	void setBaudRate(int _baudRate) { baudRate = _baudRate; }
	int getBaudRate() { return baudRate; }

	void setDataBits(serial::bytesize_t _dataBits) { dataBits = _dataBits; }
	serial::bytesize_t getDataBits() { return dataBits; }

	void setParity(serial::parity_t _parity) { parity = _parity; }
	serial::parity_t getParity() { return parity; }

	void setStopBits(serial::stopbits_t _stopBits) { stopBits = _stopBits; }
	serial::stopbits_t getStopBits() { return stopBits; }

	void setFlowControl(serial::flowcontrol_t _flowControl) { flowControl = _flowControl; }
	serial::flowcontrol_t getFlowControl() { return flowControl; }

private:
	std::string name;                    //!<  COM1 --- n 
	int baudRate;                        //!<  4200 - 9600 --- 
	serial::bytesize_t dataBits;         //!<  5 - 6 - 7 - 8
	serial::parity_t parity;             //!<  None, Even, Odd, Mark, Space 
	serial::stopbits_t stopBits;         //!<  1, 1.5, 2
	serial::flowcontrol_t flowControl;   //!<  None, RTS/CTS, XON/XOFF 
	bool localEchoEnabled;

}; // END COMSettings struct

// structure to handle solutions name parameters
struct solutionsParams {

	explicit solutionsParams() {   // default values
		this->vol_well1 = 0.0;
		this->vol_well2 = 0.0;
		this->vol_well3 = 0.0;
		this->vol_well4 = 0.0;
		this->vol_well5 = 0.0;
		this->vol_well6 = 0.0;
		this->vol_well7 = 30.0;
		this->vol_well8 = 30.0;
	    this->sol1 = "No name given to sol 1";
		this->sol2 = "No name given to sol 2";
		this->sol3 = "No name given to sol 3";
		this->sol4 = "No name given to sol 4";
		this->sol5 = "No name given to sol 5";
		this->sol6 = "No name given to sol 6";
		this->sol1_color = QColor::fromRgb(255, 189, 0);
		this->sol2_color = QColor::fromRgb(255, 40, 0);
		this->sol3_color = QColor::fromRgb(0, 158, 255);
		this->sol4_color = QColor::fromRgb(130, 255, 0);
		this->sol4_color = QColor::fromRgb(255, 40, 0);
		this->sol4_color = QColor::fromRgb(255, 189, 0);
		this->pulse_duration_well1 = 500.0;
		this->pulse_duration_well2 = 500.0;
		this->pulse_duration_well3 = 500.0;
		this->pulse_duration_well4 = 500.0;
		this->pulse_duration_well5 = 500.0;
		this->pulse_duration_well6 = 500.0;
		this->continuous_flowing_sol1 = 1;
		this->continuous_flowing_sol2 = 1;
		this->continuous_flowing_sol3 = 1;
		this->continuous_flowing_sol4 = 1;
		this->continuous_flowing_sol5 = 1;
		this->continuous_flowing_sol6 = 1;

	}

	double vol_well1;                 //!<  initial volume of solution in the well 1 in ul
	double vol_well2;                 //!<  initial volume of solution in the well 2 in ul
	double vol_well3;                 //!<  initial volume of solution in the well 3 in ul
	double vol_well4;                 //!<  initial volume of solution in the well 4 in ul
	double vol_well5;                 //!<  initial volume of solution in the well 5 in ul
	double vol_well6;                 //!<  initial volume of solution in the well 6 in ul
	double vol_well7;                 //!<  initial volume of solution in the well 7 in ul
	double vol_well8;                 //!<  initial volume of solution in the well 8 in ul
	QString sol1;                     //!<  name of the solution 1
	QString sol2;                     //!<  name of the solution 2
	QString sol3;                     //!<  name of the solution 3
	QString sol4;                     //!<  name of the solution 4
	QString sol5;                     //!<  name of the solution 4
	QString sol6;                     //!<  name of the solution 4
	QColor sol1_color;                //!<  color of the solution 1
	QColor sol2_color;                //!<  color of the solution 2
	QColor sol3_color;                //!<  color of the solution 3
	QColor sol4_color;                //!<  color of the solution 4
	QColor sol5_color;                //!<  color of the solution 4
	QColor sol6_color;                //!<  color of the solution 4
	double pulse_duration_well1;
	double pulse_duration_well2;
	double pulse_duration_well3;
	double pulse_duration_well4;
	double pulse_duration_well5;
	double pulse_duration_well6;
	bool continuous_flowing_sol1;
	bool continuous_flowing_sol2;
	bool continuous_flowing_sol3;
	bool continuous_flowing_sol4;
	bool continuous_flowing_sol5;
	bool continuous_flowing_sol6;
}; // END solutionsNames struct


struct pipetteStatus{

	explicit pipetteStatus() {   // default values
		this->pon_set_point = 0.0;
		this->poff_set_point = 0.0;
		this->v_recirc_set_point = 0.0;
		this->v_switch_set_point = 0.0;
		this->delta_pressure = 0.0;
		this->outflow_on = 0.0;
		this->outflow_off = 0.0; 
		this->outflow_tot = 0.0; 
		this->inflow_recirculation = 0.0;
		this->inflow_switch = 0.0;
		this->in_out_ratio_on = 0.0;
		this->in_out_ratio_off = 0.0;
		this->in_out_ratio_tot = 0.0;
		this->solution_usage_off = 0.0;
		this->solution_usage_on = 0.0;
		this->flow_well1 = 0.0;
		this->flow_well2 = 0.0;
		this->flow_well3 = 0.0;
		this->flow_well4 = 0.0;
		this->flow_well5 = 0.0;
		this->flow_well6 = 0.0;
		this->flow_well7 = 0.0;
		this->flow_well8 = 0.0;
		this->rem_vol_well1 = 0.0;
		this->rem_vol_well2 = 0.0;
		this->rem_vol_well3 = 0.0;
		this->rem_vol_well4 = 0.0;
		this->rem_vol_well5 = 0.0;
		this->rem_vol_well6 = 0.0;
		this->rem_vol_well7 = 0.0;
		this->rem_vol_well8 = 0.0;

	}

	double pon_set_point;          //!< holds the set point during the execution
	double poff_set_point;         //!< holds the set point during the execution
	double v_recirc_set_point;     //!< holds the set point during the execution
	double v_switch_set_point;     //!< holds the set point during the execution
	double delta_pressure;   
	double outflow_on;
	double outflow_off;
	double outflow_tot;
	double inflow_recirculation;
	double inflow_switch;
	double in_out_ratio_on;
	double in_out_ratio_off;
	double in_out_ratio_tot;
	double solution_usage_off;
	double solution_usage_on;
	double flow_well1;                //!<  flow in the well 1 nl/s
	double flow_well2;                //!<  flow in the well 2 nl/s
	double flow_well3;                //!<  flow in the well 3 nl/s
	double flow_well4;                //!<  flow in the well 4 nl/s
	double flow_well5;                //!<  flow in the well 5 nl/s
	double flow_well6;                //!<  flow in the well 6 nl/s
	double flow_well7;                //!<  flow in the well 7 nl/s
	double flow_well8;                //!<  flow in the well 8 nl/s
	double rem_vol_well1;             //!<  remaining volume of solution in the well 1 in ml
	double rem_vol_well2;             //!<  remaining volume of solution in the well 2 in ml
	double rem_vol_well3;             //!<  remaining volume of solution in the well 3 in ml
	double rem_vol_well4;             //!<  remaining volume of solution in the well 4 in ml
	double rem_vol_well5;             //!<  remaining volume of solution in the well 5 in ml
	double rem_vol_well6;             //!<  remaining volume of solution in the well 6 in ml
	double rem_vol_well7;             //!<  remaining volume of solution in the well 7 in ml
	double rem_vol_well8;             //!<  remaining volume of solution in the well 8 in ml
};

	// structure to handle solutions name parameters
struct pr_params {


	explicit pr_params() : p_on_max (450), p_on_min(0), p_on_default(190), p_on_classical(190),
				p_off_max(450), p_off_min(0), p_off_default(21), p_off_classical(21),
				v_switch_max(0), v_switch_min(-300), v_switch_default(-115), v_switch_classical(-115),
				v_recirc_max(0), v_recirc_min(-300), v_recirc_default(-115), v_recirc_classical(-115),
				p_on_preset1(190), p_off_preset1(21), v_switch_preset1(-115), v_recirc_preset1(-115),
				p_on_preset2(190), p_off_preset2(21), v_switch_preset2(-115), v_recirc_preset2(-115),
				p_on_preset3(190), p_off_preset3(21), v_switch_preset3(-115), v_recirc_preset3(-115),
				p_on_sAs(190), p_off_sAs(21), v_switch_sAs(-115), v_recirc_sAs(-115),
				p_on_sAr(190), p_off_sAr(21), v_switch_sAr(-115), v_recirc_sAr(-115),
				p_on_lAs(190), p_off_lAs(21), v_switch_lAs(-115), v_recirc_lAs(-115),
				p_on_lAr(190), p_off_lAr(21), v_switch_lAr(-115), v_recirc_lAr(-115),
				base_ds_increment(10), base_fs_increment(5), base_v_increment(5),
		        verboseOut(true), useDefValSetPoint(true), enableFilter(true), filterSize (20), waitSyncTimeout(60)
	{   // default values
	}

	void setDefValues(int _p_on_def, int _p_off_def, 
		              int _v_sw_def, int _v_rec_def) {
		p_on_default = _p_on_def;
		p_off_default = _p_off_def;
		v_switch_default = _v_sw_def;
		v_recirc_default = _v_rec_def;
	}

	int p_on_max;                //!< max P_on value
	int p_on_min;                //!< min P_on value
	int p_on_default;            //!< default P_on value
	int p_on_classical;    //!< classical default P_on value, this is used to get back to default values
	//TODO: the difference between classical and default is not clear yet, the classical value is never modified and it is used to 
	//      reset the default value of p_on_default that has that name for historical reasons but cannot be called default anymore
	int p_off_max;               //!< max P_off value
	int p_off_min;               //!< min P_off value
	int p_off_default;           //!< default P_off value
	int p_off_classical;           //!< default P_off value
	int v_switch_max;            //!< max V_switch value
	int v_switch_min;            //!< min V_switch value
	int v_switch_default;        //!< default V_switch value
	int v_switch_classical;        //!< default V_switch value
	int v_recirc_max;            //!< max V_recirc value
	int v_recirc_min;            //!< min V_recirc value
	int v_recirc_default;        //!< defauls V_recirc value 
	int v_recirc_classical;        //!< defauls V_recirc value 
	int base_ds_increment;       //!< base increment for droplet size in zone control
	int base_fs_increment;       //!< base increment for flow speed in zone control
	int base_v_increment;        //!< base increment for vacuum size in zone control
	int p_on_preset1;            //!< P_on value for the preset 1
	int p_off_preset1;           //!< P_off value  for the preset 1
	int v_switch_preset1;        //!< V_switch value for the preset 1
	int v_recirc_preset1;        //!< V_recirc value for the preset 1
	int p_on_preset2;            //!< P_on value for the preset 2
	int p_off_preset2;           //!< P_off value  for the preset 2
	int v_switch_preset2;        //!< V_switch value for the preset 2
	int v_recirc_preset2;        //!< V_recirc value for the preset 2
	int p_on_preset3;            //!< P_on value for the preset 3
	int p_off_preset3;           //!< P_off value  for the preset 3
	int v_switch_preset3;        //!< V_switch value for the preset 3
	int v_recirc_preset3;        //!< V_recirc value for the preset 3

	// Standard and slow
	int p_on_sAs;            //!< P_on value for the standard and slow operational mode
	int p_off_sAs;           //!< P_off value  for the standard and slow operational mode
	int v_switch_sAs;        //!< V_switch value for the standard and slow operational mode
	int v_recirc_sAs;        //!< V_recirc value for the standard and slow operational mode

	// Standard and regular
	int p_on_sAr;            //!< P_on value for the standard and regular operational mode
	int p_off_sAr;           //!< P_off value  for the standard and regular operational mode
	int v_switch_sAr;        //!< V_switch value for the standard and regular operational mode
	int v_recirc_sAr;        //!< V_recirc value for the standard and regular operational mode

	// Large and slow
	int p_on_lAs;            //!< P_on value for the large and slow operational mode
	int p_off_lAs;           //!< P_off value  for the large and slow operational mode
	int v_switch_lAs;        //!< V_switch value for the large and slow operational mode
	int v_recirc_lAs;        //!< V_recirc value for the large and slow operational mode

	// Large and regular
	int p_on_lAr;            //!< P_on value for the large and regular operational mode
	int p_off_lAr;           //!< P_off value  for the large and regular operational mode
	int v_switch_lAr;        //!< V_switch value for the large and regular operational mode
	int v_recirc_lAr;        //!< V_recirc value for the large and regular operational mode

	//TODO check this custom preset as it may be useless
	int p_on_customPreset;       //!< P_on value for the preset 3
	int p_off_customPreset;      //!< P_off value  for the preset 3
	int v_switch_customPreset;   //!< V_switch value for the preset 3
	int v_recirc_customPreset;   //!< V_recirc value for the preset 3

	//this are now here, but maybe I will set a new structure
	bool verboseOut;             //!< Verbose out on PPC1 api 
	bool useDefValSetPoint;      //!< Use default value instead of set points for the calculation of percentages
	bool enableFilter;           //!< Enable data filtering
	int	filterSize;              //!< Filter window size
	int waitSyncTimeout;         //!< Timeout in seconds for the waitSync function, default value 60 sec


}; // END pr_params struct

/*struct tip
{
public:
	double length_to_tip;
	double length_to_zone;

public:
	tip() :
		length_to_tip(0.065),
		length_to_zone(0.062)
	{}
};
*/
struct GUIparams {


	enum languages {
		Chinese = 0,
		English = 1,
		Italiano = 2,
		Svenska = 3,		
	};

	explicit GUIparams()
	{   // default values
		this->showTextToolBar = Qt::ToolButtonIconOnly;
		this->automaticUpdates_idx = 0;
		this->enableToolTips = false;
		this->verboseOutput = true;
		this->enableHistory = true;
		this->dumpHistoryToFile = true;
		this->speechActive = true;
		this->outFilePath = "./Ext_data/";
		this->language = English;

	}

	void setLanguage(int _language) {	
		this->language = languages(_language);
	}

	/**  \brief Simple cast of the enumerator into the corresponding command as a string.
	*
	**/
	std::string getLanguageAsString()
	{
		static const char* const text[] =
		{ "Chinese", "English", "Italiano", "Svenska" };
		return  text[int(this->language)]; // cast to integer
	}

	Qt::ToolButtonStyle showTextToolBar;     //!<  ToolButtonIconOnly --- n 
	int automaticUpdates_idx;  // 0 = no updates, 1 = notify all updates, 2 = notify release only
	bool enableToolTips;
	bool verboseOutput;
	bool enableHistory;
	bool dumpHistoryToFile;
	bool speechActive;
	QString	outFilePath;
	languages language;


}; // END COMSettings struct


struct protocolCommands {

	enum command {
		allOff = 0, 
		solution1 = 1,
		solution2 = 2,
		solution3 = 3,
		solution4 = 4,
		solution5 = 5,
		solution6 = 6,
		setPon = 7,
		setPoff = 8,
		setVrecirc = 9,
		setVswitch = 10,
		waitSync = 11,
		syncOut = 12,
		wait = 13,
		ask = 14,
		pumpsOff = 15,
		loop = 16,
		comment = 17,
		button1 = 18,
		button2 = 19,
		button3 = 20,
		button4 = 21,
		button5 = 22,
		button6 = 23,
		ramp = 24,
		operational = 25,
		initialize = 26,
		standby = 27,
		function = 28,
		standardAndSlow = 29,
		standardAndRegular = 30,
		largeAndSlow = 31,
		largeAndRegular = 32,
		END //THIS IS TO TAKE TRACK OF ENUMERATION
	};

	static std::string protocolCommands::asString(int _idx)
	{
		switch (_idx)
		{
		case allOff: return "allOff";
		case solution1: return "solution1";
		case solution2: return "solution2";
		case solution3: return "solution3";
		case solution4: return "solution4";
		case solution5: return "solution5";
		case solution6: return "solution6";
		case setPon: return "setPon";
		case setPoff: return "setPoff";
		case setVrecirc: return "setVrecirc";
		case setVswitch: return "setVswitch";
		case waitSync: return "waitSync";
		case syncOut: return "syncOut";
		case wait: return "wait";
		case ask: return "ask";
		case pumpsOff: return "pumpsOff";
		case loop: return "loop";
		case comment: return "comment";
		case button1: return "button1";
		case button2: return "button2";
		case button3: return "button3";
		case button4: return "button4";
		case button5: return "button5";
		case button6: return "button6";
		case ramp: return "ramp";
		case operational: return "operational";
		case initialize: return "initialize";
		case standby: return "standby";
		case function: return "function";
		case standardAndSlow: return "standardAndSlow";
		case standardAndRegular: return "standardAndRegular";
		case largeAndSlow: return "largeAndSlow";
		case largeAndRegular: return "largeAndRegular";
		case END: return "END";
		}
		return "Invalid";
	}

};



   // structure to handle editor parameters for now only the columns in the editor tree widget are used
struct editorParams {

	enum columnIndex {
		c_idx = 0,
		c_command = 1,
		c_range = 2,
		c_value = 3,
		c_msg = 4,
		c_level = 5,
	};

}; // END solutionsNames struct


struct QFled {

	enum ColorType {
		nocolor = 0,
		green = 1,
		orange = 2,
		red = 3,
		grey = 4
	}; 

	QLabel *led_label;
	ColorType color_type;


	QFled(QLabel *_led_label,  ColorType _color_type) {
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
			case grey:{
				this->color_type = grey;
				this->led_label->setPixmap(*led_grey);
				break;
			}
			}
		}
	}

private: 
	QPixmap * led_green;
	QPixmap * led_orange;
	QPixmap * led_red;
	QPixmap * led_grey;
	QPainter * painter_led_green;
	QPainter * painter_led_orange;
	QPainter * painter_led_red;
	QPainter * painter_led_grey;
};

#endif /* DATASTRUCTURES_H_ */