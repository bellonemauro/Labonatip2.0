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
// warning can be ignored
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


/**  \brief Define the Fluicell namespace, all the classes will be in here
  *  
  **/
namespace fluicell { namespace PPC1dataStructures
{
	
	
		/**  \brief PCC1 Output data stream specification
		*
		*  PCC1 Output data stream specification
		*
		*    A|-0.000000|0.114514|0.000000|0 \n
		*    B|-0.000000|0.034291|0.000000|0 \n
		*    C|0.000000|-0.103121|0.000000|0 \n
		*    D|0.000000|0.028670|0.000000|0 \n
		*    i0|j0|k0|l0 \n
		*    IN1|OUT1 \n
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
		struct PPC1_data
		{
			// define class constants for ranges in vacuum and pressures
			#define MIN_CHAN_A -300.0       //!< V_recirc in mbar
			#define MAX_CHAN_A -0.0         //!< V_recirc in mbar
			#define MIN_CHAN_B -300.0       //!< V_switch in mbar
			#define MAX_CHAN_B -0.0         //!< V_switch in mbar
			#define MIN_CHAN_C 0.0          //!< P_off in mbar
			#define MAX_CHAN_C 450.0        //!< P_off in mbar
			#define MIN_CHAN_D 0.0          //!< P_on in mbar
			#define MAX_CHAN_D 450.0        //!< P_on in mbar
			#define MIN_STREAM_PERIOD 0     //!< in msec
			#define MAX_STREAM_PERIOD 500   //!< in msec
			#define MIN_PULSE_PERIOD 20     //!< in msec
			#define MIN_ZONE_SIZE_PERC 50   //!< %
			#define MAX_ZONE_SIZE_PERC 200  //!< %
			#define MAX_ZONE_SIZE_INCREMENT 40  //!< %
			#define MIN_FLOW_SPEED_PERC 50  //!< %
			#define MAX_FLOW_SPEED_PERC 220 //!< %
			#define MAX_FLOW_SPEED_INCREMENT 40 //!< %
			#define MIN_VACUUM_PERC 50      //!< %
			#define MAX_VACUUM_PERC 250     //!< %
			#define MAX_VACUUM_INCREMENT 40     //!< %
			#define DEFAULT_LENGTH_TO_TIP_PRIME 0.065     /*!< length of the pipe to the tip, this value is used  
									             for the calculation of the flow using the Poiseuille equation
												 see function getFlow() -- default value 0.065 m; */ 
			#define DEFAULT_LENGTH_TO_ZONE_PRIME 0.062    /*!< length of the pipe to the zone, this value is used 
												 for the calculation of the flow using the Poiseuille equation
											 	 see function getFlow()-- default value 0.124 m; */
			#define DEFAULT_LENGTH_TO_TIP_FLEX 0.068     /*!< length of the pipe to the tip, this value is used  
												 for the calculation of the flow using the Poiseuille equation
												 see function getFlow() -- default value 0.065 m; */
			#define DEFAULT_LENGTH_TO_ZONE_FLEX 0.063    /*!< length of the pipe to the zone, this value is used 
												 for the calculation of the flow using the Poiseuille equation
												 see function getFlow()-- default value 0.124 m; */
			#define PPC1_VID "16D0"  //!< device vendor ID
			#define PPC1_PID "083A"  //!< device product ID
			#define PPC1_6CH_PID "0830"

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
			struct channel
			{
			public:
				double set_point;                 //!< the closed loop PID controller input value (in mbar)
				double sensor_reading;            //!< the actual current pressure value (in mbar), filtered if active
				double PID_out_DC;                //!< PID_out_DC PID output duty cycle is the output value of closed loop PID controller.
				int state;                        //!< state shows error flags

