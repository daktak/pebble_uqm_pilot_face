#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1

typedef struct ClaySettings {
  bool hd_gfx;
} ClaySettings;

void prv_inbox_received_handler(DictionaryIterator *iter, void *context);
void prv_load_settings();
ClaySettings get_settings();
