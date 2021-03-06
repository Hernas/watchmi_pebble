var currentType = 0;

Pebble.addEventListener("ready",
  function (e) {
    console.log("JavaScript app ready and running!");
  }
);
var itemsToSend;


var sendNextItem = function (index, type) {
  if (itemsToSend.length > index) {
    var title = itemsToSend[index][0];
    var subtitle = itemsToSend[index][1];
    console.log('sendItem: ' + index + ' type: ' + type);


    var data = {};
    data[2] = index;

    if (type === 0) {
      data[3] = title;
    }
    if (type === 1) {
      data[4] = subtitle;
    }
    console.log('Data to send: ' + JSON.stringify(data));
    Pebble.sendAppMessage(data, function () {
      var typeToSend = type ? 0 : 1;
      sendNextItem(index + type, typeToSend);
    }, function () {
      console.log('Item ' + index + ' fucked');
      sendNextItem(index, type);
    });
  } else {

    Pebble.sendAppMessage({"5": 1}, function () {
      console.log('End sent');
    }, function () {
      console.log('End fucked');
    });
  }
};
var sendResultsToPebble = function (data) {
  var dataToSend = {
    1: data.length
  };

  console.log('Data to send: ' + JSON.stringify(dataToSend));

  var transactionId = Pebble.sendAppMessage(dataToSend,
    function (e) {
      console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
      itemsToSend = data;
      sendNextItem(0, 0);
    },
    function (e) {
      console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + JSON.stringify(e));
    }
  );
  console.log("Sending message, transactionId=" + transactionId);
};
var formatDate = function (time) {
  var date = new Date(time * 1000);
// hours part from the timestamp
  var hours = ("0" + date.getHours()).substr(0, 2);
// minutes part from the timestamp
  var minutes = ("0" + date.getMinutes()).substr(0, 2);

// will display time in 10:30:23 format
  return hours + ':' + minutes;
};

var items = [];
var latestItems = [];
var generateItemDetails = function (index) {
  var data = [
    [' '],
    [' ']
  ];
  if (latestItems[index]) {
    data = [];
    var item = latestItems[index];
    //console.log('Detail parsing item: ' + JSON.stringify(item));
    data.push(['Channel', item.channelNameLong]);
    if (item.epgData) {
      data.push(['Title', item.epgData.tit[0].value]);
      if(item.epgData.time) {
        data.push(['Starts', formatDate(item.epgData.time.strt)]);
        data.push(['Ends', formatDate(item.epgData.time.end)]);
      }
      if(item.epgData.chr && item.epgData.chr.length>=1) {
        data.push(['Character', item.epgData.chr[0].value]);
      }
      if(item.epgData.cat && item.epgData.cat.length>=1) {
        data.push(['Category', item.epgData.cat[0].value]);
      }
      if(item.epgData.pg && item.epgData.pg.val && item.epgData.pg.val.length>=1) {
        data.push(['PG Rating', item.epgData.pg.val[0]+""]);
      }
      if(item.epgData.prdct) {
        data.push(['Prod. country', item.epgData.prdct.cntr]);
        data.push(['Production started', item.epgData.prdct.yfst + ""]);
        data.push(['Production ended', item.epgData.prdct.ylst + ""]);
      }
    }
  }
  return data;
};
var getDataForItem = function (item) {
  var title = '';
  var subtitle = '';
  var id = 0;
  //console.log('Parsing item: '+JSON.stringify(item));
  if (currentType === 0) {
    title = item.epgData.tit[0].value;
    subtitle = 'Start: ' + formatDate(item.epgData.time.strt);
    subtitle += ' End: ' + formatDate(item.epgData.time.end);
  } else if (currentType === 1) {
    title = item.DName.Long.value;
    subtitle = 'Language: ' + item.Lang[0];
    id = item.id;
  }
  if (!title) {
    title = ' ';
  }
  if (!subtitle) {
    subtitle = ' ';
  }
  return [title.substr(0, 20), subtitle, id];
};
var latestChannels = [];
var fetchData = function (index) {
  var type = currentType;
  var id = 0;
  if (type === 0 && latestChannels[index]) {
    id = latestChannels[index][2];
  }
  if (type <= 2) {
    console.info('Type: ' + currentType + ', index: ' + index + ', id: ' + id);
    var url = '';
    switch (type) {
      case 0:
        if (!id) {
          id = 'primetime';
        } else {
          id = 'channel/' + id;
        }
        url = 'http://hackathon.lab.watchmi.tv/api/example.com/broadcasts/format/json/' + id;
        break;
      case 1:
        url = "http://hackathon.lab.watchmi.tv/api/example.com/channels/format/json/";
        break;
    }
    var req = new XMLHttpRequest();
    console.log('Loading "' + url + '"');
    req.open('GET', url, true);
    req.onload = function (e) {
      console.log('Loaded with status: ' + req.status + ', ready state: ' + req.readyState);
      if (req.readyState == 4 && req.status == 200) {
        if (req.status == 200) {
          var response = JSON.parse(req.responseText);
          console.log('Got ' + response.totalResults + ' results');
          items = [];
          if (type === 0) {
            latestItems = [];
          }
          for (var i = 0; i < response.results.length && i < 20; i++) {
            items.push(getDataForItem(response.results[i]));
            if (type === 0) {
              latestItems.push(response.results[i]);
            }
          }
          if (type == 1) {
            latestChannels = items;
          }
          sendResultsToPebble(items);

        } else {
          console.log("Error");
        }
      }
    };
    req.send(null);
  } else {
    sendResultsToPebble(generateItemDetails(index));
  }
};


Pebble.addEventListener("appmessage",
  function (e) {
    console.log("Received message: " + JSON.stringify(e.payload));
    currentType = e.payload[0];
    var index = e.payload[1];
    fetchData(index);
  }
);