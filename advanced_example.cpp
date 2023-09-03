#include <iostream>

#include "include/eos.hpp"

#define NB(msg) std::cout << "	" << msg << '\n';

// Rather than using the default eos server 'EOS_INIT', a custom server can be used to have better control over the eos server ownership
// Simply derive eos::server_interface
class mySingleton : public eos::server_interface
{
public:
	mySingleton() {}
};

static mySingleton inst;

// This must be declared globally or within the same namespace as the events that use it.
EOS_SERVER
{
	return inst;
}

// Declare an event called "someEvent" which passes a single integer value...
EOS_EVENT_1(someEvent, int, value)

int main(int argc, char** argv)
{
	// Observer invocation order...

	// create some observers...
	eos_observer::someEvent observer1([](int value) 
		{
			std::cout << "(observer1) someEvent occured with value " << value << '\n';
		});
	eos_observer::someEvent observer2([](int value) 
		{
			std::cout << "(observer2) someEvent occured with value " << value << '\n';
		});
	
	// dispatch the event, this will use the mySingleton eos server...
	// ordering the observers; observer1 receives the event before observer2...")
	eos_dispatch::someEvent(42);

	// By default, observers will be called in the order they are declared. Observers can be positioned at the front or back of the observer queue...

	// put observer2 at the front of the queue, it will be invoked first...
	observer2.observer_to_front();

	// dispatch the event again; observer2 receives the event before observer1 this time...
	eos_dispatch::someEvent(42);

	// Alternatively, the observer list can be manually re-ordered entirely, this will apply to all subsequent dispatches...
	
	// Reverse the order of the observers so that observer1 is again invoked first...
	eos::server().observers("someEvent", [](const eos::observer_list& obs) 
		{ 
			return eos::observer_list(obs.rbegin(), obs.rend());
		});

	// Dispatch the event again; observer1 receives the event before observer2 this time...
	eos_dispatch::someEvent(42);

	// By deriving the observers with different classes, a 'custom' dispatch can then use casting (or any other criteria) to choose the ordering 
	// of observers being invoked with the event...
	class myObj1 : public eos_observer::someEvent
	{
	public:
		void someEvent_event(int value) { std::cout << "(myObj1) someEvent occured with value " << value << '\n'; }
	};

	class myObj2 : public eos_observer::someEvent
	{
	public:
		void someEvent_event(int value) { std::cout << "(myObj2) someEvent occured with value " << value << '\n'; }
	};

	class myObj3 : public eos_observer::someEvent
	{
	public:
		void someEvent_event(int value) { std::cout << "(myObj3) someEvent occured with value " << value << '\n'; }
	};

	// Instantiate some observers...
	myObj1 e1;
	myObj2 e2;
	myObj3 e3;

	// A one-time predicate can be supplied when dispatching a specific event for custom ordering of the observer invocations for that event; myObj2 then myObj3 then myOb1...
	eos_dispatch::someEvent(42, [](const eos::observer_list& obs) 
		{ 
			// create a new list of observers for a one-time call of this event...
			eos::observer_list new_list;

			// myObj2 first...
			new_list.emplace_back(*std::find_if(obs.begin(), obs.end(), [](eos::observer_interface* observer) { return dynamic_cast<myObj2*>(observer); }));

			// then myObj3...
			new_list.emplace_back(*std::find_if(obs.begin(), obs.end(), [](eos::observer_interface* observer) { return dynamic_cast<myObj3*>(observer); }));

			// and finally myObj1...
			new_list.emplace_back(*std::find_if(obs.begin(), obs.end(), [](eos::observer_interface* observer) { return dynamic_cast<myObj1*>(observer); }));

			// N.B Observers do not have to be added to the new list if caller does not wish to invoke them...

			return new_list; 
		});
	
	return 0;
}