/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB - Lab-on-a-tip                                               |
*  |  Copyright 2017 © Fluicell AB, http://fluicell.com/                       |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#pragma once

#include "export.h"

// OS Specific 
#if defined (_WIN64) || defined (_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
    #endif

// thread and mutex generate warning C4251 - dll interface needed
// however the objects are private so I will never export this data, warning can be ignored
// see http://www.unknownroad.com/rtfm/VisualStudio/warningC4251.html
	#pragma warning( disable: 4251 )
    #ifndef _EXPORT_H
    #define _EXPORT_H
    #endif _EXPORT_H
    //#include <windows.h>  // OS specific Sleep
#else
	#include <unistd.h>
#endif

// standard libraries 
#include <iostream>
#include <string>
#include <cstdio>
#include <cmath>
#include <thread>
#include <mutex>

#include <serial/serial.h>

using namespace std;

/**  \brief All fluicell software will begin with fluicell namespace
  *  
  **/
namespace fluicell
{
	
	/**  \brief Simple API for the communication with the Fluicell PPC1 controller
	*
	*  The PPC1 controller is ...
	*
	*
	*
	*
	*  IMPORTANT: for the definition of all the constants, parameters and bit-conversion
	*             please refer to the documents:
	*                    CommandsetManualForFluicell.pdf
	*                    FluicellDeviceManager.pdf
	*
	*  Default parameters: Serial port baud rate 115200, 8 data bits, 1 stop bit, no parity
    *  NB! All commands must end with a newline character \n
	*
	*  Pressure setting commands. Argument %f - pressure in mbar as floating point number. Vacuum channels expects the
    *  argument with minus sign. \n
    *    - "A%f\n" set vacuum on channel A
    *    - "B%f\n" set vacuum on channel B
    *    - "C%f\n" set pressure on channel C
    *    - "D%f\n" set pressure on channel D
    *  Example: Vacuum on channel A "A-223.4\n" or pressure on C "C402.7\n"
	*
	*
	*  Valve opening or closing commands. Argument "%u" means 1 closed, 0 opened \n
    *    - "l%u\n" set valve state on channel A (Do not use in closed loop (automatic) mode)
    *    - "k%u\n" set valve state on channel B (Do not use in closed loop (automatic) mode)
    *    - "j%u\n" set valve state on channel C (Do not use in closed loop (automatic) mode)
    *    - "i%u\n" set valve state on channel D (Do not use in closed loop (automatic) mode)
	*
	*
	*  "u%u\n" sets serial data stream period. Argument "%u" is unsigned integer marking the time period in ms. Zero turns the data
    *  stream off. (This is useful in order to catch the output of ? # or ~ commands in the terminal window) Default data stream
    *  period is 100ms, and the minimum is 25ms.\n
    *  Example "u200\n" sets the data stream period to 200ms
	*
	*
	*  "o%u\n" sets the TTL output state where %u is either 1 or 0 (high or low)
	*
    *  Besides setting the state of the output with "o1\n" or "o0\n" one can generate pulses to the TTL output.
	* 
	*  "p%u\n" generates a pulse with a length of %u milliseconds. For example p30\n generates a 30 milliseconds long pulse. The
    *  initial state of the output is dependent of the previous "o%u" commands (default low). If the state of output before the
    *  pulse command is high, the pulse command puts the output low for the specified amount of time. The minimum pulse
    *  length is 20 milliseconds. \n
    *  "*\n" Gets the temperature of the micro-controller in degrees of Celsius. Not very accurate.
    *  "!\n" Reboots the device
	*
	*
	*
	*  <b>Usage:</b><br>
	*		- 	define the object :      fluicell::PPC1api *my_ppc1 = new fluicell::PPC1api();
	*	    -   connect the device :     my_ppc1->connectCOM();
	*	    -   run the thread    :      my_ppc1->run();
	*       -   to stop the thread :     my_ppc1->stop();
	*	    -   disconnect the device :  my_ppc1->disconnectCOM();
	*
	*
	*  <b>About the algorithm:</b><br>
	*		- Write me
	*
	*
	* <b>Changes history</b>
	*		- MAY/2017: Creation (MB).
	*  \ingroup __
	*/
	class PPC1API_EXPORT PPC1api
	{

