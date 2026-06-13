#include <pebble.h>
#include "src/c/settings.h"
#include "src/c/main.h"

static Window *s_main_window;
static GBitmap *s_pilot_bitmap = NULL; // Initialized to NULL for safety
static BitmapLayer *s_pilot_layer;

// New variables for the Clock
static TextLayer *s_time_layer;
static GFont s_custom_font;

// Create an array of your pilot resource IDs
static const uint32_t PILOT_RESOURCES[] = {RESOURCE_ID_ELUDER, RESOURCE_ID_GUARDIAN, RESOURCE_ID_SKIFF, RESOURCE_ID_BROODHOME, RESOURCE_ID_AVATAR, RESOURCE_ID_MAULER,
                        RESOURCE_ID_CRUISER, RESOURCE_ID_AVENGER, RESOURCE_ID_MARAUDER, RESOURCE_ID_TRADER, RESOURCE_ID_XFORM, RESOURCE_ID_PODSHIP,
                        RESOURCE_ID_NEMESIS, RESOURCE_ID_FURY, RESOURCE_ID_SCOUT, RESOURCE_ID_PROBE, RESOURCE_ID_BLADE, RESOURCE_ID_PENETRATOR,
                        RESOURCE_ID_TORCH, RESOURCE_ID_DRONE, RESOURCE_ID_DREADNOUGHT, RESOURCE_ID_JUGGER, RESOURCE_ID_INTRUDER,
                        RESOURCE_ID_TERMINATOR, RESOURCE_ID_STINGER};

static const uint32_t PILOT_RESOURCES_HIRES[] = {RESOURCE_ID_ELUDER_HIRES, RESOURCE_ID_GUARDIAN_HIRES, RESOURCE_ID_SKIFF_HIRES, RESOURCE_ID_BROODHOME_HIRES, RESOURCE_ID_AVATAR_HIRES, RESOURCE_ID_MAULER_HIRES,
                          RESOURCE_ID_CRUISER_HIRES, RESOURCE_ID_AVENGER_HIRES, RESOURCE_ID_MARAUDER_HIRES, RESOURCE_ID_TRADER_HIRES, RESOURCE_ID_XFORM_HIRES, RESOURCE_ID_PODSHIP_HIRES,
                          RESOURCE_ID_NEMESIS_HIRES, RESOURCE_ID_FURY_HIRES, RESOURCE_ID_SCOUT_HIRES, RESOURCE_ID_PROBE_HIRES, RESOURCE_ID_BLADE_HIRES, RESOURCE_ID_PENETRATOR_HIRES,
                          RESOURCE_ID_TORCH_HIRES, RESOURCE_ID_DRONE_HIRES, RESOURCE_ID_DREADNOUGHT_HIRES, RESOURCE_ID_JUGGER_HIRES, RESOURCE_ID_INTRUDER_HIRES,
                          RESOURCE_ID_TERMINATOR_HIRES, RESOURCE_ID_STINGER_HIRES};

void update_pilot() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  int num_pilots = sizeof(PILOT_RESOURCES) / sizeof(PILOT_RESOURCES[0]);

  // CHANGE THIS LINE:
  // int index = tick_time->tm_yday % num_pilots; 
  int index = tick_time->tm_min % num_pilots; // This changes every minute!

  if (s_pilot_bitmap != NULL) {
    gbitmap_destroy(s_pilot_bitmap);
    s_pilot_bitmap = NULL;
  }
  
  ClaySettings s = get_settings();
  const uint32_t *resources = s.hd_gfx ? PILOT_RESOURCES_HIRES : PILOT_RESOURCES;
  s_pilot_bitmap = gbitmap_create_with_resource(resources[index]);
  bitmap_layer_set_bitmap(s_pilot_layer, s_pilot_bitmap);
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();

  // Update pilot if the day changed
  if (units_changed & MINUTE_UNIT) {
//   if (units_changed & DAY_UNIT) {
    update_pilot();
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // --- 1. PILOT LAYER SETUP ---
  s_pilot_layer = bitmap_layer_create(GRect(0, 0, bounds.size.w, 100));
  bitmap_layer_set_compositing_mode(s_pilot_layer, GCompOpSet);
  bitmap_layer_set_alignment(s_pilot_layer, GAlignCenter); 

  layer_add_child(window_layer, bitmap_layer_get_layer(s_pilot_layer));

  // --- 2. TIME SETUP ---
  s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UQM_24));

  int time_height = 40;
  s_time_layer = text_layer_create(GRect(0, bounds.size.h - time_height - 5, bounds.size.w, time_height));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_custom_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // --- 3. INITIAL LOAD ---
  update_pilot();
  update_time();
}

static void main_window_unload(Window *window) {
  if (s_pilot_bitmap) {
    gbitmap_destroy(s_pilot_bitmap);
  }
  bitmap_layer_destroy(s_pilot_layer);
  fonts_unload_custom_font(s_custom_font);
  text_layer_destroy(s_time_layer);
}

static void init() {
  prv_load_settings();
  // Create main Window element and assign to pointer
  // Open AppMessage connection
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

//   tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT, tick_handler);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
