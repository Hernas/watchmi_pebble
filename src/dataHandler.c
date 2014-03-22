#include "pebble.h"

void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered
 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }

 void in_received_handler(DictionaryIterator *received, void *context) {  
	 APP_LOG(APP_LOG_LEVEL_DEBUG, "Got some message from phone");
   
	 Tuple *numberOfItemsTuple = dict_find(received, 1);
	 int numberOfItems = 0;
	 
	 Tuple *currentItemTuple = dict_find(received, 2);
	 int currentItem = 0;

	 if (numberOfItemsTuple) {
		 numberOfItems = numberOfItemsTuple->value->uint16;
		 APP_LOG(APP_LOG_LEVEL_DEBUG, "numberOfItemsTuple: %d", numberOfItems); 
	 }
	 
	 if (currentItemTuple) {
		 currentItem = currentItemTuple->value->uint16;
		 

		 Tuple *titleTuple = dict_find(received, 10+currentItem);
		 char *title = "";
		 Tuple *subTitleTuple = dict_find(received, 11+currentItem);
		 char *subTitle = "";
		 
		 if(titleTuple) {
			 title = titleTuple->value->cstring;
		 }
		 if(subTitleTuple) {
			 subTitle = subTitleTuple->value->cstring;
		 }
		 APP_LOG(APP_LOG_LEVEL_DEBUG, "item  %s: %s", title, subTitle); 
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

