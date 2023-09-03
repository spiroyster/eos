#ifndef EOS_HPP
#define EOS_HPP

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>

namespace eos
{
	class observer_interface;

	typedef std::string event_id;
	typedef std::list<observer_interface*> observer_list;

	class server_interface
	{
		std::map<event_id, observer_list> observers_;
	public:
		void register_observer(const event_id& id, observer_interface* observer) { observers_[id].emplace_back(observer); }
		void unregister_observer(const event_id& id, observer_interface* observer) { observers_[id].remove_if([=](observer_interface* ob) { return ob == observer; }); }
		void to_front(const event_id& id, observer_interface* observer) 
		{ 
			auto obs = observers_.find(id);
			if (obs != observers_.end())
			{
				auto itr = std::find_if(obs->second.begin(), obs->second.end(), [=](const observer_interface* item) { return item == observer; });
				if (itr != obs->second.begin() && itr != obs->second.end())
					obs->second.splice(obs->second.begin(), obs->second, itr, std::next(itr));
			}
		}
		void to_back(const event_id& id, observer_interface* observer) 
		{
			auto obs = observers_.find(id);
			if (obs != observers_.end())
			{
				auto itr = std::find_if(obs->second.begin(), obs->second.end(), [=](const observer_interface* item) { return item == observer; });
				if (itr != obs->second.end())
					obs->second.splice(obs->second.end(), obs->second, itr);
			}
		}
		observer_list observers(const event_id& id) { return observers_[id]; }
		void observers(const event_id& id, std::function<eos::observer_list(const eos::observer_list&)> predicate) { observers_[id] = predicate(observers_[id]); }
	};

	extern server_interface& server();

	class observer_interface
	{
		event_id id_;
	public:
		observer_interface(const event_id& id) : id_(id) { server().register_observer(id_, this); }
		virtual ~observer_interface() { server().unregister_observer(id_, this); }
		void observer_to_front() { server().to_front(id_, this); }
		void observer_to_back() { server().to_back(id_, this); }
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
	static void EOS_DECLARE_NAME(name)(std::function<eos::observer_list(const eos::observer_list&)> predicate = nullptr)\
	{\
		auto observers = predicate ? predicate(eos::server().observers(#name)) : eos::server().observers(#name);\
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
	static void EOS_DECLARE_NAME(name)(argt1 argv1, std::function<eos::observer_list(const eos::observer_list&)> predicate = nullptr)\
	{\
		auto observers = predicate ? predicate(eos::server().observers(#name)) : eos::server().observers(#name);\
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
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2, std::function<eos::observer_list(const eos::observer_list&)> predicate = nullptr)\
	{\
		auto observers = predicate ? predicate(eos::server().observers(#name)) : eos::server().observers(#name);\
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
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2, argt3 argv3, std::function<eos::observer_list(const eos::observer_list&)> predicate = nullptr)\
	{\
		auto observers = predicate ? predicate(eos::server().observers(#name)) : eos::server().observers(#name);\
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
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2, argt3 argv3, argt4 argv4, std::function<eos::observer_list(const eos::observer_list&)> predicate = nullptr)\
	{\
		auto observers = predicate ? predicate(eos::server().observers(#name)) : eos::server().observers(#name);\
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
	static void EOS_DECLARE_NAME(name)(argt1 argv1, argt2 argv2, argt3 argv3, argt4 argv4, argt5 argv5, std::function<eos::observer_list(const eos::observer_list&)> predicate = nullptr)\
	{\
		auto observers = predicate ? predicate(eos::server().observers(#name)) : eos::server().observers(#name);\
		for (auto o = observers.begin(); o != observers.end(); ++o) { dynamic_cast<eos_observer::EOS_DECLARE_NAME(name)*>(*o)->EOS_DECLARE_METHOD(name)(argv1, argv2, argv3, argv4, argv5); }\
	}\
}

#endif // EOS_HPP
