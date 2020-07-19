/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | PPC1 API                                                                  |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#include "fluicell/ppc1api/ppc1api.h"
#include <iomanip>

#ifdef VLD_MEMORY_CHECK
 #include <vld.h>
#endif

#define HERE std::string(__FUNCTION__ + std::string(" at line ") + std::to_string(__LINE__))

fluicell::PPC1api::PPC1api() :
	m_PPC1_data(new fluicell::PPC1dataStructures::PPC1_data),
	m_PPC1_status(new fluicell::PPC1dataStructures::PPC1_status),
	m_tip(new fluicell::PPC1dataStructures::tip),
	m_verbose(false),
	m_PPC1_serial(new serial::Serial()), // initialize serial port objects
	m_COMport("COM1"),
	m_baud_rate(115200),
	m_dataStreamPeriod(200),
	m_COM_timeout(250),
	m_wait_sync_timeout(60),
	m_excep_handler(false)
{
	// set default values for pressures and vacuums
	setDefaultPV();
	
	// set default filter values
	m_filter_enabled = true;
	m_filter_size = 20;

	// initialize thread variables
	m_threadTerminationHandler = false; // it will be true when the thread starts
	m_isRunning = false;
}

void fluicell::PPC1api::threadSerial() 
{
	try {
		std::mutex my_mutex;
		m_isRunning = true;
		while (!m_threadTerminationHandler)
		{
			if(my_mutex.try_lock())
			{
				std::string data;
				if (readData(data))
						m_PPC1_data->data_corrupted = !decodeDataLine(data, m_PPC1_data);
				this->updateFlows(*m_PPC1_data, *m_PPC1_status); 
				my_mutex.unlock();
			}
			else {
				logError(HERE, " impossible to lock ");
				my_mutex.unlock();
				m_threadTerminationHandler = true;
			}
		}
		m_isRunning = false;
	}
	catch (serial::IOException &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close(); 
		logError(HERE, " IOException " + std::string(e.what()));
		m_excep_handler = true;
		return;
	}
	catch (serial::SerialException &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close(); 
		logError(HERE, " SerialException " + std::string(e.what())); 
		m_excep_handler = true;
		return;
	}
	catch (std::exception &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close();
		logError(HERE, 
			std::string(" exception " + std::string(e.what())));
		m_excep_handler = true;
		return;
	}
}

bool fluicell::PPC1api::decodeDataLine(const std::string &_data, 
	fluicell::PPC1dataStructures::PPC1_data *_PPC1_data) const
{
	// check for empty data
	if (_data.empty())
	{
		logError(HERE, " Error in decoding line - Empty line ");
		return false;
	}

	// check for _PPC1_data initialized
	if (_PPC1_data == NULL)
	{
		logError(HERE, " Error in decoding line - _PPC1_data not initialized ");
		return false;
	}

	std::vector<double> line;  // decoded line 

	if (_data.at(0) == 'A') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			_PPC1_data->channel_A->setChannelData ( line.at(0), line.at(1),
				line.at(2), (int)line.at(3));
			return true;
		}
		else {
			logError(HERE, " Error in decoding line ");
			return false;
		}
	}

	if (_data.at(0) == 'B') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			_PPC1_data->channel_B->setChannelData(line.at(0), line.at(1),
				line.at(2), (int)line.at(3));
			return true;
		}
		else {
			logError(HERE, " Error in decoding line ");
			return false;
		}
	}

	if (_data.at(0) == 'C') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			_PPC1_data->channel_C->setChannelData(line.at(0), line.at(1),
				line.at(2), (int)line.at(3));
			return true;
		}
		else {
			logError(HERE, " Error in decoding line ");
			return false;
		}
	}

	if (_data.at(0) == 'D') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			_PPC1_data->channel_D->setChannelData(line.at(0), line.at(1),
				line.at(2), (int)line.at(3));
			return true;
		}
		else {
			logError(HERE, " Error in decoding line ");
			return false;
		}
	}

	if (_data.at(0) == 'i') {
		// string format:  i0|j0|k0|l0
		// char index   :  0123456789
		int value = toDigit(_data.at(1));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->i = value;
		}
		else {
			logError(HERE, 
				" Error in decoding line _PPC1_data->i string: " + 
				_data + " value " + std::to_string(value));
			return false;
		}

		value = toDigit(_data.at(4));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->j = value;
		}
		else {
			logError(HERE,
				" Error in decoding line _PPC1_data->j string: " + 
				_data + " value " + std::to_string(value));
			return false;
		}

		value = toDigit(_data.at(7));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->k = value;
		}
		else { 
			logError(HERE,
				" Error in decoding line _PPC1_data->k string: " + 
				_data + " value " + std::to_string(value));
			return false;
		}

		value = toDigit(_data.at(10));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->l = value;
		}
		else {
			logError(HERE,
				" Error in decoding line _PPC1_data->l string: " + 
				_data + " value " + std::to_string(value));
			return false;
		}
		return true;
	}

	if (_data.at(0) == 'I') {
		// string format: IN1|OUT1 or IN0|OUT0
		// char index:    01234567
		int value = toDigit(_data.at(2));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->ppc1_IN = value;
		}
		else {
			logError(HERE, " Error in decoding line _PPC1_data->ppc1_IN " );
			return false;
		}
		value = toDigit(_data.at(7));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->ppc1_OUT = value;
		}
		else {
			logError(HERE, " Error in decoding line _PPC1_data->ppc1_OUT "); 
			return false;
		}
		return true;
	}

	if (_data.at(0) == 'P') {  // FALLING TTL signal detected
		// string format: P\n
		// char index:    01
		_PPC1_data->trigger_fall = true;
		_PPC1_data->trigger_rise = false;
		return true;
	} 

	if (_data.at(0) == 'R') {  //RISING TTL signal detected
		// string format: R\n
		// char index:    01
		_PPC1_data->trigger_fall = false;
		_PPC1_data->trigger_rise = true;
		return true;
	}

	return false;  // in case _data(0) is none of the expected value
}