		// define class constants for ranges in vacuum and pressures
	#define MIN_CHAN_A -350.0 //!< in mbar
	#define MAX_CHAN_A -0.0 //!< in mbar
	#define MIN_CHAN_B -350.0 //!< in mbar
	#define MAX_CHAN_B -0.0 //!< in mbar
	#define MIN_CHAN_C 0.0 //!< in mbar
	#define MAX_CHAN_C 500.0 //!< in mbar
	#define MIN_CHAN_D 0.0 //!< in mbar
	#define MAX_CHAN_D 500.0 //!< in mbar
	#define MIN_STREAM_PERIOD 0 //!< in msec
	#define MAX_STREAM_PERIOD 500 //!< in msec
	
	// this values are the constants to have 100% droplet size 
	#define DEFAULT_PON 190 //!< in mbar
	#define DEFAULT_POFF 21 //!< in mbar
	#define DEFAULT_VACUUM 115 //!< in mbar (negative value!)

	#define PPC1_VID "16D0"  //!< device vendor ID
	#define PPC1_PID "083A"  //!< device product ID


	/**  PCC1 Output data stream specification
	*
	*    A|-0.000000|0.114514|0.000000|0
	*    B|-0.000000|0.034291|0.000000|0
	*    C|0.000000|-0.103121|0.000000|0
	*    D|0.000000|0.028670|0.000000|0
	*    i0|j0|k0|l0
	*    IN1|OUT1
	*
	*    One stream packet contains minimum 6 lines, each ending with newline character \n. First 4 lines correspond to pressure
	*    and vacuum info, the 5. line shows the valve states and the last line TTL input and output states. All data fields are
	*    separated using | sign.
	*
	*    First 4 line contain following fields:
	*          Channel character | setpoint | sensor reading | PID output duty cycle | state \n
	*    This structure is implemented in the ChannelN data structures
	*
	*
	*    Channel character: A and B are vacuums, C and D pressures
	*    Set point is the closed loop PID controller input value (in mbar)
	*    sensor reading shows the actual current pressure value in mbar
	*    PID output duty cycle is the output value of closed loop PID controller. Values 0 to 50000 mark the on time period in
	*    microseconds. PWM frequency is 20Hz
	*    state shows error flags. 0 means no errors, 1 is set point timeout error, which occurs when the channel has not reached in
	*    the range of +-5mbar of the set point within 30 seconds. In this case the output and set point will be set to 0. The error flag
	*    clears when a new set point is set.
	*
	*
	*    5. line:
	*       i%u|j%u|k%u|l%u\n where the characters i, j, k and l mark the output channels 8, 7, 6, 5 respectively and %u is 1 when the
	*       output channel is connected to pressure channel D and 0 when channel C.
	*
	*
	*    6. line:
	*       INx|OUTy\n where x and y are either 0 or 1 and show the input and output states.
	*
	*
	*
	*  Current interpreted state:
	*
	*    Pon: set 123mbar, read: 123.456789 mbar
	*    Poff: set 123mbar, read: 123.456789 mbar
	*    Vswitch: set 123mbar, read: 123.456789 mbar
	*    Vrecirc: set 123mbar, read: 123.456789 mbar
	*    Valves: 0101
	*
	* \note
	*/
		struct PPC1API_EXPORT PPC1_data
		{
		public:
			/**  \brief Channel data structure contains the information about a PPC1 channel
			*         PPC1 has four channels, A and B for vacuum and C and P for pressure,
			*         all the channels have the same data structure.
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
			struct PPC1API_EXPORT channel
			{
			public:
				double set_point;
				double sensor_reading;
				double PID_out_DC;
				int state;

				channel() : set_point(0.0), sensor_reading(0.0), PID_out_DC(0.0), state(0) {}
			};
			channel *channel_A;  //!< vacuum channel A
			channel *channel_B;  //!< vacuum channel B
			channel *channel_C;  //!< pressure channel C
			channel *channel_D;  //!< pressure channel D

			/*	i%u | j%u | k%u | l%u\n where the characters i, j, k and l mark the output channels 8, 7, 6, 5 respectively and %u is 1 when the
			*		output channel is connected to pressure channel D and 0 when channel C.
			*/
			int i;  //!< 8
			int j;  //!< 7
			int k;  //!< 6
			int l;  //!< 5

