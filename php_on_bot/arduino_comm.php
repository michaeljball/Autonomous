<?php
#################################################################################################################
# Arduino_comm.php 
# Written by Michael Ball  October 2013
# 
# This application runs on the Raspberry Pi on the autonymous rover, and manages 
# communications between the two arduino's (one for motor control and sonar pod, the
# other for proximity and wheel encoder data) and the rover's database.
# 
#################################################################################################################
# Define SQL queries
$sql_soundings = "INSERT INTO scanning (gps_uid, cmd_uid, ardtime, heading, offset, radius, cart_x, cart_y) 
                                VALUES ( :gps_uid, :cmd_uid, :ardtime, :heading, :offset, :radius, :cart_x, :cart_y);" ;
$sql_telemetry = "INSERT INTO telemetry (gps_uid, cmd_uid, ardtime, motion, heading, offset, accel_x, accel_y, accel_z, directahead, directbehind, directleft, directright, ticksleft, ticksright, voltage) 
                                VALUES (:gps_uid, :cmd_uid, :ardtime, :motion, :heading, :offset, :accel_x, :accel_y, :accel_z, :directahead, :directbehind, :directleft, :directright, :ticksleft, :ticksright, :voltage);"; 
$sql_commands = "SELECT * FROM cmd WHERE completed = 0 ORDER BY uid;";
$sql_commands_count = "SELECT COUNT(*) FROM cmd WHERE completed = 0;";
$sql_cmd_update = "UPDATE cmd SET completed = 1 WHERE uid =";
$sql_cmd_clear = "UPDATE cmd SET completed = 1 WHERE completed = 0";
$sql_get_gps_UID = "SELECT uid FROM gps ORDER BY uid DESC LIMIT 1;"  ;

#################################################################################################################


include 'database_creds.inc'; 
// require_once('php_serial.class.php');

//Set timezone
date_default_timezone_set('America/Toronto');
print("Timezone Set:  ");
//Connect to Arduino
exec('stty -F /dev/ttyACM0 57600 raw');
$arduino = fopen("/dev/ttyACM0", "w+");
print("Arduino attached:  ");

//Connect to Arduino
exec('stty -F /dev/ttyUSB0 57600 raw');
$sensors = fopen("/dev/ttyUSB0", "w+");
print("Sensors attached:  ");

$last_cmd = 0; 
$heading = 0; 
$arduinodata['resp_uid'] = 0;

	
	try {
   	 $dbh = new PDO("mysql:host=$hostname;dbname=mapdb", $username, $password);
   	 /*** echo a message saying we have connected ***/
   	 echo 'Connected to database\n';


	}
	catch(PDOException $e) {
   	 echo $e->getMessage();
  }

// Remove any commands sitting in the queue prior to startup.
  $cstmt = $dbh->prepare($sql_cmd_clear);
	$cstmt->execute();

     

