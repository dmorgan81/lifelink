/* global Pebble */
Pebble.addEventListener("showConfiguration", function (e) {
    Pebble.openURL("http://pebble-config.herokuapp.com/config?title=Lifelink&fields=Player%20One%20Name,Player%20Two%20Name");
});

Pebble.addEventListener("webviewclosed", function(e) {
    var configuration = JSON.parse(e.response);
    Pebble.sendAppMessage(configuration);
});