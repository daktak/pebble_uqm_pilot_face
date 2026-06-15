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
static GFont s_time_font;

static int s_current_pilot;
static int s_win_w;
static int s_img_y;
static int s_sep_y;
static GRect s_img_border;

static const char *RACE_NAMES[RACE_COUNT] = {
  "SPATHI", "ANDROSYNTH", "ARILOU", "CHENJESU", "CHMMR",
  "DRUUGE", "HUMAN", "ILWRATH", "KOHR-AH", "MELNORME",
  "MMRNMHRM", "MYCON", "ORZ", "PKUNK", "SHOFIXTI",
  "SLYLANDRO", "SUPOX", "SYREEN", "THRADDASH", "UMGAH",
  "UR-QUAN", "UTWIG", "VUX", "YEHAT", "ZOQ-FOT-PIK",
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
#define TIME_FONT FONT_KEY_GOTHIC_28
#define RACE_H 28
#define CAP_H 34
#define TIME_H 36
#elif defined(PBL_PLATFORM_EMERY)
#define RACE_FONT FONT_KEY_GOTHIC_18
#define CAP_FONT FONT_KEY_GOTHIC_24
#define TIME_FONT FONT_KEY_GOTHIC_28
#define RACE_H 22
#define CAP_H 30
#define TIME_H 40
#elif defined(PBL_PLATFORM_CHALK)
#define RACE_FONT FONT_KEY_GOTHIC_14
#define CAP_FONT FONT_KEY_GOTHIC_18
#define RACE_H 18
#define CAP_H 22
#define TIME_H 36
#else
#define RACE_FONT FONT_KEY_GOTHIC_14
#define CAP_FONT FONT_KEY_GOTHIC_18
#define RACE_H 18
#define CAP_H 22
#define TIME_H 36
#endif
#define GAP 2
#define PAD 4

int get_current_pilot() {
  return s_current_pilot;
}

void update_pilot() {
  ClaySettings s = get_settings();
  const uint32_t *resources = s.hd_gfx ? PILOT_RESOURCES_HIRES : PILOT_RESOURCES;

  if (s_pilot_bitmap) {
    gbitmap_destroy(s_pilot_bitmap);
  }
  s_pilot_bitmap = gbitmap_create_with_resource(resources[s_current_pilot]);

  GSize img_size = gbitmap_get_bounds(s_pilot_bitmap).size;
  int img_x = (s_win_w - img_size.w) / 2;

  bitmap_layer_set_bitmap(s_pilot_layer, s_pilot_bitmap);
  layer_set_frame(bitmap_layer_get_layer(s_pilot_layer), GRect(img_x, s_img_y, img_size.w, img_size.h));

  s_img_border = GRect(img_x - 2, s_img_y - 2, img_size.w + 4, img_size.h + 4);

  int cap_y = s_img_y + img_size.h + GAP - 1;
  layer_set_frame(text_layer_get_layer(s_cap_layer), GRect(PAD, cap_y, s_win_w - PAD * 2, CAP_H));
  s_sep_y = cap_y + CAP_H;
  if (s_border_layer) layer_mark_dirty(s_border_layer);

  update_race();
  update_captain_text();
}

void update_race() {
  text_layer_set_text(s_race_layer, RACE_NAMES[s_current_pilot]);
}

void update_captain_text() {
  text_layer_set_text(s_cap_layer, get_captain(s_current_pilot));
}

void set_pilot(int race) {
  s_current_pilot = race;
  update_pilot();
}

void change_pilot() {
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

static bool is_quiet_time(struct tm *tick_time, ClaySettings *s) {
  if (!s->quiet_time) return false;
  int h = tick_time->tm_hour;
  if (s->quiet_start < s->quiet_stop) {
    return h >= s->quiet_start && h < s->quiet_stop;
  } else {
    return h >= s->quiet_start || h < s->quiet_stop;
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();

  if (units_changed & MINUTE_UNIT) {
    ClaySettings s = get_settings();
    int min = tick_time->tm_min;

    if (is_quiet_time(tick_time, &s)) return;

    if (s.pilot_select == 0 && s.pilot_change > 0 && min % s.pilot_change == 0) {
      change_pilot();
    } else if (s.cap_change > 0 && min % s.cap_change == 0) {
      change_captain_only();
    }
  }
}

static void draw_bezel(GContext *ctx, GRect r) {
  graphics_context_set_stroke_width(ctx, 2);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(r.origin.x, r.origin.y), GPoint(r.origin.x + r.size.w - 1, r.origin.y));
  graphics_draw_line(ctx, GPoint(r.origin.x, r.origin.y), GPoint(r.origin.x, r.origin.y + r.size.h - 1));
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_draw_line(ctx, GPoint(r.origin.x + r.size.w - 1, r.origin.y), GPoint(r.origin.x + r.size.w - 1, r.origin.y + r.size.h - 1));
  graphics_draw_line(ctx, GPoint(r.origin.x, r.origin.y + r.size.h - 1), GPoint(r.origin.x + r.size.w - 1, r.origin.y + r.size.h - 1));
}

static void border_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
#if defined(PBL_COLOR)
  draw_bezel(ctx, bounds);
  if (s_img_border.size.w > 0 && s_img_border.size.h > 0) {
    draw_bezel(ctx, s_img_border);
  }
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, GPoint(PAD, s_sep_y), GPoint(s_win_w - PAD, s_sep_y));
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_draw_line(ctx, GPoint(PAD, s_sep_y + 1), GPoint(s_win_w - PAD, s_sep_y + 1));
#else
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_rect(ctx, bounds);
  if (s_img_border.size.w > 0 && s_img_border.size.h > 0) {
    graphics_draw_rect(ctx, s_img_border);
  }
  graphics_draw_line(ctx, GPoint(PAD, s_sep_y), GPoint(s_win_w - PAD, s_sep_y));
#endif
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  s_win_w = bounds.size.w;
  int h = bounds.size.h;

  int time_y = h - PAD - TIME_H;
  int race_y = PAD - 1;
  s_img_y = race_y + RACE_H + GAP;
  s_sep_y = 0;

#if defined(TIME_FONT)
  s_custom_font = NULL;
  s_time_font = fonts_get_system_font(TIME_FONT);
#else
  s_custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_UQM_24));
  s_time_font = s_custom_font;
