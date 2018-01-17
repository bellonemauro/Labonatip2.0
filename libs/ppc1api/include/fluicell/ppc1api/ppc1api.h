/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | PPC1 API                                                                  |
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
#define _USE_MATH_DEFINES
#include <cmath>

#include <iostream>
#include <string>
#include <cstdio>



#include <ctime>
#include <thread>
#include <mutex>

#include <serial/serial.h>


using namespace std;

/**  \brief Define the Fluicell namespace, all the classes will be in here
  *  
  **/
namespace fluicell
{
	
	/**  \brief Simple API for the communication with the Fluicell PPC1 controller
	*
	*  The PPC1 controller allows the control of microfluids in the Fluicell biopen system, 
	*  more details <a href="http://fluicell.com/thebiopensystem/"> here </a>
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
	* <b>Changes history</b>
	*		- MAY/2017: Creation (MB).
	*  \ingroup __
	*/
	class PPC1API_EXPORT PPC1api
	{

	// define class constants for ranges in vacuum and pressures
	#define MIN_CHAN_A -300.0 //!< in mbar
	#define MAX_CHAN_A -0.0 //!< in mbar
	#define MIN_CHAN_B -300.0 //!< in mbar
	#define MAX_CHAN_B -0.0 //!< in mbar
	#define MIN_CHAN_C 0.0 //!< in mbar
	#define MAX_CHAN_C 450.0 //!< in mbar
	#define MIN_CHAN_D 0.0 //!< in mbar
	#define MAX_CHAN_D 450.0 //!< in mbar
	#define MIN_STREAM_PERIOD 0 //!< in msec
	#define MAX_STREAM_PERIOD 500 //!< in msec
	#define MIN_PULSE_PERIOD 20 //!< in msec

	#define PPC1_VID "16D0"  //!< device vendor ID
	#define PPC1_PID "083A"  //!< device product ID

	public:

		/**  \brief PCC1 Output data stream specification
		*
		*  PCC1 Output data stream specification
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
		*          Channel character | set point | sensor reading | PID output duty cycle | state \n
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

				/**  \brief Constructor for PPC1 channel data container
				*
				**/
				channel() : 
					set_point(0.0), 
					sensor_reading(0.0),
					PID_out_DC(0.0), 
					state(0)
				{}
			};

			channel *channel_A;  //!< vacuum channel A   --- V_recirc
			channel *channel_B;  //!< vacuum channel B   --- V_switch
			channel *channel_C;  //!< pressure channel C --- P_off
			channel *channel_D;  //!< pressure channel D --- P_on

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


		//TODO write guide 
		struct PPC1API_EXPORT PPC1_status
		{
		public:

			double delta_pressure;
			double pipe_length;
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
			double flow_rate_1;
			double flow_rate_2;
			double flow_rate_3;
			double flow_rate_4;
			double flow_rate_5;
			double flow_rate_6;
			double flow_rate_7;
			double flow_rate_8;

			PPC1_status() :
				delta_pressure(0.0), pipe_length(0.0), 
				outflow_on(0.0), outflow_off(0.0), outflow_tot(0.0), 
				inflow_recirculation(0.0), inflow_switch(0.0), 
				in_out_ratio_on(0.0), in_out_ratio_off(0.0), in_out_ratio_tot(0.0), 
				solution_usage_off(0.0), solution_usage_on(0.0),
				flow_rate_1(0.0), flow_rate_2(0.0), flow_rate_3(0.0), flow_rate_4(0.0),
				flow_rate_5(0.0), flow_rate_6(0.0), flow_rate_7(0.0), flow_rate_8(0.0)
			{}
		};


	/**  \brief Serial device info data structure
		*
		*  @param port is the port name
		*  @param description is the hardware description
		*  @param hardware_ID contains hardware information
		*  @param VID is the vendor ID
		*  @param PID is the product ID
		*
		*
		*  \note The structure is useful to check if VID/PID match the fluicell PPC1 device
		**/
		struct PPC1API_EXPORT serialDeviceInfo
		{
		public:
			string port;
			string description;
			string hardware_ID;
			string VID;
			string PID;

