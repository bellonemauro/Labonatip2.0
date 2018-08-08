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

fluicell::PPC1api::PPC1api() :
	m_PPC1_data(new PPC1api::PPC1_data),
	m_PPC1_status(new PPC1api::PPC1_status),
	m_verbose(false)
{
	// initialize and connect serial port objects
	m_PPC1_serial = new serial::Serial();

	// com port name
	m_COMport = "COM1";

	// exception handler
	m_excep_handler = false;

	// baud rate
	m_baud_rate = 115200;

	// data stream period
	m_dataStreamPeriod = 200;
	m_COM_timeout = 250;

	// default waitsync time out 
	m_wait_sync_timeout = 60; // 1 minute

	// set default values for pressures and vacuums
	setDefaultPV();
	
	// set default filter values
	m_filter_enabled = false;
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
//			if(m_verbose) cout << " thread running " << endl;
			if(my_mutex.try_lock())
			{
				string data;
				if (readData(data))
					if (!decodeDataLine(data, m_PPC1_data)) 
						cerr << currentDateTime() 
						     << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: " 
					  	     << "corrupted data " << endl;
				
				this->updateFlows(*m_PPC1_data, *m_PPC1_status); 
				my_mutex.unlock();
			}
			else {
				cerr << currentDateTime() 
					 << " fluicell::PPC1api::threadSerial  ----  error --- MESSAGE: "
					 << " impossible to lock " << endl;
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
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: IOException : " 
			 << e.what() << endl;
		m_excep_handler = true;
		return;
	}
	catch (serial::SerialException &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close(); 
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: SerialException : " 
			 << e.what() << endl;
		m_excep_handler = true;
		return;
	}
	catch (exception &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close();
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: --exception : " 
			 << e.what() << endl;
		m_excep_handler = true;
		return;
	}

	return;
}

bool fluicell::PPC1api::decodeDataLine(const string &_data, PPC1_data *_PPC1_data) 
{
	// check for empty data
	if (_data.empty())
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::decodeDataLine ::: " 
			 << "Error in decoding line - Empty line " 
			<< endl;
		return false;
	}

	// check for _PPC1_data initialized
	if (_PPC1_data == NULL)
	{
		cerr << currentDateTime()
			<< " fluicell::PPC1api::decodeDataLine ::: "
			<< " Error in decoding line - _PPC1_data not initialized " 
			<< endl;
		return false;
	}

	vector<double> line;  // decoded line 

	if (_data.at(0) == 'A') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			_PPC1_data->channel_A->setChannelData ( line.at(0), line.at(1),
				line.at(2), (int)line.at(3));
			return true;
		}
		else {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " 
				 << endl;
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
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " 
				 << endl;
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
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " 
				 << endl;
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
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " << endl;
			return false;
		}
	}

	if (_data.at(0) == 'i') {
		//cout << " line type i " << _data << endl;
		// string format:  i0|j0|k0|l0
		// char index   :  0123456789
		int value = toDigit(_data.at(1));  
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->i = value;
		}
		else {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line _PPC1_data->i string:" 
				 << _data << " value " << value << endl;
			return false;
		}

		value = toDigit(_data.at(4));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->j = value;
		}
		else {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line _PPC1_data->j" 
				<< endl;
			return false;
		}

		value = toDigit(_data.at(7));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->k = value;
		}
		else { 
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line _PPC1_data->k" 
				<< endl;
			return false;
		}

		value = toDigit(_data.at(10));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->l = value;
		}
		else {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line _PPC1_data->l" 
				<< endl;
			return false;
		}
		return true;
	}

	if (_data.at(0) == 'I') {
		
		//cout << " line type I " << _data << endl;
		// string format: IN1|OUT1 or IN0|OUT0
		// char index:    01234567
		int value = toDigit(_data.at(2));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->ppc1_IN = value;
		}
		else {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line _PPC1_data->ppc1_IN" 
				<< endl;
			return false;
		}
		value = toDigit(_data.at(7));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			_PPC1_data->ppc1_OUT = value;
		}
		else {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line _PPC1_data->ppc1_OUT" 
				<< endl;
			return false;
		}
		return true;
	}

	if (_data.at(0) == 'P') {  // FALLING TTL signal detected
		//cout << " line type I " << _data << endl;
		// string format: P\n
		// char index:    01
		_PPC1_data->trigger_fall = true;
		_PPC1_data->trigger_rise = false;
		return true;
	} //TODO: this can be the opposite

	if (_data.at(0) == 'R') {  //RISING TTL signal detected
		//cout << " line type I " << _data << endl;
		// string format: P\n
		// char index:    01
		_PPC1_data->trigger_fall = false;
		_PPC1_data->trigger_rise = true;

		return true;
	}

	return false;  // in case _data(0) is none of the expected value
}