			int ppc1_IN;  //!< INx where x and y are either 0 or 1 and show the input state.
			int ppc1_OUT; //!< OUTy where x and y are either 0 or 1 and show the output state


			/**  \brief Constructor for PPC1 data to group all information
			*
			**/
			PPC1_data() :
				channel_A(new channel),
				channel_B(new channel),
				channel_C(new channel),
				channel_D(new channel),
				i(0), j(0), k(0), l(0),
				ppc1_IN(0), ppc1_OUT(0)
			{ }

		};

public:
	/**  \brief Serial device info data structure
		*
		*  @param port is the port name
		*  @param description is the hardware description
		*  @param hardware_ID contains hardware information
		*  @param VID is the vendor ID
		*  @param PID is the product ID
		*
		*
		*  \note The structure is usefull to check if VID/PID match the fluicell PPC1 device
		**/
		struct PPC1API_EXPORT serialDeviceInfo
		{
		public:
			string port;
			string description;
			string hardware_ID;
			string VID;
			string PID;
		
			serialDeviceInfo() {}
		};

public:
		/**  \ brief PCC1 Command --- 
		*
		*
		*    Contains the information to run a macro command
		*
		*
		*
		*
		* \note
		*/
		struct PPC1API_EXPORT command
		{
		public:
			/**  \brief Command data structure .
			*
			*  @param loops
			*
			**/
			int loops;              // number of loops
			int P_on;               //(int: pressure in mbar) 
			int P_off;              //(int: pressure in mbar)
			int V_switch;           //(int: pressure in mbar) 
			int V_recirc;           //(int: pressure in mbar) 
			int Duration;           //duration for the application of the command 
			bool ask;               //set true to stop execution and ask confirmation to continue
			string ask_message;     // message to ask if @\param(ask)- is true
			string status_message;  // message to show as status during the command running
			bool open_valve_a;      // closes other valves, then opens valve a for solution 1 valve only
			bool open_valve_b;      // closes other valves, then opens valve b for solution 2 valve
			bool open_valve_c;      // closes other valves, then opens valve c for solution 3 valve
			bool open_valve_d;      // closes other valves, then opens valve d for solution 4 valve
			bool wait_sync;         // macro stops until trigger signal is received
			int sync_out;           // if negative then default state is 1 and pulse is 0, 
									// if positive, then pulse is 1 and default is 0

			command() :
				loops(1),
				P_on(190), P_off(21), V_switch(-115), V_recirc(-115), Duration(1),
				open_valve_a(false), open_valve_b(false),
				open_valve_c(false), open_valve_d(false),
				ask(false), ask_message(""), wait_sync(false), sync_out(false)
			{	}

		};

	public:
	
		/** \brief Initialize objects and parameters using default values
		*        
		*/
		PPC1api();

		/** \brief Make sure the thread and the serial communication 
		  *        are properly closed
		  */
		~PPC1api();

	private:

		/**  \brief Decode data line function
		  *
		  *   This function decode every line out from the PPC1 device and fill
		  *   the data structure m_PPC1_data with the current values.
		  *   Values are overridden every iteration. 
		  *
		  *  
		  *  string data format:
		  *
		  *    A|-0.000000|0.114514|0.000000|0\n
		  *    B|-0.000000|0.034291|0.000000|0\n
		  *    C|0.000000|-0.103121|0.000000|0\n
		  *    D|0.000000|0.028670|0.000000|0\n
		  *    i0|j0|k0|l0\n
		  *    IN1|OUT1\n
		  *
		  *  Current interpreted state:
		  *
		  *    Pon: set 123mbar, read: 123.456789 mbar\n
		  *    Poff: set 123mbar, read: 123.456789 mbar\n
		  *    Vswitch: set 123mbar, read: 123.456789 mbar\n
		  *    Vrecirc: set 123mbar, read: 123.456789 mbar\n
		  *    Valves: 0101\n
		  *
		  * \return true if success, false for any error
		  *
		  * \note
		  */
		bool decodeDataLine(string &_data);