			/**  \brief Constructor for serial device info
			*
			**/
			serialDeviceInfo() {}
		};


		/**  \brief PCC1 Command data structure definition
		*
		*
		*    The command data structure contains the information to run a command in the PPC1 controller.
		*    The final objective is to define a class of commands able to control the PPC1 controller and run a protocol
		*    as a set of commands.
		*
		*
		*  <b>Usage:</b><br>
		*		- 	define the object :                   fluicell::PPC1api::command *my_command;
		*	    -   a protocol is a vector of commands :     std::vector<fluicell::PPC1api::command> *_protocol;
		*	    -   run the command still missing implementation !! 
		*
		*/
		struct PPC1API_EXPORT command
		{
		public:

			/**  \brief Enumerator for the supported commands.
			*
			**/
			enum PPC1API_EXPORT instructions {
				setPon = 0,
				setPoff = 1,
				setVswitch = 2,
				setVrecirc = 3,
				solution1 = 4,
				solution2 = 5,
				solution3 = 6,
				solution4 = 7,
				dropletSize = 8,
				flowSpeed = 9,
				vacuum = 10,
				loop = 11,
				sleep = 12,
				ask_msg = 13,
				allOff = 14,
				pumpsOff = 15,
				setValveState = 16,
				waitSync = 17,
				syncOut = 18
			};


			/**  \brief Command data structure .
			*
			*  @param loops
			*
			**/
			//int loops;              //!< number of loops
			//int P_on;               //!< (int: pressure in mbar) ---- Channel D
			//int P_off;              //!< (int: pressure in mbar) ---- Channel C
			//int V_switch;           //!< (int: pressure in mbar) ---- Channel B
			//int V_recirc;           //!< (int: pressure in mbar) ---- Channel A
			//int Duration;           //!< duration for the application of the command 
			//bool ask;               //!< set true to stop execution and ask confirmation to continue
			//string ask_message;     //!< message to ask if @\param(ask)- is true
			
			
			//bool open_valve_a;      //!< closes other valves, then opens valve a for solution 1 valve only
			//bool open_valve_b;      //!< closes other valves, then opens valve b for solution 2 valve
			//bool open_valve_c;      //!< closes other valves, then opens valve c for solution 3 valve
			//bool open_valve_d;      //!< closes other valves, then opens valve d for solution 4 valve
			//bool wait_sync;         //!< protocol stops until trigger signal is received
			//int sync_out;           //!< if negative then default state is 1 and pulse is 0, if positive, then pulse is 1 and default is 0
			


			/**  \brief Command constructor
			*
			**/
			command() :
				instruction(instructions::setPon), value (0),
				visualize_status(false), status_message("No message")
			{ }


			/**  \brief Get the command from the enumerator.
			*
			**/
			instructions getInstruction() { return this->instruction; }

			/**  \brief Set the command .
			*
			**/
			void setInstruction(instructions _instruction) { this->instruction = _instruction; }

			/**  \brief Simple cast of the enumerator into the corresponding command as a string.
			*
			**/
			std::string getCommandAsString()
			{
				static const char* const text[] =
				{ "setPon", "setPoff", "setVswitch", "setVrecirc",
					"solution1", "solution2","solution3","solution4",
					"dropletSize", "flowSpeed", "vacuum",
					"loop", "sleep", "ask_msg",
					"allOff", "pumpsOff", "setValveState",
					"waitSync", "syncOut" };
				return  text[int(this->instruction)]; // cast to integer
			}

			/**  \brief Get the value for the corresponding command.
			*
			**/
			double getValue() { return this->value; }

			/**  \brief Set the value for the corresponding command.
			*
			**/
			void setValue(double _value) { 
				this->value = _value; }

			/**  \brief True if the status is set to be visualized.
			*
			**/
			bool isStatusVisualized() { 
				return this->visualize_status; }