//Read status from Arduino
while($arduino){
  
  $gstmt = $dbh->prepare($sql_get_gps_UID);   // Get most recent GPS UID
  $gstmt->execute();
  $gps_UID = $gstmt->fetchColumn();
  echo 'GPS UID: '; echo $gps_UID; echo "\n";
  
  $buffer = fgets($arduino);                 // Read a line from the Arduino and parse
  $line = explode(',',$buffer);
  echo "Buffer is: " .$buffer."\n";
  if($line[0] =='SNSR'){
    	
        $arduinodata['ardtime'] = $line[1];
        $arduinodata['motion'] = $line[2];
        $arduinodata['accel_x'] = $line[3];
        $arduinodata['accel_y'] = $line[4];
        $arduinodata['accel_z'] = $line[5];
        $arduinodata['heading'] = $line[6];
        $arduinodata['offset'] = $line[7];
        $arduinodata['directahead'] = $line[8];
        $arduinodata['directbehind'] = $line[9];
        $arduinodata['directright'] = $line[10];
        $arduinodata['directleft'] = $line[11];
        $arduinodata['ticksright'] = $line[12];
        $arduinodata['ticksleft'] = $line[13];
        $arduinodata['voltage'] = $line[14];
        $arduinodata['resp_UID'] = $line[15];
        
        $resp_UID = $line[15];
        $heading = $line[6];
        
        print_r($arduinodata);
        echo "\n\n";
    		
      	$stmt = $dbh->prepare($sql_telemetry);
    	$stmt->execute(array(
    	   ':gps_uid' => $gps_UID,
           ':cmd_uid' => $arduinodata['resp_UID'],
           ':ardtime' => $arduinodata['ardtime'],
           ':motion' =>  $arduinodata['motion'],
           ':heading' => $arduinodata['heading'],
           ':offset' => $arduinodata['offset'],
           ':accel_x' => $arduinodata['accel_x'],
           ':accel_y' => $arduinodata['accel_y'],
           ':accel_z' => $arduinodata['accel_z'],
           ':directahead' => $arduinodata['directahead'],
           ':directbehind' => $arduinodata['directbehind'],
           ':directright' => $arduinodata['directright'],
           ':directleft' => $arduinodata['directleft'],
           ':ticksright' => $arduinodata['ticksright'],
           ':ticksleft' => $arduinodata['ticksleft'],
           ':voltage' => $arduinodata['voltage']
         ));
    		
    	 $affected_rows = $stmt->rowCount();
    
    	 echo "rows = "; echo $affected_rows;
    		
    		
        if($arduinodata['resp_uid'] >= $last_cmd ){            // Fallback check for command completion.

            $update = $sql_cmd_update .  $arduinodata['resp_uid'];       // append UID to sql update command
            echo $update . "\n";
            $cstmt = $dbh->prepare($update);             // Mark command resp_uid  as completed
    		    $cstmt->execute(); 
 	}
    		
   }else if($line[0] == "SCAN") {
            $arduinodata['ardtime'] = $line[1];
 	
            $arduinodata['frad'] = $line[2];        // Front Radians
            $arduinodata['fdis'] = $line[3];        // Front Distance
            $arduinodata['resp_UID'] = $line[6];    // Response CMD_UID
  
            $h=deg2rad($arduinodata['frad'] + $heading);	       # offset by compass heading
            
            # Convert polar coordinates from Sonar to Cartesian (x,y) coordinates
            $arduinodata['cart_x'] = $arduinodata['fdis'] * cos($h);  
            $arduinodata['cart_y'] = $arduinodata['fdis'] * sin($h);
 
      	    $stmt = $dbh->prepare($sql_soundings);
    	    $stmt->execute(array(
    		':gps_uid' => $gps_UID,
                ':cmd_uid' => $arduinodata['resp_UID'],
                ':ardtime' => $arduinodata['ardtime'],
                ':heading' => $arduinodata['heading'],
                ':offset' => $arduinodata['frad'],
                ':radius' => $arduinodata['fdis'],
                ':cart_x' => $arduinodata['cart_x'],
                ':cart_y' => $arduinodata['cart_y']
            
           ));
    		
 
            $arduinodata['rrad'] = $line[4];        // Rear Radians
            $arduinodata['rdis'] = $line[5];        // Rear Distance
            $arduinodata['resp_UID'] = $line[6];    // Response CMD_UID
 
            $h=deg2rad($arduinodata['rrad'] + $arduinodata['heading']);	       # offset by compass heading
            
            # Convert polar coordinates from Sonar to Cartesian (x,y) coordinates
            $arduinodata['cart_x'] = $arduinodata['rdis'] * sin($h);  
            $arduinodata['cart_y'] = $arduinodata['rdis'] * cos($h);
 
      	    $stmt = $dbh->prepare($sql_soundings);
    	    $stmt->execute(array(
    		':gps_uid' => $gps_UID,
                ':cmd_uid' => $arduinodata['resp_UID'],
                ':ardtime' => $arduinodata['ardtime'],
                ':heading' => $arduinodata['heading'],
                ':offset' => $arduinodata['rrad'],
                ':radius' => $arduinodata['rdis'],
                ':cart_x' => $arduinodata['cart_x'],
                ':cart_y' => $arduinodata['cart_y']
            
            ));

            print_r($arduinodata);
            echo "\n\n";   		
            $affected_rows = $stmt->rowCount();
    
    	    echo "rows = "; echo $affected_rows;


    }elseif($line[0] == 'CMD'){
        $arduinodata['command'] = $line[1];
        $arduinodata['parameter'] = $line[2];
        $arduinodata['resp_uid'] = $line[3];

            $update = $sql_cmd_update .  $arduinodata['resp_uid'];       // append UID to sql update command
            echo $update . "\n";
            $cstmt = $dbh->prepare($update);
    		    $cstmt->execute();                                // Mark command resp_uid  as completed
    }elseif($line[0] == 'RDY'){
         $arduinodata['resp_uid'] = $line[1];

            $update = $sql_cmd_update .  $arduinodata['resp_uid'];       // append UID to sql update command
            echo $update . "\n";
            $cstmt = $dbh->prepare($update);
    		    $cstmt->execute();                                // Mark command resp_uid  as completed
    }		
			
		
    $cstmt = $dbh->prepare($sql_commands);   // Get most recent GPS UID
    $cstmt->execute();
    $cmd = $cstmt->fetch();
    echo 'Command: '; echo $cmd[1]; echo "  Parameter: "; echo $cmd[2]; echo "\n";
	
  if($cmd[1] > 0 && $cmd[0] != $last_cmd) {     // If there is a command in the queue send to Arduino
    $ard_cmd = sprintf("99,%s,%s,%s", $cmd[1], $cmd[2], $cmd[0] )	;
    echo $ard_cmd;  echo "\n";
    fwrite($arduino,$ard_cmd);
    $last_cmd = $cmd[0];
    
    if($cmd[1] == "3" || $cmd[1] =="4" || $cmd[1] =="5") {                         // If a "LOOK" command is issued
        sleep(3);
        exec('python /home/pi/webcam.py');
    }
       
  }
	
}
	
    	/*** close the database connection ***/
    	$dbh = null;
    	$sth = null;

?>
