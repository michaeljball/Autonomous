<?php 
    require_once 'google_auth.php';  
?>
     
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml"
xml:lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="">
    <meta name="author" content="">
    <link rel="shortcut icon" href="bootstrap/assets/ico/favicon.png">

    <!-- Bootstrap core CSS -->
    <link href="bootstrap/dist/css/bootstrap.css" rel="stylesheet">

   <!-- Custom styles for this template -->
    <link href="sticky-footer-navbar.css" rel="stylesheet">
    
   <link href="carousel.css" rel="stylesheet">
   

    <title>Control Testing for Raspberry Pi/Arduino Autonomous Platform</title>
    <meta http-equiv="Content-Type"
    content="text/html; charset=iso-8859-1" />
    <script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js" type="text/javascript"></script>
    <script src="http://maps.googleapis.com/maps/api/js?sensor=false"></script>
    <script src='script/speedometer.js'></script>
    <script src='script/autonomous.js'></script>
   
 
    <!-- HTML5 shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!--[if lt IE 9]>
      <script src="bootstrap/assets/js/html5shiv.js"></script>
      <script src="bootstrap/assets/js/respond.min.js"></script>
    <![endif]-->
</head>

<body  onload='init();' >


    <!-- Fixed navbar -->
    <div class="navbar navbar-default navbar-fixed-top">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="http://bot.robbie-robot.com">Robbie-Robot</a>
        </div>
        <div class="navbar-collapse collapse">
          <ul class="nav navbar-nav">
            <li class="active"><a href="#">Home</a></li>
            <li><a href="about.php">About</a></li>
            <li><a href="contact.php">Contact</a></li>
            <li class="dropdown">
              <a href="#" class="dropdown-toggle" data-toggle="dropdown">Links <b class="caret"></b></a>
              <ul class="dropdown-menu">
                <li><a href="http://arduino-pi.blogspot.ca" target ="new">Arduino-pi Blog</a></li>
                <li><a href="http://www.myrobots.com/Robot/robbie-robot" target ="new">MyRobots.com</a></li>
                <li><a href="https://github.com/michaeljball" target ="new">Github for this site</a></li>
                <li><a href="http://letsmakerobots.com" target ="new">letsmakerobots.com</a></li>
                <li><a href="http://www.societyofrobots.com/robotforum/index.php" target ="new">Society Of Robots</a></li>
                <li class="divider"></li>
                <li class="dropdown-header">Research</li>
                <li><a href="http://openslam.org" target="new">OpenSLAM</a></li>
                <li><a href="http://opencv.org" target="new"">OpenCV</a></li>
               <li><a href="http://ros.org" target="new"">ROS: Robot Operating System</a></li>
               <li><a href="http://www.instructables.com/id/Robots/" target="new"">Instructables: Robots</a></li>
                
               <li><a href="http://www.instructables.com/id/Raspberry-Pi-and-ROS-Robotic-Operating-System" target="new"">Instructables: ROS Raspberry Pi</a></li>
              </ul>
            </li>
          </ul>
          <ul class="nav navbar-nav navbar-right">
            <li><a href="reports.php">Reports</a></li>
            <li><a href="soundingsmap.php">Soundings Map</a></li>
            <li class="active"><a href="./">Floorplan Map</a></li>
          
            <li> <a href="https://accounts.google.com/logout"><b>Logout</b></a> </li>
          </ul>  
        </div><!--/.nav-collapse -->
           
           
        </div>    
      </div>
    </div>

  
    
    <div class="container">
       
    <div class="row">
      <table width="900" align="center">
          <tr>
              <td  width="30%" valign="top" align="center">
                <b>Webcam</b><br>

                <b>Description : </b> 
                <font size =" -1">Autonomous Arduino-Raspberry Pi platform<br></font>
                <b>Robot Page : </b>
                <font size =" -2">
                <a href="http://www.myrobots.com/Robot/robbie-robot " target="new">http://www.myrobots.com/Robot/robbie-robot </a>
                </font><p>
                 <center>

                        <div id="myCarousel" class="carousel slide" style="height:200px;width:300px;">
                      <!-- Indicators -->
                      <ol class="carousel-indicators">
                        <li data-target="#myCarousel" data-slide-to="0" class="active"></li>
                        <li data-target="#myCarousel" data-slide-to="1"></li>
                        <li data-target="#myCarousel" data-slide-to="2"></li>
                        <li data-target="#myCarousel" data-slide-to="3"></li>
                        <li data-target="#myCarousel" data-slide-to="4"></li>
                      </ol>
                      <div class="carousel-inner">
                        <div class="item active">
                          <img src="getWebcamImage.php?thumb=1" >
                          <div class="container">
                            <div class="carousel-caption">
                              <b>First Thumb.</b>
                              <p><a class="btn btn-large btn-primary" href="getWebcamImage.php?thumb=1" target = "new">View Image</a></p>
                            </div>
                          </div>
                        </div>

                        <div class="item">
                          <img src="getWebcamImage.php?thumb=2" >
                          <div class="container">
                            <div class="carousel-caption">
                              <b>Second Thumb.</b>
                              <p><a class="btn btn-large btn-primary" href="getWebcamImage.php?thumb=2" target = "new">View Image</a></p>
                            </div>
                          </div>
                        </div>
                        <div class="item">
                          <img src="getWebcamImage.php?thumb=3" >
                          <div class="container">
                            <div class="carousel-caption">
                              <b>Third Thumb.</b>
                              <p><a class="btn btn-large btn-primary" href="getWebcamImage.php?thumb=3" target = "new">View Image</a></p>
                            </div>
                          </div>
                        </div>
                        <div class="item">
                          <img src="getWebcamImage.php?thumb=4" >
                          <div class="container">
                            <div class="carousel-caption">
                              <b>Fourth Thumb.</b>
                              <p><a class="btn btn-large btn-primary" href="getWebcamImage.php?thumb=4" target = "new">View Image</a></p>
                            </div>
                          </div>
                        </div>
                        <div class="item">
                          <img src="getWebcamImage.php?thumb=5" >
                          <div class="container">
                            <div class="carousel-caption">
                              <b>Fifth Thumb.</b>
                              <p><a class="btn btn-large btn-primary" href="getWebcamImage.php?thumb=5" target = "new">View Image</a></p>
                            </div>
                          </div>
                        </div>

                      </div>
                      <a class="left carousel-control" href="#myCarousel" data-slide="prev"><span class="glyphicon glyphicon-chevron-left"></span></a>
                      <a class="right carousel-control" href="#myCarousel" data-slide="next"><span class="glyphicon glyphicon-chevron-right"></span></a>
                    </div><!-- /.carousel -->


 
                 </center>
              </td>
              <td  width="30%"> 
                    <center><b>Navigation Controls</b></center>
                    <div> 
                   <center> <?PHP include 'show_button_box.php'; ?> 
                            <?PHP include 'show_recorder.php'; ?> <P>
                   </center>
                    </div>

              </td>
              <td  width="30%"> 
                <center>
                <table border ='0'> <tr>  
                   <td align="center"  valign='top' colspan ='2'>
                       <B>Sonar Scan</B><br>
                    <div id="sonar"> <img src="show_sonar_chart.php" width="300" height="300"></img></div>
                  </td>
                </tr>

                <tr>
                   <td>
                       <br><br>
                    <canvas id="telem" width="200" height="135"></canvas>
                   </td><td>
                    <canvas id="compass" width="100" height="100"></canvas>
                    </td>
                </tr>
                </table> 
                </center>
              </td>
          </tr>
      </table>
        <hr>
      <table align="center">
          <tr>
              <td  width="30%">  
                Acceleration in last 60 Seconds<br>
                <div id="accel"> <img src="show_accel_chart.php" width="300" height="122"></img></div>
              </td><td >
                  <table border ="0" align ="center" valign ="top"><tr>
                    <td>        
                    <center>Current Horizontal speed<br>(inches per second)</center>
                    <div> <canvas id="speedometer" width="220" height="110"></div>
                    </td><td>
                    <div> <canvas id="thermometer" width="80" height="110"></div>
                    </td><td>
                    <center>Humidity</center><br>
                    <div><canvas id="hygrometer" width="110" height="110"></div>
                    </td><td>
                    <center>Voltage</center><br>
                    <div><canvas id="voltmeter" width="110" height="110"></div>
                    </td></tr></table>
               </td>
          </tr>
      </table>
            <hr>
        <table align="center">
          <tr>
              <td  width="30%">            
                    <b>Current Location</b><br>
                            <div> <canvas id="mapcanvas" width="300px" height="300px" style="border:1px solid grey;">
                    </canvas> </div>
                            <br><small><a href="http://maps.google.com/maps?f=q&amp;source=embed&amp;hl=en&amp;geocode=&amp;q=43.900385+-78.705738&amp;aq=&amp;sll=37.0625,-95.677068&amp;sspn=41.275297,77.695313&amp;ie=UTF8&amp;ll=43.900385,-78.705738&amp;spn=0.009199,0.018969&amp;t=m&amp;z=14" style="color:#0000FF;text-align:left" target ="new">View Larger Map</a></small>
                    </center>               </td><td>
                     <b>Floorplan View</b>
                     <div><canvas id="floorplan" width="650" height="350"></div>
                      </center>               </td>
          </tr>
      </table>
    </div> <!-- /container -->

    
    <div id="footer">
      <div class="container">
        <p class="text-muted credit">Copyright 2013 Michael Ball 
            <a href="http://arduino-pi.blogspot.com" target ="new">Arduino-Pi</a> 
            Email:  <a href="mailto:unix_guru@hotmail.com?Subject=Arduino-Pi" target="new">unix_guru@hotmail.com</a></p>
      </div>
    </div>



   <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="bootstrap/assets/js/jquery.js"></script>
    <script src="bootstrap/dist/js/bootstrap.min.js"></script>
  
</body>
</html>
