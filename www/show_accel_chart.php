<?php
  
 header('content-type: image/png');

// Used Google Chart API to render an accelleration chart for x/y/z over time
include('database_creds.inc');

$limit = 60;			// How many results to show in chart

try {
    $dbh = new PDO("mysql:host=$hostname;dbname=mapdb", $username, $password);
    /*** echo a message saying we have connected ***/
    //echo 'Connected to database<br>';

	$sth = $dbh->query('SELECT COUNT(uid) FROM telemetry');
	$startrow = $sth->fetchColumn(); 
	$startrow -= $limit;
	
	//printf("Startrow is %s <br>", $startrow); 


	$sql = 'SELECT * FROM telemetry WHERE uid > :startrow LIMIT :limit ';
  
	$sth = $dbh->prepare($sql);
	$sth->bindValue(':startrow', $startrow, PDO::PARAM_INT);
	$sth->bindValue(':limit', $limit, PDO::PARAM_INT);
	$sth->execute();
   
   
  $url = 'https://chart.googleapis.com/chart';
  $chd = 't:';
  $chtt = 'Motion: ';
  $motion = "";
  $gps_lat = "";
  $gps_long = "";
  $gps_alt= "";
  $accel_x = "";
  $accel_y = "";
  $accel_z = "";
  $heading = "";
  


while ($row = $sth->fetch(PDO::FETCH_ASSOC)) {
	
	
 	$accel_x .= ($row["accel_x"]*10) . ',';
 	$accel_y .= ($row["accel_y"]*10) . ',';
 	$accel_z .= ($row["accel_z"]*10) . ',';
        //$chtt = 'HEADING: ' . $row["heading"];

}
 
  $accel_x = substr($accel_x, 0, -1);
  $accel_y = substr($accel_y, 0, -1);
  $accel_z = substr($accel_z, 0, -1);

  $chd .=  $accel_x . '|'  . $accel_y . '|'  . $accel_z; 
  //$chtt .= $row["heading"];

//printf("CHD = %s", $chd);

  // Add data, chart type, chart size, and scale to params.
  $chart = array(
    'cht' => 'lc',
	'chtt' => $chtt,
	'chxr' => '0,0,60',
	'chds' => '0,20',
    'chg'  => '8.33,0,5,5',
	'chs'  => '400x100',
	'chxt' => 'x,y',
	'chxl' => '1:|AX|AY|AZ',
	'chco' => '000000,FF0000|00FF00|0000FF',
    'chd' => $chd);


  // Send the request, and print out the returned bytes.
  $context = stream_context_create(
    array('http' => array(
      'method' => 'POST',
      'content' => http_build_query($chart))));
  fpassthru(fopen($url, 'r', false, $context));
  

    /*** close the database connection ***/
    $dbh = null;
    $sth = null;
}
catch(PDOException $e)
    {
    echo $e->getMessage();
    }

?>