bool fluicell::PPC1api::decodeChannelLine(const std::string &_data, std::vector<double> &_line) const
{
	// check for empty data
	if (_data.empty())
	{
		logError(HERE, " Error in decoding line - Empty line "); 
		return false;
	}

	_line.clear();
	unsigned int byte_counter = 2;              // in the line 0 is letter and 1 is the separator e.g. A|
	while (byte_counter < _data.length())       // scan the whole string
	{
		std::string value;
		// extract line 
		// extract the value before the character "new line"
		while (_data.at(byte_counter) != m_separator)//*separator)
		{
			if (_data.at(byte_counter) == m_end_line) // if the char is the endline the function break
			{
				_line.push_back(std::stod(value));
				break;
			}

			// check the char for validity
			// this is to make sure that stod function get an actual number instead of a character 
			if (!isdigit(_data.at(byte_counter))) // if the char is not a digit
				if (_data.at(byte_counter) != m_minus) // if the char is not the minus sign
					if (_data.at(byte_counter) != m_decimal_separator) // if the char is not the decimal separator
					{
						return false;  // something is wrong with the string (not a number)
					}

			// validity check passed
			value.push_back(_data.at(byte_counter));
			byte_counter++;
			if (byte_counter >= _data.length()) 
				break;
		}
		byte_counter++;

		// check if the value is empty
		if (!value.empty()) {
			_line.push_back(stod(value)); 
			if (_line.size() > 3)
				return true; // we expect 4 values so we exit at the 4th
		}
		else {
			_line.push_back(0.0);
		}
	}

	// check for proper data size
	if (_line.size() < 3) {
		logError(HERE, " Error in decoding line - corrupted data line "); 
		return false;
	}

	return true;
}

void fluicell::PPC1api::updateFlows(const fluicell::PPC1dataStructures::PPC1_data &_PPC1_data, 
	fluicell::PPC1dataStructures::PPC1_status &_PPC1_status) const
{
	// calculate inflow
	double delta_pressure = 100.0 * (-_PPC1_data.channel_A->sensor_reading);//   v_r;

	_PPC1_status.inflow_recirculation = 2.0 * 
		this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * (-_PPC1_data.channel_A->sensor_reading +
		2.0 * _PPC1_data.channel_C->sensor_reading * ( 1 - m_tip->length_to_tip / m_tip->length_to_zone) );
	_PPC1_status.inflow_switch = 2.0 * this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * 2.0 * _PPC1_data.channel_C->sensor_reading;
	_PPC1_status.solution_usage_off = this->getFlowSimple(delta_pressure, 2.0 * m_tip->length_to_zone);

	delta_pressure = 100.0 * _PPC1_data.channel_D->sensor_reading;
	_PPC1_status.solution_usage_on = this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * (_PPC1_data.channel_D->sensor_reading +
		(_PPC1_data.channel_C->sensor_reading * 3.0) -
		(-_PPC1_data.channel_B->sensor_reading * 2.0));
	_PPC1_status.outflow_on = this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

	delta_pressure = 100.0 * ((_PPC1_data.channel_C->sensor_reading * 4.0) -
		(-_PPC1_data.channel_B->sensor_reading * 2.0));
	_PPC1_status.outflow_off = 2.0 * this->getFlowSimple(delta_pressure, 2.0 * m_tip->length_to_zone);

	_PPC1_status.in_out_ratio_on = _PPC1_status.outflow_on / _PPC1_status.inflow_recirculation;
	_PPC1_status.in_out_ratio_off = _PPC1_status.outflow_off / _PPC1_status.inflow_recirculation;

	if (_PPC1_data.i || _PPC1_data.j ||
		_PPC1_data.k || _PPC1_data.l) // if one of the solutions is on
	{
		delta_pressure = 100.0 * (_PPC1_data.channel_D->sensor_reading +
			(_PPC1_data.channel_C->sensor_reading * 3.0) -
			(-_PPC1_data.channel_B->sensor_reading * 2.0));

		_PPC1_status.outflow_tot = _PPC1_status.outflow_on;
		_PPC1_status.in_out_ratio_tot = _PPC1_status.in_out_ratio_on;

		_PPC1_status.flow_rate_1 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_2 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_3 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_4 = _PPC1_status.solution_usage_off;

		if (_PPC1_data.l) _PPC1_status.flow_rate_1 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.k) _PPC1_status.flow_rate_2 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.j) _PPC1_status.flow_rate_3 = _PPC1_status.solution_usage_on;
		if (_PPC1_data.i) _PPC1_status.flow_rate_4 = _PPC1_status.solution_usage_on;
	}
	else {

		_PPC1_status.outflow_tot = _PPC1_status.outflow_off;
		_PPC1_status.in_out_ratio_tot = _PPC1_status.in_out_ratio_off;

		_PPC1_status.flow_rate_1 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_2 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_3 = _PPC1_status.solution_usage_off;
		_PPC1_status.flow_rate_4 = _PPC1_status.solution_usage_off;
	}

	_PPC1_status.flow_rate_5 = _PPC1_status.inflow_switch / 2.0;
	_PPC1_status.flow_rate_6 = _PPC1_status.inflow_switch / 2.0;
	_PPC1_status.flow_rate_7 = _PPC1_status.inflow_recirculation / 2.0;
	_PPC1_status.flow_rate_8 = _PPC1_status.inflow_recirculation / 2.0;

}

