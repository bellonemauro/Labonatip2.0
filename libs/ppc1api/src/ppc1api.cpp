/*  +---------------------------------------------------------------------------+
 *  |                                                                           |
 *  |  Fluicell AB - Lab-on-a-tip                                               |
 *  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
 *  |                                                                           |
 *  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
 *  | Released under GNU GPL License.                                           |
 *  +---------------------------------------------------------------------------+ */

#include "fluicell/ppc1api/PPC1api.h"


fluicell::PPC1api::PPC1api() :
	m_PPC1_data(new PPC1_data)
{
	// initialize and connect serial port objects
	m_PPC1_serial = new serial::Serial();

	// com port name
	m_COMport = "COM1";

	// baud rate
	m_baud_rate = 115200;

	// data stream period
	m_dataStreamPeriod = 200;

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
//			cout << " thread running " << endl;
			if(my_mutex.try_lock())
			{
				string data;
				if (readData(data))
					if (!decodeDataLine(data)) 
						cerr << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: corrupted data " << endl;
				
				my_mutex.unlock();
			}
			else {
				cerr << " fluicell::PPC1api::threadSerial  ----  error --- impossible to lock " << endl;
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
		cerr << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: IOException : " << e.what() << endl;
		return;
	}
	catch (serial::SerialException &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close(); 
		cerr << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: SerialException : " << e.what() << endl;
		return;
	}
	catch (exception &e) 	{
		m_isRunning = false; 
		m_threadTerminationHandler = true;
		m_PPC1_serial->close();
		cerr << " fluicell::PPC1api::threadSerial  ---- error --- MESSAGE: --exception" << e.what() << endl;
		return;
	}

	return;
}


bool fluicell::PPC1api::decodeDataLine(string &_data)
{
	// check for empty data
	if (_data.empty())
	{
		cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line - Empty line " << endl;
		return false;
	}

	vector<double> line;  // decoded line 

	if (_data.at(0) == 'A') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			m_PPC1_data->channel_A->set_point = line.at(0);
			m_PPC1_data->channel_A->sensor_reading = line.at(1);
			m_PPC1_data->channel_A->PID_out_DC = line.at(2);
			m_PPC1_data->channel_A->state = (int)line.at(3);
			return true;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " << endl;
			return false;
		}
	}

	if (_data.at(0) == 'B') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			m_PPC1_data->channel_B->set_point = line.at(0);
			m_PPC1_data->channel_B->sensor_reading = line.at(1);
			m_PPC1_data->channel_B->PID_out_DC = line.at(2);
			m_PPC1_data->channel_B->state = (int)line.at(3);
			return true;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " << endl;
			return false;
		}
	}

	if (_data.at(0) == 'C') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			m_PPC1_data->channel_C->set_point = line.at(0);
			m_PPC1_data->channel_C->sensor_reading = line.at(1);
			m_PPC1_data->channel_C->PID_out_DC = line.at(2);
			m_PPC1_data->channel_C->state = (int)line.at(3);
			return true;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " << endl;
			return false;
		}
	}

	if (_data.at(0) == 'D') {
		if (decodeChannelLine(_data, line))  // decode the line 
		{   // and fill the right place in the data structure
			m_PPC1_data->channel_D->set_point = line.at(0);
			m_PPC1_data->channel_D->sensor_reading = line.at(1);
			m_PPC1_data->channel_D->PID_out_DC = line.at(2);
			m_PPC1_data->channel_D->state = (int)line.at(3);
			return true;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line " << endl;
			return false;
		}
	}

	if (_data.at(0) == 'i') {
		//cout << " line tipe i " << _data << endl;
		// string format:  i0|j0|k0|l0
		// char index   :  0123456789
		int value = toDigit(_data.at(1));  
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			m_PPC1_data->i = value;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line m_PPC1_data->i string:" << _data << " value " << value << endl;
			return false;
		}

		value = toDigit(_data.at(4));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			m_PPC1_data->j = value;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line m_PPC1_data->j" << endl;
			return false;
		}

		value = toDigit(_data.at(7));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			m_PPC1_data->k = value;
		}
		else { 
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line m_PPC1_data->k" << endl;
			return false;
		}

		value = toDigit(_data.at(10));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			m_PPC1_data->l = value;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line m_PPC1_data->l" << endl;
			return false;
		}
		return true;
	}

	if (_data.at(0) == 'I') {
		//cout << " line tipe I " << _data << endl;
		// string format: IN1|OUT1
		// char index:    01234567
		int value = toDigit(_data.at(2));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			m_PPC1_data->ppc1_IN = value;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line m_PPC1_data->ppc1_IN" << endl;
			return false;
		}
		value = toDigit(_data.at(7));
		if (value == 0 || value == 1) { // admitted values are only 0 and 1
			m_PPC1_data->ppc1_OUT = value;
		}
		else {
			cerr << " fluicell::PPC1api::decodeDataLine ::: Error in decoding line m_PPC1_data->ppc1_OUT" << endl;
			return false;
		}
		return true;
	}

	return false;  // in case _data(0) is none of the expected value
}