				/**  \brief Set channel data
				*
				*    It allows the to set all the data in the channel in one line by giving all the data in one line.
				*    this function also includes the data filtering function on the sensor reading, it uses two
				*    class members: m_filter_enabled and m_filter_size
				*
				*    The implemented filter is a rolling average filter that averages the last n samples, 
				*    where n is defined in the class member m_filter_size
				*
				*   @param _set_point 
				*   @param _sensor_reading  
				*   @param PID_out_DC 
				*   @param _state 
				*    
				*
				**/
				void setChannelData(const double _set_point, const double _sensor_reading, 
					const double PID_out_DC, const int _state)
				{
					this->set_point = _set_point;
					//this->sensor_reading = _sensor_reading;
					this->PID_out_DC = PID_out_DC;
					this->state = _state;

					if (m_filter_enabled) {
						// TODO: enable lowPassFiltering to improve efficiency but mantain same behavior
						//this->sensor_reading = movingAveragefilter(m_reading_vec, _sensor_reading);
						this->sensor_reading = lowPassFilter(this->sensor_reading, _sensor_reading);
					}
					else {
						this->sensor_reading = _sensor_reading;
					}
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
					m_filter_size(20),
					m_filter_alpha(0.1)
				{}
				
				bool isFilterEnables() { return m_filter_enabled; }
				void enableFilter(bool _enable) { m_filter_enabled = _enable; }
				int getFiltersize() { return m_filter_size; }
				void setFiltersize(int _size) { m_filter_size = _size; }
				double getFilterAlpha() { return m_filter_alpha; }
				void setFilterAlpha(double _alpha) { m_filter_alpha = _alpha; }
			private:

				double lowPassFilter(const double lastValue, const double currentValue) const
				{
					double value = (1 - m_filter_alpha)*lastValue + m_filter_alpha * currentValue;
					return value;
				}
				/**  \brief Implement a moving average filter of PPC1 data
				*
				*   @param _reading_vec the stack containing the history of readings
				*   @param _sensor_reading the actual sensor reading
				*
				*
				*  \note: it make use of m_filter_size data member for the size of the filter
				*
				*   \return the filtered value
				*
				**/
				double movingAveragefilter(std::vector<double> &_reading_vec, double _sensor_reading)
				{
					if (_reading_vec.size() < m_filter_size) {
						_reading_vec.push_back(_sensor_reading);
					}
					else {
						_reading_vec.erase(_reading_vec.begin());
						while (_reading_vec.size() > m_filter_size) {
							_reading_vec.erase(_reading_vec.begin());
						}
						this->m_reading_vec.push_back(_sensor_reading);
					}

					double sum = std::accumulate(_reading_vec.begin(), _reading_vec.end(), 0.0);
					double filtered_reading = sum / _reading_vec.size();
					return filtered_reading; 
				}

				bool m_filter_enabled;          //!< class member to enable to filtering in the data reading
				unsigned int m_filter_size;     //!< class member to set the filter size
				double m_filter_alpha;          //!< alpha for the low pass filter
				std::vector<double> m_reading_vec;   //!< internal vector used for the filter to save the history
			};

		public: 

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

			bool trigger_fall;   //!< this is false always, it becomes true when the trigger (fall) is detected
			bool trigger_rise;  //!< this is false always, it becomes true when the trigger (rise) is detected
			bool TTL_out_trigger;    //!< true = high, false = low
			bool data_corrupted;     //!< true in case of corrupted data, false otherwise

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
				ppc1_IN(0), ppc1_OUT(0),
				TTL_out_trigger(false),
				trigger_fall(false),
				trigger_rise(false),
				data_corrupted(false)
			{ }

			/**  \brief Set size for the rolling average filter
			*
			*   @param _size size of the filter, accepts positive values, reasonable values are between [10 - 30]
			*
			*   \return false in case of negative _size
			*
			**/
			void enableFilter(bool _enable) {
								
				this->channel_A->enableFilter(_enable);
				this->channel_B->enableFilter(_enable);
				this->channel_C->enableFilter(_enable);
				this->channel_D->enableFilter(_enable);
				
			}

