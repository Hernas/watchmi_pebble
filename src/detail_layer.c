#include "pebble.h"
#include "dataHandler.h"	
    
int currentChannel;
    
    int numberOfDetails = 0;
char **currentDetailsTitles;
char **currentDetailsSubTitles;
char *currentTitle;
static Window *details_window;

// This is a menu layer
// You have more control than with a simple menu layer
static MenuLayer *details_menu_layer;

// You can draw arbitrary things in a menu item such as a background
static GBitmap *details_menu_background;

// A callback is used to specify the amount of sections of menu items
// With this, you can dynamically add and remove sections
static uint16_t details_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

// Each section has a number of items;  we use a callback to specify this
// You can also dynamically add and remove items using this
static uint16_t details_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      if(numberOfDetails>0) {
          return numberOfDetails;
      } else {
          return 1;
      }

    default:
      return 0;
  }
}

// A callback is used to specify the height of the section header
static int16_t details_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  // This is a define provided in pebble.h that you may use for the default height
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void details_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, currentTitle);
      break;
  }
}

// This is the menu item draw callback where you specify what each item should look like
static void details_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
    int index = cell_index->row;
    if(index<numberOfDetails) {
           menu_cell_basic_draw(ctx, cell_layer, currentDetailsTitles[index], currentDetailsSubTitles[index], NULL);
    } else {   
           menu_cell_basic_draw(ctx, cell_layer, "Loading...", "Please wait", NULL);
    }
}

// Here we capture when a user selects a menu item
void details_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {

}

// This initializes the menu upon window load
void details_window_load(Window *window) {
 
  // And also load the background
  details_menu_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND_BRAINS);

  // Now we prepare to initialize the menu layer
  // We need the bounds to specify the menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  details_menu_layer = menu_layer_create(bounds);

  // Set all the callbacks for the menu layer
  menu_layer_set_callbacks(details_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = details_menu_get_num_sections_callback,
    .get_num_rows = details_menu_get_num_rows_callback,
    .get_header_height = details_menu_get_header_height_callback,
    .draw_header = details_menu_draw_header_callback,
    .draw_row = details_menu_draw_row_callback,
    .select_click = details_menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(details_menu_layer, window);

  // Add it to the window for display
  layer_add_child(window_layer, menu_layer_get_layer(details_menu_layer));
}

void details_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(details_menu_layer);


  // And cleanup the background
  gbitmap_destroy(details_menu_background);
    
    
    for(int i=0; i<numberOfDetails;i++)
    {
        free(currentDetailsTitles[i]);
        free(currentDetailsSubTitles[i]);
    }
  numberOfDetails = 0;
    free(currentDetailsTitles);
    free(currentDetailsSubTitles);
}


static bool details_send_to_phone(int index) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter == NULL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "null iter");
    return false;
  }

  Tuplet tuple = TupletInteger(0, 3);
  dict_write_tuplet(iter, &tuple);
    

  Tuplet tuple2 = TupletInteger(1, index);
  dict_write_tuplet(iter, &tuple2);
  dict_write_end(iter);

  app_message_outbox_send();
  return true;
}

void details_watchme_data_loaded(int count, char **titles, char **subTitles) {
    
  numberOfDetails = count;
    currentDetailsTitles = titles;
    currentDetailsSubTitles = subTitles; 
    menu_layer_reload_data(details_menu_layer);
}

void open_details(int index, char *title) {   
    currentTitle = title;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Details"); 
    
  details_window = window_create();

  // Setup the window handlers
  window_set_window_handlers(details_window, (WindowHandlers) {
    .load = details_window_load,
    .unload = details_window_unload,
  });

  window_stack_push(details_window, true /* Animated */);
    
    
    watchme_loaded_callback = details_watchme_data_loaded;
	details_send_to_phone(index);
}

