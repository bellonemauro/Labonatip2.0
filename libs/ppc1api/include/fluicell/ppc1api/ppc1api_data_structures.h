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
#include <string>
#include <numeric>



using namespace std;

/**  \brief Define the Fluicell namespace, all the classes will be in here
  *  
  **/
namespace fluicell
{
	
	
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

			/**  \brief Channel data structure 
			*
			*         it contains the information about a PPC1 channel
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
				double set_point;                 //!< is the closed loop PID controller input value (in mbar)
				double sensor_reading;            //!< shows the actual current pressure value (in mbar)
				double filtered_sensor_reading;   //!< filtered sensor reading (in mbar)
				double PID_out_DC;                //!< PID_out_DC PID output duty cycle is the output value of closed loop PID controller.
				int state;                        //!< state shows error flags

				/**  \brief Set channel data
				*
				*    It allows the to set all the data in the channel in one line by giving all the data in one line.
				*    this function also includes the data filtering function on the sensor reading, it uses two
				*    class members: m_filter_enabled and m_filter_size
				*
				*    The implemented filted is a rolling average filter that averages the last n samples, 
				*    where n is defined in the class member m_filter_size
				*
				*   @param _set_point 
				*   @param _sensor_reading  
				*   @param PID_out_DC 
				*   @param _state 
				*    
				*
				**/
				void setChannelData(double _set_point, double _sensor_reading, double PID_out_DC, int _state)
				{
					this->set_point = _set_point;
					this->sensor_reading = _sensor_reading;

					if (m_filter_enabled)
					{
						if (this->m_reading_vec.size() < m_filter_size) {
							this->m_reading_vec.push_back(_sensor_reading);
						}
						else {
							this->m_reading_vec.erase(this->m_reading_vec.begin());
							while (this->m_reading_vec.size() > m_filter_size) {
								this->m_reading_vec.erase(this->m_reading_vec.begin());
							}
							this->m_reading_vec.push_back(_sensor_reading);
						}

						double sum = std::accumulate(this->m_reading_vec.begin(), this->m_reading_vec.end(), 0.0);
						double mean = sum / this->m_reading_vec.size();
						this->filtered_sensor_reading = mean;
						this->sensor_reading = mean; //TODO: this is now the same, is filtered_sensor_reading really necessary ?

					}
					else {
						this->filtered_sensor_reading = _sensor_reading;
					}

					this->PID_out_DC = PID_out_DC;
					this->state = _state;
				
				}

				/**  \brief Constructor for PPC1 channel data container
				*
				**/
				channel() : 
					set_point(0.0), 
					sensor_reading(0.0),
					PID_out_DC(0.0), 
					state(0),
					m_filter_enabled(true),
					m_filter_size(20)
				{}
				
				bool isFilterEnables() { return m_filter_enabled; }
				void enableFilter(bool _enable) { m_filter_enabled = _enable; }
				int getFiltersize() { return m_filter_size; }
				void setFiltersize(int _size) { m_filter_size = _size; }

