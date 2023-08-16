#ifndef EOS_HPP
#define EOS_HPP

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>

namespace eos
{
	typedef std::string event_id;
	class observer_interface;

	class server_interface
	{
	protected:
		std::map<event_id, std::list<observer_interface*>> observers_;
	public:
		void register_observer(const event_id& id, observer_interface* observer) { observers_[id].emplace_back(observer); }
		void unregister_observer(const event_id& id, observer_interface* observer) { observers_[id].remove_if([=](observer_interface* ob) { return ob == observer; }); }
		void to_front(const event_id& id, observer_interface* observer) {}
		void to_back(const event_id& id, observer_interface* observer) {}
		std::list<observer_interface*> observers(const event_id& id) { return observers_[id]; }
	};

	extern server_interface& server();

	class observer_interface
	{
		event_id id_;
	public:
		observer_interface(const event_id& id) : id_(id) { server().register_observer(id_, this); }
		virtual ~observer_interface() { server().unregister_observer(id_, this); }
		void observer_interface_to_front() { server().to_front(id_, this); }
		void observer_interface_to_back() { server().to_back(id_, this); }
		const event_id& id() const { return id_; }
	};

}

#define EOS_INIT \
namespace eos\
{\
	std::unique_ptr<server_interface> server_instance_ = std::make_unique<server_interface>();\
	server_interface& server() { return *server_instance_.get(); }\
}

#define EOS_SERVER \
eos::server_interface& eos_server_impl();\
namespace eos { server_interface& server() { return eos_server_impl(); } }\
eos::server_interface& eos_server_impl()



#define EOS_DECLARE_METHOD(x) x ## _event
#define EOS_DECLARE_NAME(x) x

