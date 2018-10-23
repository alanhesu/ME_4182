
// Cart to Index


///////////////////////////DECLARE ALL REQUIREMENTS//////////////////////////////////////
var app = require('express')();
var http = require('http').Server(app);

var SerialPort = require("serialport").SerialPort;//for GPS and Bauderate

var io = require('socket.io')(http);
var ConnController = require('node-pid-controller');
var LidController = require('node-pid-controller');
var fs = require('fs'); //for data logging info and writing to text file
var five = require("johnny-five");

///////////////////////////END - DECLARE ALL REQUIREMENTS//////////////////////////////////////




///////////////////////////DECLARE AND INITIALIZE ARDUINO BOARDS//////////////////////////////////////
var ControlArd = new five.Board({ //configure port for Control Arduino. This arduino includes steering, acceleration and brakes.
		port : "COM28"
	});
var SensorArd = new five.Board({ //configure port for Control Arduino. This arduino includes speed sensors, lidar and others.
		port : "COM17"
	});
	
var serialPort = new SerialPort("COM1",{
		baudrate: 57600
	});
	 
///////////////////////////END - DECLARE AND INITIALIZE ARDUINO BOARDS//////////////////////////////////////





///////////////////////////DECLARE AND INITIALIZE INDEX FILE//////////////////////////////////////
app.get('/', function CallIndex(req, res) {
	res.sendfile('index.html');
});

http.listen(5000, function ListenHost() {
	console.log('listening on *:5000');
});
///////////////////////////END - DECLARE AND INITIALIZE INDEX FILE//////////////////////////////////////





///////////////////////////DELCARE GLOBAL VARIABLES//////////////////////////////////////
//////CONTROL ARDUINO//////
//PINS
var strpinPWM = 11;
var strpinDG = 52;
var brkpinPWM = 12;
var accpinPWM = 13;
var micpinDG = 53;
var hrnpinDG = 51;
var stapinDG = 50;
var rvrpinDG = 49;
var fwdpinDG = 48;
var readAngPin = 0;
var readDirPin = 1;
//END - PINS

//CONTROL
var accGlobal = 0;
var preSentAcc = 1;
var brkGlobal = 0;
var preSentBrk = 1;
var AccScale = 140;
var BrkScale = 200;
var angGlobal = 0;
//END - CONTROL
//////END - CONTROL ARDUINO//////

//////SENSOR ARDUINO//////

//PINS///
var SpeedPin=5;
var preCountSpeed=0;
var preSpeedTimeStamp=0;
var deltaDist=0;
var preDist=0;
var DistanceTraveled=0;
// END PINS///

//////END - SENSOR ARDUINO////////

////////GPS////////////////////////

var latitude = 0;
var longitude = 0;
var data1 = '';
var data2 ='';
var data3 ='';
var data4 ='';

//////CONNECTION TESTER//////
var preTimeStamp = 0;
var nowTimeStamp = 0;
var WarningTime = 700; //in milliseconds
var EmergencyTime = 1500;//in milliseconds
var preConnThrottle;
//////END - CONNECTION TESTER//////

//////LIDAR//////

var lidist=0;
var lideg=0;
var preLidarThrottle;
//////END - LIDAR//////

//////THROTLER//////
var ConnThrottle = 1;//connection throttle
var ConnTarget = 1;//connection Target
var connTrigger = 0;//connection Trigger
var EmePreTime =0;
var EmePreTime = 0;


var LidarThrottle = 1;//lidar throttle
var LidarTarget = 1;
var lidarTrigger = 0;//lidar Trigger

var ctr = new ConnController(0.2/2, 0.01/2, 0.1/2,1); // initiate PID controller for connection throttling
ctr.setTarget(ConnTarget);
var ltr = new LidController(0.2/2, 0.01/2, 0.1/2,1); // initiate PID controller for lidar throttling
ltr.setTarget(LidarTarget);

var LagChecker = setInterval(function () {LagCheckAndTarget()}, 100);//check lag time every 100 milliseconds
//////END - THROTLER//////
///////////////////////////END - DELCARE GLOBAL VARIABLES//////////////////////////////////////







