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

	// dispatch "someEvent" event...
	eos_dispatch::someEvent();

	eos_observer::someEventWithArg someEventWithArgObserver([](int i)
		{
			std::cout << "someEventWithArgObserver: someEventWithArg occured with value " << i << " ...\n";
		});

	eos_dispatch::someEventWithArg(42);

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

	eos_dispatch::someEventWithArg(1);
	eos_dispatch::someEventWithArg(2);
	eos_dispatch::someEventWithArg(3);

	// Longevity...

	someNamespace::eos_observer::someEventWithTwoArg event1([](int a, float b)
		{
			std::cout << "event1: someEventWithTwoArg occured with values a:" << a << ", and b:" << b << " ...\n";
		});
	{
		someNamespace::eos_observer::someEventWithTwoArg event2([](int a, float b)
			{
				std::cout << "event2: someEventWithTwoArg occured with values a:" << a << ", and b:" << b << " ...\n";
			});

		someNamespace::eos_dispatch::someEventWithTwoArg(42, 43.0f);
	}
	someNamespace::eos_dispatch::someEventWithTwoArg(44, 3.141592654f);


	auto someEventObservers = eos::server().observers("someEvent");



	return 0;
}