			/**  \brief True if the status is set to be visualized.
			*
			**/
			void setVisualizeStatus(bool _visualize_status) { 
				this->visualize_status = _visualize_status; }

			/**  \brief Get the status message.
			*
			**/
			string getStatusMessage() { 
				return this->status_message; }

			/**  \brief Set the status message.
			*
			**/
			void setStatusMessage(string _status_message) { 
				this->status_message = _status_message; }

	private:
			instructions instruction;	 //!< command
			double value;                //!< corresponding value to be applied to the command
			bool visualize_status;       //!< if active the status message will be visualized
			string status_message;       //!< message to show as status during the command running

		};

	public:
	
		/** \brief Constructor, initialize objects and parameters using default values
		*        
		*/
		PPC1api();

		/** \brief Destructor implementation to make sure everything is properly closed
		  *  
		  *   Make sure the thread and the serial communication 
		  *   are properly closed, then free memory
		  */
		~PPC1api();

	private:

		/** \brief Threaded routine that stream data from PPC1
		*
		* \note return an exception for any serial error
		*/
		void threadSerial();

		/**  \brief Decode data line function
		  *
		  *   This function decode every line out from the PPC1 device and fill
		  *   the data structure m_PPC1_data with the current values.
		  *   Values are overridden every iteration. 
		  *
		  *  
		  *  <b>string data format:</b>
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
		bool decodeDataLine(const string &_data, PPC1_data &_PPC1_data);

		/**  \brief Decode one channel line
		*
		*    the channel line contains the following fields:
		*          Channel character | set point | sensor reading | PID output duty cycle | state \n
		*    example of input data : B|-0.000000|0.034291|0.000000|0\n
		*
		*  @param _data  input data to be decoded
		*  @param _line  output line to be filled with decoded values
		*
		* \return true if success, false for any error
		*
		* \note
		*/
		bool decodeChannelLine(const string &_data, vector<double> &_line);


		/** \brief Update flow calculation //TODO: write guide here
		*
		* \note -
		*/
		void updateFlows(const PPC1_data &_PPC1_data, PPC1_status &_PPC1_status);


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


		/** \brief Check if the connection on _port is going to the PPC1 or any other device 
		*
		* \return true if PPC1 VID/PID definitions match the VID/PID of the device connected on _port
		*/
		bool checkVIDPID(std::string _port);


		/** \brief  Get current date/time, format is YYYY-MM-DD.HH:mm:ss
		*
		* \return a string
		*/
		const std::string currentDateTime();

		// Serial port configuration parameters, only serial port number 
		// and baud rate are configurable for the user, this is intentional!
		serial::Serial *m_PPC1_serial;  //!< Pointer to serial port communication class
		//std::vector<serialDeviceInfo> m_devices;   //!< infos of connected devices
		string m_COMport;   //!< port number
		int m_baud_rate;    //!< baud rate
		int m_dataStreamPeriod; //!< data stream for the PPC1
		int m_COM_timeout;  //!< timeout for the serial communication --- default value 250 ms


		thread m_thread; //!< Member for the thread handling
		
		bool m_threadTerminationHandler;  //!< set to TRUE to stop the thread, automatically set to false during the thread starting
		bool m_isRunning; //!< true when the thread is running

	    // this values are the constants to have 100% droplet size 
		double m_default_pon;   //!< in mbar  -- default value  190.0 mbar
		double m_default_poff;  //!< in mbar  -- default value   21.0 mbar
		double m_default_v_recirc;  //!< in mbar (negative value!)  -- default value 115 mbar
		double m_default_v_switch;  //!< in mbar (negative value!)  -- default value 115 mbar
		double m_pipe_length2tip;  /*!< length of the pipe to the tip, this value is used for the calculation 
								        of the flow using the Poiseuille equation see function getFlow() -- default value 0.065 m; */
		double m_pipe_length2zone; /*!< length of the pipe to the zone, this value is used for the calculation 
								        of the flow using the Poiseuille equation see function getFlow() -- default value 0.124 m;*/
		bool m_verbose;


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