bool fluicell::PPC1api::decodeChannelLine(string &_data, vector<double> &_line)
{
	// check for empty data
	if (_data.empty())
	{
		cerr << " fluicell::PPC1api::decodeChannelLine ::: Error in decoding line - Empty line " << endl;
		return false;
	}

	_line.clear();
	unsigned int byte_counter = 2;        // in the line 0 is letter and 1 is the separator e.g. A|
	const char separator[] = "|";       // separator between data
	while (byte_counter < _data.length())   // scan the whole string
	{
			string value;
			// extract line 
			while (_data.at(byte_counter) != *separator)    // extract the value before the character "new line"
			{
				value.push_back(_data.at(byte_counter));
				byte_counter++;
				if (byte_counter >= _data.length()) break;
			}
			byte_counter++;
			_line.push_back(stod(value));
	}

	// check for proper data size
	if (_line.size() < 3) {
		cerr << " fluicell::PPC1api::decodeChannelLine ::: Error in decoding line - corrupted data line " << endl;
		return false;
	}

	return true;
}


bool fluicell::PPC1api::connectCOM()
{
	try {
		m_PPC1_serial->setPort(m_COMport);
		m_PPC1_serial->setBaudrate(m_baud_rate);
		m_PPC1_serial->setFlowcontrol(serial::flowcontrol_none);
		m_PPC1_serial->setParity(serial::parity_none);
        //m_PPC1_serial->setTimeout(serial::Timeout::simpleTimeout(250));

		if (!checkVIDPID(m_COMport)) {
			cerr << " fluicell::PPC1api::connectCOM :::  no match VID/PID device "  << endl;
			return false;
		}

		//cout << "Is the port open?";
		if (m_PPC1_serial->isOpen())
			//cout << " Serial Port already open" << endl;
			std::this_thread::sleep_for(std::chrono::microseconds(100));  //--> do nothing, wait
		else {
			//cout << " Serial port not open, opening . . ." << endl;
			m_PPC1_serial->open();
            std::this_thread::sleep_for(std::chrono::microseconds(100));  
		}

		// if the first attempt to open the port fails then the connection fails
		if (!m_PPC1_serial->isOpen()) {

			cerr << " fluicell::PPC1api::connectCOM ::: FAILED - Serial port not open." << endl;
			return false;
		}
		else 
			return true; // open connection verified 
	}
	catch (serial::IOException &e)
	{
		cerr << " fluicell::PPC1api::connectCOM ::: IOException : " << e.what() << endl;
		m_PPC1_serial->close(); 
		return false;
	}

	catch (serial::SerialException &e)
	{
		cerr << " fluicell::PPC1api::connectCOM ::: SerialException : " << e.what() << endl;
		m_PPC1_serial->close(); 
		return false;
	}
	catch (exception &e) {
	cerr << " fluicell::PPC1api::connectCOM ::: Unhandled Exception: " << e.what() << endl;
	m_PPC1_serial->close(); 
	return false;
	}
}

void fluicell::PPC1api::disconnectCOM()
{
	if (m_PPC1_serial->isOpen())
		m_PPC1_serial->close();
}

