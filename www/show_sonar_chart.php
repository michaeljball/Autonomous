<?php
  
 header('content-type: image/png');

 include 'database_creds.inc';

$limit = 360;			// How many results to show in chart

try {
    $dbh = new PDO("mysql:host=$hostname;dbname=mapdb", $username, $password);
    /*** echo a message saying we have connected ***/
   // echo 'Connected to database<br>';

	$sth = $dbh->query('SELECT MAX(uid) FROM scanning');
	$maxuid = $sth->fetchColumn(); 
	

 	$sql = 'SELECT ardtime FROM scanning WHERE uid = :maxuid';
   $sth = $dbh->prepare($sql);
	$sth->bindValue(':maxuid', $maxuid, PDO::PARAM_INT);
	$sth->execute();
   $row = $sth->fetch(PDO::FETCH_ASSOC);
   $ardtime .= $row['ardtime'];
  
 
//	 printf("MAXUID = %s, ARDTIME = %s<BR>",$maxuid, $ardtime);
	
	$sql = 'SELECT * FROM scanning WHERE ardtime = :ardtime ORDER BY offset';
  
	$sth = $dbh->prepare($sql);
	$sth->bindValue(':ardtime', $ardtime, PDO::PARAM_STR);
	$sth->execute();
     
  $i=1;
  $url = 'https://chart.googleapis.com/chart';
  $chd = 't:';
  $chxl= '0:';
  


while ($row = $sth->fetch(PDO::FETCH_ASSOC)) {
// printf("ardtime %s, radius %s, offset %s  \n", $row["ardtime"], $row["radius"], $row["offset"]);
	
	
	if ($row["radius"] > 0) {
		$chd .= $row["radius"] . ','; 
    		$chxl .= $i . '|';
		$i++;
	}

}
 
  $chd = substr($chd, 0, -1);
  $chxl = substr($chxl, 0, -1);

  $chart = array(
    'cht' => 'rs',
    'chs' => '280x280',
    'chds' => '0,60',
    'chxr' => '0,0.0,360.0',
    'chg' =>  '25.0,25.0,4.0,4.0',
	'chm' => 'h,0000FF,0,1.0,4.0',
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