		/**  \brief Connect to serial port overload to set the serial port where to connect and the baud rate
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
		  *  \return true if success, false for any error
		  *
		  **/
		void pumpingOff();

		/**  \brief Close all the valves i to l
		*
		*     It recalls the setValvesState with the message 0xFF
	    *
		*  \return true if success, false for any error
		**/
		bool openAllValves();

		/**  \brief Close all the valves i to l
		*         
		*     It recalls the setValvesState with the message 0xF0
		**/
		bool closeAllValves();

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

		/** \brief Set a value on the vacuum channel A, admitted values are [-300.0, 0.0] in mbar
		  *
		  *  Send the string A%f\n to set vacuum on channel A to activate vacuum at a specific value
		  *
		  * \note - Channel A correspond to V_switch value
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setVacuumChannelA(const double _value = -0.0);

		/** \brief Set a value on the vacuum channel B, admitted values are [-300.0, 0.0] in mbar
		  *
		  *  Send the string B%f\n to set vacuum on channel B to activate vacuum at a specific value
		  *
		  * \note - Channel B correspond to V_switch value
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setVacuumChannelB(const double _value = -0.0);

		/** \brief Set a value on the pressure channel C, admitted values are [0.0, 450.0] in mbar
		  *
		  *
		  *  Send the string C%f\n to set pressure on channel C
		  *
		  * \note - Channel C correspond to Poff value
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setPressureChannelC(const double _value = 0.0);

		/** \brief Set a value on the pressure channel D, admitted values are [0.0, 450.0] in mbar
		  *
		  *
		  *  Send the string D%f\n to set pressure on channel D to pressure at a specific value.
		  *
		  * \note - Channel D correspond to Pon value
		  *
		  *  @param  _value is the set value %f
		  *
		  *  \note -  0.0 by default  ensure to avoid pump stress in case of error
		  **/
		bool setPressureChannelD(const double _value = 0.0);

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
		bool setValve_l(const bool _value = false);

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
		bool setValve_k(const bool _value = false);

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
		bool setValve_j(const bool _value = false);

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
		bool setValve_i(const bool _value = false);
		
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
		bool setValvesState(const int _value = 0x00);


		/** \brief Sets the TTL output state (high or low)
		*
		*  Send the string o%u\n to set the TTL output state where %u is either 1 or 0 (high or low)
		*
		*
		*  @param  _value is boolean
		*
		**/
		bool setTTLstate(const bool _value);

		/** \brief Send a pulse lasting _value (ms)
		*
		*  Send the string p%u\n that generates a pulse with a length of %u milliseconds. 
		*  For example p30\n generates a 30 milliseconds long pulse. 
		*  The initial state of the output is dependent of the previous "o%u" commands (default low). 
		*  If the state of output before the pulse command is high, 
		*  the pulse command puts the output low for the specified amount of time. 
		*  The minimum pulse length is 20 milliseconds.
		*
		*  @param  _value is the duration of the pulse in ms
		*
		**/
		bool setPulsePeriod(const int _value);


		/** \brief Set the runtime timeout to _value (mbar)
		*
		*  Send the string z%u\n  to set runtime set point timeout threshold to %u mbar. 
		*  If the set point is not in a window of +- threshold in 30 seconds, 
		*  set point will be zeroed and error bit will be set.  Default value is 5 mbar
		*
		*  @param  _value is the threshold in mbar
		*
		**/
		bool setRuntimeTimeout(const int _value);


		/** \brief Change the droplet size to a specific _percentage, default value = 100.0 %
		*
		*  Change the droplet size to a specific _percentage with respect to the default values
		*  of vacuum and pressures.
		*
		*  \note: the droplet size linear increment corresponds to a cubic power 
		*         of the desired values for pressures and vacuums
		* 		
		*  @param  _percentage is the desired percentage value
		*
		*  \return -  false in case of errors
		**/
		bool setDropletSize(double _percentage = 100.0);

		bool changeDropletSizeBy(double _percentage = 0.0);