bool fluicell::PPC1api::decodeChannelLine(const string &_data, vector<double> &_line)
{
	// check for empty data
	if (_data.empty())
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::decodeChannelLine ::: Error in decoding line - Empty line " 
			<< endl;
		return false;
	}

	//cerr << _data.c_str() << endl;
	_line.clear();
	unsigned int byte_counter = 2;              // in the line 0 is letter and 1 is the separator e.g. A|
	const char separator[] = "|";               // separator between data
	const char decimal_separator[] = ".";       // separator between data
	const char minus[] = "-";					// minus sign
	const char end_line[] = "\n";					// minus sign
	while (byte_counter < _data.length())       // scan the whole string
	{
		string value;
		// extract line 
		// extract the value before the character "new line"
		while (_data.at(byte_counter) != *separator)
		{
			if (_data.at(byte_counter) == *end_line) // if the char is the endline the function break
			{
				_line.push_back(stod(value));
				break;
			}

			// check the char for validity
			if (!isdigit(_data.at(byte_counter))) // if the char is not a digit
				if (_data.at(byte_counter) != *minus) // if the char is not the minus sign
					if (_data.at(byte_counter) != *decimal_separator) // if the char is not the decimal separator
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
			//cerr << value.c_str() << endl; //TODO: take this out

			_line.push_back(stod(value)); //TODO: error invalid stod argument
			if (_line.size() > 3)
				return true; // we expect 4 values so we exit at the 4th
		}
		else {
			_line.push_back(0.0);
		}
	}

	// check for proper data size
	if (_line.size() < 3) {
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::decodeChannelLine ::: " 
			 << " Error in decoding line - corrupted data line " 
			 << endl;
		return false;
	}

	return true;
}

