
function clearCanvas() {
// clear canvas
ctx.clearRect(0, 0, 640, 480);
}


function imgLoaded() {
// Image loaded event complete. Start the timer
ctx.drawImage(img,0,0);
setInterval(draw, 100);
}

function blob2canvas(canvas,blob){
    var img = new Img;
    var ctx = canvas.getContext('2d');
    img.onload = function () {
        ctx.drawImage(img,0,0);
    }
    img.src = blob;
}

function init() {
	
var ctx = document.getElementById('webcam').getContext('2d');
blob2canvas(ctx,getWebcamImage.php);

}