void fluicell::PPC1api::pumpingOff()
{
	if (m_PPC1_serial->isOpen()) {
		setVacuumChannelA(0.0); // sendData("A0.0\n");
		setVacuumChannelB(0.0); //sendData("B0.0\n");
		setPressureChannelC(0.0); // sendData("C0.0\n");
		setPressureChannelD(0.0); //sendData("D0.0\n");
        setValve_a(false);
        setValve_b(false);
        setValve_c(false);
        setValve_d(false);
	}
}

void fluicell::PPC1api::closeAllValves()
{
	if (m_PPC1_serial->isOpen()) {
		setValve_a(false);
		setValve_b(false);
		setValve_c(false);
		setValve_d(false);
	}

}

void fluicell::PPC1api::reboot()
{
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->write("!\n");
	}
}

bool fluicell::PPC1api::setVacuumChannelA(double _value)
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
		cerr << " fluicell::PPC1api::setPressureChannelA ::: out of range " << endl;
		sendData("A0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setVacuumChannelB(double _value)
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
		cerr << " fluicell::PPC1api::setPressureChannelB ::: out of range " << endl;
		sendData("B0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setPressureChannelC(double _value)
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
		cerr << " fluicell::PPC1api::setPressureChannelC ::: out of range " << endl;
		sendData("C0.0\n");  // send 0
		return false;
	}
	return false;
}

bool fluicell::PPC1api::setPressureChannelD(double _value)
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
		cerr << " fluicell::PPC1api::setPressureChannelD ::: out of range " << endl;
		sendData("D0.0\n");  // send 0
		return false;
	}

	return false;
}

bool fluicell::PPC1api::setValve_a(bool _value)
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