			private:
				bool m_filter_enabled;          //!< class member to enable to filtering in the data reading
				unsigned int m_filter_size;     //!< class member to set the filter size
				vector<double> m_reading_vec;   //!< internal vector used for the filter to save the history
			};

		public: //protected: //TODO: this should be protected

			channel *channel_A;  //!< pointer to vacuum channel A   --- V_recirc
			channel *channel_B;  //!< pointer to vacuum channel B   --- V_switch
			channel *channel_C;  //!< pointer to pressure channel C --- P_off
			channel *channel_D;  //!< pointer to pressure channel D --- P_on

			/*	i%u | j%u | k%u | l%u\n where the characters i, j, k and l 
			*   mark the output channels 8, 7, 6, 5 respectively and %u is 1 when the
			*	output channel is connected to pressure channel D and 0 when channel C.
			*/
			int i;  //!< 8
			int j;  //!< 7
			int k;  //!< 6
			int l;  //!< 5

			int ppc1_IN;  //!< INx where x and y are either 0 or 1 and show the input state.
			int ppc1_OUT; //!< OUTy where x and y are either 0 or 1 and show the output state

		public:
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

			/**  \brief Set size for the rolling average filter
			*
			*   @param _size size of the filter, accepts positive values, resonable values are between [10 - 30]
		    * 
			*   \return false in case of negative _size
			*
			**/
			bool setFilterSize(int _size) {
				if (_size < 0) { 
					cerr << " fluicell::PPC1_data::channel :: ---- error --- MESSAGE:" 
						 << "Size of the filter in PPC1api cannot be negative " << endl;
					return false; 
				}
				this->channel_A->setFiltersize(_size);
				this->channel_B->setFiltersize(_size);
				this->channel_C->setFiltersize(_size);
				this->channel_D->setFiltersize(_size);
				return true;
			}

			/**  \brief Detor
			*
			*    Essentially delete all the channels data structures (pointers) initialized
			**/
			~PPC1_data() {
				delete channel_A;
				delete channel_B;
				delete channel_C;
				delete channel_D;
			}
		};


		/**  \brief PPC1_status structure contains the inflow and outflow data for each well in the pipette
		*
		*  @param delta_pressure
		*  @param pipe_length
		*  @param outflow_on
		*  @param outflow_off
		*  @param outflow_tot
		*  @param inflow_recirculation
		*  @param inflow_switch
		*  @param in_out_ratio_on
		*  @param in_out_ratio_off
		*  @param in_out_ratio_tot
		*  @param solution_usage_off
		*  @param solution_usage_on
		*  @param flow_rate_1
		*  @param flow_rate_2
		*  @param flow_rate_3
		*  @param flow_rate_4
		*  @param flow_rate_5
		*  @param flow_rate_6
		*  @param flow_rate_7
		*  @param flow_rate_8
		*
		*
		*  \note : TODO: the guide is still not entirely complete
		**/
		struct PPC1API_EXPORT PPC1_status
		{
		public: //protected: //TODO: this should be protected

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

		public:

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
		public: //protected: //TODO: this should be protected

			string port;
			string description;
			string hardware_ID;
			string VID;
			string PID;

		public:
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
		*    Supported commands: 
		*
		*    enum index    |   Command       |   value         |
		*   ---------------+-----------------+-----------------+-------------------------------------------------------------
		*      0           |   setPon        |  int [0 MAX]    |  (int: pressure in mbar) ---- Channel D
		*      1           |   setPoff       |  int [0 MAX]    |  (int: pressure in mbar) ---- Channel C
		*      2           |   setVswitch    |  int [MIN 0]    |  (int: pressure in mbar) ---- Channel B
		*      3           |   setVrecirc    |  int [MIN 0]    |  (int: pressure in mbar) ---- Channel A
		*      4           |   solution1     |  true / false   |  closes other valves, then opens valve a for solution 1  
		*      5           |   solution2     |  true / false   |  closes other valves, then opens valve b for solution 2 
		*      6           |   solution3     |  true / false   |  closes other valves, then opens valve c for solution 3 
		*      7           |   solution4     |  true / false   |  closes other valves, then opens valve d for solution 4 
		*      8           |   dropletSize   |  int [0 MAX]    |  TODO: to be implemented
		*      9           |   flowSpeed     |  int [0 MAX]    |  TODO: to be implemented
		*      10          |   vacuum        |  int [0 MAX]    |  TODO: to be implemented
		*      11          |   loop          |  int [0 MAX]    |  number of loops 
		*      12          |   sleep         |  int n          |  wait for n seconds
		*      13          |   ask_msg       |  true / false   |  set true to stop execution and ask confirmation to continue, 
		*                  |                 |                 |  INTEPRETED but NOT IMPLEMENTED at API level
		*      14          |   allOff        |       -         |  stop all solutions flow
		*      15          |   pumpsOff      |       -         |  stop pressures and vacuum by setting the channels to 0
		*      16          |   setValveState |  hex  0x00      |  set solutions flow using a hex value
		*      17          |   waitSync      |  int [0 MAX]    |  protocol stops until trigger signal is received
		*      18          |   syncOut       |  int [0 MAX]    |  if negative then default state is 1 and pulse is 0, if positive, then pulse is 1 and default is 0
		*   ---------------+-----------------+-----------------+-------------------------------------------------------------
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
		public: //protected: //TODO: this should be protected but the derived class cannot access the protected member in the base class ?

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


		public:
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

			/**  \brief Set the command.
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

}