void fluicell::PPC1api::updateFlows(const PPC1_data &_PPC1_data, PPC1_status &_PPC1_status)
{
	// calculate inflow
	double delta_pressure = 100.0 * (-_PPC1_data.channel_A->sensor_reading);//   v_r;

	_PPC1_status.inflow_recirculation = 2.0 * this->getFlowSimple(delta_pressure, LENGTH_TO_TIP);

	delta_pressure = 100.0 * (-_PPC1_data.channel_A->sensor_reading +
		2.0 * _PPC1_data.channel_C->sensor_reading * ( 1 - LENGTH_TO_TIP / LENGTH_TO_ZONE) );
	_PPC1_status.inflow_switch = 2.0 * this->getFlowSimple(delta_pressure, LENGTH_TO_TIP);

	delta_pressure = 100.0 * 2.0 * _PPC1_data.channel_C->sensor_reading;
	_PPC1_status.solution_usage_off = this->getFlowSimple(delta_pressure, 2.0 * LENGTH_TO_ZONE);

	delta_pressure = 100.0 * _PPC1_data.channel_D->sensor_reading;
	_PPC1_status.solution_usage_on = this->getFlowSimple(delta_pressure, LENGTH_TO_TIP);

	delta_pressure = 100.0 * (_PPC1_data.channel_D->sensor_reading +
		(_PPC1_data.channel_C->sensor_reading * 3.0) -
		(-_PPC1_data.channel_B->sensor_reading * 2.0));
	_PPC1_status.outflow_on = this->getFlowSimple(delta_pressure, LENGTH_TO_TIP);

	delta_pressure = 100.0 * ((_PPC1_data.channel_C->sensor_reading * 4.0) -
		(-_PPC1_data.channel_B->sensor_reading * 2.0));
	_PPC1_status.outflow_off = 2.0 * this->getFlowSimple(delta_pressure, 2.0 * LENGTH_TO_ZONE);

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
		//TODO: for some reason this does not work on Ubuntu
		//m_PPC1_serial->setTimeout(serial::Timeout::simpleTimeout(m_COM_timeout));// (my_timeout);

		if (!checkVIDPID(m_COMport)) {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::connectCOM :::  no match VID/PID device "  << endl;
			return false;
		}
		else {
			if (m_verbose) cout << currentDateTime()
				<< " fluicell::PPC1api::connectCOM :::  VID/PID match " << endl;
		}
		// "Is the port open?";
		if (m_PPC1_serial->isOpen())
			// if(m_verbose) cout << << currentDateTime()
			// << " fluicell::PPC1api::connectCOM ::: Serial Port already open" << endl;
			std::this_thread::sleep_for(std::chrono::microseconds(100));  //--> do nothing, wait
		else {
			// if(m_verbose) cout << << currentDateTime()
			// << " fluicell::PPC1api::connectCOM ::: Serial port not open, opening . . ." << endl;
			m_PPC1_serial->open();
            std::this_thread::sleep_for(std::chrono::microseconds(100));  
		}

		// if the first attempt to open the port fails then the connection fails
		if (!m_PPC1_serial->isOpen()) {

			cerr << currentDateTime() 
				 << " fluicell::PPC1api::connectCOM ::: FAILED - Serial port not open." 
				 << endl;
			return false;
		}
		else {
			m_excep_handler = false; //only on connection verified we reset the exception handler
			return true; // open connection verified 
		}
	}
	catch (serial::IOException &e)
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::connectCOM ::: IOException : " 
			 << e.what() << endl;
		m_PPC1_serial->close(); 
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (serial::PortNotOpenedException &e)
	{
		cerr << currentDateTime()
			 << " fluicell::PPC1api::connectCOM ::: PortNotOpenedException : " 
			 << e.what() << endl;
		m_PPC1_serial->close();
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (serial::SerialException &e)
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::connectCOM ::: SerialException : " 
			 << e.what() << endl;
		m_PPC1_serial->close(); 
		//throw e;
		m_excep_handler = true;
		return false;
	}
	catch (exception &e) {
	cerr << currentDateTime() 
		 << " fluicell::PPC1api::connectCOM ::: Unhandled Exception: " 
		 << e.what() << endl;
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

void fluicell::PPC1api::pumpingOff()
{
	if (m_PPC1_serial->isOpen()) {
		setVacuumChannelA(0.0);   // sendData("A0.0\n");
		setVacuumChannelB(0.0);   // sendData("B0.0\n");
		setPressureChannelC(0.0); // sendData("C0.0\n");
		setPressureChannelD(0.0); // sendData("D0.0\n");
		closeAllValves();
	}
}

bool fluicell::PPC1api::openAllValves()
{
	if (m_PPC1_serial->isOpen()) 
		return setValvesState(0xFF);
	else return false;
}

bool fluicell::PPC1api::closeAllValves()
{
	if (m_PPC1_serial->isOpen()) 
		return setValvesState(0xF0);
	else return false;
}

void fluicell::PPC1api::reboot()
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->write("!\n");
	}
}