bool fluicell::PPC1api::setValve_b(bool _value) {
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

bool fluicell::PPC1api::setValve_c(bool _value) {
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

bool fluicell::PPC1api::setValve_d(bool _value) {
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

bool fluicell::PPC1api::increaseDropletSize()
{
	double value = m_PPC1_data->channel_A->set_point;
	if (!setVacuumChannelA(value*0.975)) {// decrease A by 2.5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	value = m_PPC1_data->channel_D->set_point;
	if (!setPressureChannelD(value*1.025)) { // increase C by 2.5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	return true;
}

bool fluicell::PPC1api::decreaseDropletSize()
{
	double value = m_PPC1_data->channel_A->set_point;
	if (!setVacuumChannelA(value*1.025)) {// increase A by 2.5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	value = m_PPC1_data->channel_D->set_point;
	if (!setPressureChannelD(value*0.975)) { // reduce C by 2.5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec

	return true;
}

double fluicell::PPC1api::getDropletSizePercentage()
{
	double value_A = m_PPC1_data->channel_A->sensor_reading;
	double value_D = m_PPC1_data->channel_D->sensor_reading;

	double p1 = std::abs(100 * value_A / DEFAULT_VACUUM);
	double p2 = std::abs(100 * value_D / DEFAULT_PON);
	double mean_percentage = (p1 + p2) / 2;

	return mean_percentage;
}

bool fluicell::PPC1api::increaseFlowspeed()
{
	double value = m_PPC1_data->channel_A->set_point;
	if (!setVacuumChannelA(value*1.05)) { // increase A by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	value = m_PPC1_data->channel_B->set_point;
	if (!setVacuumChannelB(value*1.05)) { // increase B by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	value = m_PPC1_data->channel_C->set_point;
	if (!setPressureChannelC(value*1.05)) { // increase C by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	value = m_PPC1_data->channel_D->set_point;
	if (!setPressureChannelD(value*1.05)) { // increase D by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	return true;
}

bool fluicell::PPC1api::decreaseFlowspeed()
{
	double value = m_PPC1_data->channel_A->set_point;
	if (!setVacuumChannelA(value*0.95)) { // reduce A by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	value = m_PPC1_data->channel_B->set_point;
	if (!setVacuumChannelB(value*0.95)) { // reduce B by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	value = m_PPC1_data->channel_C->set_point;
	if (!setPressureChannelC(value*0.95)) { // reduce C by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	value = m_PPC1_data->channel_D->set_point;
	if (!setPressureChannelD(value*0.95)) { // increase D by 5%
		return false;
	}
	std::this_thread::sleep_for(std::chrono::microseconds(10000)); // wait 10msec
	
	return true;

}

double fluicell::PPC1api::getFlowSpeedPercentage()
{
	double value_A = m_PPC1_data->channel_A->sensor_reading;
	double value_B = m_PPC1_data->channel_B->sensor_reading;
	double value_C = m_PPC1_data->channel_C->sensor_reading;
	double value_D = m_PPC1_data->channel_D->sensor_reading;

	double p1 = std::abs(100 * value_A / DEFAULT_VACUUM);
	double p2 = std::abs(100 * value_B / DEFAULT_VACUUM);
	double p3 = std::abs(100 * value_C / DEFAULT_POFF);
	double p4 = std::abs(100 * value_D / DEFAULT_PON);
	double mean_percentage = (p1 + p2 + p3 + p4) / 4;
	return mean_percentage;
}

bool fluicell::PPC1api::increaseVacuum5p()
{
	double value = m_PPC1_data->channel_A->set_point;
	return setVacuumChannelA(value*1.05); // 1.05 is the 5% increment
}

bool fluicell::PPC1api::decreaseVacuum5p()
{
	double value = m_PPC1_data->channel_A->set_point;
	return setVacuumChannelA(value*0.95); // 0.95 is the 5% decrement
}

double fluicell::PPC1api::getVacuumPercentage()
{
	double value_A = m_PPC1_data->channel_A->sensor_reading;
	double p1 = std::abs(100 * value_A / DEFAULT_VACUUM);

	return p1;
}

bool fluicell::PPC1api::setDataStreamPeriod(int _value) {
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
		cerr << " fluicell::PPC1api::setDataStreamPeriod ::: out of range " << endl;
		sendData("u200\n");  // send default value
		return false;
	}
	return false;

}

string fluicell::PPC1api::getDeviceID()
{
	string serialNumber; //device serial number

	if (!m_PPC1_serial->isOpen()) {
		cerr << " fluicell::PPC1api::getDeviceID ::: cannot return the device serial number, device not connected" << endl;
		return "";
	}

	// stop the stream to be able to get the value
	setDataStreamPeriod(0);

	// send the character to get the device serial number
	sendData("#\n");
	std::this_thread::sleep_for(std::chrono::microseconds(50000));

	readData(serialNumber);
	//cout << " fluicell::PPC1api::getDeviceID :::  the serial number is : " << serialNumber << endl;
	std::this_thread::sleep_for(std::chrono::microseconds(50000));

	// restore the data stream to the default value
	setDataStreamPeriod(200);// (m_dataStreamPeriod);
	std::this_thread::sleep_for(std::chrono::microseconds(50000));
	//cout << " fluicell::PPC1api::getDeviceID :::  set data stream to : " << m_dataStreamPeriod << endl;

	return serialNumber;
}

bool fluicell::PPC1api::sendData(const string &_data) {
	//_data->append("\n");
    if (m_PPC1_serial->write(_data) > 0) {
		return true;
	}
    else {
		return false;
	}
}

bool fluicell::PPC1api::readData(string &_data) {
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->flush();   // make sure that the buffer is clean
		if (m_PPC1_serial->readline(_data, 65536, "\n") > 0) {
			//decodeDataLine(data);
			return true;
		}
		else {
			cerr << " fluicell::PPC1api::readData ::: cannot read data --- readline " << endl;
			return false;
		}
	} // if the port is not open we cannot read data
	else {
		cerr << " fluicell::PPC1api::readData ::: cannot read data " << endl;
		return false;
	}

	cerr << " fluicell::PPC1api::readData ::: cannot read data end" << endl;
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
		for (unsigned int j = 0; j < hw_info.size() - 2; j++)
		{
			// extract 3 characters looking for the strings VID or PID
			string s = hw_info.substr(j, 3);
			if (s.compare(v) == 0) {
				// extract the 4 characters after VID_
				string vid = hw_info.substr(j + 4, 4); //TODO: no check for the string length
				dev.VID = vid;
			}
			if (s.compare(p) == 0) {
				// extract the 4 characters after PID_
				string pid = hw_info.substr(j + 4, 4); //TODO: no check for the string length
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

fluicell::PPC1api::~PPC1api()
{
	// make sure the thread and the communications are properly closed
	if (!m_threadTerminationHandler) {
		this->stop();
	}
	if (m_PPC1_serial->isOpen()) {
		m_PPC1_serial->close();
	}
}