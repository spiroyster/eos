#include <iostream>

#include "include/eos.hpp"

// Initialise 
EOS_INIT

// Declare an event called "someEvent"...
EOS_EVENT(someEvent)

// Declare an event called "someEventWithArg" which can pass a single integer value with the event...
EOS_EVENT_1(someEventWithArg, int, value)

// Declare an event called "someEventWithTwoArg" which can pass two arguments, an integer and a float, 
// contained in a namespace called "someNamespace"...
namespace someNamespace
{
	EOS_EVENT_2(someEventWithTwoArg, int, a, float, b)
}


int main(int argc, char** argv)
{
	// Two ways to observe an event...

	// base observer with a callback...
	eos_observer::someEvent someEventObserver([]()
		{
			std::cout << "someEventObserver: someEvent occured...\n";
		});

	// derived observer interface with method...
	class myObj : public eos_observer::someEvent
	{
	public:
		void someEvent_event()
		{
			std::cout << "myObj: someEvent occured...\n";
		}
	};

	// instantiate the derived object...
	myObj m;

	// dispatch "someEvent" event, both 'someEventObserver' and 'm' received the event...
	eos_dispatch::someEvent();

	// Create an observer which receives a parameter...
	eos_observer::someEventWithArg someEventWithArgObserver([](int i)
		{
			std::cout << "someEventWithArgObserver: someEventWithArg occured with value " << i << " ...\n";
		});

	// dispatch thi event with value 42 as parameter/argument...
	eos_dispatch::someEventWithArg(42);

	// custom adding class which adds the event argument to a running total...
	class myAdder : public eos_observer::someEventWithArg
	{
	public:
		void someEventWithArg_event(int toAdd)
		{
			total_ += toAdd;
			std::cout << "myAdder: someEventWithArg occured with value " << toAdd << ", current total is " << total_ << " ...\n";
		}
		int total_ = 0;
	};

	myAdder a;

	// dispatch the events with various values to be added to running total...
	eos_dispatch::someEventWithArg(1);
	eos_dispatch::someEventWithArg(2);
	eos_dispatch::someEventWithArg(3);

	// Longevity...
	
	// Obeserver interfaces follow RAII principles, thus the longevity is well-defined...
	// they register themseleves upon construction, and also unregister upon destruction...
	someNamespace::eos_observer::someEventWithTwoArg event1([](int a, float b)
		{
			std::cout << "event1: someEventWithTwoArg occured with values a:" << a << ", and b:" << b << " ...\n";
		});
	{
		// event2 observer is constructed within a block and thus will destruct (and unregister) when out of scope...
		someNamespace::eos_observer::someEventWithTwoArg event2([](int a, float b)
			{
				std::cout << "event2: someEventWithTwoArg occured with values a:" << a << ", and b:" << b << " ...\n";
			});

		// Both event1 and event2 are subscribed to 'someEventWithTwoArg' when this disptach occurs...
		someNamespace::eos_dispatch::someEventWithTwoArg(42, 43.0f);
	}

	// event2 destructs and unregisteres when out of scope and thus only event1 will observe 'someEventWithTwoArg' here...
	someNamespace::eos_dispatch::someEventWithTwoArg(44, 3.141592654f);

	return 0;
}