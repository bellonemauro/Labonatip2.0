/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */


#include <iostream>
#include <ctime>
#include <ostream>

// PPC1api test
#include <fluicell/ppc1api/ppc1api.h>

// declare some useful variables
static std::string COMport;
unsigned long baudRate;
bool threadStopped;
serial::Serial my_serial;


void print_usage()
{
	cout << "Usage: PPC1api_test_cmdline {-e|<serial port number>} " << endl;
	cout << "<baudrate> [test string]" << endl;
	cout << " example : PPC1api_test_cmdline.exe COM5 115200 " << endl;
	cout << "           PPC1api_test_cmdline.exe -e        //--> to enumerate" << endl;
	// baudrate enum
	// Baud4800 4,800 baud. 
	// Baud9600 9,600 baud. 
	// Baud19200 19,200 baud. 
	// Baud38400 38,400 baud. 
	// Baud57600 57,600 baud. 
	// Baud115200 115,200 baud. 
	// Baud230400 230,400 baud. 

}

void enumerate_ports()
{
	vector<serial::PortInfo> devices_found = serial::list_ports();
	vector<serial::PortInfo>::iterator iter = devices_found.begin();

	while (iter != devices_found.end())
	{
		serial::PortInfo device = *iter++;
		printf("(%s, %s, %s)\n", device.port.c_str(), device.description.c_str(),
			device.hardware_id.c_str());
	}
}


int	main (int argc, char** argv)
{
	cout << "\n\n"
		<< " Fluicell Framework - demo for the PPC1 serial communication \n"
		<< " the test will read 1000 lines from the PPC1 and visualize them \n as strings in the command line "
		<< " \n "
		<< " Authors -  Mauro Bellone\n\n\n" << endl;

	fluicell::PPC1api *my_ppc1 = new fluicell::PPC1api();

    try {
		// analyse parsed arguments 
		if (argc < 2) {
			print_usage();
			return 0;
		}

	std::cout<<"\n >>>  PCC1api_test_cmdline 2017  <<< \n\n"<<std::endl;

	// Argument 1 is the serial port or enumerate flag
	COMport = argv[1];

	if (COMport == "-e") {
		enumerate_ports();
		return 0;
	}
	else if (argc < 3) {
		print_usage();
		return 1;
	}

	// Argument 2 is the baudrate
	baudRate = 0;
    //sscanf_s(argv[2], "%lu", &baudRate);
    sscanf(argv[2], "%lu", &baudRate);

    {
		my_ppc1->setCOMport(COMport);
		my_ppc1->setBaudRate(baudRate);
		my_ppc1->setVerbose(true);
		if (!my_ppc1->connectCOM()) {

			cout << " cannot connect to Fluicell PPC1 -- press enter to exit " << endl;
			cin.get();
			return 0;
		}

		my_ppc1->run();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		
		if (my_ppc1->isRunning()) {
			cout << " PPC1 running ready for test \n press enter to continue " << endl;
		}
		else
		{
			cout << " PPC1 not running reboot the device and try again \n press enter to exit " << endl;
			return 0;
		}
		cin.get();

		// for some reason everything works but getDeviceID()
		cout << " Getting device id ... " <<  endl;
		string id = my_ppc1->getDeviceID();
		cout << " device id is " << id << "\n\n press enter to continue " << endl;
		

		// try to just read from the data in the threaded class
		int count = 0;
		while (count < 20) {
			
//			cout << " data on channel A " << my_ppc1->m_PPC1_data->channel_A->sensor_reading << endl;
//			cout << " data on channel B " << my_ppc1->m_PPC1_data->channel_B->sensor_reading << endl;
//			cout << " data on channel C " << my_ppc1->m_PPC1_data->channel_C->sensor_reading << endl;
//			cout << " data on channel D " << my_ppc1->m_PPC1_data->channel_D->sensor_reading << endl;
			count++;
			
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			if (my_ppc1->isExceptionHappened()) {
				cout << " Exception has been caught, implement safe exit" << endl;
				cin.get();
				break;
			}
		}

		// try to send a command and read the result : channel A
        std::this_thread::sleep_for(std::chrono::microseconds(2000000));

		for (int i = 0; i < 200; i++)
		{
//			cout << " data on channel A : set point = " << my_ppc1->m_PPC1_data->channel_A->set_point 
//				 << " sensor reading = " <<  my_ppc1->m_PPC1_data->channel_A->sensor_reading << endl;
			my_ppc1->setVacuumChannelA(-(float)i);
		}
		my_ppc1->setVacuumChannelA(-100.0);
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));

		// try to send a command and read the result : channel B
		for (int i = 0; i < 100; i++)
		{
//			cout << " data on channel B : set point = " << my_ppc1->m_PPC1_data->channel_B->set_point
//				<< " sensor reading = " << my_ppc1->m_PPC1_data->channel_B->sensor_reading << endl;
			my_ppc1->setVacuumChannelB(-(float)i / 10);
		}
		my_ppc1->setVacuumChannelB(-0.0);
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));

		// try to send a command and read the result : channel C
		for (int i = 0; i < 100; i++)
		{
//			cout << " data on channel C : set point = " << my_ppc1->m_PPC1_data->channel_C->set_point
//				<< " sensor reading = " << my_ppc1->m_PPC1_data->channel_C->sensor_reading << endl;
			my_ppc1->setPressureChannelC((float)i / 10);
		}
		my_ppc1->setPressureChannelC(0.0);
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));

		// try to send a command and read the result : channel C
		for (int i = 0; i < 100; i++)
		{
//			cout << " data on channel D : set point = " << my_ppc1->m_PPC1_data->channel_D->set_point
//				<< " sensor reading = " << my_ppc1->m_PPC1_data->channel_D->sensor_reading << endl;
			my_ppc1->setPressureChannelD((float)i / 10);
		}
		my_ppc1->setPressureChannelD(0.0);
        std::this_thread::sleep_for(std::chrono::microseconds(1000000));
		
		my_ppc1->setPressureChannelD(200);

		cout << "opening l " << endl;
		my_ppc1->setValve_l(true);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));
		cout << "closing l " << endl;
		my_ppc1->setValve_l(false);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));

		cout << "opening k " << endl;
		my_ppc1->setValve_k(true);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));
		cout << "closing k " << endl;
		my_ppc1->setValve_k(false);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));
		
		
		cout << "opening j " << endl;
		my_ppc1->setValve_j(true);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));
		cout << "closing j " << endl;
		my_ppc1->setValve_j(false);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));

		cout << "opening i " << endl;
		my_ppc1->setValve_i(true);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));
		cout << "closing i " << endl;
		my_ppc1->setValve_i(false);
		std::this_thread::sleep_for(std::chrono::microseconds(10000000));

		my_ppc1->pumpingOff();
		std::this_thread::sleep_for(std::chrono::microseconds(1000000));

		my_ppc1->stop();
        std::this_thread::sleep_for(std::chrono::microseconds(100000));
		my_ppc1->disconnectCOM();

		std::cout << "\n >>> TEST finished, press ok to exit  <<< \n\n" << std::endl;
		cin.get();
	}
	
	}
	catch ( std::exception &e)
	{
		cerr << "ciaoooooooooooosdasd : " << e.what() << endl;
		cin.get();
	}
	catch ( ... ) {
		cerr << "Unhandled Exception: " <<  endl;
		cin.get();
	}

	std::cout << "\n >>> ciao  <<< \n\n" <<  std::endl;

	return 0;
}

