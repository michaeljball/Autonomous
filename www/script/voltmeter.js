/*jslint plusplus: true, sloppy: true, indent: 4 */
(function () {
    "use strict";
    // this function is strict...
}());

var iCurrentVolts = 0,
iTargetVolts = 0,
bDecrement = null,
job = null;

function degToRad(angle) {
// Degrees to radians
return ((angle * Math.PI) / 180);
}

function radToDeg(angle) {
// Radians to degree
return ((angle * 180) / Math.PI);
}

function drawLine(options, line) {
// Draw a line using the line object passed in
options.ctx.beginPath();

// Set attributes of open
options.ctx.globalAlpha = line.alpha;
options.ctx.lineWidth = line.lineWidth;
options.ctx.fillStyle = line.fillStyle;
options.ctx.strokeStyle = line.fillStyle;
options.ctx.moveTo(line.from.X,
line.from.Y);

// Plot the line
options.ctx.lineTo(
line.to.X,
line.to.Y
);

options.ctx.stroke();
}

function createLine(fromX, fromY, toX, toY, fillStyle, lineWidth, alpha) {
// Create a line object using Javascript object notation
return {
from: {
X: fromX,
Y: fromY
},
to:	{
X: toX,
Y: toY
},
fillStyle: fillStyle,
lineWidth: lineWidth,
alpha: alpha
};
}

function drawOuterMetallicArc(options) {
/* Draw the metallic border of the Voltsmeter
* Outer grey area
*/
options.ctx.beginPath();

// Nice shade of grey
options.ctx.fillStyle = "rgb(127,127,127)";

// Draw the outer circle
options.ctx.arc(options.center.X,
options.center.Y,
options.radius,
0,
Math.PI,
true);

// Fill the last object
options.ctx.fill();
}

function drawInnerMetallicArc(options) {
/* Draw the metallic border of the Voltsmeter
* Inner white area
*/

options.ctx.beginPath();

// White
options.ctx.fillStyle = "rgb(255,255,255)";

// Outer circle (subtle edge in the grey)
options.ctx.arc(options.center.X,
options.center.Y,
(options.radius / 25) * 23,
0,
Math.PI,
true);

options.ctx.fill();
}

function drawMetallicArc(options) {
/* Draw the metallic border of the Voltsometer
* by drawing two semi-circles, one over lapping
* the other with a bot of alpha transparency
*/

drawOuterMetallicArc(options);
drawInnerMetallicArc(options);
}

function drawBackground(options) {
/* Black background with alphs transparency to
* blend the edges of the metallic edge and
* black background
*/
    var i = 0;

options.ctx.globalAlpha = 0.2;
options.ctx.fillStyle = "rgb(0,0,0)";

// Draw semi-transparent circles
for (i = 42; i < 48; i++) {
options.ctx.beginPath();

options.ctx.arc(options.center.X,
options.center.Y,
i,
0,
Math.PI,
true);

options.ctx.fill();
}
}

function applyDefaultContextSettings(options) {
/* Helper function to revert to gauges
* default settings
*/

options.ctx.lineWidth = 2;
options.ctx.globalAlpha = 0.5;
options.ctx.strokeStyle = "rgb(255, 255, 255)";
options.ctx.fillStyle = 'rgb(255,255,255)';
}

