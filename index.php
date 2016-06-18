<?
/*
*		create by Xanio
*		date 10/11/2015
*
*		switch by https://proto.io/freebies/onoff/
*/

 	$range = file('led.json');		// อ่านค่าเงื่อนไขถ่าน text file

?>
<html>
	<head>
		<title>eDuck temperature</title>
		<meta charset="utf-8">
		<script src="lib/js/jquery-ui-1.11.4.custom/jquery-1.11.3.min.js"></script>
		<script src="lib/js/jquery-ui-1.11.4.custom/jquery-ui.js"></script>
		<script src="lib/js/scriptMQTT.js"></script>
		<link rel="stylesheet" href="lib/js/jquery-ui-1.11.4.custom/jquery-ui.css">
		<link rel="stylesheet" href="css/style.css">
		<link href="css/bootstrap.min.css" rel="stylesheet">
		<script type="text/javascript" src="lib/js/bootstrap.min.js"></script>
		<style>
			.test{
				/*border:1px solid #000;*/
				width:800px;
				height:400px;
			}

			.DEBUG_Border	{border:0px solid #000;}
			.style_1		{font-size: 10px}

			#statusMQTT.connect 	{background: #E18C1A;color: #FFF;}
			#statusMQTT.connected 	{background: #00AE04;color: #FFF;}

			h3		{height:40px;font-size:11px}
			td		{padding: 2px;}
		</style>


		<script>
		var tempo_01 = 0;
		var switch_01 = 0;
		var APIKey = "OJDJI20NWYIRFWWB"; 			//thinkSpeak.com
		var Time_1 = 0;
		var messageSubscrip;

		var config = {
			mqtt_server: "m11.cloudmqtt.com",
			mqtt_websockets_port: 35507,
			mqtt_user: "monitor",
			mqtt_password: "123456789",
			mqtt_topic: "DEVICE/01/status/"
		};

		var jqxhr = $.getJSON( "tempControl.php", function( jsonData ) {
			$( "#slider-range" ).slider({
				values: [jsonData['t1'] ,jsonData['t2']],
			});
			switch_01 = jsonData['led1'];
			adjustControl();
		});

		$(function(){
      var client = new Paho.MQTT.Client(config.mqtt_server, config.mqtt_websockets_port, "test");
			client.connect({
				useSSL: true,
				userName: config.mqtt_user,
				password: config.mqtt_password,
				onSuccess: function() {			// Once a connection has been made, make a subscription and send a message.
					console.log("onConnect");
					$("#statusMQTT").text("Ready").removeClass().addClass("btn btn-success");
					client.subscribe(config.mqtt_topic);

				},
				onFailure: function(e) {
					$("#status").text("Error : " + e).removeClass().addClass("error");
          console.log("MQTT Connection losed");
			 		console.log(e);
				}
			});

			client.onMessageArrived = function(message) {
				messageSubscrip = JSON.parse(message.payloadString);
				console.log(message.payloadString);

				if (messageSubscrip.led_01 != null ) {
						$("#statusDevice").text("Ready").removeClass().addClass("btn btn-success");
						onOffSwitch(message.payloadString);
						Time_1 = Date.now();
				}
			}

      // called when the client loses its connection
      client.onConnectionLost = function(responseObject){
        if (responseObject.errorCode !== 0) {
          console.log("onConnectionLost:"+responseObject.errorMessage);
          console.reconnect();
        }
      }

			//$("#test").html($( "#amount" ).val());
			setInterval(function(){
				$.get( "https://api.thingspeak.com/channels/62976/fields/1/last",function(data){
					tempo_01 = data;
				});
				$("#temp_01").html(tempo_01 + "ºC");
				$("#thermo_glass").height(tempo_01+"%");
			},1000);

			$( "#slider-range" ).slider({
						range: true,
						min: 0,
						max: 100,
						values: [25 ,50],
						slide: function( event, ui ) {	controlTemporature();	}
			});

			$("#myonoffswitch").click(function(){
				controlTemporature();
			});

			adjustControl();

			setInterval(function(){
				var resume = Date.now() - Time_1;
				if(resume < 15000){
					//alert(resume);
					$("#statusDevice").text("Ready").removeClass().addClass("btn btn-success");
				}else
					$("#statusDevice").text("Warning").removeClass().addClass("btn btn-warning");
			},5000);

		});

		function controlTemporature(){
			var value = $( "#slider-range" ).slider( "option", "values" );


			if ($("#myonoffswitch").is(':checked')){
				switch_01 = 1;
			}else{
				switch_01 = 0;
			}

			$( "#amount" ).val(  value[ 0 ] + " <-> " + value[ 1 ] +"°C");
			$.get( "tempControl.php", { t1: value[ 0 ], t2: value[ 1 ] ,'led1':switch_01} );

		}

		function adjustControl(){
			$( "#amount" ).val( $( "#slider-range" ).slider( "values", 0 ) +" <-> " + $( "#slider-range" ).slider( "values", 1 ) +"ºC");
			$("#myonoffswitch").prop( "checked", switch_01 );
		}

		function onOffSwitch(data){
			var obj = JSON.parse(data);
			$("#ledStatus").text(obj.led_01);
			$("#myonoffswitch").prop( "checked", obj.led_01 );
		}


		</script>
	</head>
	<body>
<!-------------------------------->
<!--widget thermometer			-->
<!-- 							-->

		<div class="ui-accordion ui-widget ui-helper-reset test">
			<h3 class="ui-accordion-header ui-state-default ui-accordion-icons ui-corner-all">Thermometer</h3>
			<div style="padding:5px" >
				<div style="width:330px;float:left;" class="DEBUG_Border">
					<div class="" style="float:left; width:196px; background-color:#E3F6CE;" class="DEBUG_Border">
						<h3 class="ui-accordion-header ui-state-default ui-accordion-header-active ui-state-active ui-corner-top ui-accordion-icons" >Status</h3>
						<div style="padding:3px 3px 20px 3px" class="DEBUG_Border">
							<table class="style_1">
								<tr>
									<td>MQTT Broker : </td>
									<td><button type="button" id="statusMQTT" class="btn btn-warning">Warning</button></td>
								</tr>
								<tr>
									<td>Device 1:</td>
									<td><button type="button" id="statusDevice" class="btn btn-warning">Warning</button></td>
								</tr>
							</table>

						</div>
						<h3 class="ui-accordion-header ui-state-default ui-accordion-header-active ui-state-active ui-corner-top ui-accordion-icons" style="font-size:14px">Temperature setting</h3>
						<div style="padding:3px 3px 20px 3px" class="DEBUG_Border">
							<p>
								<input type="text" id="amount" readonly style="border:0; color:#f6931f;font-size:12px;margin-left:50px;background-color:#E3F6CE;width:100px">
							</p>
							<div id="slider-range"></div>
						</div>
					<!-- Alert Setting panel -->
						<h3 class="ui-accordion-header ui-state-default ui-accordion-header-active ui-state-active ui-corner-top ui-accordion-icons">LED light Display</h3>
						<!-- onoffswitch -->
						<div style="height:80px;padding:3px;">
							<div class="onoffswitch" style="margin-top:20px;margin-left:45px ">
								<input type="checkbox" name="onoffswitch" class="onoffswitch-checkbox" id="myonoffswitch" checked>
								<label class="onoffswitch-label" for="myonoffswitch">
									<span class="onoffswitch-inner"></span>
									<span class="onoffswitch-switch"></span>
								</label>
							</div>
							<button type="button" id="ledStatus" class="btn btn-warning">0</button>
						</div>
						<!-- onoffswitch -->
					<!--End Alert Setting panel -->
					</div>
					<div class="donation-meter" style="float:right; ">
						<span class="glass">
							<span class="amount" style="height: 36%"id="thermo_glass"></span>
							<strong class="total textAlignVer" style="bottom: 30%" id="temp_01"></strong>
						</span>
						<div class="bulb">
							<span class="red-circle"></span>
							<span class="filler">
								<span></span>
							</span>
						</div>
					</div>
				</div>
<!--

https://thingspeak.com/channels/62976

-->
				<div style="float:right">
					<iframe width="450" height="260" style="border: 1px solid #cccccc;" src="http://api.thingspeak.com/channels/62976/charts/1?width=450&height=260&dynamic=true&title=iDuck" ></iframe>
				</div>
			</div>
		</div>
		<div id="test">

		</div>
	</body>
</html>
