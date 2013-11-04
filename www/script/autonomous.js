// #########################################################################
// 
// Global variables

var img = null,
needle = null,
ctx = null;
var degrees = 0;
var fp = null,
robot = null,
fpctx = null;
var latitude = null;
var longitude = null;
var google_tile = null;          
var map_canvas = dnull;
var mapcontext = null;   
var gmap = null;
var x = null;
var y = null;

function draw_compass() {

ctx.clearRect(0, 0, 100, 100);

ctx.drawImage(img, 0, 0);       // Draw the compass onto the canvas
ctx.save();                     // Save the current drawing state
ctx.translate(50, 50);          // Now move across and down half the image
ctx.rotate(degrees * (Math.PI / 180));  // Rotate around this point
ctx.drawImage(needle, -50, -50);        // Draw the needle back and up
ctx.restore();                  // Restore the previous drawing state

}

function findPos(obj) {
    var curleft = 0, curtop = 0;
    if (obj.offsetParent) {
        do {
            curleft += obj.offsetLeft;
            curtop += obj.offsetTop;
        } while (obj = obj.offsetParent);
        return { x: curleft, y: curtop };
    }
    return 0;
}


function drawfloorplan() {

fpctx.clearRect(0, 0, 625, 325);
$.get('get_telemetry.php', function(row){       // Get the current telemetry to display
    JSONdata = JSON.parse(row);
    degrees = JSONdata.heading;
    directahead = JSONdata.directahead;
    directbehind = JSONdata.directbehind;
    directright = JSONdata.directright;
    directleft = JSONdata.directleft;
 });

$.get('get_gps.php', function(row){       // Get the current gps coords to display
    JSONgps = JSON.parse(row);
    latitude = JSONgps.latitude;
    longitude = JSONgps.longitude;
 });

$.get('get_location.php', function(row){       // Get the current gps coords to display
    JSONloc = JSON.parse(row);
    x_coord = JSONloc.x_coord;
    y_coord = JSONloc.y_coord;
 });

$('#floorplan').mousemove(function(e) {
    var pos = findPos(this);
    x = e.pageX - pos.x;
    y = e.pageY - pos.y;
});
// Draw the floorplan onto the canvas with scaling

fpctx.drawImage(fp, 0, 0, 1231, 615, 0, 0, 625, 325);
// Save the current drawing state
fpctx.fillStyle = "blue";
fpctx.font = "bold 12px Arial";
fpctx.fillText("Current Position:  ", 40, 25); 
fpctx.fillText("X,Y:  ", 40, 35); 
fpctx.font = "12px Arial";
fpctx.fillText(x_coord,  70, 35);
fpctx.fillText(",",  95, 35);
fpctx.fillText(y_coord,  100, 35);

fpctx.font = "bold 12px Arial";
fpctx.fillText("Mouse Position:  ", 40, 45); 
fpctx.fillText("X,Y:  ", 40, 55); 
fpctx.font = "12px Arial";
fpctx.fillText(x,  70, 55);
fpctx.fillText(",",  95, 55);
fpctx.fillText(y,  100, 55);

fpctx.save();

fpctx.translate(x_coord/2, y_coord/2);      // Now move to rover location
fpctx.rotate(degrees * (Math.PI / 180));    // Rotate around this point
fpctx.drawImage(robot, -20, -20);           // Draw the rover back and up

// Restore the previous drawing state
fpctx.restore();

}

function draw_googlemap() {

        mapcontext.drawImage(gmap, 0, 0);
        mapcontext.fillStyle = "blue";
        mapcontext.font = "bold 14px Arial";
        mapcontext.fillText("Lat:  ", 40, 285);            
        mapcontext.fillText(latitude, 70, 285);            
        mapcontext.fillText("Long:  ", 140, 285);            
        mapcontext.fillText(longitude, 180, 285);            
   
}

function draw_telemetry() {

        telemctx.clearRect(0,0,200,140);
        telemctx.fillStyle = "blue";
        telemctx.font = "12px Arial";
        telemctx.fillText("Current Heading:", 0, 10);            
        telemctx.fillText(degrees, 110, 10);            
         telemctx.fillText("Lat:  ", 0, 25);            
        telemctx.fillText(latitude, 80, 25);            
        telemctx.fillText("Long:  ", 0, 40);            
        telemctx.fillText(longitude, 76, 40);            
        telemctx.fillText("Ahead:  ", 0, 55);            
        telemctx.fillText(directahead, 45, 55);            
        telemctx.fillText("Behind:  ", 75, 55);            
        telemctx.fillText(directbehind, 120, 55);            
        telemctx.fillText("Right:  ", 0, 70);            
        telemctx.fillText(directright, 45, 70);            
        telemctx.fillText("Left:  ", 75, 70);            
        telemctx.fillText(directleft, 120, 70);            
   
}

 



function imgLoaded() {
// Image loaded event complete. Start the timer
setInterval(draw_compass, 500);
setInterval(drawfloorplan, 500);
setInterval(draw_googlemap, 500);
setInterval(draw_telemetry, 500);

}


function showButtons(name){                 // Show imagemap buttons for controlling robot
  document.myform.stage.value = name;
}

function changeSpeed(){
    var speed=document.getElementById("speed");
    $.get("insert_commands.php", { command: 18, parameter: speed.value },
    function(command,parameter){
  }, "json");
}

function sendCommand(command,parameter){

    $.get("insert_commands.php", { command: command, parameter: parameter },
  function(command,parameter){
  }, "json");
}

function init() {
latitude = 43.9003;
longitude = -78.7057;
google_tile = "http://maps.google.com/maps/api/staticmap?sensor=false&center=" + latitude + "," +
                longitude + "&zoom=14&size=400x300&markers=color:blue|label:U|" +
                latitude + ',' + longitude;          
map_canvas = document.getElementById("mapcanvas");
mapcontext = map_canvas.getContext("2d");   
gmap = new Image();
gmap.src = google_tile;

init_speedo(0);                         // calls init script from speedometer.js

  
    // Grab the compass element
var compass_canvas = document.getElementById('compass');
// Canvas supported?
if (compass_canvas.getContext('2d')) {
    ctx = compass_canvas.getContext('2d');
    // Load the needle image
    needle = new Image();
    needle.src = 'images/needle_small.png';

    // Load the compass image
    img = new Image();
    img.src = 'images/compass_small.png';
    img.onload = imgLoaded;
} else {
    alert("Canvas not supported!");
}
// Initalize the floorplan canvas
var floorplan_canvas = document.getElementById('floorplan');
// Canvas supported?
if (floorplan_canvas.getContext('2d')) {
    fpctx = floorplan_canvas.getContext('2d');
    // Load the robot image
    robot = new Image();
    robot.src = 'images/robbie-top-small.png';

    // Load the floorplan image
    fp = new Image();
    fp.src = 'images/61-baxter_mainfloor.png';

    //fp.onload = imgLoaded;
} else {
    alert("Canvas not supported!");
}

// Initalize the telemetry canvas
var telemetry_canvas = document.getElementById('telem');
// Canvas supported?
if (telemetry_canvas.getContext('2d')) {
    telemctx = telemetry_canvas.getContext('2d');

}  else {
    alert("Canvas not supported!");
}
}
