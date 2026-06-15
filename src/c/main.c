#include <pebble.h>
#include "src/c/races.h"
#include "src/c/captain.h"
#include "src/c/settings.h"
#include "src/c/main.h"

static Window *s_main_window;
static GBitmap *s_pilot_bitmap;
static BitmapLayer *s_pilot_layer;
static TextLayer *s_time_layer;
static TextLayer *s_race_layer;
static TextLayer *s_cap_layer;
static Layer *s_border_layer;
static GFont s_custom_font;

static int s_current_pilot;
static GRect s_race_rect;
static GRect s_cap_rect;

static const char *RACE_NAMES[RACE_COUNT] = {
  "Spathi", "Androsynth", "Arilou", "Chenjesu", "Chmmr",
  "Druuge", "Human", "Ilwrath", "Kohr-Ah", "Melnorme",
  "Mmrnmhrm", "Mycon", "Orz", "Pkunk", "Shofixti",
  "Slylandro", "Supox", "Syreen", "Thraddash", "Umgah",
  "Ur-Quan", "Utwig", "Vux", "Yehat", "Zoq-Fot-Pik",
};

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

#if defined(PBL_PLATFORM_GABBRO)
#define RACE_FONT FONT_KEY_GOTHIC_24
#define CAP_FONT FONT_KEY_GOTHIC_28
#elif defined(PBL_PLATFORM_EMERY)
#define RACE_FONT FONT_KEY_GOTHIC_18
#define CAP_FONT FONT_KEY_GOTHIC_24
#elif defined(PBL_PLATFORM_CHALK)
#define RACE_FONT FONT_KEY_GOTHIC_14
#define CAP_FONT FONT_KEY_GOTHIC_18
#else
#define RACE_FONT FONT_KEY_GOTHIC_14
#define CAP_FONT FONT_KEY_GOTHIC_18
#endif

int get_current_pilot() {
  return s_current_pilot;
}

static void set_initial_pilot() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  s_current_pilot = tick_time->tm_min % RACE_COUNT;
}

void update_pilot() {
  ClaySettings s = get_settings();
  const uint32_t *resources = s.hd_gfx ? PILOT_RESOURCES_HIRES : PILOT_RESOURCES;

  if (s_pilot_bitmap) {
    gbitmap_destroy(s_pilot_bitmap);
  }
  s_pilot_bitmap = gbitmap_create_with_resource(resources[s_current_pilot]);
  bitmap_layer_set_bitmap(s_pilot_layer, s_pilot_bitmap);

  update_race();
  update_captain_text();
}

void update_race() {
  text_layer_set_text(s_race_layer, RACE_NAMES[s_current_pilot]);
}

void update_captain_text() {
  text_layer_set_text(s_cap_layer, get_captain(s_current_pilot));
}

static void change_pilot() {
  s_current_pilot = rand() % RACE_COUNT;
  update_pilot();
}

static void change_captain_only() {
  update_captain_text();
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

  if (units_changed & MINUTE_UNIT) {
    ClaySettings s = get_settings();
    int min = tick_time->tm_min;

    if (s.pilot_change > 0 && min % s.pilot_change == 0) {
      change_pilot();
    } else if (s.cap_change > 0 && min % s.cap_change == 0) {
      change_captain_only();
    }
  }
}