function drawSmallTickMarks(options) {
/* The small tick marks against the coloured
* arc drawn every 5 mph from 10 degrees to
* 170 degrees.
*/

var tickvalue = options.levelRadius - 8,
iTick = 0,
gaugeOptions = options.gaugeOptions,
iTickRad = 0,
onArchX,
onArchY,
innerTickX,
innerTickY,
fromX,
fromY,
line,
toX,
toY;

applyDefaultContextSettings(options);

// Tick every 20 degrees (small ticks)
for (iTick = 10; iTick < 180; iTick += 20) {

iTickRad = degToRad(iTick);

/* Calculate the X and Y of both ends of the
* line I need to draw at angle represented at Tick.
* The aim is to draw the a line starting on the
* coloured arc and continuing towards the outer edge
* in the direction from the center of the gauge.
*/

onArchX = gaugeOptions.radius - (Math.cos(iTickRad) * tickvalue);
onArchY = gaugeOptions.radius - (Math.sin(iTickRad) * tickvalue);
innerTickX = gaugeOptions.radius - (Math.cos(iTickRad) * gaugeOptions.radius);
innerTickY = gaugeOptions.radius - (Math.sin(iTickRad) * gaugeOptions.radius);

fromX = (options.center.X - gaugeOptions.radius) + onArchX;
fromY = (gaugeOptions.center.Y - gaugeOptions.radius) + onArchY;
toX = (options.center.X - gaugeOptions.radius) + innerTickX;
toY = (gaugeOptions.center.Y - gaugeOptions.radius) + innerTickY;

// Create a line expressed in JSON
line = createLine(fromX, fromY, toX, toY, "rgb(127,127,127)", 2, 0.6);

// Draw the line
drawLine(options, line);

}
}

function drawLargeTickMarks(options) {
/* The large tick marks against the coloured
* arc drawn every 10 mph from 10 degrees to
* 170 degrees.
*/

var tickvalue = options.levelRadius - 8,
iTick = 0,
        gaugeOptions = options.gaugeOptions,
        iTickRad = 0,
        innerTickY,
        innerTickX,
        onArchX,
        onArchY,
        fromX,
        fromY,
        toX,
        toY,
        line;

applyDefaultContextSettings(options);

tickvalue = options.levelRadius - 2;

// 10 units (major ticks)
for (iTick = 20; iTick < 180; iTick += 20) {

iTickRad = degToRad(iTick);

/* Calculate the X and Y of both ends of the
* line I need to draw at angle represented at Tick.
* The aim is to draw the a line starting on the
* coloured arc and continueing towards the outer edge
* in the direction from the center of the gauge.
*/

onArchX = gaugeOptions.radius - (Math.cos(iTickRad) * tickvalue);
onArchY = gaugeOptions.radius - (Math.sin(iTickRad) * tickvalue);
innerTickX = gaugeOptions.radius - (Math.cos(iTickRad) * gaugeOptions.radius);
innerTickY = gaugeOptions.radius - (Math.sin(iTickRad) * gaugeOptions.radius);

fromX = (options.center.X - gaugeOptions.radius) + onArchX;
fromY = (gaugeOptions.center.Y - gaugeOptions.radius) + onArchY;
toX = (options.center.X - gaugeOptions.radius) + innerTickX;
toY = (gaugeOptions.center.Y - gaugeOptions.radius) + innerTickY;

// Create a line expressed in JSON
line = createLine(fromX, fromY, toX, toY, "rgb(127,127,127)", 2, 0.6);

// Draw the line
drawLine(options, line);
}
}

function drawTicks(options) {
/* Two tick in the coloured arc!
* Small ticks every 5
* Large ticks every 10
*/
drawSmallTickMarks(options);
drawLargeTickMarks(options);
}

