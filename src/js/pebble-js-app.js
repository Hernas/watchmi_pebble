

Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");
  }
);


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
    if (req.readyState == 4 && req.status == 200) {
      if(req.status == 200) {
        var response = JSON.parse(req.responseText);
		console.log(JSON.stringify(response));
        
	/*var transactionId = Pebble.sendAppMessage( { "0": 42, "1": "String value" },
  function(e) {
    console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
  },
  function(e) {
    console.log("Unable to deliver message with transactionId="+ e.data.transactionId + " Error is: " + e.error.message);
  }
);
console.log("Sending message, "+transactionId);*/
      } else { console.log("Error"); }
    }
  };
  req.send(null);
};


Pebble.addEventListener("appmessage",
  function(e) {
    console.log("Received message: " + JSON. stringify (e.payload));
	fetchData(e.payload.load);
  }
);