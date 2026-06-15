#include <pebble.h>

#include "settings.h"
#include "src/c/main.h"

static ClaySettings settings;

ClaySettings get_settings() {
  return settings;
}

static void prv_default_settings() {
  settings.hd_gfx = true;
  settings.pilot_change = 5;
  settings.cap_change = 5;
}

void prv_load_settings() {
  prv_default_settings();
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *hd2x_t = dict_find(iter, MESSAGE_KEY_Hires);
  if (hd2x_t) {
    bool old_hd2x = settings.hd_gfx;
    settings.hd_gfx = hd2x_t->value->uint32==1;
    if (old_hd2x != settings.hd_gfx) {
      update_pilot();
    }
  }
  Tuple *pilot_change_t = dict_find(iter, MESSAGE_KEY_PilotChange);
  if (pilot_change_t) {
    settings.pilot_change = atoi(pilot_change_t->value->cstring);
  }
  Tuple *cap_change_t = dict_find(iter, MESSAGE_KEY_CapChange);
  if (cap_change_t) {
    settings.cap_change = atoi(cap_change_t->value->cstring);
  }
  prv_save_settings();
}