		/**  \brief Decode one channel line
		*
		*    the channel line contains the following fields:
		*          Channel character | setpoint | sensor reading | PID output duty cycle | state \n
		*    example of input data : B|-0.000000|0.034291|0.000000|0\n
		*
		*  @param _data  input data to be decoded
		*  @param _line  output line to be filled with decoded values
		*
		* \return true if success, false for any error
		*
		* \note
		*/
		bool decodeChannelLine(string &_data, vector<double> &_line);

		/** \brief Threaded routine that stream data from PPC1
		*
		* \note return an exception for any serial error
		*/
		void threadSerial();


		/** \brief Send a string to the PPC1 controller
		  *
		  * \note: does not return exceptions, but it print errors in the serial port fail
		  *
		  */
		bool sendData(const string &_data);

		/** Read data from serial port
		  *
		  * \return false for any error
		  *
		  * \note this function read one line until the new line \n
		  */
		bool readData(string &_data);

		/**  \brief Convert char to digit 
		*
		*  Internal function that makes sure the char is properly interpreted as a digit
		*
		* \return value
		*
		* \note (int)char return the integer ASCII code for char so sub '0' is required
		*/
		int toDigit(char _c) {return _c - '0';}// '0' = 48 in ASCII 


		/** Check if the connection on _port is going to the PPC1 or any other device 
		*
		* \return true if PPC1 VID/PID definitions match the VID/PID 
		*         of the device connected on _port
		*/
		bool checkVIDPID(std::string _port);

		// Serial port configuration parameters, only serial port number 
		// and baud rate are configurable for the user, this is intentional!
		serial::Serial *m_PPC1_serial;  //!< Pointer to serial port communication class
		//std::vector<serialDeviceInfo> m_devices;   //!< infos of connected devices
		string m_COMport;   //!< port number
		int m_baud_rate;    //!< baud rate
		int m_dataStreamPeriod; //!< data stream for the PPC1
		//string m_serialNumber;  //!< device serial number --- NOT USED 

		thread m_thread; //!< Member for the thread handling
		
		bool m_threadTerminationHandler;  //!< set to TRUE to stop the thread, automatically set to false during the thread starting
		bool m_isRunning; //!< true when the thread is running

	public:

		/**  \brief Connect to serial port
		  *
		  *  \return true if success, false for any error, a message is printed in case of fault
		  *
		  **/
		bool connectCOM();

		/**  \brief Connect to serial port overload to set the serial port where to connect
		*
		*  @param comPort;
		*
		*  \return true if success, false for any error, a message is printed in case of fault
		*
		**/
		bool connectCOM(string _COMport) {
			setCOMport(_COMport);
			return connectCOM();
		}

		/**  \brief Connect to serial port overload to set the serial port where to connect and the baudrate
		*
		*  @param comPort;
		*  @param baudRate;
		*
		*  \return true if success, false for any error, a message is printed in case of fault
		*
		**/
		bool connectCOM(string _COMport, int _baudRate) {
			setCOMport(_COMport);
			setBaudRate(_baudRate);
			return connectCOM();
		}

		/**  \brief Disconnect serial port
		  *
		  *  Forces to disconnect hardware, it does not connect again until
		  *  connectCOM method is called or manager is restarted
		  **/
		void disconnectCOM();

		/**  \brief Put all the set points to zero and close the valves,
		  *         the device is now in a sleep mode
		  *
		  **/
		void pumpingOff();

		/**  \brief Close all the valves i to l
		*         
		*     It recalls the setValvesState with the message 0xFF
		**/
		void closeAllValves();

		/**  \brief Send a reboot character to the device
		  *
		  *  \note - Known issue: weird behaviour in disconnect/connect
		  **/
		void reboot();

		/** \brief Run the thread
		  *
		  *  During the thread the PPC1 stream data on the open serial port and
		  *  listen messages on the same port
		  *
		  *
		  **/
		virtual void run() {
			m_threadTerminationHandler = false; //TODO: too weak, add checking if running and initialized
			m_thread = thread(&PPC1api::threadSerial, this);
			// run_thread.join();
		}

		/**  \brief Safe stop the thread
		  *
		  **/
		void stop() {
			pumpingOff();  // as we end the thread is good to put the PPC1 to zero
			m_thread.detach();
			m_thread.~thread();
			m_threadTerminationHandler = true;
			std::this_thread::sleep_for(std::chrono::microseconds(1000));  //--> wait the last execution
		}

