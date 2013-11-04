<?php
#################################################################################################################
# decode_nmea.php 
# Written by Michael Ball  October 2013
# 
# This application runs on the Raspberry Pi on the autonymous rover, and decodes 
# standard NMEA sentences from the GPS, uploading them to the rover's database.
# 
#################################################################################################################

// Converts DMS ( Degrees / minutes / seconds ) 
// to decimal format longitude / latitude
function DMStoDEC($dms, $longlat){

	if($longlat == 'lattitude'){
		$deg = substr($dms, 0, 2);
		$min = substr($dms, 2, 8);
		$sec = '';
	}
	if($longlat == 'longitude'){
		$deg = substr($dms, 0, 3);
		$min = substr($dms, 3, 8);
		$sec='';
	}
	

    return $deg+((($min*60)+($sec))/3600);
}   

include 'database_creds.inc'; 

//Set timezone
date_default_timezone_set('America/Toronto');
print("Timezone Set:  ");
//Connect to GPS
exec('stty -F /dev/ttyAMA0 9600 raw');
$gps = fopen("/dev/ttyAMA0", "r+");
print("GPS attached:  ");

$sql_gps = "INSERT INTO gps (sentence, status, latitude, longitude, altitude, speed, track, mode, fix, sats, time_utc) 
                     VALUES (:sentence, :status, :latitude, :longitude, :altitude, :speed, :track, :mode, :fix, :sats, :time_utc);" ;

	
	try {
   	 $dbh = new PDO("mysql:host=$hostname;dbname=mapdb", $username, $password);
   	 /*** echo a message saying we have connected ***/
   	 //echo 'Connected to database<br>';


	}
	catch(PDOException $e) {
   	 echo $e->getMessage();
  }




//Read data from GPS
while($gps){
	$buffer = fgets($gps);
	if(substr($buffer, 0, 6)=='$GPRMC'){
		echo $buffer."\n";
	
		$gprmc = explode(',',$buffer);
		$gpsdata['timestamp'] = strtotime('now');
		$gpsdata['utc_time'] = $gprmc[1];
		$gpsdata['utc_date'] = $gprmc[9];
		$gpsdata['status'] = $gprmc[2];

		$gpsdata['lattitude_dms'] = $gprmc[3];
		$gpsdata['lattitude_decimal'] = DMStoDEC($gprmc[3],'lattitude');
		$gpsdata['lattitude_direction'] = $gprmc[4];
		
		$gpsdata['longitude_dms'] = $gprmc[5];
		$gpsdata['longitude_decimal'] = DMStoDEC($gprmc[5],'longitude');
		$gpsdata['longitude_direction'] = $gprmc[6];
		
		$gpsdata['speed_knots'] = $gprmc[7];

		$gpsdata['bearing'] = $gprmc[8];
		$gpsdata['sats'] = '0';		
		$gpsdata['fix'] = '0';	
		$gpsdata['google_map'] = 'http://maps.google.com/maps?q='.$gpsdata['lattitude_decimal'].','.$gpsdata['longitude_decimal'].'+(PHP Decoded)&iwloc=A';
	
		print_r($gpsdata);
		echo "\n\n";
		
  	$stmt = $dbh->prepare($sql_gps);
		$stmt->execute(array(':sentence' => $gprmc[0],
		                     ':status' => $gprmc[2],
		                     ':latitude' => $gpsdata['lattitude_decimal'], 
		                     ':longitude' => $gpsdata['longitude_decimal'], 
		                     ':altitude' => '0', 
		                     ':speed' => $gprmc[7], 
		                     ':track' => $gprmc[8], 
		                     ':mode' => $gprmc[11], 
		                     ':fix' => $gpsdata['fix'], 
		                     ':sats' => $gpsdata['sats'], 
		                     ':time_utc' => $gprmc[1]));
		                     
		$affected_rows = $stmt->rowCount();

	
		
		
	} else if(substr($buffer, 0, 6)=='$GPGGA'){
		echo $buffer."\n";
	
	 //  $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47


		$gpgga = explode(',',$buffer);
		$gpsdata['timestamp'] = strtotime('now');
		$gpsdata['utc_time'] = $gpggc[1];
		if($gpggc[6] >0) $gpsdata['fix'] = $gpggc[6];

		$gpsdata['lattitude_dms'] = $gpggc[2];
		$gpsdata['lattitude_decimal'] = DMStoDEC($gpggc[2],'lattitude');
		$gpsdata['lattitude_direction'] = $gpggc[3];
		
		$gpsdata['longitude_dms'] = $gpggc[4];
		$gpsdata['longitude_decimal'] = DMStoDEC($gpggc[4],'longitude');
		$gpsdata['longitude_direction'] = $gpggc[5];
		
		if($gpggc[7] >0) $gpsdata['sats'] = $gpggc[7];

		if($gpggc[8] >0) $gpsdata['hdop'] = $gpggc[8];
		if($gpggc[9] >0) $gpsdata['altitude'] = $gpggc[9];
		
		
		$gpsdata['google_map'] = 'http://maps.google.com/maps?q='.$gpsdata['lattitude_decimal'].','.$gpsdata['longitude_decimal'].'+(PHP Decoded)&iwloc=A';
	
		print_r($gpsdata);
		echo "\n\n";
		
  	$stmt = $dbh->prepare($sql_gps);
		$stmt->execute(array(':sentence' => $gpggc[0],
		                     ':status' => $gprmc[2],
		                     ':latitude' => $gpsdata['lattitude_decimal'], 
		                     ':longitude' => $gpsdata['longitude_decimal'], 
		                     ':altitude' => $gpsdata['altitude'], 
		                     ':speed' => $gprmc[7], 
		                     ':track' => $gprmc[8], 
		                     ':mode' => $gprmc[11], 
		                     ':fix' => $gpggc[6], 
		                     ':sats' => $gpsdata['sats'],	
		                     ':time_utc' => $gpsdata['utc_time']));		

		$affected_rows = $stmt->rowCount();
	
		
		
}
	
}
    	/*** close the database connection ***/
    	$dbh = null;
    	$sth = null;

?>
