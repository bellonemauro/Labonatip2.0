/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  | Fluicell AB, http://fluicell.com/                                         |
*  | PPC1 API                                                                  |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <functional>
#include <map>


// An attempt to implement a signal-slot architecture using the standard library
// following this guide  http://simmesimme.github.io/tutorials/2015/09/20/signal-slot

namespace fluicell {
	// A signal object may call multiple slots with the
	// same signature. You can connect functions to the signal
	// which will be called when the emit() method on the
	// signal object is invoked. Any argument passed to emit()
	// will be passed to the given functions.
	template <typename... Args>
	class ppc1apiSignals {

	public:

		ppc1apiSignals() : m_current_id(0) {}

		// copy creates new signal
		ppc1apiSignals(ppc1apiSignals const& _other) : m_current_id(0) {}

		// connects a member function to this Signal
		template <typename T>
		int connectPPC1Signal2member(T *inst, void (T::*func)(Args...)) 
		{
			return connectPPC1Signal([=](Args... args) {
				(inst->*func)(args...);
			});
		}

		// connects a const member function to this Signal
		template <typename T>
		int connectPPC1Signal2member(T *inst, void (T::*func)(Args...) const) 
		{
			return connectPPC1Signal([=](Args... args) {
				(inst->*func)(args...);
			});
		}

		// connects a std::function to the signal. The returned
		// value can be used to disconnect the function again
		int connectPPC1Signal(std::function<void(Args...)> const& slot) const 
		{
			m_slots.insert(std::make_pair(++m_current_id, slot));
			return m_current_id;
		}

		// disconnects a previously connected function
		void disconnectPPC1Signal(int _id) const {
			m_slots.erase(_id);
		}

		// disconnects all previously connected functions
		void disconnectAllPPC1Signals() const {
			m_slots.clear();
		}

		// calls all connected functions
		void emitPPC1Signal(Args... _p) {
			for (auto it : m_slots) {
				it.second(_p...);
			}
		}

		// assignment creates new Signal
		ppc1apiSignals& operator=(ppc1apiSignals const& _other) {
			disconnectAllPPC1Signals();
		}

	private:
		mutable std::map<int, std::function<void(Args...)>> m_slots;
		mutable int m_current_id;
	};
}
#endif /* SIGNAL_HPP */