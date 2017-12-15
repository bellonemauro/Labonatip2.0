/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | Lab-on-a-tip 2.0                                                          |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#ifndef Labonatip_TOOLSDATASTRUCTURES_H_
#define Labonatip_TOOLSDATASTRUCTURES_H_

// standard libraries
#include <iostream>
#include <string>

#include <QSettings>
#include <QColor>

// PPC1api 
#include <fluicell/ppc1api/ppc1api.h>
#include <serial/serial.h>


// structure to handle COM port parameters
struct COMSettings {

public:
	COMSettings() {   // default values
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


	solutionsParams() {   // default values
		this->vol_well1 = 0;
		this->vol_well2 = 0;
		this->vol_well3 = 0;
		this->vol_well4 = 0;
		this->vol_well5 = 0;
		this->vol_well6 = 0;
		this->vol_well7 = 0;
		this->vol_well8 = 0;
		this->rem_vol_well1 = 0;
		this->rem_vol_well2 = 0;
		this->rem_vol_well3 = 0;
		this->rem_vol_well4 = 0;
		this->rem_vol_well5 = 0;
		this->rem_vol_well6 = 0;
		this->rem_vol_well7 = 0;
		this->rem_vol_well8 = 0;

		this->sol1 = "No name given to sol 1";
		this->sol2 = "No name given to sol 2";
		this->sol3 = "No name given to sol 3";
		this->sol4 = "No name given to sol 4";
		this->sol1_color = QColor::fromRgb(255, 189, 0);
		this->sol2_color = QColor::fromRgb(255, 40, 0);
		this->sol3_color = QColor::fromRgb(0, 158, 255);
		this->sol4_color = QColor::fromRgb(130, 255, 0);
	}

	double vol_well1;                 //!<  initial volume of solution in the well 1 in uL
	double vol_well2;                 //!<  initial volume of solution in the well 2 in uL
	double vol_well3;                 //!<  initial volume of solution in the well 3 in uL
	double vol_well4;                 //!<  initial volume of solution in the well 4 in uL
	double vol_well5;                 //!<  initial volume of solution in the well 5 in uL
	double vol_well6;                 //!<  initial volume of solution in the well 6 in uL
	double vol_well7;                 //!<  initial volume of solution in the well 7 in uL
	double vol_well8;                 //!<  initial volume of solution in the well 8 in uL
	double rem_vol_well1;             //!<  remaining volume of solution in the well 1 in mL
	double rem_vol_well2;             //!<  remaining volume of solution in the well 2 in mL
	double rem_vol_well3;             //!<  remaining volume of solution in the well 3 in mL
	double rem_vol_well4;             //!<  remaining volume of solution in the well 4 in mL
	double rem_vol_well5;             //!<  remaining volume of solution in the well 5 in mL
	double rem_vol_well6;             //!<  remaining volume of solution in the well 6 in mL
	double rem_vol_well7;             //!<  remaining volume of solution in the well 7 in mL
	double rem_vol_well8;             //!<  remaining volume of solution in the well 8 in mL
	QString sol1;                  //!<  name of the solution 1
	QString sol2;                  //!<  name of the solution 2
	QString sol3;                  //!<  name of the solution 3
	QString sol4;                  //!<  name of the solution 4
	QColor sol1_color;             //!<  color of the solution 1
	QColor sol2_color;             //!<  color of the solution 2
	QColor sol3_color;             //!<  color of the solution 3
	QColor sol4_color;             //!<  color of the solution 4
}; // END solutionsNames struct

	// structure to handle solutions name parameters
struct pr_params {


	pr_params() : p_on_max (450), p_on_min(0), p_on_default(190),
				p_off_max(450), p_off_min(0), p_off_default(21),
				v_switch_max(0), v_switch_min(-300), v_switch_default(-115),
				v_recirc_max(0), v_recirc_min(-300), v_recirc_default(-115),
				base_ds_increment(10), base_fs_increment(5), base_v_increment(5)
	{   // default values
	}

	void setDefValues(int _p_on_def, int _p_off_def, int _v_sw_def, int _v_rec_def) {
		p_on_default = _p_on_def;
		p_off_default = _p_off_def;
		v_switch_default = _v_sw_def;
		v_recirc_default = _v_rec_def;
	}

	int p_on_max;                    //!< name of the solution 1
	int p_on_min;
	int p_on_default;
	int p_off_max;
	int p_off_min;
	int p_off_default;
	int v_switch_max;
	int v_switch_min;
	int v_switch_default;
	int v_recirc_max;
	int v_recirc_min;
	int v_recirc_default;
	int base_ds_increment;
	int base_fs_increment;
	int base_v_increment;

}; // END pr_params struct


struct GUIparams {
	Qt::ToolButtonStyle showTextToolBar;     //!<  ToolButtonIconOnly --- n 
	bool enableToolTips;

	GUIparams() {   // default values
		this->showTextToolBar = Qt::ToolButtonIconOnly;
		this->enableToolTips = false;
	}

}; // END COMSettings struct


#endif /* Labonatip_TOOLSDATASTRUCTURES_H_ */
