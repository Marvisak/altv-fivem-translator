#pragma once

#include "../main.h"

struct EventHandler {
    std::string name;
    std::function<int(const alt::CEvent*, lua_State*)> args;
};

typedef std::map<alt::CEvent::Type, EventHandler> handler_map;

extern handler_map event_handlers;

void RegisterResourceEvents(handler_map& event_handlers);

void RegisterEvents();
