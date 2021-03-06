#include "CountingBot.h"

#include "Event/EventHandler.h"
#include "Command/CommandEventHandler.h"
#include "Message/MessageEventHandler.h"

#include "Message/MessageEvent.h"

#include <HyperDiscord.h>
#include <Events/MessageEvents.h>

#ifdef DELETE
#undef DELETE
#endif

void CountingBot::Init(const char* token) {
	Logger::Init();	// Initialize the logger
	if (!token) throw std::exception("Token was not given!");

	// Create a CommandEventHandler instance and register it.
	this->commandEventHandler = new CommandEventHandler();
	EventHandler::RegisterEventHandler(this->commandEventHandler);

	// Create a MessageEventHandler instance and register it.
	this->messageEventHandler = new MessageEventHandler();
	EventHandler::RegisterEventHandler(this->messageEventHandler);

	// Create a HyperClient instance.
	this->hyperClient = new HyperDiscord::HyperClient(token, HyperDiscord::TokenType::BOT);

	this->hyperClient->OnEvent(std::bind(&CountingBot::OnHyperDiscordEvent, this, std::placeholders::_1));

	this->running = true;	// Set running to true such that it will update the CountingBot.
}

void CountingBot::Update() {
	this->hyperClient->Update();

	EventHandler::HandleEvents();	// Handle the events in the queue.
}

void CountingBot::DeInit() {
	Logger::DeInit();	// Deinitialize the logger
}

bool CountingBot::IsRunning() {
	return this->running;
}

void CountingBot::OnHyperDiscordEvent(HyperDiscord::Event& event) {
	HyperDiscord::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<HyperDiscord::MessageCreateEvent>([&](HyperDiscord::MessageCreateEvent& messageCreateEvent) {
		std::cout << "Message Create" << std::endl;
		EventHandler::PushEvent(new MessageEvent(messageCreateEvent.GetMessage(), MessageEventType::CREATE));
		return true;
		});

	dispatcher.Dispatch<HyperDiscord::MessageUpdateEvent>([&](HyperDiscord::MessageUpdateEvent& messageUpdateEvent) {
		std::cout << "Message Update" << std::endl;
		EventHandler::PushEvent(new MessageEvent(messageUpdateEvent.GetMessage(), MessageEventType::UPDATE));
		return true;
		});

	dispatcher.Dispatch<HyperDiscord::MessageDeleteEvent>([&](HyperDiscord::MessageDeleteEvent& messageDeleteEvent) {
		std::cout << "Message Delete" << std::endl;
		EventHandler::PushEvent(new MessageEvent({ messageDeleteEvent.GetId() }, MessageEventType::DELETE));
		return true;
		});
}

int main(int argc, char** argv) {
	Logger mainLogger{ "Main" };
	const char* token = nullptr;
	if (argc > 0) token = argv[1];
	CountingBot countingBot;
	try {
		countingBot.Init(token);

		try {
			while (countingBot.IsRunning()) countingBot.Update();
		} catch (std::exception e) {
			mainLogger.LogError("An exception was thrown from the countingBot.Update()!\n%s", e.what());
		}

		try {
			countingBot.DeInit();
		} catch (std::exception e) {
			mainLogger.LogError("An exception was thrown from countingBot.DeInit()!\n%s", e.what());
		}
	} catch (std::exception e) {
		mainLogger.LogError("An exception was thrown from countingBot.Init()!\n%s", e.what());
		Logger::DeInit();
	}
}