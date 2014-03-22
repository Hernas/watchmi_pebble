

Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");
  }
);


var sendItem = function(index, title, subtitle) {
    console.log('sendItem '+index);
    index = index*2;
	var data = {};
	data[2] = index;
	data[10+index] = title;
	data[11+index] = subtitle;
	console.log('Data to send: '+JSON.stringify(data));
	Pebble.sendAppMessage(data);
};

var sendResultsToPebble = function(data) {
	var dataToSend = {
		1: data.length
	};
	
	console.log('Data to send: '+JSON.stringify(dataToSend));
	
var transactionId = Pebble.sendAppMessage( dataToSend,
  function(e) {
    console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
	for(var i =0; i<data.length; i++) {
        sendItem(i, data[i][0], data[i][1]);
	}
  },
  function(e) {
    console.log("Unable to deliver message with transactionId="+ e.data.transactionId + " Error is: " + JSON.stringify(e));
  }
);
	console.log("Sending message, transactionId="+transactionId);
};


var fetchData = function(type) {
	if(typeof type == "undefined") {
		type = 0;
	}
	
	var stringType = "primetime";
	switch(type) {
		case 0:
			stringType = "primetime";
			break;
	}
 var req = new XMLHttpRequest();
	var url  = 'http://hackathon.lab.watchmi.tv/api/example.com/broadcasts/format/json/'+stringType;
	console.log('Loading "'+url+'"');
  req.open('GET', url, true);
  req.onload = function(e) {
	console.log('Loaded with status: '+req.status+', ready state: '+req.readyState);
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        var response = JSON.parse(req.responseText);
		console.log('Got '+response.totalResults+' results');
		sendResultsToPebble([['title', 'subtitle'], ['2title', 'subtitle']]);
        
      } else { console.log("Error"); }
    }
  };
  req.send(null);
};


Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + JSON. stringify (e.payload));
	fetchData(e.payload[0]);
  }
);