#define EOS_EVENT(name)\
namespace eos_observer\
{\
	class EOS_DECLARE_NAME(name) : public eos::observer_interface\
	{\
		std::function<void()> callback_;\
	public:\
		EOS_DECLARE_NAME(name)(std::function<void()> callback = 0) : eos::observer_interface(#name), callback_(callback) {}\
		virtual void EOS_DECLARE_METHOD(name)() { callback_(); }\
	};\
}\
namespace eos_dispatch\
{\
	static void EOS_DECLARE_NAME(name)()\
	{\
		auto observers = eos::server().observers(#name);\
		for (auto o = observers.begin(); o != observers.end(); ++o) { dynamic_cast<eos_observer::EOS_DECLARE_NAME(name)*>(*o)->EOS_DECLARE_METHOD(name)(); }\
	}\
}

#define EOS_EVENT_1(name, argt1, argv1)\
namespace eos_observer\
{\
	class EOS_DECLARE_NAME(name) : public eos::observer_interface\
	{\
		std::function<void(argt1)> callback_;\
	public:\
		EOS_DECLARE_NAME(name)(std::function<void(argt1)> callback = 0) : eos::observer_interface(#name), callback_(callback) {}\
		virtual void EOS_DECLARE_METHOD(name)(argt1 argv1) { callback_(argv1); }\
	};\
}\
namespace eos_dispatch\
{\
	static void EOS_DECLARE_NAME(name)(argt1 argv1)\
	{\
		auto observers = eos::server().observers(#name);\
		for (auto o = observers.begin(); o != observers.end(); ++o) { dynamic_cast<eos_observer::EOS_DECLARE_NAME(name)*>(*o)->EOS_DECLARE_METHOD(name)(argv1); }\
	}\
}

#define EOS_EVENT_2(name, argt1, argv1, argt2, argv2)\
namespace eos_observer\
{\
	class EOS_DECLARE_NAME(name) : public eos::observer_interface\
	{\
		std::function<void(argt1, argt2)> callback_;\
	public:\
		EOS_DECLARE_NAME(name)(std::function<void(argt1, argt2)> callback = 0) : eos::observer_interface(#name), callback_(callback) {}\
		virtual void EOS_DECLARE_METHOD(name)(argt1 argv1, argt2 argv2) { callback_(argv1, argv2); }\
	};\
}\
namespace eos_dispatch\
{\
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2)\
	{\
		auto observers = eos::server().observers(#name);\
		for (auto o = observers.begin(); o != observers.end(); ++o) { dynamic_cast<eos_observer::EOS_DECLARE_NAME(name)*>(*o)->EOS_DECLARE_METHOD(name)(argv1, argv2); }\
	}\
}

#define EOS_EVENT_3(name, argt1, argv1, argt2, argv2, argt3, argv3)\
namespace eos_observer\
{\
	class EOS_DECLARE_NAME(name) : public eos::observer_interface\
	{\
		std::function<void(argt1, argt2, argt3)> callback_;\
	public:\
		EOS_DECLARE_NAME(name)(std::function<void(argt1, argt2, argt3)> callback = 0) : eos::observer_interface(#name), callback_(callback) {}\
		virtual void EOS_DECLARE_METHOD(name)(argt1 argv1, argt2 argv2, argt3 argv3) { callback_(argv1, argv2, argv3); }\
	};\
}\
namespace eos_dispatch\
{\
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2, argt3 argv3)\
	{\
		auto observers = eos::server().observers(#name);\
		for (auto o = observers.begin(); o != observers.end(); ++o) { dynamic_cast<eos_observer::EOS_DECLARE_NAME(name)*>(*o)->EOS_DECLARE_METHOD(name)(argv1, argv2, argv3); }\
	}\
}

#define EOS_EVENT_4(name, argt1, argv1, argt2, argv2, argt3, argv3, argt4, argv4)\
namespace eos_observer\
{\
	class EOS_DECLARE_NAME(name) : public eos::observer_interface\
	{\
		std::function<void(argt1, argt2, argt3, argv4)> callback_;\
	public:\
		EOS_DECLARE_NAME(name)(std::function<void(argt1, argt2, argt3, argt4)> callback = 0) : eos::observer_interface(#name), callback_(callback) {}\
		virtual void EOS_DECLARE_METHOD(name)(argt1 argv1, argt2 argv2, argt3 argv3, argt4 argv4) { callback_(argv1, argv2, argv3, argv4); }\
	};\
}\
namespace eos_dispatch\
{\
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2, argt3 argv3, argt4 argv4)\
	{\
		auto observers = eos::server().observers(#name);\
		for (auto o = observers.begin(); o != observers.end(); ++o) { dynamic_cast<eos_observer::EOS_DECLARE_NAME(name)*>(*o)->EOS_DECLARE_METHOD(name)(argv1, argv2, argv3, argv4); }\
	}\
}

#define EOS_EVENT_5(name, argt1, argv1, argt2, argv2, argt3, argv3, argt4, argv4, argt5, argv5)\
namespace eos_observer\
{\
	class EOS_DECLARE_NAME(name) : public eos::observer_interface\
	{\
		std::function<void(argt1, argt2, argt3, argt4, argt5)> callback_;\
	public:\
		EOS_DECLARE_NAME(name)(std::function<void(argt1, argt2, argt3, argt4, argt5)> callback = 0) : eos::observer_interface(#name), callback_(callback) {}\
		virtual void EOS_DECLARE_METHOD(name)(argt1 argv1, argt2 argv2, argt3 argv3, argt4 argv4, argt5 argv5) { callback_(argv1, argv2, argv3, argv4, argv5); }\
	};\
}\
namespace eos_dispatch\
{\
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2, argt3 argv3, argt4 argv4, argt5 argv5)\
	{\
		auto observers = eos::server().observers(#name);\
		for (auto o = observers.begin(); o != observers.end(); ++o) { dynamic_cast<eos_observer::EOS_DECLARE_NAME(name)*>(*o)->EOS_DECLARE_METHOD(name)(argv1, argv2, argv3, argv4, argv5); }\
	}\
}

#endif // EOS_HPP
