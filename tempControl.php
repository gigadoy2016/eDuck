<?php
require("lib/phpMQTT.php");
 $host 			= "m11.cloudmqtt.com"; 
 $port 			= 15507;
 $username 		= "monitor"; 
 $password 		= "123456789"; 
 $topic 		="DEVICE/01/command/";


if(!(empty($_GET["t1"]) && empty($_GET["t2"]))){
	if(!empty($_GET["t1"]))	$t1 = $_GET["t1"];
	
	if(!empty($_GET["t2"]))	$t2 = $_GET["t2"];
	
	if(!empty($_GET["led1"]))	$led1 = $_GET["led1"];
	else $led1 =0;

/************************* log Json **********************************/

	$array = array('t1'=>$t1, 't2'=>$t2, 'led1'=>$led1);
	$myfile = fopen("led.json", "w") or die("Unable to open file!");
	fwrite($myfile, json_encode($array));
	fclose($myfile);
/*********************** End log Json *******************************/

/************************* MQTT to Cloud ****************************/
	$mqtt = new phpMQTT($host, $port, "ClientID".rand()); 

 	if ($mqtt->connect(true,NULL,$username,$password)) {
 		$mqtt->publish($topic,$led1, 0);
    	$mqtt->close();
    	echo "send topic".$topic." message".$led1."<br>";
 	}else{
	 	echo "Fail or time out<br />";
 	}
	
}

$myfile = fopen("led.json", "r") or die("Unable to open file!");
echo fread($myfile,filesize("led.json"));
fclose($myfile);

?>