function drawTextMarkers(options) {
/* The text labels marks above the coloured
* arc drawn every 10 mph from 10 degrees to
* 170 degrees.
*/
var innerTickX = 0,
innerTickY = 0,
        iTick = 0,
        gaugeOptions = options.gaugeOptions,
        iTickToPrint = 0;

applyDefaultContextSettings(options);

// Font styling
options.ctx.font = 'italic 8px sans-serif';
options.ctx.textBaseline = 'top';

options.ctx.beginPath();

// Tick every 20 (small ticks)
for (iTick = 10; iTick < 180; iTick += 20) {

innerTickX = gaugeOptions.radius - (Math.cos(degToRad(iTick)) * gaugeOptions.radius);
innerTickY = gaugeOptions.radius - (Math.sin(degToRad(iTick)) * gaugeOptions.radius);

// Some cludging to center the values (TODO: Improve)
if (iTick <= 10) {
options.ctx.fillText(iTickToPrint, (options.center.X - gaugeOptions.radius -6) + innerTickX,
(gaugeOptions.center.Y - gaugeOptions.radius -6) + innerTickY + 2.5);
} else if (iTick < 50) {
options.ctx.fillText(iTickToPrint, (options.center.X - gaugeOptions.radius -6) + innerTickX - 2.5,
(gaugeOptions.center.Y - gaugeOptions.radius -6) + innerTickY + 2.5);
} else if (iTick < 90) {
options.ctx.fillText(iTickToPrint, (options.center.X - gaugeOptions.radius -6) + innerTickX,
(gaugeOptions.center.Y - gaugeOptions.radius -6) + innerTickY);
} else if (iTick === 90) {
options.ctx.fillText(iTickToPrint, (options.center.X - gaugeOptions.radius -6) + innerTickX + 4,
(gaugeOptions.center.Y - gaugeOptions.radius -6) + innerTickY);
} else if (iTick < 145) {
options.ctx.fillText(iTickToPrint, (options.center.X - gaugeOptions.radius -6) + innerTickX + 5,
(gaugeOptions.center.Y - gaugeOptions.radius -6) + innerTickY);
} else {
options.ctx.fillText(iTickToPrint, (options.center.X - gaugeOptions.radius -6) + innerTickX + 7,
(gaugeOptions.center.Y - gaugeOptions.radius -6) + innerTickY + 2.5);
}

// MPH increase by 10 every 20 degrees
iTickToPrint += Math.round(100 / 10);
}

    options.ctx.stroke();
}

function drawVoltsometerPart(options, alphaValue, strokeStyle, startPos) {
/* Draw part of the arc that represents
* the colour Voltsometer arc
*/

options.ctx.beginPath();

options.ctx.globalAlpha = alphaValue;
options.ctx.lineWidth = 3;
options.ctx.strokeStyle = strokeStyle;

options.ctx.arc(options.center.X,
options.center.Y,
options.levelRadius,
Math.PI + (Math.PI / 360 * startPos),
0 - (Math.PI / 360 * 10),
false);

options.ctx.stroke();
}

function drawVoltsometerColourArc(options) {
/* Draws the colour arc. Three different colours
* used here; thus, same arc drawn 3 times with
* different colours.
* TODO: Gradient possible?
*/

var startOfRed = 10,
endOfRed = 100,
endOfOrange = 180;

drawVoltsometerPart(options, 1.0, "rgb(255, 0, 0)", startOfRed);
drawVoltsometerPart(options, 0.9, "rgb(198, 111, 0)", endOfRed);
drawVoltsometerPart(options, 0.9, "rgb(82, 240, 55)", endOfOrange);

}

function drawNeedleDial(options, alphaValue, strokeStyle, fillStyle) {
/* Draws the metallic dial that covers the base of the
* needle.
*/
    var i = 0;

options.ctx.globalAlpha = alphaValue;
options.ctx.lineWidth = 2;
options.ctx.strokeStyle = strokeStyle;
options.ctx.fillStyle = fillStyle;

// Draw several transparent circles with alpha
for (i = 0; i < 15; i++) {

options.ctx.beginPath();
options.ctx.arc(options.center.X,
options.center.Y,
i,
0,
Math.PI,
true);

options.ctx.fill();
options.ctx.stroke();
}
}

function convertVoltsToAngle(options) {
/* Helper function to convert a Volts to the
* equivalent angle.
*/
var iVolts = (options.Volts / 10),
iVoltsAsAngle = ((iVolts * 20) + 10) % 180;

// Ensure the angle is within range
if (iVoltsAsAngle > 180) {
        iVoltsAsAngle = iVoltsAsAngle - 180;
    } else if (iVoltsAsAngle < 0) {
        iVoltsAsAngle = iVoltsAsAngle + 180;
    }

return iVoltsAsAngle;
}