///////////////////////////THROTLE////////////////////////////////////////////

function throttle() { ////update throttle values every 20 ms
	
	//ConnThrottle=updateThrottle(ConnTarget,ConnThrottle,ctr); //executes updatethrottle function - updates the throttle value according to a pre-set target (target set in security section)
	
	LidarThrottle=updateThrottle(LidarTarget,LidarThrottle,ltr); //executes updatethrottle function - updates the throttle value according to a pre-set target (target set in security section)
	
	var SendEmeVal = new Date();
	EmeTime = SendEmeVal.getTime(); 

	if (((connTrigger == 1) || (lidarTrigger == 1))&&(EmeTime-EmePreTime>50)) { //////if Trigger is 1, the connection assumed lost and local control is taken (send values to arduino every 50 ms). Once connection is re-established->Trigger is set //to 0
		//only send every 50 milliseconds (avoid saturation)
		accGlobal=1; //makes acceleration 0
		SendAcceleration(accGlobal);
		//brkGlobal=0; //makes brake 50%
		//SendBrake(brkGlobal);
		EmePreTime = EmeTime;
		
	}
}

function updateThrottle(UpTarget,UpValue,UpCat){
	
	UpCat.setTarget(UpTarget);
	correction = UpCat.update(UpValue); ////calculates update value according to target set
	UpValue += correction; ////applies calculated value

	if (UpValue > 1) { ////////corrects for overshooting
		UpValue = 1;
	} else if (UpValue < 0) { ////////corrects for undershooting
		UpValue = 0;
	}
	return UpValue
}

///////////////////////////END - THROTLE////////////////////////////////////////////






///////////////////////////SECURITY////////////////////////////////////////////

/////CONECTION TESTER/////////



function LagCheckAndTarget(){ //checks lag time every 100 milliseconds and sets target for throttling
	var d = new Date();
	nowTimeStamp = d.getTime(); 
	
	var lag=nowTimeStamp-preTimeStamp;

	if (lag<=WarningTime){ //if lag is less than warning time then all is good
		ConnTarget = 1; //ideal target is 1
		connTrigger = 0;//set trigger to 0 - this means that the brake IS NOT triggered
	} else if (WarningTime<lag && lag<=EmergencyTime){//lag is larger than warning time, start throttling
		ConnTarget=1+(0-1)*(lag-WarningTime)/(EmergencyTime-WarningTime);//y=Ya+(Yb-Ya)*(x-Xa)/(Xb-Xa) set target with y=target and x=lag
	} else if (EmergencyTime<lag){//lag is larger than emergency time, set targets to stop cart
		ConnTarget = 0;
		connTrigger = 1;//set trigger to 1 - this means that the brake IS triggered
	};
		
	if (preConnThrottle!=ConnThrottle){
		console.log('Connection throttle (Lag) set to ' + ConnThrottle + '\r\n');
		preConnThrottle=ConnThrottle;
	}
};

io.on('connection', function ConnectionTester(socket) {//receives random number from operator page for connection check and sets throttling interval
	
	socket.on('ConnRanNum', function (ConnRanNum) {//receives random number from operator page
		var d = new Date();
		preTimeStamp = d.getTime(); 
	});
});
/////END - CONECTION TESTER/////////


/////LIDAR/////////

//lidar throttle should work similar to connection throttle. based on closest distance (lowest number) set lidar throttle until threashold is met => lidartriger is activated

/////END - LIDAR/////////

///////////////////////////END - SECURITY////////////////////////////////////////////