bool fluicell::PPC1api::connectCOM() 
{
	try {

		m_PPC1_serial->setPort(m_COMport);
		m_PPC1_serial->setBaudrate(m_baud_rate);
		m_PPC1_serial->setFlowcontrol(serial::flowcontrol_none);
		m_PPC1_serial->setParity(serial::parity_none);

		if (!checkVIDPID(m_COMport)) {
			logError(HERE, " no match VID/PID device "); 
			return false;
		}
		else {
			logStatus(HERE, " VID/PID match ");
		}
		// "Is the port open?";
		if (m_PPC1_serial->isOpen())
			std::this_thread::sleep_for(std::chrono::microseconds(100));  //--> do nothing, wait
		else {
			m_PPC1_serial->open();
            std::this_thread::sleep_for(std::chrono::microseconds(100));  
		}

		// if the first attempt to open the port fails then the connection fails
		if (!m_PPC1_serial->isOpen()) {
			logError(HERE, "FAILED - Serial port not open ");
			return false;
		}
		else {
			m_excep_handler = false; //only on connection verified we reset the exception handler
			return true; // open connection verified 
		}
	}
	catch (serial::IOException &e)
	{
		logError(HERE, " IOException " + std::string(e.what()));
		m_PPC1_serial->close(); 
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (serial::PortNotOpenedException &e)
	{
		logError(HERE, " PortNotOpenedException " + std::string(e.what()));
		m_PPC1_serial->close();
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (serial::SerialException &e)
	{
		logError(HERE, " SerialException " + std::string(e.what()));
		m_PPC1_serial->close(); 
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (std::exception &e) 
	{
		logError(HERE, " Unhandled Exception " + std::string(e.what()));
	m_PPC1_serial->close(); 
	//throw e;  // TODO: this crashes
	m_excep_handler = true;
	return false;
	}
}

void fluicell::PPC1api::disconnectCOM()
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->close();
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		m_excep_handler = false;
	}
}

void fluicell::PPC1api::pumpingOff() const
{
	if (m_PPC1_serial->isOpen()) {
		setVacuumChannelA(0.0);   
		setVacuumChannelB(0.0);   
		setPressureChannelC(0.0); 
		setPressureChannelD(0.0); 
		closeAllValves();
	}
}

bool fluicell::PPC1api::openAllValves() const
{
	if (m_PPC1_serial->isOpen()) 
		return setValvesState(0xFF);
	else return false;
}

bool fluicell::PPC1api::closeAllValves() const
{
	if (m_PPC1_serial->isOpen()) 
		return setValvesState(0xF0);
	else return false;
}

void fluicell::PPC1api::reboot() const
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->write("!\n");
	}
}

