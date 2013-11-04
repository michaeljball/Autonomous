<?php
if (isset($argv)) {			// If running from the command line
    $cur_image = $argv[1];
}
else {
    $cur_image = $_GET['thumb'];
}

if (isset($cur_image)) {	
include 'database_creds.inc';


    try {
        $dbh = new PDO("mysql:host=$hostname;dbname=mapdb", $username, $password);
        /*** echo a message saying we have connected ***/
        //echo 'Connected to database<br>';

            $sth = $dbh->query('SELECT MAX(uid) FROM webcam');
            $startrow = $sth->fetchColumn(); 
            $startrow = $startrow - $cur_image;
            $sql = 'SELECT camimage FROM webcam WHERE uid = :startrow ';

            $sth = $dbh->prepare($sql);
            $sth->bindValue(':startrow', $startrow, PDO::PARAM_INT);
            $sth->execute();


            $row = $sth->fetch(PDO::FETCH_ASSOC);

       header("Content-type: image/jpeg");
       echo $row['camimage'];



        /*** close the database connection ***/
        $dbh = null;
        $sth = null;
    }
    catch(PDOException $e)
        {
        echo $e->getMessage();
        }
}
?>

