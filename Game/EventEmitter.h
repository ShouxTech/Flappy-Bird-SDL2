#pragma once
#include <string>
#include <functional>

class EventEmitter {
	public:
		static void Emit(const std::string& eventName);
		static void On(const std::string& eventName, std::function<void()> receiver);
};