#endif

  s_pilot_layer = bitmap_layer_create(GRect(0, s_img_y, s_win_w, 0));
  bitmap_layer_set_compositing_mode(s_pilot_layer, GCompOpSet);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_pilot_layer));

  s_race_layer = text_layer_create(GRect(PAD, race_y, s_win_w - PAD * 2, RACE_H));
  text_layer_set_background_color(s_race_layer, GColorClear);
  text_layer_set_text_color(s_race_layer, GColorWhite);
  text_layer_set_font(s_race_layer, fonts_get_system_font(RACE_FONT));
  text_layer_set_text_alignment(s_race_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_race_layer));

  s_cap_layer = text_layer_create(GRect(PAD, s_img_y, s_win_w - PAD * 2, CAP_H));
  text_layer_set_background_color(s_cap_layer, GColorClear);
  text_layer_set_text_color(s_cap_layer, GColorWhite);
  text_layer_set_font(s_cap_layer, fonts_get_system_font(CAP_FONT));
  text_layer_set_text_alignment(s_cap_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_cap_layer));

  s_time_layer = text_layer_create(GRect(PAD, time_y, s_win_w - PAD * 2, TIME_H));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  s_border_layer = layer_create(bounds);
  layer_set_update_proc(s_border_layer, border_update_proc);
  layer_add_child(window_layer, s_border_layer);

  time_t now = time(NULL);
  struct tm *tick_time = localtime(&now);
  ClaySettings s2 = get_settings();
  int ps = s2.pilot_select;
  if (ps < 0 || ps > RACE_COUNT) ps = 0;
  s_current_pilot = (ps > 0) ? (ps - 1) : (tick_time->tm_min % RACE_COUNT);
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
  if (s_custom_font) {
    fonts_unload_custom_font(s_custom_font);
  }
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
