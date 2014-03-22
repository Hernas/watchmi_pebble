#include "pebble.h"

int currentIndex;

Window *detailWindow;

void detailview_window_load(Window *window) {
    
}

void detailview_window_unload(Window *window) {
    
}

void open_detail_layer(int index) {   
    currentIndex = index;
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Detail view, index: %d", currentIndex); 
    
  detailWindow = window_create();

  // Setup the window handlers
  window_set_window_handlers(detailWindow, (WindowHandlers) {
    .load = detailview_window_load,
    .unload = detailview_window_unload,
  });

  window_stack_push(detailWindow, true /* Animated */);
    

}