///////////////////////////CONTROL ARDUINO FUNCTIONS//////////////////////////////////////
function SendSteering(SteerVal) { //Send steering values to control arduino
	//var angle1 = Steering;
	if (SteerVal >= 0) { // SteerVal larger than zero is right turn
		var SteerArd = SteerVal * 255; //scale steering value for arduino
		var dir = 'RIGHT';
		if (SteerArd >= 250) {
			SteerArd = 250; // truncate steering angle
			ControlArd.analogWrite(strpinPWM, SteerArd); // send truncated value (250) to arduino
			ControlArd.digitalWrite(strpinDG, 1); // send HIGH (right) to the arduino
		} else {
			ControlArd.analogWrite(strpinPWM, SteerArd); // send value to arduino
			ControlArd.digitalWrite(strpinDG, 1); // send HIGH (right)LOW (left) to the arduino
		}
	} else if (SteerVal < 0) { // SteerVal smaller than zero is left turn
		var SteerArd = SteerVal * (-1) * 255; //scale steering value for arduino
		var dir = 'LEFT';
		if (SteerArd >= 250) {
			SteerArd = 250; // truncate steering angle
			ControlArd.analogWrite(strpinPWM, SteerArd); // send truncated value (250) to arduino
			ControlArd.digitalWrite(strpinDG, 0); // send LOW (left) to the arduino
		} else {
			ControlArd.analogWrite(strpinPWM, SteerArd); // send value to arduino
			ControlArd.digitalWrite(strpinDG, 0); // send LOW (left) to the arduino
		}
	}
	console.log('\r\n');
	console.log('STEERING value is: ' + SteerVal); //send feedback to console
	console.log('Arduino Value is: ' + dir + ' ' + SteerArd); //send feedback to console
}; //END - Send steering values to control arduino

function SendAcceleration(AccelVal) { //Send acceleration values to control arduino
	accGlobal = (-1 * AccelVal + 1) / 2 * (AccScale); //scale acceleration value for arduino
	ControlArd.analogWrite(accpinPWM, accGlobal * ConnThrottle * LidarThrottle); // send value to arduino
	if (accGlobal < 20) { //micro switch trigger
		ControlArd.digitalWrite(micpinDG, 0);
	} else {
		ControlArd.digitalWrite(micpinDG, 1);
	}
	
	if (preSentAcc!=accGlobal * ConnThrottle * LidarThrottle){
		console.log('\r\n');
		console.log('ACCELERATION value is: ' + AccelVal); //send feedback to console
		console.log('Arduin Value is: ' + (accGlobal * ConnThrottle * LidarThrottle) + ' Connection throtle at ' + (ConnThrottle) + ' LIDAR throtle at ' + (LidarThrottle));
		preSentAcc=accGlobal * ConnThrottle * LidarThrottle;
	}
	
}; //END - Send acceleration values to control arduino

function SendBrake(BrakeVal) { //Send Brake values to control arduino
	//var brk1 = BrakeInput;
	brkGlobal = (-1 * BrakeVal + 1) / 2 * (BrkScale); //scale brake value for arduino
	ControlArd.analogWrite(brkpinPWM, brkGlobal); // send value to arduino
	
	preSentBrk
	
	if (preSentBrk!=brkGlobal){
		console.log('\r\n');
		console.log('BRAKE is: ' + BrakeVal); //send feedback to console
		console.log('Arduino Value is: ' + (brkGlobal));
		preSentBrk=brkGlobal;
	}
	
	
}; //END - Send Brake values to control arduino

function SendForward(forward) { //Send forward value to control arduino
	ControlArd.digitalWrite(fwdpinDG, forward); // send value to arduino
	console.log('\r\n');
	console.log('Forward Button value is ' + forward); //send feedback to console
}; //END - Send forward value to control arduino

function SendReverse(reverse) { //Send reverse value to control arduino
	ControlArd.digitalWrite(rvrpinDG, reverse);
	console.log('\r\n');
	console.log('Reverse Button is Pressed ');
	console.log('Reverse Button value is ' + reverse);
}; //END - Send reverse value to control arduino

function SendHorn(horn) { //Send horn value to control arduino
	ControlArd.digitalWrite(hrnpinDG, horn);
	console.log('\r\n');
	console.log('Horn Button is Pressed ');
	console.log('Horn Button value is ' + horn);
}; //END - Send horn value to control arduino

function SendStart(start) { //Send start value to control arduino
	ControlArd.digitalWrite(stapinDG, start);
	console.log('\r\n');
	console.log('Start Button is Pressed ');
	console.log('Start Button value is ' + start);
}; //END - Send start value to control arduino