		/** \brief Set a value on the vacuum channel A, admitted values are [-350.0, 0.0] in mbar
		  *
		  *
		  *  Send the string A%f\n to set vacuum on channel A to activate vacuum at a specific value
		  *
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setVacuumChannelA(double _value = -0.0);

		/** \brief Set a value on the vacuum channel B, admitted values are [-350.0, 0.0] in mbar
		  *
		  *
		  *  Send the string B%f\n to set vacuum on channel B to activate vacuum at a specific value
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setVacuumChannelB(double _value = -0.0);

		/** \brief Set a value on the pressure channel C, admitted values are [0.0, 500.0] in mbar
		  *
		  *
		  *  Send the string C%f\n to set pressure on channel C
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setPressureChannelC(double _value = 0.0);

		/** \brief Set a value on the pressure channel D, admitted values are [0.0, 500.0] in mbar
		  *
		  *
		  *  Send the string D%f\n to set pressure on channel D to pressure at a specific value
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setPressureChannelD(double _value = 0.0);

		/** \brief Set the valve "l" value to true/false to Open/Close
		  *
		  *  Send the string c%u\n to set valve state on valve a
		  *  (Do not use in closed loop (automatic) mode)
		  *
		  *  @param  _value true ==>   Open ==> %u=0
		  *          _value false ==>  Closed ==> %u=1
		  *
		  *  \note -  false by default
		  **/
		bool setValve_l(bool _value = false);

		/** \brief Set the valve "k" value to true/false to Open/Close
		  *
		  *  Send the string c%u\n to set valve state on valve b
		  *  (Do not use in closed loop (automatic) mode)
		  *
		  *  @param  _value true ==>   Open ==> %u=0
		  *          _value false ==>  Closed ==> %u=1
		  *
		  *  \note -  false by default
		  **/
		bool setValve_k(bool _value = false);

		/** \brief Set the valve "j" value to true/false to Open/Close
		  *
		  *  Send the string c%u\n to set valve state on valve c
		  *  (Do not use in closed loop (automatic) mode)
		  *
		  *  @param  _value true ==>   Open ==> %u=0
		  *          _value false ==>  Closed ==> %u=1
		  *
		  *  \note -  false by default
		  **/
		bool setValve_j(bool _value = false);

		/** \brief Set the valve "i" value to true/false to Open/Close
		  *
		  *  Send the string c%u\n to set valve state on valve d
		  *  (Do not use in closed loop (automatic) mode)
		  *
		  *  @param  _value true ==>   Open ==> %u=0
		  *          _value false ==>  Closed ==> %u=1
		  *
		  *  \note -  false by default
		  **/
		bool setValve_i(bool _value = false);
		
		/** \brief Set all the valves state in one command using a binary number where 1/0 are Open/Close
		*
		*  Send the string v%X\n set valve e...l states as one command. 
		*  %X is unsigned hexadecimal byte where each bit represents a valve state from MSB=e to LSB=l.
		*
		*  Example 	"vff\n"    closes all valves (0xff = 0b11111111)
		*           "v0\n"     opens all valves (0x00 = 0b00000000)
		*           "v0f\n"    opens e to h and closes i to l (0x0f = 0b00001111)
		*           "v8a\n"    closes valves e i and k, opens all others (0x8a = 0b10001010)
		*
		*  @param  _value is an hex number 
		*
		*  \note -  valves e to h are not supported by the PPC1 
		**/
		bool setValvesState(int _value = 0x00);

		/** \brief Increase the droplet size 
		*
		*  A bit more advanced control can increase the droplet size by 2.5%
		*  
		*
		*  \return -  false in case of errors
		**/
		bool increaseDropletSize();

		/** \brief Reduce the droplet size
		*
		*  A bit more advanced control can reduce the droplet size by 2.5%
		*
		*
		*  \return -  false in case of errors
		**/
		bool decreaseDropletSize();
			
		/** \brief Get the current droplet size in percentage
		*
		* \note: the calculation is based on the the actual sensor readings
		*
		*  \return -  value as the average of the percentage with respect to the default values
		**/
		double getDropletSizePercentage();


