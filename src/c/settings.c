#include <pebble.h>

#include "settings.h"
#include "src/c/main.h"

static ClaySettings settings;

/* helper - put int in buffer and log
void log_int(int num) {
  static char s_buffer[10];
  snprintf(s_buffer, 10, "%i", num);
  APP_LOG(APP_LOG_LEVEL_INFO, s_buffer);
}*/

ClaySettings get_settings() {
  return settings;
}

// Initialize the default settings
static void prv_default_settings() {
  settings.hd_gfx = true;
}

void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

//inbox
void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *hd2x_t = dict_find(iter, MESSAGE_KEY_Hires);
  if (hd2x_t) {
    bool old_hd2x = settings.hd_gfx;
    settings.hd_gfx = hd2x_t->value->uint32==1;
    if (old_hd2x != settings.hd_gfx) {
      update_pilot();
    }
  }
  prv_save_settings();
}