function ReportWheelPosition(angle, direction, socket) { //Report actual steering wheel position
	if (direction == 1) { //interpret direction value from arduino
		direction = 'LEFT';
	} else {
		direction = 'RIGHT';
	}
	angle = Math.round(angle / 425 * 360 / 3); //scaling -> 1 encoder rev is 425 counts -> scaled to 360 degrees -> 3 motor revs to a wheel rev
	socket.emit('SteeringAngle',angle); //send angle to operator page
	socket.emit('SteeringDirection', direction); //send direction to operator page
	//console.log('\r\n');
	//console.log('Steering angle is: ' + angle + ' degrees to the ' + direction);
}; //END - Report actual steering wheel position

///////////////////////////END - CONTROL ARDUINO FUNCTIONS//////////////////////////////////////



///////////////////////////INITIALIZE CONTROL ARDUINO //////////////////////////////////////
ControlArd.on("ready", function InitControlArduino() { //initialize control arduino and set pins status
	/* Mode 	Value 	Constant
	INPUT 	0 	Pin.INPUT
	OUTPUT 	1 	Pin.OUTPUT
	ANALOG 	2 	Pin.ANALOG
	PWM 	3 	Pin.PWM
	SERVO 	4 	Pin.SERVO */
	ControlArd.pinMode(strpinPWM, five.Pin.PWM); // Steering (Signal)
	ControlArd.pinMode(strpinDG, five.Pin.OUTPUT); // Steering (High/Low)
	ControlArd.pinMode(brkpinPWM, five.Pin.PWM); // Brake
	ControlArd.pinMode(accpinPWM, five.Pin.PWM); // Acceleration

	ControlArd.pinMode(micpinDG, five.Pin.OUTPUT); // Fwd (MicroSwitch)

	ControlArd.pinMode(hrnpinDG, five.Pin.OUTPUT); // horn/Emergency
	ControlArd.pinMode(stapinDG, five.Pin.OUTPUT); // Start
	ControlArd.pinMode(rvrpinDG, five.Pin.OUTPUT); // Reverse
	ControlArd.pinMode(fwdpinDG, five.Pin.OUTPUT); // Forward

	ControlArd.pinMode(readAngPin, five.Pin.ANALOG); // read angle from encoder
	ControlArd.pinMode(readDirPin, five.Pin.ANALOG); // read angle direction from encoder
}); //initialize control arduino
///////////////////////////END - INITIALIZE CONTROL ARDUINO //////////////////////////////////////


///////////////////////////SEND CONTROL ARDUINO VALUES//////////////////////////////////////
io.on('connection', function SocketSendControl(socket) {

	socket.on('Steering', function (Steering) { //send steering values to control arduino
		SendSteering(Steering);
	});
	socket.on('AccelInput', function (AccelInput) { //send acceleration values to control arduino
		SendAcceleration(AccelInput);
	});
	socket.on('BrakeInput', function (BrakeInput) { //send brake values to control arduino
		SendBrake(BrakeInput);
	});
	socket.on('FWD', function (FWD) { //send forward value to control arduino
		SendForward(FWD);
	});
	socket.on('REV', function (REV) { //send reverse value to control arduino
		SendReverse(REV);
	});
	socket.on('EMG', function (EMG) { //send horn value to control arduino -------------- TO DO, change signal in operator page to "HRN", not EMG
		SendHorn(EMG);
	});
	socket.on('STR', function (STR) { //send start value to control arduino
		SendStart(STR);
	});

});
///////////////////////////END - SEND CONTROL ARDUINO VALUES//////////////////////////////////////


///////////////////////////RECEIVE CONTROL ARDUINO VALUES//////////////////////////////////////
io.on('connection', function SocketReadControl(socket) {
	console.log('Sending Wheel Position Statistics');
	
	ControlArd.analogRead(0, function (angle) { //register angle for logging
		angGlobal = angle;
	});
	ControlArd.analogRead(1, function (direction) { //report angle and direction
		ReportWheelPosition(angGlobal, direction, socket);
	});


});
///////////////////////////END - RECEIVE CONTROL ARDUINO VALUES//////////////////////////////////////







///////////////////////////SENSOR ARDUINO FUNCTIONS//////////////////////////////////////

