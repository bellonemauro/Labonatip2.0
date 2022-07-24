/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | PPC1 API                                                                  |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#pragma once

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
#include <cmath>
#include <iostream>
#include <string>
#include <cstdio>
#include <ctime>
#include <thread>
#include <mutex>

// third party serial library
#include <serial/serial.h>

#include "ppc1api_data_structures.h"

/**  \brief Define the Fluicell namespace, all the classes will be in here
  *  
  **/
namespace fluicell
{
	
	class  ppc1Exception : public std::exception
	{
		// Disable copy constructors
		ppc1Exception& operator=(const ppc1Exception&);
		std::string e_what_;
	public:
		ppc1Exception(const char *description) {
			std::stringstream ss;
			ss << "ppc1Exception " << description << " failed.";
			e_what_ = ss.str();
		}
		ppc1Exception(const ppc1Exception& other) : e_what_(other.e_what_) {}
		virtual ~ppc1Exception() throw() {}
		virtual const char* what() const throw () {
			return e_what_.c_str();
		}
	};

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
	*  Pressure setting commands. Argument %f - pressure in mbar as floating point number. 
    *  Vacuum channels expects the argument with minus sign. \n
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
	*  "u%u\n" sets serial data stream period. Argument "%u" is unsigned integer 
	*  marking the time period in ms. Zero turns the data stream off. 
    *  (This is useful in order to catch the output of ? # or ~ commands in the terminal window) 
	*  Default data stream period is 100ms, and the minimum is 25ms.\n
    *  
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
	class PPC1api 
	{

	public:
	
		/** \brief Constructor, initialize objects and parameters using default values
		*        
		*/
		explicit PPC1api();

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
		  *    P\n
		  *    R\n
		  *
		  *  Current interpreted state:
		  *
		  *    Pon: set 123mbar, read: 123.456789 mbar\n
		  *    Poff: set 123mbar, read: 123.456789 mbar\n
		  *    Vswitch: set 123mbar, read: 123.456789 mbar\n
		  *    Vrecirc: set 123mbar, read: 123.456789 mbar\n
		  *    Valves: 0101\n
		  *
		  *  @param _data       input data to be decoded
		  *  @param _PPC1_data  output data to be filled with decoded values
          *
		  * \return true if success, false for any error
		  *
		  * \note _PPC1_data will hold the last value in case of any error
		  */
		bool decodeDataLine(const std::string &_data,
			fluicell::PPC1dataStructures::PPC1_data * _PPC1_data) const;

		/**  \brief Decode one channel line
		*
		*    The channel line contains the following fields:
		*
		*          Channel character | set point | sensor reading | PID output duty cycle | state \n
		*
		*    example of input data : B|-0.000000|0.034291|0.000000|0\n
		*
		*  @param _data  input data to be decoded
		*  @param _line  output line to be filled with decoded values
		*
		* \return true if success, false for any error (for instance broken messages)
		*
		* \note False occur for: empty data, NaN in the string, wrong data size
		*/
		bool decodeChannelLine(const std::string &_data, std::vector<double> &_line) const;


		/** \brief Update inflow and outflow calculation 
		*
		*   This function updates flows calculation (inflow and outflow)
		*   to allow the user to get the current status of the PPC1. 
		*   This is currently based on calculation no sensor data are available.
		*   
		*
		*  @param _PPC1_data  input data stream from PPC1 
		*  @param _PPC1_status  output _PPC1_status with current flows
		*
		* \note - For details about the specific calculations, refer to the
		*         excel sheet in the resources folder
		*/
		void updateFlows(const fluicell::PPC1dataStructures::PPC1_data &_PPC1_data, 
			fluicell::PPC1dataStructures::PPC1_status &_PPC1_status) const;


		/** \brief Send a string to the PPC1 controller
		  *
		  * \note: does not return exceptions, but it prints errors in the serial port fail
		  *
		  */
		bool sendData(const std::string &_data) const;

		/** Read data from serial port
		  *
		  * \return false for any error
		  *
		  * \note this function read one line until the new line \n
		  */
		bool readData(std::string &_out_data);

		/**  \brief Convert char to digit 
		*
		*  Internal function that makes sure the char is properly interpreted as a digit
		*
		* \return value
		*
		* \note (int)char return the integer ASCII code for char so sub '0' is required
		*/
		int toDigit(const char _c) const { 
			//TODO: too weak, there is no check for validity over _c
			//      remember that '0' = 48 in ASCII  and '9' = 57 in ASCII 
			//      but in principle this function only returns 0 or 1 according to 
			//      the PPC1 api serial message
			return _c - '0';
		}// '0' = 48 in ASCII 


		/** \brief Check if the connection on _port is going to the PPC1 or any other device 
		*
		* \return true if PPC1 VID/PID definitions match the VID/PID of the device connected on _port
		*/
		bool checkVIDPID(const std::string& _port) const;

		/** \brief Allows to log errors with caller function and time stamp
		*
		*/
		void logError(const std::string& _caller, const std::string& _message) const;

		/** \brief Allows to log status with caller function and time stamp
		*
		*/
		void logStatus(const std::string& _caller, const std::string& _message) const;

		/** \brief  Get current date/time, format is YYYY-MM-DD.HH:mm:ss
		*
		* \return a string
		*/
		std::string currentDateTime() const;

		//some useful constants to decode the data messages
		const char m_separator = '|';              // separator between data
		const char m_decimal_separator = '.';      // decimal separator
		const char m_minus = '-';					// minus sign
		const char m_end_line = '\n';				// minus sign

		// Serial port configuration parameters, only serial port number 
		// and baud rate are configurable for the user, this is intentional!
		serial::Serial *m_PPC1_serial;  //!< Pointer to serial port communication class
		std::string m_COMport;	            //!< port number
		int m_baud_rate;                //!< baud rate	
		int m_COM_timeout;              //!< timeout for the serial communication --- default value 250 ms
		
		fluicell::PPC1dataStructures::PPC1_data *m_PPC1_data; /*!< ppc1 output structure */
		fluicell::PPC1dataStructures::PPC1_status *m_PPC1_status;/*!< pipette status */
		fluicell::PPC1dataStructures::tip *m_tip;
		int m_wait_sync_timeout;        //!< timeout for wait sync function in seconds, default value 60 sec

		// threads
		std::thread m_thread;                   //!< Member for the thread handling		
		bool m_threadTerminationHandler;   //!< set to TRUE to stop the thread, automatically set to false during the thread starting
		bool m_isRunning;                  //!< true when the thread is running

	    // this values are the constants to have 100% droplet size 
		double m_default_pon;              //!< in mbar  -- default value  190.0 mbar
		double m_default_poff;             //!< in mbar  -- default value   21.0 mbar
		double m_default_v_recirc;         //!< in mbar (negative value!)  -- default value 115 mbar
		double m_default_v_switch;         //!< in mbar (negative value!)  -- default value 115 mbar
		
		int m_dataStreamPeriod;             //!< data stream for the PPC1
		bool m_verbose;                     //!< verbose output when active
		bool m_filter_enabled;              //!< if active enable data filtering from PPC1
		int m_filter_size;                  //!< if m_filter_enabled active define the number of samples to be considered in the filter

		bool m_excep_handler;  // normally false, it will be true in case of exception
							   //TODO: this is an easy and dirty way of forwarding exceptions, find the proper solution to it

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
		bool connectCOM(std::string _COMport) {
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
		bool connectCOM(std::string _COMport, int _baudRate) {
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
		void pumpingOff() const;

		/**  \brief Close all the valves i to l
		*
		*     It recalls the setValvesState with the message 0xFF
	    *
		*  \return true if success, false for any error
		**/
		bool openAllValves() const;

		/**  \brief Close all the valves i to l
		*         
		*     It recalls the setValvesState with the message 0xF0
		**/
		bool closeAllValves() const;

		/**  \brief Send a reboot character to the device
		  *
		  *  \note - Known issue: weird behaviour in disconnect/connect
		  **/
		void reboot() const;

		/** \brief Run the thread
		  *
		  *  During the thread the PPC1 stream data on the open serial port and
		  *  listen messages on the same port
		  *
		  *
		  **/
		virtual void run() {
			m_threadTerminationHandler = false; //TODO: too weak, add checking if running and initialized
			m_thread = std::thread(&PPC1api::threadSerial, this);
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
		bool setVacuumChannelA(const double _value = -0.0) const;

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
		bool setVacuumChannelB(const double _value = -0.0) const;

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
		bool setPressureChannelC(const double _value = 0.0) const;

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
		bool setPressureChannelD(const double _value = 0.0) const;

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
		bool setValve_l(const bool _value = false) const;

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
		bool setValve_k(const bool _value = false) const;

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
		bool setValve_j(const bool _value = false) const;

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
		bool setValve_i(const bool _value = false) const;
		
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
		bool setValvesState(const int _value = 0x00) const;


		/** \brief Sets the TTL output state (high or low)
		*
		*  Send the string o%u\n to set the TTL output state where %u is either 1 or 0 (high or low)
		*
		*
		*  @param  _value is boolean
		*
		**/
		bool setTTLstate(const bool _value) const;

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
		bool setPulsePeriod(const int _value) const;


		/** \brief Set the runtime timeout to _value (mbar)
		*
		*  Send the string z%u\n  to set runtime set point timeout threshold to %u mbar. 
		*  If the set point is not in a window of +- threshold in 30 seconds, 
		*  set point will be zeroed and error bit will be set.  Default value is 5 mbar
		*
		*  @param  _value is the threshold in mbar
		*
		**/
		bool setRuntimeTimeout(const int _value) const;


		/** \brief Set the zone size to a specific _percentage, default value = 100.0 %
		*
		*  Set the droplet size to a specific _percentage with respect to the default values
		*  of vacuum and pressures.
		*  The droplet size linear increment corresponds to a cubic power of the desired values 
		*  for pressures and vacuums
		*
		*  new_Pon_value = default_Pon * (_percentage^1.3);
		*  new_Vrecirc_value = default_Vrecirc * (_percentage^1.3);
		*
		*         
		*  \note: The values of pressures and vacuum for Poff and Vswitch will be 
		*         reset to the default values
		*  \note: This function accepts values in [MIN_ZONE_SIZE_PERC, MAX_ZONE_SIZE_PERC]
		*         
		*
		*   new_Poff_value = default_Poff
		*   new_Vrecirc_value = default_Vrecirc
		*
		*  @param  _percentage is the desired percentage value
		*
		*  \return -  false in case of errors
		**/
		bool setZoneSizePerc(double _percentage = 100.0) const;

		/** \brief Change the zone size by a specific amount + or - 
		*
		*  Change the droplet size by adding a specific _percentage with respect to the default values
		*  of vacuum and pressures.
		*  The droplet size linear increment corresponds to a cubic power of the desired values 
		*  for pressures and vacuums.
		*
		*  new_Pon_value = Pon_set_point + default_Pon * (_percentage^1.3);
		*  new_Vrecirc_value = Vrecirc_set_point + default_Vrecirc * (_percentage^1.3);
		*
		*  \note: the other pressures/values will be untouched
		*  \note: This function accepts values in [-MAX_ZONE_SIZE_INCREMENT, MAX_ZONE_SIZE_INCREMENT]
		*
		*  \note: example: if _percentage = 5% ==> the size goes to 105%
		*
		*  @param  _percentage is the desired percentage value
		*
		*  \return -  false in case of errors
		**/
		bool changeZoneSizePercBy(double _percentage = 0.0) const;

		/** \brief Get the current droplet size as percentage
		*
		*  The real percentage of the droplet is the cubic root of the real value
		*
		*  \note: As the droplet size linear increment corresponds to a cubic power
		*         of the desired values for pressures and vacuums, the percentage 
		*         corresponds to the cubic root
		*
		* \note: the calculation is based on the the actual sensor readings
		* \note: the calculation is the cube root of the visualized value 
		*
		*  \return -  value as the average of the percentage with respect to the default values
		**/
		double getZoneSizePerc() const;

	
		/** \brief Set the flow speed to _percentage, default value = 100.0 %
		*
		*  To increase the flow speed, all the values of pressures and vacuum are 
		*  increased/decreased to the same percentage with respect to the default values
		*
		*  \note: This function accepts values in [MIN_ZONE_SIZE_PERC, MAX_ZONE_SIZE_PERC]
		*
		*
		*  @param  _percentage is the desired percentage value
		*
		*
		*  \return -  false in case of errors
		**/
		bool setFlowSpeedPerc(const double _percentage = 100.0) const;


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
		bool changeFlowSpeedPercBy(const double _percentage = 0.0) const;

		/** \brief Get the current flow speed in percentage
		*
		* \note: the calculation is based on the the actual sensor readings
		*
		*  \return -  value = average percentage among all the channels
		**/
		double getFlowSpeedPerc() const;
		
		/** \brief Set the vacuum by _percentage 
		*
		*  new recirculation value on Channel A = m_default_v_recirc * percentage
		*
		*
		*  \return -  false in case of errors
		**/
		bool setVacuumPerc(const double _percentage = 100.0) const;

		/** \brief Change the vacuum by _percentage
		*
		*  Positive / negative values will increase decrease the percentage
		*
		*  new recirculation value on Channel A = m_default_v_recirc * percentage
		*
		*
		*  \return -  false in case of errors
		**/
		bool changeVacuumPercBy(const double _percentage = 0.0) const;

		/** \brief Get the current vacuum value in percentage
		*
		* \note: the calculation is based on the the actual sensor reading
		*
		*  \return -  value = 100 * channel_A->sensor_reading / DEFAULT_VACUUM
		**/
		double getVacuumPerc() const;


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
		double getFlow(double _square_channel_mod = 1.128, 
			double _pipe_diameter = 0.00003,
			double _delta_pressure = -14600.0,
			double _dynamic_viscosity = 0.00089,
			double _pipe_length = 0.124	) const;

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
		double getFlowSimple (
			double _delta_pressure = -14600.0,
			double _pipe_length = 0.124	) const
		{
			double square_channel_mod = 1.128;
			double pipe_diameter = 0.00003;
			double dynamic_viscosity = 0.00089;
			return getFlow(square_channel_mod, pipe_diameter, _delta_pressure, dynamic_viscosity, _pipe_length);

		}


		/** \brief Run a command for the PPC1
		*
		*  This function runs commands on the PPC1, it implements an simple interpreter
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
		bool runCommand(fluicell::PPC1dataStructures::command _cmd) const;

		/**  \brief Set the data stream period on the serial port
		  *
		  *  Send the string u%u\n to set the data stream period
		  *
		  *  u%u\n sets serial data stream period. Argument %u is unsigned 
		  *  integer marking the time period in ms. Zero turns the data stream off.
		  *  (This is useful in order to catch the output of ? # or ~ commands 
		  *  in the terminal window)
		  *  Default data stream period is 100ms, and the minimum is 25ms.
		  *  
		  *  Example "u200\n" sets the data stream period to 200ms
		  *
		  *  @param  _value in msec, default value = 200
		  * 
		  *  \return  true if success, false for any error
		  **/
		bool setDataStreamPeriod(const int _value = 200); 

		/** \brief Allow to set the COM port name
		*
		*  @param  _COMport COM1, COM2, COMn
		*
		*  \note -  there is no actual check on the string, but it will out an error if the serial cannot connect
		**/
		void setCOMport(std::string _COMport = "COM1") { m_COMport = _COMport; }

		/** \brief Allow to set the baudRate for the specified com port
		*
		*   Possible baudrates depends on the system but some safe baudrates include:
        *   110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 56000,
        *   57600, 115200
		*
        *   Some other baudrates that are supported by some comports:
        *   128000, 153600, 230400, 256000, 460800, 921600
        *
		*  @param  _baud_rate default value 115200 baud/s
		*
		*  \note -  there is no actual check on the number
		**/
		void setBaudRate(int _baud_rate = 115200) { m_baud_rate = _baud_rate; }

		/** \brief Set verbose output
		*
		*   api messages will be printed only if verbose is true, 
		*   whereas api error messages will always be printed
		*
		*  @param  _verbose true or false
		**/
		void setVerbose(bool _verbose = false) { m_verbose = _verbose; }

		/** \brief Reset the default values of pressures and vacuum to 100 droplet size
		*
		*  @param  m_default_pon  default value of pressures 
		*  @param  m_default_poff  default value of pressures 
		*  @param  m_default_v_recirc  default value of vacuum 
		*  @param  m_default_v_switch  default value of vacuum 
		*
		*  \return true if success, false for out of range values
		*
		*  \note -  values are in mbar
		**/
		bool setDefaultPV(double _default_pon = 190.0,
			double _default_poff = 21.0,
			double _default_v_recirc = -115.0,
			double _default_v_switch = -115.0);

		/** \brief Set the tip parameters
		*
		*  @param  _length_to_tip  see data structure reference for details
		*  @param  _length_to_zone  see data structure reference for details
		*
		*  \note -  call this function without arguments to reset to the default values
		*  \note -  values are in meters
		**/
		void setTipParameters( double _length_to_tip,// = DEFAULT_LENGTH_TO_TIP,
			                   double _length_to_zone){// = DEFAULT_LENGTH_TO_ZONE) {
			m_tip->length_to_tip = _length_to_tip;
			m_tip->length_to_zone = _length_to_zone;
		}

		fluicell::PPC1dataStructures::tip::tipType getTipType() const {
			return m_tip->type;
		}

		/** \brief Allows to set the tip type using pre-determined values
		*
		*   Two tips are currently allowed Prime and Flex 
		*
		*  @param  _tip : true = Prime, False = Flex
		*
		*  \note -  call this function without argument reset the tip to Prime
		**/
		void setTip(bool _tip = true) {
			if (_tip == true) m_tip->usePrimeTip();
			if (_tip == false) m_tip->useFlexTip();
		}

		/** \brief Get the length_to_tip value
		*
		*  \return length to tip
		**/
		double getLegthToTip()  const {
			return m_tip->length_to_tip;
		}

		/** \brief Get the length_to_zone value
		*
		*  \return length to zone
		**/
		double getLegthToZone()  const {
			return m_tip->length_to_zone;
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
		  * \return the device ID as a string
		  **/
		std::string getDeviceID();

		/**  \brief is filter enabled
		*
		* Check for the filter to be enabled
		*
		* \return return true if the data filtering is enabled
		**/
		bool isFilterEnabled() const { return m_filter_enabled; }

		/**  \brief Enable or disable the filter
		*
		* Allows the user to activate and deactivate the data filtering
		*
		* @param  _enable  true or false
		*/
		void setFilterEnabled(bool _enable);

		/**  \brief Get filter size
		*
		* The implemented filter is a moving average over a specific time span
		* which is defined in the window size of the filter
		*
		* \return return true if the data filtering is enabled
		**/
		int getFilterSize() const { return m_filter_size; }


		/**  \brief Set filter size
		*
		* The implemented filter is a moving average over a specific time span
		* which is defined in the window size of the filter. This allows the user 
		* to reset the window size of the filter.
		*
		* @param  _size  new size positive value 
		**/
		void setFilterSize(int _size);

		/** \brief Check if the serial port is open and the PPC1 is connected
		*
		*  \return true if open
		**/
		bool isConnected()  const { return m_PPC1_serial->isOpen(); }


		/** \brief Check if the thread is running
		*
		*  \return true if running
		**/
		bool isRunning()  const { return m_isRunning; }

		/** \brief Set the wait sync timeout
		*
		*  The wait sync function will wait for the sync TTL signal,
		*  if the sync TTL signal does not arrives in a specific amount of time
		*  the function waitSync will fall into a break and return false
		*
		*  if _wait_sync_timeout is lower than 0 it will not be set 
		*
		*
		* @param  _wait_sync_timeout  integer > 0
		**/
		void setWaitSyncTimeout(int _wait_sync_timeout) {
			if (_wait_sync_timeout > 0)
				m_wait_sync_timeout = _wait_sync_timeout;
		}

		/** \brief Reset the sync signals
		*
		*  Used to reset the sync signals trigger_rise and trigger_fall to a specific value,
		*  false is used when the waitSync command starts, 
		*  whereas true can be used to manually simulate the sync signal arrived
		*
		*  
		* @param  _state  new sync signals state
		**/
		void resetSycnSignals(bool _state)  const {
			m_PPC1_data->trigger_rise = _state;
			m_PPC1_data->trigger_fall = _state;
		}

		/** \brief Detect the sync signal arrived
		*
		*  Check the trigger of PPC1 and return true when the trigger,
		*  rise or fall, is detected. 
		*  If _state = true, it detects the rising trigger
		*  if _state = false, it detects the falling trigger
		*
		* @param  _state  new size value
		*
		*  \return true when the signal is detected
		**/
		bool syncSignalArrived(bool _state)  const {
			if (_state == true) // check rise state 
			{
				if (m_PPC1_data->trigger_rise == true )
				return true;
			}
			if (_state == false)  // check fall state
			{
				if (m_PPC1_data->trigger_fall == true )
				return true;
			}

			return false;		
			
		}		//     "pX" is sent to make pulse output, where X is integer number equal or larger than 20 indicating the pulse length in milliseconds
				//     "P" or "R" are use wait pulse input, either falling or rising front



		/** \brief Check if an exception has been caught
		*
		*  \return true if exception
		**/
		inline bool isExceptionHappened()  const { return m_excep_handler; }


		/** \brief get vacuum recirculation set point
		*
		*  \return double recirculation set point
		**/
		inline double getVrecircSetPoint() const { return m_PPC1_data->channel_A->set_point; }

		/** \brief get vacuum recirculation sensor reading
		*
		*  \return double recirculation sensor reading
		**/
		inline double getVrecircReading()  const { return m_PPC1_data->channel_A->sensor_reading; }
		
		/** \brief get vacuum recirculation state of the error flag
		*
		*  \return int error flag
		**/
		inline int getVrecircState() const { return m_PPC1_data->channel_A->state; }

		/** \brief get vacuum switch set point
		*
		*  \return double switch set point
		**/
		inline double getVswitchSetPoint()  const { return m_PPC1_data->channel_B->set_point; }

		/** \brief get vacuum switch sensor reading
		*
		*  \return double switch sensor reading
		**/
		inline double getVswitchReading() const { return m_PPC1_data->channel_B->sensor_reading; }

		/** \brief get vacuum switch state of the error flag
		*
		*  \return int error flag
		**/
		inline int getVswitchState()  const { return m_PPC1_data->channel_B->state; }

		/** \brief get pressure off set point
		*
		*  \return double pressure off set point
		**/
		inline double getPoffSetPoint() const { return m_PPC1_data->channel_C->set_point; }

		/** \brief get pressure off sensor reading
		*
		*  \return double pressure off sensor reading
		**/
		inline double getPoffReading() const { return m_PPC1_data->channel_C->sensor_reading; }

		/** \brief get pressure off state of the error flag
		*
		*  \return int error flag
		**/
		inline int getPoffState()  const { return m_PPC1_data->channel_C->state; }
		
		/** \brief get pressure on set point
		*
		*  \return double pressure on set point
		**/
		inline double getPonSetPoint()  const { return m_PPC1_data->channel_D->set_point; }

		/** \brief get pressure on sensor reading
		*
		*  \return double pressure on sensor reading
		**/
		inline double getPonReading() const { return m_PPC1_data->channel_D->sensor_reading; }

		/** \brief get pressure on state of the error flag
		*
		*  \return int error flag
		**/
		inline int getPonState() const { return m_PPC1_data->channel_D->state; }

		/** \brief Get the communication state from the corrupted data flag
		*
		*  \return true if communication is ok, false in case of corrupted data
		**/
		inline bool getCommunicationState() const { return !m_PPC1_data->data_corrupted; }


		/** \brief Check if the well 1 is open
		*
		*  \return true if the well 1 is open, false otherwise
		**/
		bool isWeel1Open() const {
			if (m_PPC1_data->l == 1) return true;
			else return false;
		}

		/** \brief Check if the well 2 is open
		*
		*  \return true if the well 2 is open, false otherwise
		**/
		bool isWeel2Open() const {
			if (m_PPC1_data->k == 1) return true;
			else return false;
		}

		/** \brief Check if the well 3 is open
		*
		*  \return true if the well 3 is open, false otherwise
		**/
		bool isWeel3Open()  const {
			if (m_PPC1_data->j == 1) return true;
			else return false;
		}

		/** \brief Check if the well 4 is open
		*
		*  \return true if the well 4 is open, false otherwise
		**/
		bool isWeel4Open() const {
			if (m_PPC1_data->i == 1) return true;
			else return false;
		}


		/** \brief Calculate the protocol duration in seconds
		*
		*   Calculate the protocol duration in seconds by summing
		*   the waiting commands in a protocol
		*
		*  @param _protocol a protocol as a list of PPC1api commands
		*
		* \return a double with the protocol duration in seconds
		*/
		double protocolDuration(std::vector<fluicell::PPC1dataStructures::command> &_protocol)  const;

		/** \brief Get the pipette status 
		*
		*  \return a copy of the data member
		**/
		const fluicell::PPC1dataStructures::PPC1_status* getPipetteStatus() const { return m_PPC1_status; }
   };

}
