#include "events.h"

int OnResourceStart(const alt::CEvent* ev, lua_State* state) {
    auto event = dynamic_cast<const alt::CResourceStartEvent*>(ev);
    lua_pushstring(state, event->GetResource()->GetName().c_str());
    return 1;
}

void RegisterResourceEvents(handler_map& event_handlers) {
    event_handlers[alt::CEvent::Type::RESOURCE_START] = {"onResourceStart", OnResourceStart};
}