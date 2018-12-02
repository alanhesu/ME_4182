#include <Arduino.h>
#include "Cart.h"

Cart::Cart(){

}

void Cart::driveBegin() {
    CLOSE_RELAY(EMERGENCY_RELAY);
    delay(SWITCHING_TIME);
    CLOSE_RELAY(IGNITION_RELAY);
    delay(2000);
}

void Cart::setCartForward() {
    OPEN_RELAY(REVERSE_RELAY);
    delay(SWITCHING_TIME);
    CLOSE_RELAY(FORWARD_RELAY);
}

void Cart::setCartReverse() {
    OPEN_RELAY(FORWARD_RELAY);
    delay(SWITCHING_TIME);
    CLOSE_RELAY(REVERSE_RELAY);
}

void Cart::setCartNeutral() {
    OPEN_RELAY(FORWARD_RELAY);
    OPEN_RELAY(REVERSE_RELAY);
}

void Cart::driveShutdown() {
    OPEN_RELAY(IGNITION_RELAY);
    delay(2000);
    OPEN_RELAY(EMERGENCY_RELAY);
}
