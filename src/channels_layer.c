#include "pebble.h"
#include "dataHandler.h"	
#include "feature_menu_layer.h"
    
int currentChannel;
    
    int numberOfChannels = 0;
char **currentChannelsTitles;
char **currentChannelsSubTitles;

static Window *channels_window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *channels_menu_layer;

// You can draw arbitrary things in a menu item such as a background
static GBitmap *channels_menu_background;

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t channels_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t channels_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      if(numberOfChannels>0) {
          return numberOfChannels;
      } else {
          return 1;
      }

    default:
      return 0;
  }
}

// A callback is used to specify the height of the section header
static int16_t channels_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void channels_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "Channels");
      break;
  }
}

// This is the menu item draw callback where you specify what each item should look like
static void channels_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    int index = cell_index->row;
    if(index<numberOfChannels) {
           menu_cell_basic_draw(ctx, cell_layer, currentChannelsTitles[index], currentChannelsSubTitles[index], NULL);
    } else {   
           menu_cell_basic_draw(ctx, cell_layer, "Loading...", "Please wait", NULL);
    }
}

// Here we capture when a user selects a menu item
void channels_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
    open_advanced_menu(cell_index->row, currentChannelsTitles[cell_index->row]);
}

// This initializes the menu upon window load
void channels_window_load(Window *window) {
 
  // And also load the background
  channels_menu_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_BRAINS);

  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  channels_menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(channels_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = channels_menu_get_num_sections_callback,
    .get_num_rows = channels_menu_get_num_rows_callback,
    .get_header_height = channels_menu_get_header_height_callback,
    .draw_header = channels_menu_draw_header_callback,
    .draw_row = channels_menu_draw_row_callback,
    .select_click = channels_menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(channels_menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(channels_menu_layer));
}

void channels_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(channels_menu_layer);


  // And cleanup the background
  gbitmap_destroy(channels_menu_background);
    
    
  numberOfChannels = 0;
    free(currentChannelsTitles);
    free(currentChannelsSubTitles);
}


static bool channels_send_to_phone(int key, int index) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "null iter");
    return false;
  }

  Tuplet tuple = TupletInteger(key, index);
  dict_write_tuplet(iter, &tuple);
  dict_write_end(iter);

  app_message_outbox_send();
  return true;
}

void channels_watchme_data_loaded(int count, char **titles, char **subTitles) {
    
  numberOfChannels = count;
    currentChannelsTitles = titles;
    currentChannelsSubTitles = subTitles; 
    menu_layer_reload_data(channels_menu_layer);
}

void open_channels() {   
    
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Channels"); 
    
  channels_window = window_create();

  // Setup the window handlers
  window_set_window_handlers(channels_window, (WindowHandlers) {
    .load = channels_window_load,
    .unload = channels_window_unload,
  });

  window_stack_push(channels_window, true /* Animated */);
    
    
    watchme_loaded_callback = channels_watchme_data_loaded;
	channels_send_to_phone(0, 1);
}