bool fluicell::PPC1api::setVacuumChannelA(const double _value) const
{

	if (_value >= MIN_CHAN_A && _value <= MAX_CHAN_A)
	{
		std::string ss;
		ss.append("A");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		logError(HERE, " out of range " );
		sendData("A0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setVacuumChannelB(const double _value) const
{
	if (_value >= MIN_CHAN_B && _value <= MAX_CHAN_B)
	{
		std::string ss;
		ss.append("B");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		logError(HERE, " out of range ");
		sendData("B0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setPressureChannelC(const double _value) const
{
	if (_value >= MIN_CHAN_C && _value <= MAX_CHAN_C)
	{
		std::string ss;
		ss.append("C");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		logError(HERE, " out of range ");
		sendData("C0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setPressureChannelD(const double _value) const
{
	if (_value >= MIN_CHAN_D && _value <= MAX_CHAN_D)
	{
		std::string ss;
		ss.append("D");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else {
		logError(HERE, " out of range ");
		sendData("D0.0\n");  // send 0
		return false;
	}

	return false;
}

bool fluicell::PPC1api::setValve_l(const bool _value) const
{
	if (_value) 
		return sendData("l1\n");   // close
	else 
		return sendData("l0\n");  // open
}

bool fluicell::PPC1api::setValve_k(const bool _value)  const 
{
	if (_value) 
		return sendData("k1\n");   // close
	else 
		return sendData("k0\n");  // open
}

bool fluicell::PPC1api::setValve_j(const bool _value)  const 
{
	if (_value) 
		return sendData("j1\n");   // close
	else 
		return sendData("j0\n");  // open
}

bool fluicell::PPC1api::setValve_i(const bool _value) const
{
	if (_value) 
		return sendData("i1\n");   // close
	else 
		return sendData("i0\n");  // open
}

bool fluicell::PPC1api::setValvesState(const int _value) const
{
	std::stringstream value;
	value << std::setfill('0') 
		  << std::setw(2) // we expect only one byte so 2 is the number of allowed hex digits
		  << std::hex << _value;
	std::string msg;
	msg.append("v");
	msg.append(value.str());
	msg.append("\n");
	return sendData(msg);
}

bool fluicell::PPC1api::setTTLstate(const bool _value) const
{
	if (_value) {
		if (sendData("o1\n"))   // high
		{
			m_PPC1_data->TTL_out_trigger = true;
			return true;
		}
	}
	else {
		if (sendData("o0\n"))  // low
		{
			m_PPC1_data->TTL_out_trigger = false;
			return true;
		}
	}

	return false;
}

bool fluicell::PPC1api::setPulsePeriod(const int _value) const
{
	if (_value >= MIN_PULSE_PERIOD )
	{
		std::string ss;
		ss.append("p");
		ss.append(std::to_string(_value));
		ss.append("\n");
		return sendData(ss);
	}
	else
	{
		logError(HERE, " out of range ");
		return false;
	}
}

bool fluicell::PPC1api::setRuntimeTimeout(const int _value) const
{

	if (_value < 1)
	{
		std::string ss;
		ss.append("z");
		ss.append(std::to_string(_value));
		ss.append("\n");
		return sendData(ss);
	}
	else
	{
		logError(HERE, " out of range ");
		return false;
	}
}

bool fluicell::PPC1api::setZoneSizePerc(double _percentage) const
{
	// check for out of bound values
	if (_percentage < MIN_ZONE_SIZE_PERC ||
		_percentage > MAX_ZONE_SIZE_PERC)
	{
		logError(HERE, " zone size value out of range ");
		return false; // out of bound
	}

	// convert percentage
	double percentage = _percentage / 100.0;

	// the zone size is actually the cubic root of the display value	
	double value = m_default_v_recirc * (2.0 -
		std::pow(percentage, (1.0 / 3.0)));
	
	logStatus(HERE, " new recirculation value " + std::to_string(value) +
			"m_default_v_recirc" + std::to_string(m_default_v_recirc));

	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		logError(HERE, " recirculation value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setVacuumChannelA(value)) {
		return false;
	}

	// reset the switch to default
	if (!setVacuumChannelB(m_default_v_switch)) {
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// calculate the new Pon value
	// using the cubic root of the display value
	value = m_default_pon * (
		std::pow(percentage, (1.0 / 3.0)));

	logStatus(HERE," new pon value " + std::to_string(value) +
			"m_default_pon" + std::to_string(m_default_pon));

	// check for out of bound pressure values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_D || value >= MAX_CHAN_D) {
		logError(HERE, " pon pressure value out of range ");
		return false; // out of bound 
	}

	// send the command
	if (!setPressureChannelD(value)) {
		return false;
	}

	// reset the poff to default
	if (!setPressureChannelC(m_default_poff)) {
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return true;
}

bool fluicell::PPC1api::changeZoneSizePercBy(double _percentage) const
{	
	// check for out of bound values
	if (std::abs(_percentage) > MAX_ZONE_SIZE_INCREMENT )
	{
		logError(HERE, " zone size value out of range ");
		return false; // out of bound
	}

	// convert percentage
	double increment = (100.0 + _percentage) / 100.0;

	// calculate new vacuum value
	// the zone size is actually the cubic root of the display value	
	double delta = (1.0 - std::pow(increment, (1.0 / 3.0)));
	double value = m_PPC1_data->channel_A->set_point +
		m_default_v_recirc * delta;

	logStatus(HERE,	" new recirculation value " + std::to_string(value) +
			"m_default_v_recirc" + std::to_string(m_default_v_recirc));
	 
	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		logError(HERE, " recirculation value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setVacuumChannelA(value)) {
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	
	// calculate new pressure value
	// the zone size is actually the cubic root of the display value	
	//delta = (1.0 - std::pow(increment, (1.0 / 3.0)));
	value = m_PPC1_data->channel_D->set_point - m_default_pon  * delta;
	
	logStatus(HERE,	" new pon value " + std::to_string(value) +
			"m_default_pon" + std::to_string(m_default_pon));
	
	// check for out of bound pressure values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_D || value >= MAX_CHAN_D) {
		logError(HERE, " pon pressure value out of range ");
		return false; // out of bound
	}
	
	// send the command
	if (!setPressureChannelD(value)) {
		return false;
	}
	
	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	return true;
}

double fluicell::PPC1api::getZoneSizePerc() const
{
	double in_out_ratio_on = 0;

	bool use_sensor_reading = false;
	if (use_sensor_reading) {
		in_out_ratio_on = m_PPC1_status->in_out_ratio_on;
	}
	else
	{
		// calculate the outflow_on based on set value instead of the sensor reading
		double outflow_on;
		double delta_pressure = 100.0 * (m_PPC1_data->channel_D->set_point +
			(m_PPC1_data->channel_C->set_point * 3.0) -
			(-m_PPC1_data->channel_B->set_point * 2.0));
	    outflow_on = this->getFlowSimple(delta_pressure, m_tip->length_to_tip);

		// calculate inflow_recirculation based on set value instead of the sensor reading
		double inflow_recirculation;
		delta_pressure = 100.0 * (-m_PPC1_data->channel_A->set_point);//   v_r;
		inflow_recirculation = 2.0 * this->getFlowSimple(delta_pressure, m_tip->length_to_tip);
		
		in_out_ratio_on = outflow_on / inflow_recirculation;
	}
	double ds = 100.0 *(in_out_ratio_on + 0.21) / 0.31;
	return ds;
}

bool fluicell::PPC1api::setFlowSpeedPerc(const double _percentage) const
{
	// check for out of bound values
	if (_percentage < MIN_FLOW_SPEED_PERC ||
		_percentage > MAX_FLOW_SPEED_PERC)
	{
		logError(HERE, " zone size value out of range ");
		return false; // out of bound
	}

	// convert percentage
	double percentage = _percentage / 100.0;

	// calculate new recirculation value
	double value = m_default_v_recirc * percentage;  

	logStatus(HERE, " new recirculation value " + std::to_string(value) );

	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		logError(HERE, " recirculation value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setVacuumChannelA(value)) { 
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// calculate new switch value
	value = m_default_v_switch * percentage;  

	logStatus(HERE, " new switch value " + std::to_string(value));

	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_B || value >= MAX_CHAN_B) {
		logError(HERE, " switch value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setVacuumChannelB(value)) {
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// calculate new Poff value
	value = m_default_poff * percentage;  

	logStatus(HERE, " new poff value " + std::to_string(value));

	// check for out of bound pressure values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_C || value >= MAX_CHAN_C) {
		logError(HERE, " poff pressure value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setPressureChannelC(value)) { // increase C by _percentage%
		return false;
	}
	
	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	// calculate new Pon value
	value = m_default_pon * percentage;  
	logStatus(HERE,	" new pon value " + std::to_string(value));

	// check for out of bound pressure values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_D || value >= MAX_CHAN_D) {
		logError(HERE, " pon pressure value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setPressureChannelD(value)) { // increase D by _percentage%
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return true;
}

bool fluicell::PPC1api::changeFlowSpeedPercBy(const double _percentage) const
{

	// check for out of bound values
	if (std::abs(_percentage) > MAX_FLOW_SPEED_INCREMENT)
	{
		logError(HERE, " flow speed value out of range ");
		return false; // out of bound
	}

	// convert percentage
	double percentage = _percentage / 100.0;

	// calculate new recirculation value
	double value = m_PPC1_data->channel_A->set_point + 
		m_default_v_recirc * percentage;  // new recirc value

	logStatus(HERE,	" new recirculation value " + std::to_string(value));

	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		logError(HERE, " recirculation value out of range ");
		return false; // out of bound
	}

	// send the command
	if ( !setVacuumChannelA(value)) { 
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait 10msec

	//calculate new switch value
	value = m_PPC1_data->channel_B->set_point + m_default_v_switch * percentage;  
	logStatus(HERE, " new switch value " + std::to_string(value));

	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_B || value >= MAX_CHAN_B) {
		logError(HERE, " switch value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setVacuumChannelB(value)) { 
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10)); // wait 10msec

	// calculate new Poff value
	value = m_PPC1_data->channel_C->set_point + m_default_poff * percentage;  // new pressure poff value
	logStatus(HERE, " new poff value " + std::to_string(value));
	
	// check for out of bound pressure values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_C || value >= MAX_CHAN_C) {
		logError(HERE, " poff pressure value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setPressureChannelC(value)) { 
		return false;
	}
	
	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	
	// calculate new Pon value
	value = m_PPC1_data->channel_D->set_point + m_default_pon * percentage;  
	logStatus(HERE, " new pon value " + std::to_string(value)); 

	// check for out of bound pressure values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_D || value >= MAX_CHAN_D) {
		logError(HERE, " pon pressure value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setPressureChannelD(value)) { // increase D by _percentage%
		return false;
	}

	// wait 10msec - just to give time to de device to accept the new value
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	return true;
}

double fluicell::PPC1api::getFlowSpeedPerc() const
{
	bool use_sensor_reading = false;
	double p1 = 0;
	double p2 = 0;
	double p3 = 0;
	double p4 = 0;
	if (use_sensor_reading) {

		p1 = std::abs(100.0 * m_PPC1_data->channel_A->sensor_reading / m_default_v_recirc);
		p2 = std::abs(100.0 * m_PPC1_data->channel_B->sensor_reading / m_default_v_switch);
		p3 = std::abs(100.0 * m_PPC1_data->channel_C->sensor_reading / m_default_poff);
		p4 = std::abs(100.0 * m_PPC1_data->channel_D->sensor_reading / m_default_pon);
	}
	else
	{
		p1 = std::abs(100.0 * m_PPC1_data->channel_A->set_point / m_default_v_recirc);
		p2 = std::abs(100.0 * m_PPC1_data->channel_B->set_point / m_default_v_switch);
		p3 = std::abs(100.0 * m_PPC1_data->channel_C->set_point / m_default_poff);
		p4 = std::abs(100.0 * m_PPC1_data->channel_D->set_point / m_default_pon);
	}
	double mean_percentage = (p1 + p2 + p3 + p4) / 4.0; // average 4 values
	return mean_percentage;
}

bool fluicell::PPC1api::setVacuumPerc(const double _percentage) const
{
	// check for out of bound values
	if (_percentage < MIN_VACUUM_PERC ||
		_percentage > MAX_VACUUM_PERC)
	{
		logError(HERE, " vacuum value out of range ");
		return false; // out of bound
	}

	// convert percentage
	double percentage = _percentage / 100.0;

	// calculate new recirculation value
	double value = m_default_v_recirc * percentage;

	logStatus(HERE, "new recirculation value " + std::to_string(value));

	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		logError(HERE, " recirculation value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setVacuumChannelA(value))
		return false;

	// reset the poff to default
	if (!setVacuumChannelB(m_default_v_switch)) {
		return false;
	}

	// reset the poff to default
	if (!setPressureChannelC(m_default_poff)) {
		return false;
	}

	// reset the poff to default
	if (!setPressureChannelD(m_default_pon)) {
		return false;
	}

	return true;
}

bool fluicell::PPC1api::changeVacuumPercBy(const double _percentage) const
{
	// check for out of bound values
	if (std::abs(_percentage) > MAX_VACUUM_INCREMENT)
	{
		logError(HERE, " vacuum value out of range ");
		return false; // out of bound
	}

	// convert percentage
	double percentage = _percentage / 100.0;

	// calculate new recirculation value
	double value = m_PPC1_data->channel_A->set_point +
		m_default_v_recirc * percentage;  // new recirc value
	logStatus(HERE, " new recirculation value " + std::to_string(value));

	// check for out of bound vacuum values after the calculation before
	// sending the command to the PPC1
	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		logError(HERE, " recirculation value out of range ");
		return false; // out of bound
	}

	// send the command
	if (!setVacuumChannelA(value))
		return false;

	return true;
}

double fluicell::PPC1api::getVacuumPerc() const
{
	bool use_sensor_reading = false;
	double value_A = 0;
	if (use_sensor_reading) {
		value_A = m_PPC1_data->channel_A->sensor_reading;
	}
	else {
		value_A = m_PPC1_data->channel_A->set_point;
	}

	double p1 = std::abs(100.0 * value_A / m_default_v_recirc);
	return p1;
}

double fluicell::PPC1api::getFlow(double _square_channel_mod, 
	double _pipe_diameter, double _delta_pressure, 
	double _dynamic_viscosity, double _pipe_length) const
{
	double num = M_PI * std::pow((_square_channel_mod * _pipe_diameter), 4) * _delta_pressure;
	double den = 128.0 * _dynamic_viscosity * _pipe_length;
	
	// flow in transformation to nL/s
	double flow = (num / den) * 1000.0 * 1000000000.0; // per channel 

	return flow;
}

bool fluicell::PPC1api::runCommand(fluicell::PPC1dataStructures::command _cmd) const
{
	if (!_cmd.checkValidity())  {
		logError(HERE, " check validity failed ");
		return false;
	}

	logStatus(HERE, " running the command " + _cmd.getCommandAsString() +
	  " value " + std::to_string(_cmd.getValue()));

	switch (_cmd.getInstruction()) {
	case fluicell::PPC1dataStructures::command::instructions::setZoneSize: {//zoneSize
		return setZoneSizePerc(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::changeZoneSizeBy: {//zoneSize
		return changeZoneSizePercBy(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::setFlowSpeed: {//flowSpeed
		return setFlowSpeedPerc(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::changeFlowSpeedBy: {//flowSpeed
		return changeFlowSpeedPercBy(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::setVacuum: {//vacuum
		return setVacuumPerc(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::changeVacuumBy: {//vacuum
		return changeVacuumPercBy(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::wait: {//sleep
		//TODO: this is not safe as one can stop the macro without breaking the wait function
		//however, wait function is handled at GUI level not at API level
		std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(_cmd.getValue())));
		// This could solve the issue of waiting and stopping 
		// (it has to wait for 1 second to finish though)
		// moreover, this introduces uncertainty in the time line
		// TO BE TESTED
		/*
		int number_of_seconds = static_cast<int>(_cmd.getValue());
		int count = 0;
		if (!m_threadTerminationHandler && count<number_of_seconds)
		{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		count++;
		}*/
		return true;
	}
	case fluicell::PPC1dataStructures::command::instructions::allOff: {//allOff	
		return closeAllValves();
	}
	case fluicell::PPC1dataStructures::command::instructions::solution1: {//solution1
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0)valve_status = false;
		else valve_status = true;
		return setValve_l(valve_status);
	}
	case fluicell::PPC1dataStructures::command::instructions::solution2: {//solution2
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0)valve_status = false;
		else valve_status = true;
		return setValve_k(valve_status);
	}
	case fluicell::PPC1dataStructures::command::instructions::solution3: {//solution3
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0)valve_status = false;
		else valve_status = true;
		return setValve_j(valve_status);
	}
	case fluicell::PPC1dataStructures::command::instructions::solution4: {//solution4
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0)valve_status = false;
		else valve_status = true;
		return setValve_i(valve_status);
	}
	case fluicell::PPC1dataStructures::command::instructions::setPon: { //setPon
		return setPressureChannelD(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::setPoff: {//setPoff
		return setPressureChannelC(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::setVrecirc: {//setVrecirc
		return setVacuumChannelB(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::setVswitch: {//setVswitch
		return setVacuumChannelA(_cmd.getValue());
	}
	case fluicell::PPC1dataStructures::command::instructions::ask_msg: {//ask_msg
		logStatus(HERE, " ask_msg NOT implemented in the API ");
		return true;
	}
	case fluicell::PPC1dataStructures::command::instructions::pumpsOff: {//pumpsOff
		pumpingOff();
		return true;
	}
	case fluicell::PPC1dataStructures::command::instructions::waitSync: {//waitSync 
		// waitsync(front type : can be : RISE or FALL), 
		// protocol stops until trigger signal is received
		bool state;
		if (_cmd.getValue() == 0) state = false;
		else state = true;
		// reset the sync signals and then wait for the correct state to come
		resetSycnSignals(false);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		clock_t begin = clock();
		while (!syncSignalArrived(state))
		{
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			clock_t end = clock();
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
			if (elapsed_secs > m_wait_sync_timeout) // break if timeout
			{
				logError(HERE, " waitSync timeout ");
				return false;
			}
		}
		return true;

	}
	case fluicell::PPC1dataStructures::command::instructions::syncOut: {//syncOut //TODO
		// syncout(int: pulse length in ms) if negative then default state is 1
		// and pulse is 0, if positive, then pulse is 1 and default is 0
		int v = static_cast<int>(_cmd.getValue());
		logStatus(HERE, " syncOut test value " + v);
		int current_ppc1out_status = m_PPC1_data->ppc1_OUT;
		bool success = setPulsePeriod(v);
		std::this_thread::sleep_for(std::chrono::milliseconds(v));
		//TODO : this function is unsafe, in case the protocol is stop during this function, 
		//       the stop will not work, it has to wait for the wait to end
		/*
		clock_t begin = clock();
		while (current_ppc1out_status == m_PPC1_data->ppc1_OUT)
		{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
		while (current_ppc1out_status != m_PPC1_data->ppc1_OUT)
		{
		std::this_thread::sleep_for(std::chrono::microseconds(100));
		}
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;*/
		return success;
	}
	case fluicell::PPC1dataStructures::command::instructions::loop: {//loop
		logStatus(HERE, " loop NOT implemented in the API " );
		return true;
	}
	default:{
		logError(HERE, " Command NOT recognized ");
		return false;
	}
	}
	return false;
}

bool fluicell::PPC1api::setDataStreamPeriod(const int _value) {
	if (_value >=  MIN_STREAM_PERIOD && _value <=  MAX_STREAM_PERIOD )
	{
		m_dataStreamPeriod = _value;
		std::string ss;
		ss.append("u");
		ss.append(std::to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		logError(HERE, " out of range ");
		sendData("u200\n");  // send default value
		return false;
	}
	return false;

}

bool fluicell::PPC1api::setDefaultPV(double _default_pon, double _default_poff, 
	double _default_v_recirc, double _default_v_switch)
{
	logStatus(HERE, 
		" _default_pon " + std::to_string(_default_pon) +
		" _default_poff " + std::to_string(_default_poff) +
		" _default_v_recirc " + std::to_string(_default_v_recirc) +
		" _default_v_switch " + std::to_string(_default_v_switch) );

if (_default_pon > MIN_CHAN_D && _default_pon < MAX_CHAN_D) {
		m_default_pon = _default_pon;
	}
	else {
		logError(HERE, " default pon out of range ");
		return false;
	}
	if (_default_poff > MIN_CHAN_C && _default_poff < MAX_CHAN_C) {
		m_default_poff = _default_poff;
	}
	else {
		logError(HERE, " default poff out of range ");
		return false;
	}
	if (_default_v_recirc > MIN_CHAN_A && _default_v_recirc < MAX_CHAN_A) {
		m_default_v_recirc = _default_v_recirc;
	}
	else {
		logError(HERE, " default v recirculation out of range ");
		return false;
	}
	if (_default_v_switch > MIN_CHAN_B && _default_v_switch < MAX_CHAN_B) {
		m_default_v_switch = _default_v_switch;
	}
	else {
		logError(HERE, " default v switch out of range ");
		return false;
	}

	return true;
}

std::string fluicell::PPC1api::getDeviceID() 
{
	std::string serialNumber; //device serial number

	if (!m_PPC1_serial->isOpen()) {
		logError(HERE, " cannot return the device serial number, device not connected ");
		return "";
	}

	// stop the stream to be able to get the value
	if (!setDataStreamPeriod(0))
	{
		logError(HERE, " cannot set the data stream period to 0 ");
	}

	// send the character to get the device serial number
	sendData("#\n"); // this character is not properly sent maybe a serial lib bug?
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	readData(serialNumber);
	logStatus(HERE, " the serial number is : " + serialNumber);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	// restore the data stream to the default value
	setDataStreamPeriod(200);// (m_dataStreamPeriod);
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	logStatus(HERE, " set data stream to : " + m_dataStreamPeriod);

	return serialNumber;
}

void fluicell::PPC1api::setFilterEnabled(bool _enable)
{
	logStatus(HERE, " new filter size value " + _enable);
	m_filter_enabled = _enable;

	m_PPC1_data->enableFilter(_enable);
}

void fluicell::PPC1api::setFilterSize(int _size)
{
	logStatus(HERE, " new filter size value " + _size);

	if (_size < 1)
	{
		logError(HERE, " negative value on set size " + std::to_string(_size) );
		return;
	}
	
	m_PPC1_data->setFilterSize(_size); 
}

bool fluicell::PPC1api::sendData(const std::string &_data) const
{
	if (m_PPC1_serial->isOpen()) {
		logStatus(HERE, " sending the string " + _data );

		if (m_PPC1_serial->write(_data) > 0) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool fluicell::PPC1api::readData(std::string &_out_data)
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->flush();   // make sure that the buffer is clean
		if (m_PPC1_serial->readline(_out_data, 65536, "\n") > 0) {
			return true;
		}
		else {
			logError(HERE, " cannot read data --- readline ");
			return false;
		}
	} // if the port is not open we cannot read data
	else {
		logError(HERE, " cannot read data --- port not open");
		return false;
	}
}

bool fluicell::PPC1api::checkVIDPID(const std::string &_port) const
{
	// try to get device information
	std::vector<serial::PortInfo> devices = serial::list_ports();
	std::vector<fluicell::PPC1dataStructures::serialDeviceInfo> devs;
	for (unsigned int i = 0; i < devices.size(); i++) // for all the connected devices extract information
	{
		fluicell::PPC1dataStructures::serialDeviceInfo dev;
		dev.port = devices.at(i).port;
		dev.description = devices.at(i).description;
		dev.VID = "N/A";
		dev.PID = "N/A";
		std::string hw_info = devices.at(i).hardware_id;
		std::string v = "VID";
		std::string p = "PID";
		// the fluicell PPC1 device expected string is USB\VID_16D0&PID_083A&REV_0200
		if (hw_info.length() < 1) {
			logError(HERE, " hardware info string length not correct ");
			return false; 
		}

		for (unsigned int j = 0; j < hw_info.size() - 2; j++)
		{
			// extract 3 characters looking for the strings VID or PID
			std::string s = hw_info.substr(j, 3);
			if (s.compare(v) == 0 && hw_info.size() >= j + 4) { 
				// extract the 4 characters after VID_
				std::string vid = hw_info.substr(j + 4, 4); 
				dev.VID = vid;
			}
			if (s.compare(p) == 0 && hw_info.size() >= j+4 ) {
				// extract the 4 characters after PID_
				std::string pid = hw_info.substr(j + 4, 4);
				dev.PID = pid;
			}
		}
		devs.push_back(dev);
	}
	
	for (unsigned int i = 0; i < devs.size(); i++) // for all the connected devices 
		if (devs.at(i).port.compare(_port) == 0) // look for the device connected on _port
			if (devs.at(i).VID.compare(PPC1_VID) == 0) // check VID
				if (devs.at(i).PID.compare(PPC1_PID) == 0) // check PID
					return true; // if all success return true
	return false; // if only one on previous fails, return false VID/PID do not match
}

void fluicell::PPC1api::logError(const std::string& _caller, const std::string& _message) const
{
	std::cerr << currentDateTime() << "  " << _caller << ": "
		<< " ---- error --- MESSAGE:" << _message << std::endl;
}

void fluicell::PPC1api::logStatus(const std::string& _caller, const std::string& _message) const
{
	if (m_verbose)
		std::cout << currentDateTime() << "  " << _caller << ": "
			<< _message << std::endl;
}

std::string fluicell::PPC1api::currentDateTime() const
{
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

double fluicell::PPC1api::protocolDuration(
	std::vector<fluicell::PPC1dataStructures::command> &_protocol) const
{
	// compute the duration of the protocol
	double duration = 0.0;
	for (size_t i = 0; i < _protocol.size(); i++) {
		if (_protocol.at(i).getInstruction() ==
			fluicell::PPC1dataStructures::command::wait)
			duration += _protocol.at(i).getValue();
	}
	return duration;
}

fluicell::PPC1api::~PPC1api()
{
	// make sure the thread and the communications are properly closed
	if (m_threadTerminationHandler) {
		//this->stop();
	}
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->close();
	}

	// free memory
	delete m_PPC1_data;
	delete m_PPC1_status;
	delete m_PPC1_serial;
}
