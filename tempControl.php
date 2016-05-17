<?php
if(!(empty($_GET["t1"]) && empty($_GET["t2"]))){
	if(!empty($_GET["t1"]))	$t1 = $_GET["t1"];
	
	if(!empty($_GET["t2"]))	$t2 = $_GET["t2"];
	
	if(!empty($_GET["led1"]))	$led1 = $_GET["led1"];
	else $led1 =0;

	$array = array('t1'=>$t1, 't2'=>$t2, 'led1'=>$led1);
	$myfile = fopen("led.txt", "w") or die("Unable to open file!");
	fwrite($myfile, json_encode($array));
	fclose($myfile);
	
}

$myfile = fopen("led.txt", "r") or die("Unable to open file!");
echo fread($myfile,filesize("led.txt"));
fclose($myfile);

?>