#ifndef PROPERTY_HPP
#define PROPERTY_HPP

#include "ppc1api_signals.h"
/*
#include <iostream>

namespace fluicell {
	// A Property is a encpsulates a value and may inform
	// you on any changes applied to this value.

	template <typename T>
	class ppc1apiProperty {

	public:
		typedef T value_type;

		// Properties for built-in types are automatically
		// initialized to 0. See template spezialisations
		// at the bottom of this file
		ppc1apiProperty()
			: m_connection(nullptr)
			, m_current_id(-1) {}

		ppc1apiProperty(T const& val)
			: m_value(val)
			, m_connection(nullptr)
			, m_current_id(-1) {}

		ppc1apiProperty(T&& val)
			: m_value(std::move(val))
			, m_connection(nullptr)
			, m_current_id(-1) {}

		ppc1apiProperty(ppc1apiProperty<T> const& to_copy)
			: m_value(to_copy.m_value)
			, m_connection(nullptr)
			, m_current_id(-1) {}

		ppc1apiProperty(ppc1apiProperty<T>&& to_copy)
			: m_value(std::move(to_copy.m_value))
			, m_connection(nullptr)
			, m_current_id(-1) {}

		// returns a Signal which is fired when the internal value
		// will be changed. The old value is passed as parameter.
		virtual fluicell::ppc1apiSignals<T> const& before_change() const { return m_before_change; }

		// returns a Signal which is fired when the internal value
		// has been changed. The new value is passed as parameter.
		virtual fluicell::ppc1apiSignals<T> const& on_change() const { return m_on_change; }

		// sets the Property to a new value. before_change() and
		// on_change() will be emitted.
		virtual void set(T const& value) {
			if (value != m_value) {
				m_before_change.emitPPC1Signal(m_value);
				m_value = value;
				m_on_change.emitPPC1Signal(m_value);
			}
		}

		// sets the Property to a new value. before_change() and
		// on_change() will not be emitted
		void set_with_no_emit(T const& value) {
			m_value = value;
		}

		// emits before_change() and on_change() even if the value
		// did not change
		void touch() {
			m_before_change.emitPPC1Signal(m_value);
			m_on_change.emitPPC1Signal(m_value);
		}

		// returns the internal value
		virtual T const& get() const { return m_value; }

		// connects two Properties to each other. If the source's
		// value is changed, this' value will be changed as well
		virtual void connect_from(ppc1apiProperty<T> const& source) {
			disconnectPPC1Property();
			m_connection = &source;
			m_current_id = source.on_change().connectPPC1Signal([this](T const& value) {
				set(value);
				return true;
			});
			set(source.get());
		}

		// if this Property is connected from another property,
		// it will e disconnected
		virtual void disconnectPPC1Property() {
			if (m_connection) {
				m_connection->on_change().disconnectPPC1Property(m_current_id);
				m_current_id = -1;
				m_connection = nullptr;
			}
		}

		// if there are any Properties connected to this Property,
		// they won't be notified of any further changes
		virtual void disconnect_auditors() {
			m_on_change.disconnectAllPPC1Signals();
			m_before_change.disconnectAllPPC1Signals();
		}

		// assigns the value of another Property
		virtual ppc1apiProperty<T>& operator=(ppc1apiProperty<T> const& rhs) {
			set(rhs.m_value);
			return *this;
		}

		// assigns a new value to this Property
		virtual ppc1apiProperty<T>& operator=(T const& rhs) {
			set(rhs);
			return *this;
		}

		// compares the values of two Properties
		bool operator==(ppc1apiProperty<T> const& rhs) const {
			return ppc1apiProperty<T>::get() == rhs.get();
		}
		bool operator!=(ppc1apiProperty<T> const& rhs) const {
			return ppc1apiProperty<T>::get() != rhs.get();
		}

		// compares the values of the Property to another value
		bool operator==(T const& rhs) const { return ppc1apiProperty<T>::get() == rhs; }
		bool operator!=(T const& rhs) const { return ppc1apiProperty<T>::get() != rhs; }

		// returns the value of this Property
		T const& operator()() const { return ppc1apiProperty<T>::get(); }

	private:
		ppc1apiSignals<T> m_on_change;
		ppc1apiSignals<T> m_before_change;

		ppc1apiProperty<T> const* m_connection;
		int m_current_id;
		T m_value;
	};

	// specialization for built-in default contructors
	template<> inline ppc1apiProperty<double>::ppc1apiProperty()
		: m_connection(nullptr), m_current_id(-1), m_value(0.0) {}

	template<> inline ppc1apiProperty<float>::ppc1apiProperty()
		: m_connection(nullptr), m_current_id(-1), m_value(0.f) {}

	template<> inline ppc1apiProperty<short>::ppc1apiProperty()
		: m_connection(nullptr), m_current_id(-1), m_value(0) {}

	template<> inline ppc1apiProperty<int>::ppc1apiProperty()
		: m_connection(nullptr), m_current_id(-1), m_value(0) {}

	template<> inline ppc1apiProperty<char>::ppc1apiProperty()
		: m_connection(nullptr), m_current_id(-1), m_value(0) {}

	template<> inline ppc1apiProperty<unsigned>::ppc1apiProperty()
		: m_connection(nullptr), m_current_id(-1), m_value(0) {}

	template<> inline ppc1apiProperty<bool>::ppc1apiProperty()
		: m_connection(nullptr), m_current_id(-1), m_value(false) {}

	// stream operators
	template<typename T>
	std::ostream& operator<<(std::ostream& out_stream, ppc1apiProperty<T> const& val) {
		out_stream << val.get();
		return out_stream;
	}

	template<typename T>
	std::istream& operator >> (std::istream& in_stream, ppc1apiProperty<T>& val) {
		T tmp;
		in_stream >> tmp;
		val.set(tmp);
		return in_stream;
	}
}*/
#endif /* PROPERTY_HPP */
