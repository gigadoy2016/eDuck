<?PHP
require("phpMQTT.php");

// subscribe.php

 $host 			= "m11.cloudmqtt.com"; 
 $port 			= 15507;
 $username 		= "ktbwgckp"; 
 $password 		= "xc_om3Hl4iYn"; 
 $message 		= 0;
 $topic 		="LED01";

if(!(empty($_GET["topic"]) && empty($_GET["message"]))){
	if(!empty($_GET["topic"]))	$topic = $_GET["topic"];
	if(!empty($_GET["message"]))$message = $_GET["message"];
	
// ******************** MQTT ************************ \\
	$mqtt = new phpMQTT($host, $port, "ClientID".rand()); 

 	if ($mqtt->connect(true,NULL,$username,$password)) {
 		$mqtt->publish($topic,$message, 0);
    	$mqtt->close();
    	echo "send topic".$topic." message".$message;
 	}else{
	 	echo "Fail or time out<br />";
 	}
}else{
	echo "Can't found Topic";
}




?>