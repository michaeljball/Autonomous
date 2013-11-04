

<?php
include 'database_creds.inc';

try {
    $dbh = new PDO("mysql:host=$hostname;dbname=mapdb", $username, $password);
    /*** echo a message saying we have connected ***/
    //echo 'Connected to database<br>';

	$sth = $dbh->query('SELECT COUNT(uid) FROM telemetry');
	$startrow = $sth->fetchColumn(); 
	
	$sql = 'SELECT * FROM telemetry WHERE uid = :startrow ';
  
	$sth = $dbh->prepare($sql);
	$sth->bindValue(':startrow', $startrow, PDO::PARAM_INT);
	$sth->execute();

	$row = $sth->fetch(PDO::FETCH_ASSOC);
        echo json_encode($row);


    /*** close the database connection ***/
    $dbh = null;
    $sth = null;
   // echo json_encode($heading);
}
catch(PDOException $e)
    {
    echo $e->getMessage();
    }

?>

	

