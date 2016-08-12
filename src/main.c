#include <pebble.h>
#include "main.h"

static Window *s_window;
static TextLayer *s_label_Day, *s_label_MonthName;


//creats the font
static GFont s_date_font;



// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.ForegroundColor = GColorWhite;
  settings.ShowMonth = false;
  settings.MonthConfig = false;
}




//this is probably not needed anymore
/*

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
  text_layer_set_text(s_label_Day, s_buffer);
}

*/


//this is what updates the screen at the intervals set at the bottom (Every day)

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  //update_time();
  prv_update_display();
  
  
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
  text_layer_set_text_color(s_label_Day, settings.ForegroundColor);
  
  
  if (settings.ShowMonth) {
  text_layer_set_text_color(s_label_MonthName, settings.ForegroundColor);
 } else {
   text_layer_set_text_color(s_label_MonthName, settings.BackgroundColor); 
  }
   
  

  
  
  
  
  /*
  // MonthNumber - but really the day nuimber
  if (settings.MonthNumber) {
    text_layer_set_text(s_label_Day, "seconds: enabled");
  } else {
    text_layer_set_text(s_label_Day, "asdf");
  }
*/
  
  
  //???yes
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
  
  //Day
    static char s_buffer[16];
  strftime(s_buffer, sizeof(s_buffer), "%d", tick_time); 
  
  //MonthNumber
  static char n_buffer[16]; 
 
     if (settings.MonthConfig) {
    strftime(n_buffer, sizeof(n_buffer), "%B", tick_time); 
     } else {
      strftime(n_buffer, sizeof(n_buffer), "%m", tick_time); 
    }

  
  // Display this time on the TextLayer
  text_layer_set_text(s_label_Day, s_buffer);
  text_layer_set_text(s_label_MonthName, n_buffer);
  
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
  Tuple *second_tick_t = dict_find(iter, MESSAGE_KEY_MonthConfig);
  if (second_tick_t) {
    settings.MonthConfig = second_tick_t->value->int32 == 1;
  }
  

  // Animations
  Tuple *MonthName_t = dict_find(iter, MESSAGE_KEY_ShowMonth);
  if (MonthName_t) {
    settings.ShowMonth = MonthName_t->value->int32 == 1;
  }

  
  // Save the new settings to persistent storage
  prv_save_settings();
}

// Window Load event
static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

    
  
  //(x shift, y shift, width? , height)
  //s_label_Day = text_layer_create(GRect(0, bounds.size.h/2, bounds.size.w, 30));
  s_label_Day = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 70));
  text_layer_set_text_alignment(s_label_Day, GTextAlignmentCenter);
  text_layer_set_background_color(s_label_Day, GColorClear); //set to GColorClear when done
  layer_add_child(window_layer, text_layer_get_layer(s_label_Day));  
  // Create GFont
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_QUICKSAND_60));
  // Apply to TextLayer
  text_layer_set_font(s_label_Day, s_date_font);
  
    
  
  //s_label_MonthName = text_layer_create(GRect(0, bounds.size.h/8, bounds.size.w, 40));
  s_label_MonthName = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(40, 40), bounds.size.w, 30));
  text_layer_set_text_alignment(s_label_MonthName, GTextAlignmentCenter);
  text_layer_set_background_color(s_label_MonthName, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(s_label_MonthName));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_QUICKSAND_20));
  // Apply to TextLayer
  text_layer_set_font(s_label_MonthName, s_date_font);

   
  
  
  prv_update_display();
}

// Window Unload event
static void prv_window_unload(Window *window) {
  layer_destroy(text_layer_get_layer(s_label_Day));
  layer_destroy(text_layer_get_layer(s_label_MonthName));
  
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
  //update_time();
  
  //added from date
  // Register with TickTimerService - this also sets the frequency of updates
  tick_timer_service_subscribe(DAY_UNIT, tick_handler);
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