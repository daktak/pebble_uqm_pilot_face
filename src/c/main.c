#include <pebble.h>

static Window *s_main_window;
static GBitmap *s_pilot_bitmap = NULL; // Initialized to NULL for safety
static BitmapLayer *s_pilot_layer;

// New variables for the Clock
static TextLayer *s_time_layer;
static GFont s_custom_font;

// Create an array of your pilot resource IDs
static const uint32_t PILOT_RESOURCES[] = {
  RESOURCE_ID_GUARDIAN_000_144,
  RESOURCE_ID_SKIFF_000_144,
  RESOURCE_ID_AVATAR_000_144,
  RESOURCE_ID_BROODHOME_000_144,
  RESOURCE_ID_MAULER_000_144,
  RESOURCE_ID_AVENGER_000_144,
  RESOURCE_ID_MARAUDER_000_144,
  RESOURCE_ID_FLAGSHIP_000_144,
  RESOURCE_ID_TRADER_000_144,
  RESOURCE_ID_CRUISER_000_144,
  RESOURCE_ID_XFORM_000_144,
  RESOURCE_ID_PODSHIP_CAP_144,
  RESOURCE_ID_NEMESIS_000_144,
  RESOURCE_ID_FURY_000_144,
//   RESOURCE_ID_SAMATRA_000_144,
  RESOURCE_ID_OLDSCOUT_000_144,
  RESOURCE_ID_PROBE_000_144,
  RESOURCE_ID_ELUDER_000_144,
  RESOURCE_ID_BLADE_000_144,
  RESOURCE_ID_PENETRATOR_000_144,
  RESOURCE_ID_TORCH_000_144,
  RESOURCE_ID_DRONE_000_144,
  RESOURCE_ID_DREADNAUGHT_000_144,
  RESOURCE_ID_JUGGER_000_144,
  RESOURCE_ID_INTRUDER_000_144,
  RESOURCE_ID_TERMINIATOR_000_144,
  RESOURCE_ID_STINGER_000_144
  
  
}; // <--- ADDED THE MISSING SEMICOLON HERE

static void update_pilot() {
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

  s_pilot_bitmap = gbitmap_create_with_resource(PILOT_RESOURCES[index]);
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