		/** \brief Get the current droplet size as percentage
		*
		*  the real percentage of the droplet is the cubic root of the real value
		*  \note: As the droplet size linear increment corresponds to a cubic power
		*         of the desired values for pressures and vacuums, the percentage 
		*         corresponds to the cubic root
		*
		* \note: the calculation is based on the the actual sensor readings
		* \note: the calculation is the cube root of the visualized value 
		*
		*  \return -  value as the average of the percentage with respect to the default values
		**/
		double getDropletSize();

	
		/** \brief Set the flow speed to _percentage, default value = 100.0 %
		*
		*  To increase the flow speed, all the values of pressures and vacuum are 
		*  increased/decreased to the same percentage with respect to the default values
		*
		*  @param  _percentage is the desired percentage value
		*
		*
		*  \return -  false in case of errors
		**/
		bool setFlowspeed(const double _percentage = 100.0);


		/** \brief Change the flow speed by _percentage, default value = 0.0 %
		*
		*  To increase the flow speed, all the values of pressures and vacuum are
		*  increased/decreased to the same percentage with respect to the default values
		*
		*  @param  _percentage is the desired percentage value
		*
		*
		*  \return -  false in case of errors
		**/
		bool changeFlowspeedBy(const double _percentage = 0.0);

		/** \brief Get the current flow speed in percentage
		*
		* \note: the calculation is based on the the actual sensor readings
		*
		*  \return -  value = average percentage among all the channels
		**/
		double getFlowSpeed();
		
		/** \brief Set the vacuum by _percentage 
		*
		*  new recirculation value on Channel A = m_default_v_recirc * percentage
		*
		*
		*  \return -  false in case of errors
		**/
		bool setVacuumPercentage(const double _percentage = 100.0);

		/** \brief Change the vacuum by _percentage
		*
		*  Positive / negative values will increase descrease the percentage
		*
		*  new recirculation value on Channel A = m_default_v_recirc * percentage
		*
		*
		*  \return -  false in case of errors
		**/
		bool changeVacuumPercentageBy(const double _percentage = 0.0);

		/** \brief Get the current vacuum value in percentage
		*
		* \note: the calculation is based on the the actual sensor reading
		*
		*  \return -  value = 100 * channel_A->sensor_reading / DEFAULT_VACUUM
		**/
		double getVacuum();


		/** \brief Calculate the flow value
		*
		*  Calculate the flow in a pipe using the Poiseuille equation in laminar flow, 
		*  more details <a href="https://en.wikipedia.org/wiki/Hagen%E2%80%93Poiseuille_equation">here</a> 
		*  and <a href="https://engineering.stackexchange.com/questions/8004/how-to-calculate-flow-rate-of-water-through-a-pipe">here</a>
		*
		*  \f$ Q = \frac{\pi D^4 \Delta P}{128 \mu \Delta x} \f$
		*
		*  Specifically, here is calculate the flow using the square modifier for the specific channel
        *
		*  \f$ Q = \frac{\pi (s * D)^4 \Delta P}{128 \mu \Delta x} \f$
		*
		*  @param \f$ s \f$ _square_channel_mod  (m), default value 1.128 for the Fluicell PPC1 biopen
		*  @param \f$ D \f$ _pipe_diameter (m), default value 0.00003 
		*  @param \f$ \Delta P \f$ _delta_pressure  (100*mbar)
		*  @param \f$ \mu \f$ _dynamic_viscosity (poise ?? ) TODO:check this
		*  @param \f$ \Delta x \f$ _pipe_length (m)
		*
		*  \return -  flow (nL/s)
		**/
		double getFlow(double _square_channel_mod = 1.128, //TODO: explain the default values
			double _pipe_diameter = 0.00003,
			double _delta_pressure = -14600.0,
			double _dynamic_viscosity = 0.00089,
			double _pipe_length = 0.124
		);