function drawNeedle(options) {
/* Draw the needle in a nice red colour at the
* angle that represents the options.Volts value.
*/

var iVoltsAsAngle = convertVoltsToAngle(options),
iVoltsAsAngleRad = degToRad(iVoltsAsAngle),
        gaugeOptions = options.gaugeOptions,
        innerTickX = gaugeOptions.radius - (Math.cos(iVoltsAsAngleRad) * 20),
        innerTickY = gaugeOptions.radius - (Math.sin(iVoltsAsAngleRad) * 20),
        fromX = (options.center.X - gaugeOptions.radius) + innerTickX,
        fromY = (gaugeOptions.center.Y - gaugeOptions.radius) + innerTickY,
        endNeedleX = gaugeOptions.radius - (Math.cos(iVoltsAsAngleRad) * gaugeOptions.radius),
        endNeedleY = gaugeOptions.radius - (Math.sin(iVoltsAsAngleRad) * gaugeOptions.radius),
        toX = (options.center.X - gaugeOptions.radius) + endNeedleX,
        toY = (gaugeOptions.center.Y - gaugeOptions.radius) + endNeedleY,
        line = createLine(fromX, fromY, toX, toY, "rgb(255,0,0)", 5, 0.6);

drawLine(options, line);

// Two circle to draw the dial at the base (give its a nice effect?)
drawNeedleDial(options, 0.4, "rgb(127, 127, 127)", "rgb(255,255,255)");
drawNeedleDial(options, 0.1, "rgb(127, 127, 127)", "rgb(127,127,127)");

}

function buildOptionsAsJSON(canvas, iVolts) {
/* Setting for the Voltmeter
* Alter these to modify its look and feel
*/

var centerX = 55,
centerY = 55,
        radius = 35,
        outerRadius = 50;

// Create a Voltsmeter object using Javascript object notation
return {
ctx: canvas.getContext('2d'),
Volts: iVolts,
center:	{
X: centerX,
Y: centerY
},
levelRadius: radius - 10,
gaugeOptions: {
center:	{
X: centerX,
Y: centerY
},
radius: radius
},
radius: outerRadius
};
}

function clearCanvas(options) {
options.ctx.clearRect(0, 0, 110, 110);
applyDefaultContextSettings(options);
}

function init() {
	setInterval(draw, 1000);
}



function draw() {
/* Main entry point for drawing the Voltsmeter
* If canvas is not support alert the user.
*/

$.get('voltmeter_value.php', function(voltage){

    iTargetVolts = voltage;
   // alert(iTargetVolts);

 });

console.log('Target: ' + iTargetVolts);
console.log('Current: ' + iCurrentVolts);

var canvas = document.getElementById('voltmeter'),
options = null;

// Canvas good?
if (canvas !== null && canvas.getContext) {
options = buildOptionsAsJSON(canvas, iTargetVolts);
  //  alert(iTargetVolts);

// Clear canvas
clearCanvas(options);

// Draw the metallic styled edge
drawMetallicArc(options);

// Draw thw background
drawBackground(options);

// Draw tick marks
drawTicks(options);

// Draw labels on markers
drawTextMarkers(options);

// Draw voltmeter colour arc
drawVoltsometerColourArc(options);

// Draw the needle and base
drawNeedle(options);

} else {
alert("Canvas not supported by your browser!");
}

if(iTargetVolts == iCurrentVolts) {
clearTimeout(job);
return;
} else if(iTargetVolts < iCurrentVolts) {
bDecrement = true;
} else if(iTargetVolts > iCurrentVolts) {
bDecrement = false;
}

if(bDecrement) {
if(iCurrentVolts - 10 < iTargetVolts)
iCurrentVolts = iCurrentVolts - 1;
else
iCurrentVolts = iCurrentVolts - 5;
} else {

if(iCurrentVolts + 10 > iTargetVolts)
iCurrentVolts = iCurrentVolts + 1;
else
iCurrentVolts = iCurrentVolts + 5;
}

// job = setTimeout("draw()", 5);

}