bool fluicell::PPC1api::setVacuumChannelA(const double _value)
{

	if (_value >= MIN_CHAN_A && _value <= MAX_CHAN_A)
	{
		string ss;
		ss.append("A");
		ss.append(to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::setPressureChannelA ::: out of range "
			 << endl;
		sendData("A0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setVacuumChannelB(const double _value)
{
	if (_value >= MIN_CHAN_B && _value <= MAX_CHAN_B)
	{
		string ss;
		ss.append("B");
		ss.append(to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::setPressureChannelB ::: out of range " 
			 << endl;
		sendData("B0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setPressureChannelC(const double _value)
{
	if (_value >= MIN_CHAN_C && _value <= MAX_CHAN_C)
	{
		string ss;
		ss.append("C");
		ss.append(to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::setPressureChannelC ::: out of range "
			 << endl;
		sendData("C0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setPressureChannelD(const double _value)
{
	if (_value >= MIN_CHAN_D && _value <= MAX_CHAN_D)
	{
		string ss;
		ss.append("D");
		ss.append(to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::setPressureChannelD ::: out of range "
			 << endl;
		sendData("D0.0\n");  // send 0
		return false;
	}

	return false;
}

bool fluicell::PPC1api::setValve_l(const bool _value)
{
	if (_value) {
		if (sendData("l1\n"))   // close
		return true;
	}
	else {
		if (sendData("l0\n"))  // open
		return true;
	}

	return false;
}

bool fluicell::PPC1api::setValve_k(const bool _value) {
	if (_value) {
		if (sendData("k1\n"))   // close
			return true;
	}
	else {
		if (sendData("k0\n"))  // open
			return true;
	}

	return false;
}

bool fluicell::PPC1api::setValve_j(const bool _value) {
	if (_value) {
		if (sendData("j1\n"))   // close
			return true;
	}
	else {
		if (sendData("j0\n"))  // open
			return true;
	}

	return false;
}

bool fluicell::PPC1api::setValve_i(const bool _value) {
	if (_value) {
		if (sendData("i1\n"))   // close
			return true;
	}
	else {
		if (sendData("i0\n"))  // open
			return true;
	}

	return false;
}

bool fluicell::PPC1api::setValvesState(const int _value)
{

	stringstream value;
	value << std::setfill('0') 
		  << std::setw(2) // we expect only one byte so 2 is the number of allowed hex digits
		  << std::hex << _value;
	string msg;
	msg.append("v");
	msg.append(value.str());
	msg.append("\n");
	if (sendData(msg)) {
		return true;
	}
	
	return false;
}

bool fluicell::PPC1api::setTTLstate(const bool _value)
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

bool fluicell::PPC1api::setPulsePeriod(const int _value)
{

	if (_value >= MIN_PULSE_PERIOD )
	{
		string ss;
		ss.append("p");
		ss.append(to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::setPulsePeriod ::: out of range < 20 " 
			 << endl;
		return false;
	}

	return false;

}

bool fluicell::PPC1api::setRuntimeTimeout(const int _value)
{

	if (_value < 1)
	{
		string ss;
		ss.append("z");
		ss.append(to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::setRuntimeTimeout ::: out of range  " 
			 << endl;
		return false;
	}

	return false;

}

bool fluicell::PPC1api::setDropletSize(double _percentage)
{
	double percentage = _percentage / 100.0;

	// the droplet size is actually the cubic root of the display value	
	double value = m_default_v_recirc + (m_default_v_recirc * (2.0 -
		std::pow(percentage, (1.0 / 3.0))) - m_default_v_recirc);
	
	if (m_verbose) cout << currentDateTime()
		<< " fluicell::PPC1api::setDropletSize " << percentage
		<< " new recirculation value << " << value << " >> "
		<< " m_default_v_recirc " << m_default_v_recirc << endl;

	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		cerr << currentDateTime()
			 << " fluicell::setDropletSize -- recirculation value out of bound "
			 << endl;
		return false; // out of bound
	}

	if (!setVacuumChannelA(value)) {
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec

	value = m_default_pon + (m_default_pon * (
		std::pow(percentage, (1.0 / 3.0))) - m_default_pon);

	// the droplet size is actually the cubic root of the display value

	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::setDropletSize " << percentage
		<< " new pon value << " << value << " >> "
		<< " m_default_pon " << m_default_pon << endl;

	if (value <= MIN_CHAN_D || value >= MAX_CHAN_D) {
		cerr << currentDateTime()
			 << " fluicell::setDropletSize -- pressure value out of bound "
			 << endl;
		return false; // out of bound 
	}

	if (!setPressureChannelD(value)) {
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	return true;
}

bool fluicell::PPC1api::changeDropletSizeBy(double _percentage)
{	
	double percentage = this->getDropletSize() + _percentage;
	return setDropletSize(percentage);
}

double fluicell::PPC1api::getDropletSize()
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
	    outflow_on = this->getFlowSimple(delta_pressure, LENGTH_TO_TIP);

		// calculate inflow_recirculation based on set value instead of the sensor reading
		double inflow_recirculation;
		delta_pressure = 100.0 * (-m_PPC1_data->channel_A->set_point);//   v_r;
		inflow_recirculation = 2.0 * this->getFlowSimple(delta_pressure, LENGTH_TO_TIP);
		
		in_out_ratio_on = outflow_on / inflow_recirculation;
	}
	double ds = 100.0 *(in_out_ratio_on + 0.21) / 0.31;
	return ds;// mean_percentage;
}

bool fluicell::PPC1api::setFlowspeed(const double _percentage)
{

	double percentage = _percentage / 100.0;
	double value = m_default_v_recirc * percentage;  // new recirc value

	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::setFlowspeed"
		<< " new recirculation value << " << value << " >> " 
		<< endl;

	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		cerr << currentDateTime()
			 << " fluicell::setFlowspeed -- recirculation value out of bound " 
			 << endl;
		return false; // out of bound
	}

	if (value == 0 || !setVacuumChannelA(value)) { // reduce A by _percentage%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec

	value = m_default_v_switch * percentage;  // new switch value

	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::setFlowspeed"
		<< " new switch value << " << value << " >> " << endl;

	if (value <= MIN_CHAN_B || value >= MAX_CHAN_B) {
		cerr << currentDateTime()
			 << " fluicell::setFlowspeed -- switch value out of bound " 
			 << endl;
		return false; // out of bound
	}

	if (value == 0 || !setVacuumChannelB(value)) { // increase B by _percentage%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec

	value = m_default_poff * percentage;  // new pressure poff value

	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::setFlowspeed"
		<< " new poff value << " << value << " >> " 
		<< endl;

	if (value <= MIN_CHAN_C || value >= MAX_CHAN_C) {
		cerr << currentDateTime()
			 << " fluicell::setFlowspeed -- pressure off value out of bound " 
			 << endl;
		return false; // out of bound
	}

	if (value == 0 || !setPressureChannelC(value)) { // increase C by _percentage%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec

	value = m_default_pon * percentage;  // new pressure pon value

	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::setFlowspeed"
		<< " new pon value << " << value << " >> " << endl;

	if (value <= MIN_CHAN_D || value >= MAX_CHAN_D) {
		cerr << currentDateTime()
			 << " fluicell::setFlowspeed -- pressure on value out of bound " 
			 << endl;
		return false; // out of bound
	}

	if (value == 0 || !setPressureChannelD(value)) { // increase D by _percentage%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec

	return true;
}

bool fluicell::PPC1api::changeFlowspeedBy(const double _percentage)
{

	double percentage = this->getFlowSpeed() + _percentage;
	return setFlowspeed(percentage);
}

double fluicell::PPC1api::getFlowSpeed()
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

bool fluicell::PPC1api::setVacuumPercentage(const double _percentage)
{
	double percentage = _percentage / 100.0;
	double value = m_default_v_recirc * percentage;

	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::setVacuumPercentage"
		<< " new recirculation value << " << value << " >> " << endl;

	if (value <= MIN_CHAN_A || value >= MAX_CHAN_A) {
		cerr << currentDateTime()
			 << " fluicell::setVacuumPercentage -- recirculation value out of bound " 
			 << endl;
		return false; // out of bound
	}

	if (value == 0 || !setVacuumChannelA(value))
		return false;

	return true;
}

bool fluicell::PPC1api::changeVacuumPercentageBy(const double _percentage)
{
	
	double percentage = this->getVacuum() + _percentage;

	return setVacuumPercentage(percentage);
}

double fluicell::PPC1api::getVacuum()
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
	double _dynamic_viscosity, double _pipe_length)
{
	double num = M_PI * std::pow((_square_channel_mod * _pipe_diameter), 4) * _delta_pressure;
	double den = 128.0 * _dynamic_viscosity * _pipe_length;
	
	// flow in transformation to nL/s
	double flow = (num / den) * 1000.0 * 1000000000.0; // per channel 

	return flow;
}

bool fluicell::PPC1api::runCommand(command _cmd)
{
	if (!_cmd.checkValidity())  {
		cerr << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: check validity failed "
			<< endl; 
		return false;
	}

	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::run(command _cmd)" 
		<< " ::: running the command " << _cmd.getCommandAsString()
		<< " value = " << _cmd.getValue()
		<< " visualize_status = " << _cmd.isStatusVisualized()
		<< " status message = " << _cmd.getStatusMessage() << endl;

	switch (_cmd.getInstruction()) {
	case 0: { //setPon
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: setPon  " 
			<< _cmd.getValue() << endl;
		return setPressureChannelD(_cmd.getValue());
	}
	case 1: {//setPoff
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: setPoff  " 
			<< _cmd.getValue() << endl;
		return setPressureChannelC(_cmd.getValue());
	}
	case 2: {//setVswitch
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: setVswitch  " 
			<< _cmd.getValue() << endl;
		return setVacuumChannelA(_cmd.getValue());
	}
	case 3: {//setVrecirc
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: setVrecirc  " 
			<< _cmd.getValue() << endl;
		return setVacuumChannelB(_cmd.getValue());
	}
	case 4: {//solution1
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: solution1  " 
			<< _cmd.getValue() << endl;
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0)valve_status = false;
		else valve_status = true;
		return setValve_l(valve_status);
	}
	case 5: {//solution2
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: solution2  " 
			<< _cmd.getValue() << endl;
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0)valve_status = false;
		else valve_status = true;
		return setValve_k(valve_status);
	}
	case 6: {//solution3
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: solution3  " 
			<< _cmd.getValue() << endl;
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0 )valve_status = false;
		else valve_status = true;
		return setValve_j(valve_status);
	}
	case 7: {//solution4
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: solution4  " 
			<< _cmd.getValue() << endl;
		if (!closeAllValves())return false;
		int v = static_cast<int>(_cmd.getValue());
		bool valve_status;
		if (v == 0)valve_status = false;
		else valve_status = true; 
		return setValve_i(valve_status);
	}
	case 8: {//sleep
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: sleep  "
			<< _cmd.getValue() << endl; //TODO: this is not safe as one can stop the macro without break the wait function
		std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(_cmd.getValue())));
		return true;
	}
	case 9: {//ask_msg
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: ask_msg NOT implemented in the API " << endl;
		return true;
	}
	case 10: {//allOff	
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: allOff  " << endl;
		return closeAllValves(); 
	}
	case 11: {//pumpsOff
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: pumpsOff  " << endl;
		pumpingOff();
		return true;
	}
	case 12: {//waitSync //TODO
			  //waitsync(front type : can be : RISE or FALL), protocol stops until trigger signal is received
		bool state;
		if (_cmd.getValue() == 0) state = false;
		else state = true;
		if (m_verbose) cout << currentDateTime()
            << " fluicell::PPC1api::run(command _cmd) ::: waitSync = "
            << state << endl;
		// reset the sync signals and then wait for the correct state to come
		resetSycnSignals(false);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		clock_t begin = clock();
		while (!syncSignalArrived(state))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			clock_t end = clock();
			double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC; 
			if (elapsed_secs > m_wait_sync_timeout) // break if timeout
			{
				cerr << currentDateTime()
					<< " fluicell::PPC1api::run(command _cmd) ::: waitSync timeout "
					<< endl;
				return false;
			}
		}
		return true;

	}
	case 13: {//syncOut //TODO
        // syncout(int: pulse length in ms) if negative then default state is 1
        // and pulse is 0, if positive, then pulse is 1 and default is 0
		int v = static_cast<int>(_cmd.getValue());
		if (m_verbose) cout << currentDateTime()
			 << " fluicell::PPC1api::run(command _cmd) ::: "
			 << "syncOut NOT implemented in the API ::: test value = " 
			 << v << endl;
		int current_ppc1out_status = m_PPC1_data->ppc1_OUT;
		bool success = setPulsePeriod(v);
		std::this_thread::sleep_for(std::chrono::milliseconds(v));
		//TODO : this function is unsafe, in case the protocol is stop during this function, 
		//       the stop will not work
		/*
		clock_t begin = clock();
		while (current_ppc1out_status == m_PPC1_data->ppc1_OUT)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		while (current_ppc1out_status != m_PPC1_data->ppc1_OUT)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		clock_t end = clock();
		double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;*/
		return success;
	}
	case 14: {//dropletSize
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: dropletSize  NOT entirely implemented in the API" 
			<< _cmd.getValue() << endl;
		return setDropletSize(_cmd.getValue()); 
	}
	case 15: {//flowSpeed
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: flowSpeed  NOT entirely implemented in the API" 
			<< _cmd.getValue() << endl;
		return setFlowspeed(_cmd.getValue()); 
	}
	case 16: {//vacuum
		if (m_verbose) cout << currentDateTime()
			<< " fluicell::PPC1api::run(command _cmd) ::: vacuum  " 
			<< _cmd.getValue() << endl;
		return setVacuumPercentage(_cmd.getValue());
	}
	case 17: {//loop
		if (m_verbose) cout << currentDateTime()
			 << " fluicell::PPC1api::run(command _cmd) :::"
			 << " loop NOT implemented in the API "
			 << endl;
		
		return true;
	}
	default:{
		cerr << currentDateTime()
			 << " fluicell::PPC1api::run(command _cmd) :::" 
			 << " Command NOT recognized " 
			 << endl;
		return false;
	}
	}
	return false;
}

bool fluicell::PPC1api::setDataStreamPeriod(const int _value) {
	if (_value >=  MIN_STREAM_PERIOD && _value <=  MAX_STREAM_PERIOD )
	{
		m_dataStreamPeriod = _value;
		string ss;
		ss.append("u");
		ss.append(to_string(_value));
		ss.append("\n");
		if (sendData(ss)) return true;
	}
	else
	{
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::setDataStreamPeriod ::: out of range " 
			 << endl;
		sendData("u200\n");  // send default value
		return false;
	}
	return false;

}

bool fluicell::PPC1api::setDefaultPV(double _default_pon, double _default_poff, 
	double _default_v_recirc, double _default_v_switch)
{
	if (m_verbose) cout << currentDateTime()
		<< "fluicell::PPC1api::setDefaultPV"
		<< " _default_pon << " << _default_pon << " >> "
		<< " _default_poff << " << _default_poff << " >> "
		<< " _default_v_recirc << " << _default_v_recirc << " >> "
		<< " _default_v_switch << " << _default_v_switch << " >> " << endl;

if (_default_pon > MIN_CHAN_D && _default_pon < MAX_CHAN_D) {
		m_default_pon = _default_pon;
	}
	else {
		cerr << currentDateTime()
			<< " fluicell::PPC1api::setDefaultPV ::: default pon out of range = " 
			<< _default_pon << endl;
		return false;
	}
	if (_default_poff > MIN_CHAN_C && _default_poff < MAX_CHAN_C) {
		m_default_poff = _default_poff;
	}
	else {
		cerr << currentDateTime()
			 << " fluicell::PPC1api::setDefaultPV ::: default poff out of range = " 
			 << _default_poff << endl;
		return false;
	}
	if (_default_v_recirc > MIN_CHAN_A && _default_v_recirc < MAX_CHAN_A) {
		m_default_v_recirc = _default_v_recirc;
	}
	else {
		cerr << currentDateTime()
			 << " fluicell::PPC1api::setDefaultPV ::: default v recirc out of range = " 
			 << _default_v_recirc << endl;
		return false;
	}
	if (_default_v_switch > MIN_CHAN_B && _default_v_switch < MAX_CHAN_B) {
		m_default_v_switch = _default_v_switch;
	}
	else {
		cerr << currentDateTime()
			 << " fluicell::PPC1api::setDefaultPV ::: default v switch out of range = " 
			 << _default_v_switch << endl;
		return false;
	}

	return true;
}

string fluicell::PPC1api::getDeviceID()
{
	string serialNumber; //device serial number

	if (!m_PPC1_serial->isOpen()) {
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::getDeviceID ::: "
			 << " cannot return the device serial number, device not connected" 
			 << endl;
		return "";
	}

	// stop the stream to be able to get the value
	if (!setDataStreamPeriod(0))
	{
		cerr << currentDateTime()
			<< " fluicell::PPC1api::setDataStreamPeriod ::: "
			<< " cannot set the data stream period to 0"
			<< endl;
	}

	// send the character to get the device serial number
	sendData("#\n"); // this character is not properly sent maybe a serial lib bug?
	std::this_thread::sleep_for(std::chrono::microseconds(50000));

	readData(serialNumber);
	
	if(m_verbose)
		cout << " fluicell::PPC1api::getDeviceID :::  the serial number is : " 
			 << serialNumber << endl;
	
	std::this_thread::sleep_for(std::chrono::microseconds(50000));

	// restore the data stream to the default value
	setDataStreamPeriod(200);// (m_dataStreamPeriod);
	std::this_thread::sleep_for(std::chrono::microseconds(50000));

	if(m_verbose) 
		cout << " fluicell::PPC1api::getDeviceID :::  set data stream to : " 
			 << m_dataStreamPeriod << endl;

	return serialNumber;
}

void fluicell::PPC1api::setFilterEnabled(bool _enable)
{
	if (m_verbose) cout << currentDateTime()
		<< " fluicell::PPC1api::setFilterEnabled << " 
		<< _enable << " >> " << endl;
	m_filter_enabled = _enable;

	m_PPC1_data->enableFilter(_enable);
}

void fluicell::PPC1api::setFilterSize(int _size)
{
	if (m_verbose) cout << currentDateTime()
        << " fluicell::PPC1api::setFilterSize"
		<< " new filter size value << " << _size << " >> " << endl;

	if (_size < 1)
	{
		cerr << currentDateTime()
			<< " fluicell::PPC1api::setFilterSize  ::::  " 
			<< " negative value on set size << " << _size << " >> " << endl;
		return;
	}
	
	m_PPC1_data->setFilterSize(_size); 
}

bool fluicell::PPC1api::sendData(const string &_data) {
	//_data->append("\n");
	if (m_PPC1_serial->isOpen()) {
		if(m_verbose) 
			cout << currentDateTime() 
			     << " fluicell::PPC1api::sendData" 
			     << " sending the string << " 
			     << _data << " >> " << endl;

		if (m_PPC1_serial->write(_data) > 0) {
			return true;
		}
		else {
			return false;
		}
	}
	return false;
}

bool fluicell::PPC1api::readData(string &_data) {
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->flush();   // make sure that the buffer is clean
		if (m_PPC1_serial->readline(_data, 65536, "\n") > 0) {
			//decodeDataLine(data);
			return true;
		}
		else {
			cerr << currentDateTime() 
				 << " fluicell::PPC1api::readData ::: cannot read data --- readline " 
				 << endl;
			return false;
		}
	} // if the port is not open we cannot read data
	else {
		cerr << currentDateTime() 
			 << " fluicell::PPC1api::readData ::: cannot read data " 
			 << endl;
		return false;
	}

	cerr << currentDateTime() 
		 << " fluicell::PPC1api::readData ::: cannot read data end" 
		 << endl;
	return false; // if the port is open but the readline fails
}

bool fluicell::PPC1api::checkVIDPID(std::string _port)
{

	// try to get device information
	std::vector<serial::PortInfo> devices = serial::list_ports();
	std::vector<serialDeviceInfo> devs;
	for (unsigned int i = 0; i < devices.size(); i++) // for all the connected devices extract information
	{
		serialDeviceInfo dev;
		dev.port = devices.at(i).port;
		dev.description = devices.at(i).description;
		dev.VID = "N/A";
		dev.PID = "N/A";
		string hw_info = devices.at(i).hardware_id;
		string v = "VID";
		string p = "PID";
		// the fluicell PPC1 device expected string is USB\VID_16D0&PID_083A&REV_0200
		if (hw_info.length() < 1) {
			cerr << currentDateTime()
				 << " fluicell::PPC1api::checkVIDPID ::: "
				 << " hardware info string length not correct" << endl;
			return false; 
		}

		for (unsigned int j = 0; j < hw_info.size() - 2; j++)
		{
			// extract 3 characters looking for the strings VID or PID
			string s = hw_info.substr(j, 3);
			if (s.compare(v) == 0 && hw_info.size() >= j + 4) { 
				// extract the 4 characters after VID_
				string vid = hw_info.substr(j + 4, 4); 
				dev.VID = vid;
			}
			if (s.compare(p) == 0 && hw_info.size() >= j+4 ) {
				// extract the 4 characters after PID_
				string pid = hw_info.substr(j + 4, 4);
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

const std::string fluicell::PPC1api::currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
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