		/** \brief Calculate the outflow value using default values
		*
		*  Recall getFlow using the following default values:
		*			 square_channel_mod = 1.128;
		*       	 pipe_diameter = 0.00003;
		*            dynamic_viscosity = 0.00089;
		*
		*  @param _delta_pressure  (100*mbar)
		*  @param _pipe_length (m)
		*
		*  \return -  flow (nL/s)
		**/
		double getFlowSimple(
			double _delta_pressure = -14600.0,
			double _pipe_length = 0.124
		) {
			double square_channel_mod = 1.128;
			double pipe_diameter = 0.00003;
			double dynamic_viscosity = 0.00089;
			return getFlow(square_channel_mod, pipe_diameter, _delta_pressure, dynamic_viscosity, _pipe_length);

		}


		/** \brief Run a command for the PPC1
		*
		*  This funciton runs commands on the PPC1, it implements an simple interpreter
		*  for the enumerator in <command>.
		*  The field <value> will also be interpreted to run the command. 
		*  The commands "loop", "ask message" and "status message" are not implemented in API
		*  by design, as they are supposed to be implemented in the high-level GUI
		*
		*  The commands SyncOut and WaitSync are still not implemented in the API
		*
		*  @param  _cmd is a command, see <command> structure
		*
		**/
		bool run(command _cmd);

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
		bool setDataStreamPeriod(const int _value = 200); //in msec

		/** \brief Allow to set the COM port name
		*
		*  \param  _COMport COM1, COM2, COM_n
		*
		*  \note -  there is no actual check on the string, TODO too weak implement!
		**/
		void setCOMport(string _COMport = "COM1") { m_COMport = _COMport; }

		/** \brief Allow to set the baudRate for the specified com port
		*
		*  \param  _baud_rate 9600 19200 115200
		*
		*  \note -  there is no actual check on the number, TODO too weak implement!
		**/
		void setBaudRate(int _baud_rate = 115200) { m_baud_rate = _baud_rate; }

		/** \brief Set verbose output
		*
		*   cout will be printed only if verbose is true, whereas cerr will always
		*
		*  \param  _baud_rate 9600 19200 115200
		*
		**/
		void setVebose(bool _verbose = false) { m_verbose = _verbose; }

		/** \brief Reset the default values of pressures and vacuum to 100 droplet size
		*
		*  \param  m_default_pon  default value of pressures 
		*  \param  m_default_poff  default value of pressures 
		*  \param  m_default_v_recirc  default value of vacuum 
		*  \param  m_default_v_switch  default value of vacuum 
		*
		*  \note -  values are in mbar
		*  \note -  there is no actual check on the numbers, TODO too weak implement!
		**/
		void setDefaultPV( double _default_pon = 190.0,
						   double _default_poff = 21.0,
		                   double _default_v_recirc = -115.0,
						   double _default_v_switch = -115.0 ) {
			m_default_pon = _default_pon;
			m_default_poff = _default_poff;
			m_default_v_recirc = _default_v_recirc;
			m_default_v_switch = _default_v_switch;
		}

		/**  \brief Get device serial number
		  *
		  * \note: from the old device api this description refers to channel, not clear:
		  * Get internal device ID. These are 6-digit serial numbers of Fluika
		  * modules in version 1 hardware and consecutive numbers in same
		  * format for version 2 hardware (e.g. "000001"). Input is channel number
		  * in range 1-6, where: 1-on pressure, 2-off pressure, 3-switch vacuum,
		  * 4-recirculation vacuum, 5-valves, 6-synchronization
		  *
		  **/
		string getDeviceID();

		/** \brief Check if the serial port is open and the PPC1 is connected
		*
		*  \return true if open
		**/
		bool isConnected() { return m_PPC1_serial->isOpen(); }


		/** \brief Check if the thread is running
		*
		*  \return true if running
		**/
		bool isRunning() { return m_isRunning; }

		PPC1_data *m_PPC1_data; /*!< data structure member exposed to be used by the user, 
									 maybe this will be changed in the future */
		PPC1_status *m_PPC1_status;/*!< data structure member exposed to be used by the user, 
									 maybe this will be changed in the future */
   };

}
