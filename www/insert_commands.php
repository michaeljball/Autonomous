<?php
if (isset($argv)) {			// If running from the command line
    $command = $argv[1];
    $parameter = $argv[2];
}
else {
    $command = $_GET['command'];
    $parameter = $_GET['parameter'];
}

if (isset($command)) {			// If running from the command line
 include 'database_creds.inc'; 
	
	try {
   	 $dbh = new PDO("mysql:host=$hostname;dbname=mapdb", $username, $password);
   	 /*** echo a message saying we have connected ***/
   	 //echo 'Connected to database<br>';

  		$stmt = $dbh->prepare("INSERT INTO cmd(command,parameter) VALUES(:command,:parameter)");
		$stmt->execute(array(':command' => $command, ':parameter' => $parameter));
		$affected_rows = $stmt->rowCount();

	
    	/*** close the database connection ***/
    	$dbh = null;
    	$sth = null;
	}
	catch(PDOException $e) {
   	 echo $e->getMessage();
   }
}
?>
