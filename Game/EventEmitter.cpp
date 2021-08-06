#include <vector>
#include "EventEmitter.h"

std::vector<std::pair<std::string, std::function<void()>>> receivers;

void EventEmitter::Emit(const std::string& eventName) {
	for (const auto& receiverData : receivers) {
		if (receiverData.first == eventName) {
			receiverData.second();
		}
	}
}

void EventEmitter::On(const std::string& eventName, std::function<void()> receiver) {
	receivers.push_back(std::make_pair(eventName, receiver));
}