function SendSpeedOp(socket){ //
	var d = new Date();
	newSpeedTimeStamp = d.getTime(); 
	
	deltaDist=(DistanceTraveled-preDist);
	preDist=DistanceTraveled;
	
	Speed=(deltaDist/((newSpeedTimeStamp-preSpeedTimeStamp)/1000)*(3600/1000));
	Speed=Speed;
	preSpeedTimeStamp=newSpeedTimeStamp;
	
	socket.emit('CartSpeed', Speed); //send angle to operator page
	//console.log('Speed ' + Speed);
};


var LidarTimeout;
var LidarTimeout1;
var LidarTimeout2;

var LidarArray=[];
for (i=0;i<91;i++){
		LidarArray[i]=8000;
}

 function ClearLidarArray(array){
	//sets array values to 8000
	LidarArray=[];
	for (i=0;i<91;i++){
		LidarArray[i]=8000;
	}
	//LidarTarget = 1; //ideal target is 1
	//lidarTrigger = 0;//set trigger to 0 - this means that the brake IS NOT triggered
	//console.log('Lidar array has been reset');
	//console.log('Lidar target set to ' + LidarTarget);
	//console.log('Lidar throttle set to ' + LidarThrottle + '\r\n');	
} 

var warningDist=2000;
var emergencyDist=1500;
var minDist=600;

function LidarCheckAndTarget(string){ //
	
	LidarData = string.split(':');// separate info into distance and degrees and convert into integers
	lidist = parseInt(LidarData[0]);
	lideg = parseInt(LidarData[1]);
	
	if (lidist<minDist){lidist=8000;};
	
  if (lideg>=0 && lideg<=45){
		LidarArray[lideg+45]=lidist; // put distance into the arrays degree-equivalent index -> 0 to 45 degrees are placed in 45 to 90 index
	}else if (lideg>=315){
		LidarArray[lideg-315]=lidist;// put distance into the arrays degree-equivalent index -> 315 to 0 degrees(0=360)  are placed in 0 to 45 index
	};
	
	LidarDistance=Math.min.apply(null, LidarArray);
	
	
	if (LidarDistance>=warningDist){ //if distance is more that 4 [m] then all is good
		LidarTarget = 1; //ideal target is 1
		lidarTrigger = 0;//set trigger to 0 - this means that the brake IS NOT triggered
	} else if (emergencyDist<=LidarDistance && LidarDistance<warningDist){
		LidarTarget=1+(0-1)*(LidarDistance-warningDist)/(emergencyDist-warningDist);//y=Ya+(Yb-Ya)*(x-Xa)/(Xb-Xa) set target with y=target and x=LidarDistance
	} else if (minDist <LidarDistance && LidarDistance<emergencyDist){
		LidarTarget = 0; //
		lidarTrigger = 1;//set trigger to 1 - this means that the brake IS triggered
	};
	
	/* if (preLidarThrottle!=LidarThrottle){
		console.log('Lidar distance is ' + LidarDistance + '\r\n');
		console.log('Lidar target set to ' + LidarTarget + '\r\n');
		console.log('Lidar throttle set to ' + LidarThrottle + '\r\n');
		preLidarThrottle=LidarThrottle;
	}; */
		console.log('Lidar distance is ' + LidarDistance);
		console.log('Lidar target set to ' + LidarTarget);
		console.log('Lidar throttle set to ' + LidarThrottle + '\r\n');	
	
	clearTimeout(LidarTimeout); // stops countdown with new information arriving from arduino
	LidarTimeout = setTimeout(function(){ClearLidarArray(LidarArray);}, 1000);//starts countdown to clear Lidar Array. i.e. if 1[s] passes and no new info is received -> clear array.
	//
};



var SendLidarTrigger;


