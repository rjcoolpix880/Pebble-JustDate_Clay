#include <pebble.h>
#include "main.h"

static Window *s_window;
static TextLayer *s_label_secondtick, *s_label_animations;


//creats the font
static GFont s_date_font;



// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.ForegroundColor = GColorWhite;
  settings.SecondTick = false;
  settings.Animations = false;
}







//this is the date part I copied from the other watchface
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer (original time)
  //the actual date
   static char s_buffer[16];
  strftime(s_buffer, sizeof(s_buffer), "%d", tick_time); 
  

  // Display this time on the TextLayer
  text_layer_set_text(s_label_secondtick, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}















// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
  // Background color
  window_set_background_color(s_window, settings.BackgroundColor);

  // Foreground Color
  text_layer_set_text_color(s_label_secondtick, settings.ForegroundColor);
  text_layer_set_text_color(s_label_animations, settings.ForegroundColor);
/*
  // Seconds
  if (settings.SecondTick) {
    text_layer_set_text(s_label_secondtick, "seconds: enabled");
  } else {
    text_layer_set_text(s_label_secondtick, "seconds: disabled");
  }

  // Animations
  if (settings.Animations) {
    text_layer_set_text(s_label_animations, "animations: enabled");
  } else {
    text_layer_set_text(s_label_animations, "animations: disabled");
  }
  */
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }

  // Foreground Color
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColor);
  if (fg_color_t) {
    settings.ForegroundColor = GColorFromHEX(fg_color_t->value->int32);
  }

  // Second Tick
  Tuple *second_tick_t = dict_find(iter, MESSAGE_KEY_SecondTick);
  if (second_tick_t) {
    settings.SecondTick = second_tick_t->value->int32 == 1;
  }

  // Animations
  Tuple *animations_t = dict_find(iter, MESSAGE_KEY_Animations);
  if (animations_t) {
    settings.Animations = animations_t->value->int32 == 1;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

// Window Load event
static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //s_label_secondtick = text_layer_create(GRect(0, bounds.size.h/4, bounds.size.w, 30));
  s_label_secondtick = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 100));
  text_layer_set_text_alignment(s_label_secondtick, GTextAlignmentCenter);
  text_layer_set_background_color(s_label_secondtick, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_label_secondtick));  
  // Create GFont
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_QUICKSAND_60));
  // Apply to TextLayer
  text_layer_set_font(s_label_secondtick, s_date_font);

  
  s_label_animations = text_layer_create(GRect(0, bounds.size.h/2, bounds.size.w, 0));
  text_layer_set_text_alignment(s_label_animations, GTextAlignmentCenter);
  text_layer_set_background_color(s_label_animations, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_label_animations));

  prv_update_display();
}

// Window Unload event
static void prv_window_unload(Window *window) {
  layer_destroy(text_layer_get_layer(s_label_secondtick));
  layer_destroy(text_layer_get_layer(s_label_animations));
  
  // Unload GFont
  fonts_unload_custom_font(s_date_font);
}

static void prv_init(void) {
  prv_load_settings();

  // Listen for AppMessages
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });

  window_stack_push(s_window, true);
  
  //added from date
  // Make sure the time is displayed from the start
  update_time();
  //added from date
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void prv_deinit(void) {
  if (s_window) {
    window_destroy(s_window);
  }
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}