
// Global variable

var h_img = null,
needle = null,
hctx = null;
var humidity = 0;

function clearCanvas() {
// clear canvas
hctx.clearRect(0, 0, 100, 100);
}

function draw_hygrometer() {

clearCanvas();

// Draw the compass onto the canvas
hctx.drawImage(h_img, 0, 0);

// Save the current drawing state
hctx.save();

// Now move across and down half the
hctx.translate(50, 50);

// Rotate around this point
hctx.rotate(degrees * (Math.PI / 180));

// Draw the image back and up
hctx.drawImage(needle, -50, -50);

// Restore the previous drawing state
hctx.restore();


}

function h_imgLoaded() {
// Image loaded event complete. Start the timer
setInterval(draw, 100);
}

function init() {
// Grab the compass element
var canvas = document.getElementById('hygrometer');

// Canvas supported?
if (canvas.getContext('2d')) {
hctx = canvas.getContext('2d');

// Load the needle image
needle = new Image();
needle.src = 'images/needle_small.png';

// Load the compass image
h_img = new Image();
h_img.src = 'images/hygrometer_small.png';
h_img.onload = h_imgLoaded;
} else {
alert("Canvas not supported!");
}
}