function LidarFilterAndSend(string,socket){
	var Sendstring;
	var lidistString;
	var lidegString;
	var LidarDistanceDifference;
	
	LidarData = string.split(':');// separate info into distance and degrees and convert into integers
	lidist = parseInt(LidarData[0]);
	lideg = parseInt(LidarData[1]);
	
	
	if (lidist<minDist){lidist=8000;};
	
	
	
	if (lideg>=0 && lideg<=45){  //fill lidar array
		
		LidarDistanceDifference=Math.abs(LidarArray[lideg+45]-lidist);//
		
		if(LidarDistanceDifference>100){
			SendLidarTrigger=1;
			LidarArray[lideg+45]=lidist; // put distance into the arrays degree-equivalent index -> 0 to 45 degrees are placed in 45 to 90 index
			clearTimeout(LidarTimeout1); // stops countdown with new information arriving from arduino
			LidarTimeout1 = setTimeout(function(){ClearLidarArray(LidarArray);}, 1000);//starts countdown to clear Lidar Array. i.e. if 1[s] passes and no new info is received -> clear array.
		};
		
	}else if (lideg>=315){
		
		LidarDistanceDifference=Math.abs(LidarArray[lideg-315]-lidist)
		
		if(LidarDistanceDifference>100){
			SendLidarTrigger=1;
			LidarArray[lideg-315]=lidist;// put distance into the arrays degree-equivalent index -> 315 to 0 degrees(0=360)  are placed in 0 to 45 index
			clearTimeout(LidarTimeout2); // stops countdown with new information arriving from arduino
			LidarTimeout2 = setTimeout(function(){ClearLidarArray(LidarArray);}, 1000);//starts countdown to clear Lidar Array. i.e. if 1[s] passes and no new info is received -> clear array.
			
			
		};
		
		
		
	
	};
	
	/////format data to send to operator page
	
	if (SendLidarTrigger==1){
		if (lideg!=0){//change coordinate system, 0 stays the same
			lideg=360-lideg; // changing coordinate direction of angle (originally + to right; now + to left). Contains numbers 1 to 45 and 315 to 359
		};
		lidistString=lidist.toString();//convert distance to string
		Sendstring=lidistString.concat(":");//add ":" to distance
		lidegString=lideg.toString();//convert degrees to string
		Sendstring=Sendstring.concat(lidegString);// add degrees to string --> "NNNN:DDD"
		
		if (lidist>minDist && lidist<5000){
			socket.emit('LIDATA', Sendstring); //sending the data
			//console.log(Sendstring);
		}; 
		
	};
	SendLidarTrigger=0;
	/////END - format data to send to operator page
	
	
	//clearTimeout(LidarTimeout); // stops countdown with new information arriving from arduino
	//LidarTimeout = setTimeout(function(){ClearLidarArray(LidarArray);}, 300);//starts countdown to clear Lidar Array. i.e. if 1[s] passes and no new info is received -> clear array.
};



function TurnCAMF(CAMF){ //
	if (CAMF==1){
		SensorServo.to(60);
		console.log('camera facing front');
	};
};

function TurnCAMR(CAMR){ //
	if (CAMR==1){
		SensorServo.to(15);
		console.log('camera facing right');
	};
};

function TurnCAML(CAML){ //
	if (CAML==1){
		SensorServo.to(110);
		console.log('camera facing left');
	};
};



///////////////////////////END - SENSOR ARDUINO FUNCTIONS//////////////////////////////////////




var SensorServo;
///////////////////////////INITIALIZE SENSOR ARDUINO //////////////////////////////////////
SensorArd.on("ready", function InitSensorArduino() { //initialize sensor arduino and set pins status
	/* Mode 	Value 	Constant
	INPUT 	0 	Pin.INPUT
	OUTPUT 	1 	Pin.OUTPUT
	ANALOG 	2 	Pin.ANALOG
	PWM 	3 	Pin.PWM
	SERVO 	4 	Pin.SERVO */
	
	SensorArd.pinMode(SpeedPin, five.Pin.ANALOG); // read speed from Arduino 
	SensorServo = new five.Servo({
		pin: 7,
		board: SensorArd,
		specs: {           // Is it running at 5V or 3.3V?
                        speed: five.Servo.Continuous.speeds["@5.0V"]
                }
	});
	
/* 	var move2 = setInterval(function () {
                //var angle= Math.random() * (120 - 1) + 1;
                SensorServo.to(15);
                var degree3=setTimeout(function(){SensorServo.to(60);},1000);
                var degree4=setTimeout(function(){SensorServo.to(110);},2000);
                //console.log(angle);
	},3000); */
				
	
}); //initialize sensor arduino
///////////////////////////END - INITIALIZE SENSOR ARDUINO //////////////////////////////////////

