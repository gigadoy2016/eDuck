<?
if(!(empty($_GET["t1"]) && empty($_GET["t2"]))){
	if(!empty($_GET["t1"]))	$t1 = $_GET["t1"];
	if(!empty($_GET["t2"]))	$t2 = $_GET["t2"];

	$myfile = fopen("led.txt", "w") or die("Unable to open file!");
	$txt = "temperature1[".$t1.",".$t2."];\n";
	fwrite($myfile, $txt);
	fclose($myfile);

	
}
$range = file('led.txt');		// อ่านค่าเงื่อนไขถ่าน text file	

//print_r($range[0]);
$firstPosition = strpos($range[0],"[")+1;
$middlePosition = strpos($range[0],",");
$lastPosition = strpos($range[0],"]");

$t1 = substr($range[0],$firstPosition,$middlePosition-$firstPosition);
$t2 = substr($range[0],$middlePosition+1,$lastPosition-($middlePosition+1));
//echo "<br>".$lastPosition."-".$middlePosition;
echo '{"t1":'.$t1.',"t2":'.$t2.'}';



?>