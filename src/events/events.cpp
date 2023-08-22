#include "events.h"

handler_map event_handlers {};

void RegisterEvents() {
    RegisterResourceEvents(event_handlers);
}