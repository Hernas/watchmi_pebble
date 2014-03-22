    #include "pebble.h"
    #include "dataHandler.h"
    #include "string.h"
    
void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }

int numberOfItems = 0;
 void in_received_handler(DictionaryIterator *received, void *context) {  
	 APP_LOG(APP_LOG_LEVEL_DEBUG, "Got some message from phone");
   
	 Tuple *numberOfItemsTuple = dict_find(received, 1);
	 
	 Tuple *currentItemTuple = dict_find(received, 2);
	 int currentItem = 0;

	 if (numberOfItemsTuple) {
		 numberOfItems = numberOfItemsTuple->value->uint16;
		 APP_LOG(APP_LOG_LEVEL_DEBUG, "numberOfItemsTuple: %d", numberOfItems); 
         
         titles = malloc(numberOfItems * sizeof(char*));
         subTitles = malloc(numberOfItems * sizeof(char*));
     }
	 if (currentItemTuple) {
		 currentItem = currentItemTuple->value->uint16;
		 

		 Tuple *titleTuple = dict_find(received, 3);
		 char *title = "";
		 Tuple *subTitleTuple = dict_find(received, 4);
		 char *subTitle = "";
		 
		 if(titleTuple) {
			 title = titleTuple->value->cstring;
             //titles[currentItem] = title;
             titles[currentItem] = malloc(strlen(title) * sizeof(char*));
             strcpy(titles[currentItem], title);
		 }
		 if(subTitleTuple) {
			 subTitle = subTitleTuple->value->cstring;
             //subTitles[currentItem] = subTitle;
             subTitles[currentItem] = malloc(strlen(subTitle) * sizeof(char*));
             strcpy(subTitles[currentItem], subTitle);
		 }
		 APP_LOG(APP_LOG_LEVEL_DEBUG, "item%d  %s: %s", currentItem, title, subTitle); 
	 }
	 Tuple *endedTuple = dict_find(received, 5);
         if(endedTuple) {
             APP_LOG(APP_LOG_LEVEL_DEBUG, "Last item received! Total %d", numberOfItems); 
             watchme_loaded_callback(numberOfItems, titles, subTitles);
         }
 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }

void init_data_handler() {
   app_message_register_inbox_received(in_received_handler);
   app_message_register_inbox_dropped(in_dropped_handler);
   app_message_register_outbox_sent(out_sent_handler);
   app_message_register_outbox_failed(out_failed_handler);


   const uint32_t inbound_size = 64;
   const uint32_t outbound_size = 64;
   app_message_open(inbound_size, outbound_size);
}