		/** \brief Increase the flow speed by 5%
		*
		*  A bit more advanced control can increase the flow speed by 5%
		*  by increasing all the set points (channels A, B, C and D) by 5%
		*
		* \note: new_set_point = old_set_point + 5%
		*        the calculation is based on the set point and not the actual reading
		*
		*
		*  \return -  false in case of errors
		**/
		bool increaseFlowspeed();

		/** \brief Reduce the flow speed by 5%
		*
		*  A bit more advanced control can reduce the flow speed by 5%
		*  by reducing all the set points (channels A, B, C and D) by 5%
		*
		* \note: new_set_point = old_set_point - 5% 
 		*        the calculation is based on the set point and not the actual reading
		*
		*  \return -  false in case of errors
		**/
		bool decreaseFlowspeed();

		/** \brief Get the current flow speed in percentage
		*
		* \note: the calculation is based on the the actual sensor readings
		*
		*  \return -  value = average percentage among all the channels
		**/
		double getFlowSpeedPercentage();
		
		/** \brief Increase the vacuum by 5%
		*
		*  A bit more advanced control can increase the vacuum by 5%
		*  by increasing the value by 5% on channel A
		*  \TODO: verify channel A represents v_recirculation
		*
		* \note: new_set_point = old_set_point + 5%
		*        the calculation is based on the set point and not the actual reading
		*
		*  \return -  false in case of errors
		**/
		bool increaseVacuum5p();

		/** \brief Reduce the vacuum by 5%
		*
		*  A bit more advanced control can reduce the vacuum by 5%
		*  by decreasing the value by 5% on channel A
		*  \TODO: verify channel A represents v_recirculation
		*
		* \note: new_set_point = old_set_point - 5%
		*        the calculation is based on the set point and not the actual reading
		*
		*
		*  \return -  false in case of errors
		**/
		bool decreaseVacuum5p();

		/** \brief Get the current vacuum value in percentage
		*
		* \note: the calculation is based on the the actual sensor reading
		*
		*  \return -  value = 100 * channel_A->sensor_reading / DEFAULT_VACUUM
		**/
		double getVacuumPercentage();

		/**  \brief Set the data stream period on the serial port
		  *
		  *  Send the string u%u\n to set the data stream period
		  *
		  *  u%u\n sets serial data stream period. Argument %u is unsigned integer marking the time period in ms. Zero turns the data
		  *  stream off. (This is useful in order to catch the output of ? # or ~ commands in the terminal window) Default data stream
		  *  period is 100ms, and the minimum is 25ms.
		  *  Example "u200\n" sets the data stream period to 200ms
		  *
		  *  \return  true if success, false for any error
		  *
		  **/
		bool setDataStreamPeriod(int _value = 200); //in msec

		/**  Allow to set the COM port name
		*  \param  _COMport COM1, COM2, COM_n
		*
		*  \note -  there is no actual check on the string, TODO too weak implement!
		**/
		void setCOMport(string _COMport = "COM1") { m_COMport = _COMport; }

		/**  Allow to set the baudRate for the specified com port
		*  \param  _baud_rate 9600 19200 115200
		*
		*  \note -  there is no actual check on the number, TODO too weak implement!
		**/
		void setBaudRate(int _baud_rate = 115200) { m_baud_rate = _baud_rate; }

		/**  \brief Get device serial number
		  *
		  * \note: from the old device api this description refers to channel, not clear:
		  * Get internal device ID. These are 6-digit serial numbers of Fluika
		  * modules in version 1 hardware and consecutive numbers in same
		  * format for version 2 hardware (e.g. “000001”). Input is channel number
		  * in range 1-6, where: 1-on pressure, 2-off pressure, 3-switch vacuum,
		  * 4-recirculation vacuum, 5-valves, 6-synchronization
		  *
		  **/
		string getDeviceID();

		/**  Check if the serial port is open and the PPC1 is connected
		*
		*  \return true if open
		**/
		bool isConnected() { return m_PPC1_serial->isOpen(); }


		/**  Check if the thread is running
		*
		*  \return true if running
		**/
		bool isRunning() { return m_isRunning; }

		PPC1_data *m_PPC1_data; /*!< data structure member exposed 
									 to be used by the user, 
									 maybe this will be changed in the future */
   };

}
