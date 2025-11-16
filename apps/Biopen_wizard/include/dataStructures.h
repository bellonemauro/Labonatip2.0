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
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>

// Global definitions
#define MIN_WARNING_VOLUME 6    // in ml
#define MAX_VOLUME_IN_WELL 30   // in ml
#define MAX_WASTE_VOLUME 35     // value in ml
#define MAX_WASTE_WARNING_VOLUME 27    // in ml
#define HERE std::string(__FUNCTION__ )//+ std::string(" at line ") + std::to_string(__LINE__))

// just re-definition of a protocol command to get a shorter name
typedef fluicell::PPC1dataStructures::command::instructions pCmd;

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
		this->port = "COM1";
		this->baudRate = 115200;
		this->dataBits = serial::eightbits;
		this->parity = serial::parity_none;
		this->stopBits = serial::stopbits_one;
		this->flowControl = serial::flowcontrol_none;
		this->localEchoEnabled = false;
	}

	void setPort(std::string _port) { port = _port; }
	std::string getPort() { return port; }

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
	std::string port;                    //!<  COM1 --- n 
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
		this->vol_well5 = 30.0;
		this->vol_well6 = 30.0;
		this->vol_well7 = 30.0;
		this->vol_well8 = 30.0;
	    this->sol1 = "No name given to sol 1";
		this->sol2 = "No name given to sol 2";
		this->sol3 = "No name given to sol 3";
		this->sol4 = "No name given to sol 4";
		this->sol1_color = QColor::fromRgb(255, 189, 0);
		this->sol2_color = QColor::fromRgb(255, 40, 0);
		this->sol3_color = QColor::fromRgb(0, 158, 255);
		this->sol4_color = QColor::fromRgb(130, 255, 0);
		this->pulse_duration_well1 = 500.0;
		this->pulse_duration_well2 = 500.0;
		this->pulse_duration_well3 = 500.0;
		this->pulse_duration_well4 = 500.0;
		this->continuous_flowing_sol1 = 1;
		this->continuous_flowing_sol2 = 1;
		this->continuous_flowing_sol3 = 1;
		this->continuous_flowing_sol4 = 1;

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
	QColor sol1_color;                //!<  color of the solution 1
	QColor sol2_color;                //!<  color of the solution 2
	QColor sol3_color;                //!<  color of the solution 3
	QColor sol4_color;                //!<  color of the solution 4
	double pulse_duration_well1;
	double pulse_duration_well2;
	double pulse_duration_well3;
	double pulse_duration_well4;
	bool continuous_flowing_sol1;
	bool continuous_flowing_sol2;
	bool continuous_flowing_sol3;
	bool continuous_flowing_sol4;
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


	explicit pr_params() : p_on_max (450), p_on_min(0), p_on_default(190),
				p_off_max(450), p_off_min(0), p_off_default(21),
				v_switch_max(0), v_switch_min(-300), v_switch_default(-115),
				v_recirc_max(0), v_recirc_min(-300), v_recirc_default(-115),
				p_on_preset1(190), p_off_preset1(21), v_switch_preset1(-115), v_recirc_preset1(-115),
				p_on_preset2(190), p_off_preset2(21), v_switch_preset2(-115), v_recirc_preset2(-115),
				p_on_preset3(190), p_off_preset3(21), v_switch_preset3(-115), v_recirc_preset3(-115),
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
	int p_off_max;               //!< max P_off value
	int p_off_min;               //!< min P_off value
	int p_off_default;           //!< default P_off value
	int v_switch_max;            //!< max V_switch value
	int v_switch_min;            //!< min V_switch value
	int v_switch_default;        //!< default V_switch value
	int v_recirc_max;            //!< max V_recirc value
	int v_recirc_min;            //!< min V_recirc value
	int v_recirc_default;        //!< defauls V_recirc value 
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


}; // END GUIparams struct


struct protocolCommands {

	enum command {
		setZoneSize = 0,
		changeZoneSizeBy = 1,
		setFlowSpeed = 2,
		changeFlowSpeedBy = 3,
		setVacuum = 4,
		changeVacuumBy = 5,
		wait = 6,
		allOff = 7,
		solution1 = 8,
		solution2 = 9,
		solution3 = 10,
		solution4 = 11,
		setPon = 12,
		setPoff = 13,
		setVrecirc = 14,
		setVswitch = 15,
		ask = 16,
		pumpsOff = 17,
		waitSync = 18,
		syncOut = 19,
		sendPulses = 20,
		setSyncTimeout = 21, 
		loop = 22,
		comment = 23,	
		
		END //THIS IS TO TAKE TRACK OF ENUMERATION
	};

	static std::string asString(int _idx)
	{
		switch (_idx)
		{
		case setZoneSize: return "setZoneSize";
		case changeZoneSizeBy: return "changeZoneSizeBy";
		case setFlowSpeed: return "setFlowSpeed";
		case changeFlowSpeedBy: return "changeFlowSpeedBy";
		case setVacuum: return "setVacuum";
		case changeVacuumBy: return "changeVacuumBy";
		case wait: return "wait";
		case allOff: return "allOff";
		case solution1: return "solution1";
		case solution2: return "solution2";
		case solution3: return "solution3";
		case solution4: return "solution4";
		case setPon: return "setPon";
		case setPoff: return "setPoff";
		case setVrecirc: return "setVrecirc";
		case setVswitch: return "setVswitch";
		case ask: return "ask";
		case pumpsOff: return "pumpsOff";
		case waitSync: return "waitSync";
		case syncOut: return "syncOut";
		case loop: return "loop";
		case sendPulses: return "sendPulses";
		case setSyncTimeout: return "setSyncTimeout";
		case comment: return "comment";
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


#endif /* DATASTRUCTURES_H_ */