static void border_update_proc(Layer *layer, GContext *ctx) {
#if defined(PBL_COLOR)
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(s_race_rect.origin.x, s_race_rect.origin.y), GPoint(s_race_rect.origin.x + s_race_rect.size.w - 1, s_race_rect.origin.y));
  graphics_draw_line(ctx, GPoint(s_race_rect.origin.x, s_race_rect.origin.y), GPoint(s_race_rect.origin.x, s_race_rect.origin.y + s_race_rect.size.h - 1));
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_draw_line(ctx, GPoint(s_race_rect.origin.x + s_race_rect.size.w - 1, s_race_rect.origin.y), GPoint(s_race_rect.origin.x + s_race_rect.size.w - 1, s_race_rect.origin.y + s_race_rect.size.h - 1));
  graphics_draw_line(ctx, GPoint(s_race_rect.origin.x, s_race_rect.origin.y + s_race_rect.size.h - 1), GPoint(s_race_rect.origin.x + s_race_rect.size.w - 1, s_race_rect.origin.y + s_race_rect.size.h - 1));

  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(s_cap_rect.origin.x, s_cap_rect.origin.y), GPoint(s_cap_rect.origin.x + s_cap_rect.size.w - 1, s_cap_rect.origin.y));
  graphics_draw_line(ctx, GPoint(s_cap_rect.origin.x, s_cap_rect.origin.y), GPoint(s_cap_rect.origin.x, s_cap_rect.origin.y + s_cap_rect.size.h - 1));
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_draw_line(ctx, GPoint(s_cap_rect.origin.x + s_cap_rect.size.w - 1, s_cap_rect.origin.y), GPoint(s_cap_rect.origin.x + s_cap_rect.size.w - 1, s_cap_rect.origin.y + s_cap_rect.size.h - 1));
  graphics_draw_line(ctx, GPoint(s_cap_rect.origin.x, s_cap_rect.origin.y + s_cap_rect.size.h - 1), GPoint(s_cap_rect.origin.x + s_cap_rect.size.w - 1, s_cap_rect.origin.y + s_cap_rect.size.h - 1));
#else
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_rect(ctx, s_race_rect);
  graphics_draw_rect(ctx, s_cap_rect);
#endif
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int w = bounds.size.w;
  int h = bounds.size.h;

  int pilot_h = h * 55 / 100;
  int time_h = 36;
  int time_y = h - time_h - 4;

  int info_y = pilot_h + 4;
  int info_h = time_y - info_y - 4;

  int race_h = info_h / 2;
  int cap_h = info_h - race_h;

  s_race_rect = GRect(2, info_y + 1, w - 4, race_h - 2);
  s_cap_rect = GRect(2, info_y + race_h + 1, w - 4, cap_h - 2);

  s_pilot_layer = bitmap_layer_create(GRect(0, 0, w, pilot_h));
  bitmap_layer_set_compositing_mode(s_pilot_layer, GCompOpSet);
  bitmap_layer_set_alignment(s_pilot_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_pilot_layer));

  s_race_layer = text_layer_create(GRect(s_race_rect.origin.x + 3, s_race_rect.origin.y + 3, s_race_rect.size.w - 6, s_race_rect.size.h - 6));
  text_layer_set_background_color(s_race_layer, GColorClear);
  text_layer_set_text_color(s_race_layer, GColorWhite);
  text_layer_set_font(s_race_layer, fonts_get_system_font(RACE_FONT));
  text_layer_set_text_alignment(s_race_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_race_layer));

  s_cap_layer = text_layer_create(GRect(s_cap_rect.origin.x + 3, s_cap_rect.origin.y + 3, s_cap_rect.size.w - 6, s_cap_rect.size.h - 6));
  text_layer_set_background_color(s_cap_layer, GColorClear);
  text_layer_set_text_color(s_cap_layer, GColorWhite);
  text_layer_set_font(s_cap_layer, fonts_get_system_font(CAP_FONT));
  text_layer_set_text_alignment(s_cap_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_cap_layer));

  s_border_layer = layer_create(bounds);
  layer_set_update_proc(s_border_layer, border_update_proc);
  layer_add_child(window_layer, s_border_layer);

  s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UQM_24));
  s_time_layer = text_layer_create(GRect(0, time_y, w, time_h));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_custom_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  set_initial_pilot();
  update_pilot();
  update_time();
}

static void main_window_unload(Window *window) {
  if (s_pilot_bitmap) {
    gbitmap_destroy(s_pilot_bitmap);
  }
  bitmap_layer_destroy(s_pilot_layer);
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_race_layer);
  text_layer_destroy(s_cap_layer);
  layer_destroy(s_border_layer);
  fonts_unload_custom_font(s_custom_font);
}

static void init() {
  prv_load_settings();
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  time_t now = time(NULL);
  srand(now);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
  return 0;
}
