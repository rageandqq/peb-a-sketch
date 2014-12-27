#include <pebble.h>

static Window *main_window;
static TextLayer *x_dir_layer;
static TextLayer *y_dir_layer;

//Constants
static int numSamples = 3;
static int leftThresh = -150;
static int rightThresh = 200;
static int upThresh = 200;
static int downThresh = -200;

static void data_handler(AccelData *data, uint32_t num_samples) {
  
  float numLeft, numRight, numUp, numDown;
  numLeft = numRight = numUp = numDown = 0;
  
  for (uint32_t i = 0; i < num_samples; i++) {
    if (data[i].x < leftThresh) {
      numLeft++;
    }
    else if (data[i].x > rightThresh) {
      numRight++;
    }
    
    if (data[i].y < downThresh) {
      numDown++;
    }
    else if (data[i].y > upThresh) {
      numUp++;
    }
  }
  
  //Graphical display for debugging
  if (numLeft/num_samples >= 0.5) {
    text_layer_set_text(x_dir_layer, "X: LEFT");
  }
  else if (numRight/num_samples >= 0.5) {
    text_layer_set_text(x_dir_layer, "X: RIGHT");
  }
  else {
    text_layer_set_text(x_dir_layer, "X: NONE");
  }
  
  if (numUp/num_samples >= 0.5) {
    text_layer_set_text(y_dir_layer, "Y: UP");
  }
  else if (numDown/num_samples >= 0.5) {
    text_layer_set_text(y_dir_layer, "Y: DOWN");
  }
  else {
    text_layer_set_text(y_dir_layer, "Y: NONE");
  }
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  
  x_dir_layer = text_layer_create(GRect(5, 0, window_bounds.size.w-10, window_bounds.size.h));
  y_dir_layer = text_layer_create(GRect(5, 40, window_bounds.size.w-10, window_bounds.size.h));
  text_layer_set_font(x_dir_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(y_dir_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(x_dir_layer, "X: NONE");
  text_layer_set_text(y_dir_layer, "Y: NONE");
  
  layer_add_child(window_layer, text_layer_get_layer(x_dir_layer));
  layer_add_child(window_layer, text_layer_get_layer(y_dir_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(x_dir_layer);
  text_layer_destroy(y_dir_layer);
}

static void init()  {
  main_window = window_create();
  
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  window_stack_push(main_window, true);
  accel_data_service_subscribe(numSamples, data_handler);

  accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
}

static void deinit() {
  window_destroy(main_window);
  accel_data_service_unsubscribe();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}