#include <pebble.h>

#include "settings.h"
#include "src/c/races.h"
#include "src/c/main.h"

static ClaySettings settings;

ClaySettings get_settings() {
  return settings;
}

static void prv_default_settings() {
  settings.hd_gfx = true;
  settings.pilot_select = 0;
  settings.pilot_change = 5;
  settings.cap_change = 5;
  settings.quiet_time = false;
  settings.quiet_start = 23;
  settings.quiet_stop = 6;
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
  Tuple *pilot_select_t = dict_find(iter, MESSAGE_KEY_PilotSelect);
  if (pilot_select_t) {
    int old = settings.pilot_select;
    int val = atoi(pilot_select_t->value->cstring);
    if (val < 0) val = 0;
    if (val > RACE_COUNT) val = RACE_COUNT;
    settings.pilot_select = val;
    if (old != settings.pilot_select) {
      if (val > 0) {
        set_pilot(val - 1);
      } else {
        change_pilot();
      }
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
  Tuple *quiet_time_t = dict_find(iter, MESSAGE_KEY_PilotQuietTime);
  if (quiet_time_t) {
    settings.quiet_time = quiet_time_t->value->uint32 == 1;
  }
  Tuple *quiet_start_t = dict_find(iter, MESSAGE_KEY_PilotQuietStart);
  if (quiet_start_t) {
    settings.quiet_start = quiet_start_t->value->int32;
  }
  Tuple *quiet_stop_t = dict_find(iter, MESSAGE_KEY_PilotQuietStop);
  if (quiet_stop_t) {
    settings.quiet_stop = quiet_stop_t->value->int32;
  }
  prv_save_settings();
}