			/**  \brief Set size for the rolling average filter
			*
			*   @param _size size of the filter, accepts positive values, reasonable values are between [10 - 30]
		    * 
			*   \return false in case of negative _size
			*
			**/
			bool setFilterSize(int _size) {
				if (_size < 0) { 
					std::cerr << " fluicell::PPC1_data::setFilterSize :: ---- error --- MESSAGE:"
						 << "Size of the filter in PPC1api cannot be negative " << std::endl;
					return false; 
				}
				this->channel_A->setFiltersize(_size); 
				this->channel_A->setFilterAlpha(double(_size) / 100.0); 
				this->channel_B->setFiltersize(_size);
				this->channel_B->setFilterAlpha(double(_size) / 100.0);
				this->channel_C->setFiltersize(_size);
				this->channel_C->setFilterAlpha(double(_size) / 100.0);
				this->channel_D->setFiltersize(_size);
				this->channel_D->setFilterAlpha(double(_size) / 100.0);
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
		struct PPC1_status
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

		/**  \brief Data structure handling the type of the tip
		*
		*  This allows to modify the type of the tip
		*
		*  @param length_to_tip             /*!< length of the pipe to the tip, this value is used  
		*							             for the calculation of the flow using the Poiseuille equation
		*										 see function getFlow() -- default value 0.065 m; 
		*  @param length_to_zone            /*!< length of the pipe to the zone, this value is used
		*										 for the calculation of the flow using the Poiseuille equation
		*										 see function getFlow()-- default value 0.124 m; 
		*
		* \note
		*/
		struct tip
		{
		public: 
			
			enum tipType {
			prime = 0,
			flex = 1
			};

			double length_to_tip;
			double length_to_zone;
			tipType type;

		public:
			tip() :
				length_to_tip (DEFAULT_LENGTH_TO_TIP_PRIME),
				length_to_zone (DEFAULT_LENGTH_TO_ZONE_PRIME),
				type (prime)
				{}

			void usePrimeTip()
			{
				length_to_tip = DEFAULT_LENGTH_TO_TIP_PRIME;
				length_to_zone = DEFAULT_LENGTH_TO_ZONE_PRIME;
				type = prime;
			}

			void useFlexTip()
			{
				length_to_tip = DEFAULT_LENGTH_TO_TIP_FLEX;
				length_to_zone = DEFAULT_LENGTH_TO_ZONE_FLEX;
				type = flex;
			}
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
		struct serialDeviceInfo
		{
		public:

			std::string port;
			std::string description;
			std::string hardware_ID;
			std::string VID;
			std::string PID;

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
		*    <table>
        *     <caption id="multi_row">Supported commands</caption>
        *     <tr>
        *       <th>enum index</th> <th>Command</th> <th>value</th> <th> Comment </th>
        *     </tr>
		*     <tr>
		*       <td> 0  </td>
		*       <td> zoneSize  </td>
		*       <td> percentage [MIN, MAX]  </td>
		*       <td> Change the zone size percentage to _value  </td>
		*     </tr>
		*     <tr>
		*       <td> 1  </td>
		*       <td> changeZoneSizeBy  </td>
		*       <td> percentage [MIN, MAX]  </td>
		*       <td> Change the zone size percentage to _value  </td>
		*     </tr>
		*     <tr>
		*       <td> 2  </td>
		*       <td> flowSpeed  </td>
		*       <td> percentage [MIN, MAX]  </td>
		*       <td> Change the flow speed percentage to _value  </td>
		*     </tr>
		*     <tr>
		*       <td> 3  </td>
		*       <td> changeFlowSpeedBy  </td>
		*       <td> percentage [MIN, MAX]  </td>
		*       <td> Change the flow speed percentage to _value  </td>
		*     </tr>
		*     <tr>
		*       <td> 4  </td>
		*       <td> vacuum  </td>
		*       <td> percentage [MIN, MAX]  </td>
		*       <td> Change the vacuum percentage to _value  </td>
		*     </tr>
		*     <tr>
		*       <td> 5  </td>
		*       <td> changeVacuumBy  </td>
		*       <td> percentage [MIN, MAX]  </td>
		*       <td> Change the vacuum percentage to _value  </td>
		*     </tr>
		*     <tr>
		*       <td> 6 </td>
		*       <td> wait  </td>
		*       <td> int n  </td>
		*       <td> wait for n seconds </td>
		*     </tr>
		*     <tr>
		*       <td> 7  </td>
		*       <td> allOff  </td>
		*       <td> -  </td>
		*       <td> stop all solutions flow </td>
		*     </tr>
		*     <tr>
		*       <td> 8 </td>
		*       <td> solution1    </td>
		*       <td> true / false  </td>
		*       <td> closes other valves, then opens valve 'a' for solution 1 </td>
		*     </tr>
		*     <tr>
		*       <td> 9 </td>
		*       <td> solution2 </td>
		*       <td> true / false </td>
		*       <td> closes other valves, then opens valve 'b' for solution 1 </td>
		*     </tr>
		*     <tr>
		*       <td> 10 </td>
		*       <td> solution3 </td>
		*       <td> true / false </td>
		*       <td> closes other valves, then opens valve 'c' for solution 1 </td>
		*     </tr>
		*     <tr>
		*       <td> 11 </td>
		*       <td> solution4 </td>
		*       <td> true / false </td>
		*       <td> closes other valves, then opens valve 'd' for solution 1 </td>
		*     </tr>
		*     <tr>
        *       <td> 12 </td>  
		*       <td> setPon </td> 
		*       <td> int [0 MAX] </td> 
		*       <td> (int: pressure in mbar) - - - - Channel D </td>
        *     </tr>
        *     <tr>
        *       <td> 13 </td> 
		*       <td> setPoff </td>
		*       <td> int [0 MAX] </td> 
		*       <td> (int: pressure in mbar) - - - - Channel C </td>
        *     </tr>
		*     <tr>
		*       <td> 14 </td>
		*       <td> setVrecirc   </td>
		*       <td> int [MIN 0]   </td>
		*       <td> (int: pressure in mbar) - - - - Channel A </td>
		*     </tr>
		*     <tr>
		*       <td> 15 </td>  
		*       <td> setVswitch </td> 
		*       <td> int [MIN 0] </td> 
		*       <td> (int: pressure in mbar) - - - - Channel B </td>
		*     </tr>
		*     <tr>
		*       <td> 16 </td>  
		*       <td> ask_msg   </td>
		*       <td> true / false  </td> 
		*       <td> set true to stop execution and ask confirmation to continue,\n
		*            INTEPRETED but NO ACTION required at API level </td>
		*     </tr>
		*     <tr>
		*       <td> 17 </td>
		*       <td> pumpsOff  </td>
		*       <td> -  </td>
		*       <td> stop pressures and vacuum by setting the channels to 0 </td>
		*     </tr>
		*     <tr>
		*       <td> 18 </td>
		*       <td> waitSync  </td>
		*       <td> int [0 MAX]  </td>
		*       <td> protocol stops until trigger signal is received </td>
		*     </tr>
		*     <tr>
		*       <td> 19 </td>
		*       <td> syncOut  </td>
		*       <td> int [0 MAX]  </td>
		*       <td> if negative then default state is 1 and pulse is 0,\n
		*            if positive, then pulse is 1 and default is 0</td>
		*     </tr>
		*     <tr>
		*       <td> 20 </td>
		*       <td> loop  </td>
		*       <td> int [0 MAX] </td>
		*       <td> number of loops </td>
		*     </tr>
        *    </table>
		*     
		*   <!--This is a doxygen documentation comment block
		*    Supported commands: TODO THE GUIDE HAS TO BE CHANGED ACCORDING TO THE NEW MAPPING
		*
		*    enum index    |   Command           |   value         |
		*   ---------------+---------------------+-----------------+-------------------------------------------------------------
		*      0           |   zoneSize          |  int [MIN MAX]  |  Change the zone size percentage to _value
		*      1           |   changeZoneSizeBy  |  int [MIN MAX]  |  Change the zone size percentage to _value
		*      2           |   flowSpeed         |  int [MIN MAX]  |  Change the flow speed percentage to _value
		*      3           |   changeFlowSpeedBy |  int [MIN MAX]  |  Change the flow speed percentage to _value
		*      4           |   vacuum            |  int [MIN MAX]  |  Change the vacuum percentage to _value
		*      5           |   changeVacuumBy    |  int [MIN MAX]  |  Change the vacuum percentage to _value
		*      6           |   wait              |  int n          |  wait for n seconds
		*      7           |   allOff            |       -         |  stop all solutions flow
		*      8           |   solution1         |  true / false   |  closes other valves, then opens valve a for solution 1
		*      9           |   solution2         |  true / false   |  closes other valves, then opens valve b for solution 2
		*      10          |   solution3         |  true / false   |  closes other valves, then opens valve c for solution 3
		*      11          |   solution4         |  true / false   |  closes other valves, then opens valve d for solution 4
		*      12          |   setPon            |  int [0 MAX]    |  (int: pressure in mbar) ---- Channel D
		*      13          |   setPoff           |  int [0 MAX]    |  (int: pressure in mbar) ---- Channel C
		*      14          |   setVrecirc        |  int [MIN 0]    |  (int: pressure in mbar) ---- Channel A
		*      15          |   setVswitch        |  int [MIN 0]    |  (int: pressure in mbar) ---- Channel B
		*      16          |   ask_msg           |  true / false   |  set true to stop execution and ask confirmation to continue,
		*                  |                     |                 |  INTEPRETED but NO ACTION required at API level
		*      17          |   pumpsOff          |       -         |  stop pressures and vacuum by setting the channels to 0
		*      18          |   waitSync          |  int [0 MAX]    |  protocol stops until trigger signal is received
		*      19          |   syncOut           |  int [0 MAX]    |  if negative then default state is 1 and pulse is 0,
		*                  |                     |                 |  if positive, then pulse is 1 and default is 0
		*      20          |   loop              |  int [0 MAX]    |  number of loops, not running at API level
		*                  |                     |                 |
		*   ---------------+---------------------+-----------------+-------------------------------------------------------------
		* end commented section -->
        *
		*  <b>Usage:</b><br>
		*		- 	define the object :                      fluicell::PPC1api::command *my_command;
		*	    -   a protocol is a vector of commands :     std::vector<fluicell::PPC1api::command> *_protocol;
		*	    -   run the command still under development !! 
		*
		*/
		struct command
		{
		public: 

			/**  \brief Enumerator for the supported commands.
			*
			**/
			enum instructions {
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
				ask_msg = 16,
				pumpsOff = 17,
				waitSync = 18,
				syncOut = 19,
				loop = 20,
				ask = 21,
				END
			};


		public:
			/**  \brief Command constructor
			*
			**/
			command() :
				instruction(instructions::setPon), value (0),
				status_message("No message")
			{ }


			bool checkValidity() {
			
				// check that the instruction is valid
				if (this->instruction < 0) return false;
				if (this->instruction > instructions::END) return false;

				int inst = this->instruction;

				switch (inst) {
				case instructions::setPon: { //setPon
					if (this->value < MIN_CHAN_D ||
						this->value > MAX_CHAN_D)
						return false; // out of bound
					else
						return true;
				}
				case instructions::setPoff: {//setPoff
					if (this->value < MIN_CHAN_C ||
						this->value > MAX_CHAN_C) 
						return false; // out of bound
					else
						return true;
				}
				case instructions::setVswitch: {//setVswitch
					if (this->value < MIN_CHAN_B ||
						this->value > MAX_CHAN_B) 
						return false; // out of bound
					else
						return true;
				}
				case instructions::setVrecirc: {//setVrecirc
					if (this->value < MIN_CHAN_A ||
						this->value > MAX_CHAN_A) 
						return false; // out of bound
					else
						return true;
				}
				case instructions::solution1: 
				case instructions::solution2: 
				case instructions::solution3: 
				case instructions::solution4: {//solution1,2,3,4
					if (this->value != 0 &&
						this->value != 1 ) 
						return false; // out of bound
					else
						return true;
					return true;
				}
				case instructions::wait: {//sleep
					if (this->value < 0) 
						return false;
					else
						return true;
				}
				case instructions::ask_msg: 
				case instructions::allOff: 
				case instructions::pumpsOff: {//ask_msg //allOff //pumpsOff
					// nothing to check here, the value is ignored
					return true;
				}
				case instructions::waitSync: {//waitSync //TODO
				//not checked for now
					return true;
				}
				case instructions::syncOut: {//syncOut 
				 //not checked for now
					return true;
				}
				case instructions::setZoneSize: {//zone size
					if (this->value < MIN_ZONE_SIZE_PERC ||
						this->value > MAX_ZONE_SIZE_PERC) 
						return false; // out of bound
					else
						return true;
				}
				case instructions::changeZoneSizeBy: {//zone size
					if (this->value < -MAX_ZONE_SIZE_INCREMENT ||
						this->value > MAX_ZONE_SIZE_INCREMENT)
						return false; // out of bound
					else
						return true;
				}
				case instructions::setFlowSpeed: {//flowSpeed
					if (this->value < MIN_FLOW_SPEED_PERC ||
						this->value > MAX_FLOW_SPEED_PERC) 
						return false; // out of bound
					else
						return true;
				}
				case instructions::changeFlowSpeedBy: {//flow speed
					if (this->value < -MAX_FLOW_SPEED_INCREMENT ||
						this->value > MAX_FLOW_SPEED_INCREMENT)
						return false; // out of bound
					else
						return true;
				}
				case instructions::setVacuum: {//vacuum
					if (this->value < MIN_VACUUM_PERC ||
						this->value > MAX_VACUUM_PERC) 
						return false; // out of bound
					else
						return true;
				}
				case instructions::changeVacuumBy: {//vacuum
					if (this->value < -MAX_VACUUM_INCREMENT ||
						this->value > MAX_VACUUM_INCREMENT)
						return false; // out of bound
					else
						return true;
				}
				case instructions::loop: {//loop
					if (this->value < 0) 
						return false;
					else
						return true;
				}
				default: {
					return false;
				}
				}

				return true;
			}


			/**  \brief Get the command from the enumerator.
			*
			**/
			instructions getInstruction() const { return this->instruction; }

			/**  \brief Set the command.
			*
			**/
			void setInstruction(instructions _instruction) { this->instruction = _instruction; }

			/**  \brief Simple cast of the enumerator into the corresponding command as a string.
			*
			*
			std::string getCommandAsString() const
			{
				static const char* const text[] =
				{ 
					"setZoneSize", "changeZoneSizeBy",
					"setFlowSpeed", "changeFlowSpeedBy",
					"setVacuum", "changeVacuumBy",
					"wait",
					"solution1", "solution2","solution3","solution4",
					"setPon", "setPoff",  "setVrecirc", "setVswitch",
					"ask_msg", "allOff", "pumpsOff",
					"waitSync", "syncOut", 
					 "loop" };
				return  text[int(this->instruction)]; // cast to integer
			}*/
			std::string getCommandAsString() const
			{
				switch (int(this->instruction))
				{
				case setZoneSize: return "setZoneSize";
				case changeZoneSizeBy: return "changeZoneSizeBy";
				case setFlowSpeed: return "setFlowSpeed";
				case changeFlowSpeedBy: return "changeFlowSpeedBy";
				case setVacuum: return "setVacuum";
				case changeVacuumBy: return "changeVacuumBy";
				case wait: return "wait";
				case solution1: return "solution1";
				case solution2: return "solution2";
				case solution3: return "solution3";
				case solution4: return "solution4";
				case setPon: return "setPon";
				case setPoff: return "setPoff";
				case setVrecirc: return "setVrecirc";
				case setVswitch: return "setVswitch";
				case ask_msg: return "ask_msg";
				case allOff: return "allOff";
				case pumpsOff: return "pumpsOff";
				case waitSync: return "waitSync";
				case syncOut: return "syncOut";
				case loop: return "loop";
				case END: return "END";
				}
				return "Invalid";

			}

			/**  \brief Get the value for the corresponding command.
			*
			**/
			double getValue() const { return this->value; }

			/**  \brief Set the value for the corresponding command.
			*
			**/
			void setValue(double _value) { 
				this->value = _value; }

			/**  \brief Get the status message.
			*
			**/
			std::string getStatusMessage() const {
				return this->status_message; }

			/**  \brief Set the status message.
			*
			**/
			void setStatusMessage(std::string _status_message) {
				this->status_message = _status_message; }

	private:
			instructions instruction;	 //!< command
			double value;                //!< corresponding value to be applied to the command
			std::string status_message;       //!< message to show as status during the command running

		};

}}
