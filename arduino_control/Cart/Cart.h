//This contains definitions for pin outs of the cart
//And the cart class, which can be used to drive the cart.


/*Control relay pin definitions*/
#define EMERGENCY_RELAY 29 //Power line which starts up the vehicle. Must be held closed to operate vehicle. If unpowered, opens and stops vehicle.
#define IGNITION_RELAY 27 //Power line which must be held closed to switch to FWD or REV
#define	FORWARD_RELAY 23 //If closed, cart drives in Forward mode
#define REVERSE_RELAY 25 //If closed, cart drives in Reverse mode
//If neither the Forward or Reverse relays are closed, but both the Emergency relay and ignition are closed, the cart is in neutral
#define ACCEL_FAILSAFE 26 //Switch for acceleration pedal. In order to send acceleration signal without faulting, this must be closed before signaling
#define BRAKE_FULLSTOP 28 //Switch for brake pedal. Closing this WHILE the brake signal is >3.5V signals a full-stop
#define ACCEL_ENCODER_ENABLE 24 //Switch for swapping to spoofed accel signal in. These should simply be closed upon autonomous mode start
#define BRAKE_ENCODER_ENABLE 22 //Switch for swapping to spoofed brake signal in. These should simply be closed upon autonomous mode start
#define CLOSE_RELAY(x) (pinMode(x,OUTPUT))
#define OPEN_RELAY(x) (pinMode(x,INPUT))
#define SWITCHING_TIME 5
#define PEDAL_DELAY 0
#define STEPPER_RELAY 30

/*Accel & Brake signal definitions*/
#define ACCEL 6
#define BRAKE 5
#define SPOOF_SCALE (float(4)*170/float(5))


/*Sensor pins*/
#define HALL_SENSOR 19

/*Stepper pins*/
#define ST_1 39 //stepper drive pin 1
#define ST_2 37 //stepper drive pin 2
#define ST_3 35 //stepper drive pin 3
#define ST_4 33 //stepper drive pin 4

#define STE_1 2 //stepper encoder pin B
#define STE_2 3 //stepper encoder pin A
#define STE_3 18 //stepper encoder pin index



/*Cart Class definition*/

class Cart{
	public:
		Cart();

        void driveBegin();

        void setCartForward();

        void setCartReverse();

        void setCartNeutral();

        void driveShutdown();

    private:
        int state;
};