///////////////////////////RECEIVE SENSOR ARDUINO VALUES//////////////////////////////////////
io.on('connection', function SocketReadSensor(socket) {
	console.log('Reading Sensor Arduino');
	
	//var THROTTLER = setInterval(function () {throttle()}, 20);//set throttle update rate
	
	var SendSpeed = setInterval(function () {SendSpeedOp(socket)}, 1000); //sends current speed to Operator every 500 ms
	
	SensorArd.analogRead(SpeedPin, function (CountSpeed) { //report cart speed
		 DistanceTraveled=((CountSpeed)*Math.PI*0.4)/4*0.62;//0.4 wheel diameter 4 number of magnets on the wheel and 0.62 conversion from kmh to mph
		 //console.log('distance ' + DistanceTraveled);
		 //DistanceTraveled=CountSpeed;
	});
	
	SensorArd.on('string', function(string){
		//console.log(string);
		//LidarCheckAndTarget(string);
		LidarFilterAndSend(string,socket);
		//socket.emit('LIDATA', string);//sending the data
	})
	
	socket.on('CAMF', function (CAMF) { //turn camera to the front
		TurnCAMF(CAMF);
	});
	socket.on('CAMR', function (CAMR) { //turn camera to the right
		TurnCAMR(CAMR);
	});
	socket.on('CAML', function (CAML) { //turn camera to the left
		TurnCAML(CAML);
	});

});
///////////////////////////END - RECEIVE SENSOR ARDUINO VALUES//////////////////////////////////////

////////////////GPS////////////////////////


serialPort.on("open", function (){
	console.log('GPS serial port open');
});
	

io.on('connection', function SocketGPS(socket) {
	console.log('gps connected')
	
 	var GPSInt = setInterval(function () {
		socket.emit('GPSLatt', latitude);
		socket.emit('GPSLong', longitude); //send latest GPS data
		//console.log(latitude + ',' + longitude)
		//socket.emit('GPS1', latitude);
		//socket.emit('GPS2', longitude); //send latest GPS data
		//console.log('GPS1 ' +latitude + ' , GPS2 ' + longitude)
	}, 1000);
	
	serialPort.on('data', function(data){
		data = String(data);
        //console.log(data);
		if (data.charAt(data.length-5) == '*'){
          data = data1 + data2 + data3 + data;
		  //console.log(data);
		  data1 = "";
		  data2 = "";
		  data3 = "";
		  dataArray = data.split(',');
		  
		  latitude = dataArray[2];
		  latitude=Number(latitude)/100;
		  latitudeDegree = Math.floor(latitude); //obtain latitude degree
		  latitude=latitudeDegree+((latitude-latitudeDegree)*100/60); //transforms DD.mm.decimals to DD.decimals

		  longitude = dataArray[4];
		  longitude=Number(longitude)/100;
		  longitudeDegree= Math.floor(longitude);//obtain longitude degree
		  longitude = longitudeDegree+((longitude-longitudeDegree)*100/60); //transforms DD.mm.decimals to DD.decimals
		  
		  latdxn = dataArray[3];
		  if (latdxn == "S"){
			  latitude = latitude * -1;
			 //console.log("Latitude is: " + latitude);  
		  }
		  else {
			  //console.log("Latitude is: " + latitude);
		  }
		  londxn = dataArray[5];
		  if (londxn == "W"){
			  longitude = longitude * -1;
			  //console.log("Longitude is: " + longitude);
		  }
		  else {
			  //console.log("Longitude is: " + longitude);
		  }
		  alt = dataArray[8];
		  GPStime = dataArray[1];
	
		}	
        else if (data.substring(0,6) == "$CME=10"){
			//console.log(data);
		}
		else {
			if (data1 == ""){
				data1 = data;
			}
			else if(data2 == ""){
				data2 = data;
			}
			else if(data3 == ""){
				data3 = data;
			}
		}	
	});
	//var GPSInt = setInterval(function () {console.log(latitude + ',' + longitude)}, 1000);
});


 ////////////////END - GPS////////////////////////