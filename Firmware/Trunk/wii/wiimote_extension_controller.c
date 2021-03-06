/* Wii support based on:
   http://frank.circleofcurrent.com/cache/WII_CC_extension_lib.htm
   http://wiibrew.org/wiki/Wiimote/Extension_Controllers
*/

// README
/*
IMPORTANT NOTES:
-not working yet
-as the I2C-Bus pins are on pins C4 (SDA) and C5 (SCL) those have
 to be connected to the Wiimote plug
-due to the remapping S1 and S2 are used for Home and 4P, thus pass-through
 does not work and the 4066N has to be left out
-to support the USB firmware update the original USB-lines still work
-the Wiimote data lines have to be free of resistors and diodes, but the
 Wiimote VCC could use a diode to avoid battery draining

Wiimote Adapter
===============
The Dual Strike can act as a Wii Classic Controller when connected to a
Wiimote. You need a passive adapter from USB to the Wiimote extension port
for this working mode, its composition is detailed below.

If you get your Wiimote plug and cable of a working Wii extension (Nunchuck,
Classic Controller, ...), you can add a USB jack to the extension's side of
the cable. So you can continue using the extension by utilising your adapter.

Wiimote extension port layout (as seen looking on the Wiimote bottom): 
+---___---+
| 1  2  3 |
| ======= |
| 4  5  6 |
+---------+
1: Ground [white]
2: No Connection
3: SCL [yellow]
4: SDA [green]
5: Device Detect, in the connector wired to VCC [red]
6: VCC (3.3V) [red]

USB type B port layout (as seen looking at the port):
   --
 /    \
| 2  1 |
| ==== |
| 3  4 |
+------+
1: VCC (5V) [red]
2: D- [white]
3: D+ [green]
4: Ground [black]

Your adapter has to make the following connections:

USB Wires      | Wiimote Wires
---------------+---------------
D-     [white] | SCL    [yellow]
D+     [green] | SDA    [green]
VCC    [red]   | VCC    [red]
Ground [black] | Ground [white]

The adapter should be connected to the USB port before plugging it into
the Wiimote.
*/

#include "wiimote_extension_controller.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "wiimote.h"
#include "../DualStrike.h"

#ifndef uchar
#define uchar   unsigned char
#endif

extern uint8_t config[2];
extern uchar* data;

// device id starting at address 0xA400FA
PROGMEM const uchar classicControllerDeviceID[6] = {0x00, 0x00, 0xA4, 0x20, 0x01, 0x01};

// calibration data starting at address 0xA40020
PROGMEM const uchar classicControllerCalibrationData[16] = {
	/*
	// left stick X
	0b00000000, // -min
	0b00100000, // -mid
	0b00111111, // -max

	// left stick Y
	0b00000000, // -min
	0b00100000, // -mid
	0b00111111, // -max

	// right stick X
	0b00000000, // -min
	0b00010000, // -mid
	0b00011111, // -max

	// right stick Y
	0b00000000, // -min
	0b00010000, // -mid
	0b00011111, // -max

	// left trigger
	0b00000000, // -min
	0b00001111, // -max

	// right trigger
	0b00000000, // -min
	0b00001111, // -max
	*/

	/* Source: http://code.google.com/p/dolphin-emu/source/browse/trunk/Source/Plugins/Plugin_Wiimote/Src/EmuDefinitions.h */
    0xff, 0x00, 0x80, 0xff, 0x00, 0x80,
    0xff, 0x00, 0x80, 0xff, 0x00, 0x80,
    0x00, 0x00, 0x51, 0xa6
};

typedef struct {
	uchar	rx4_3Lx;
	uchar	rx2_1Ly;
	uchar   rx0Lt4_3Ry;	
	uchar   lt2_0Rt;
	uchar	rightDownBltMinusHomePlusBrtNA;
	uchar	bzlBbByBaBxBzrLeftUp;
} classicControllerData_t;

#define WII_CC_UP		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<0);
#define WII_CC_DOWN		wiiData->rightDownBltMinusHomePlusBrtNA &= ~(1<<6);
#define WII_CC_RIGHT	wiiData->rightDownBltMinusHomePlusBrtNA &= ~(1<<7);
#define WII_CC_LEFT		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<1);
#define WII_CC_Y 		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<5);
#define WII_CC_X		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<3);
#define WII_CC_ZR		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<2);
#define WII_CC_RT		wiiData->rightDownBltMinusHomePlusBrtNA &= ~(1<<1);
#define WII_CC_B		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<6);
#define WII_CC_A		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<4);
#define WII_CC_ZL		wiiData->bzlBbByBaBxBzrLeftUp &= ~(1<<7);
#define WII_CC_LT		wiiData->rightDownBltMinusHomePlusBrtNA &= ~(1<<5);
#define WII_CC_HOME		wiiData->rightDownBltMinusHomePlusBrtNA &= ~(1<<3);
#define WII_CC_MINUS	wiiData->rightDownBltMinusHomePlusBrtNA &= ~(1<<4);
#define WII_CC_PLUS		wiiData->rightDownBltMinusHomePlusBrtNA &= ~(1<<2);

void resetClassicControllerData() {
	classicControllerData_t* wiiData = (classicControllerData_t*)data;

	wiiData->rx4_3Lx = 0b10100000;
	wiiData->rx2_1Ly = 0b00100000;
	wiiData->rx0Lt4_3Ry = 0b00010000;
	wiiData->lt2_0Rt = 
	wiiData->rightDownBltMinusHomePlusBrtNA = 
	wiiData->bzlBbByBaBxBzrLeftUp = 0xFF;
}

void readInputWiiClassicController() {
	classicControllerData_t* wiiData = (classicControllerData_t*)data;

	resetClassicControllerData();

	// Left Joystick Directions
	if(CFG_LEFT_STICK) {
		if (!Stick_Up)
			wiiData->rx2_1Ly &= 0b11000000;
		else if (!Stick_Down)
			wiiData->rx2_1Ly |= 0x00111111;

		if (!Stick_Left)
			wiiData->rx4_3Lx &= 0b11000000;
		else if (!Stick_Right)
			wiiData->rx4_3Lx |= 0b00111111;
	}

	// Right Joystick Directions
	if(CFG_RIGHT_STICK) {
		if (!Stick_Up)
			wiiData->rx0Lt4_3Ry &= 0b11100000;
		else if (!Stick_Down)
			wiiData->rx0Lt4_3Ry |= 0b00011111;
		
		if (!Stick_Left) {
			wiiData->rx4_3Lx &= 0b00111111;
			wiiData->rx2_1Ly &= 0b00111111;
			wiiData->rx0Lt4_3Ry &= ~(1<<7);
		}
		else if (!Stick_Right) {
			wiiData->rx4_3Lx |= 0b11000000;
			wiiData->rx2_1Ly |= 0b11000000;
			wiiData->rx0Lt4_3Ry |= (1<<7);
		}
	}

	// Digital Pad Directions
	if(CFG_DIGITAL_PAD) {
		if(!Stick_Up)
			WII_CC_UP
		else if(!Stick_Down)
			WII_CC_DOWN

		if(!Stick_Right)
			WII_CC_RIGHT
		else if(!Stick_Left) 
			WII_CC_LEFT
	}

	// Buttons
	if(!Stick_LP)
		WII_CC_X

	if(!Stick_LK)
		WII_CC_Y

	if(!Stick_MP)
		WII_CC_ZL

	if(!Stick_MK)
		WII_CC_B

	if(!Stick_HP)
		WII_CC_A

	if(!Stick_HK) {
		WII_CC_RT
		wiiData->lt2_0Rt &= ~0b00011111;
	}

#ifdef EXTRA_BUTTONS					
	/* COMMENT FOR LED DEBUG: */
	if(!Stick_4P)
		WII_CC_ZL

	if(!Stick_4K) {
		WII_CC_LT
		wiiData->rx0Lt4_3Ry &= ~0b01100000;
		wiiData->lt2_0Rt &= ~0b11100000;
	}
#endif

	if(CFG_HOME_EMU && !Stick_Start && !Stick_Select)
		WII_CC_HOME
	else {
		if(!Stick_Start)
			WII_CC_MINUS

		if(!Stick_Select)
			WII_CC_PLUS
	}

	if(!Stick_Home)
		WII_CC_HOME
}

// device id starting at address 0xA400FA
PROGMEM const uchar drumDeviceID[6] = {0x01, 0x00, 0xA4, 0x20, 0x01, 0x03};

// calibration data starting at address 0xA40020
const unsigned char drumCalibrationData[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

typedef struct {
	uchar	NaNaSx;
	uchar	NaNaSy;
	uchar   HhpVelocityTypeUnknown;	
	uchar   SoftnessStaticUnknown;
	uchar	UnknownStaticStaticMinusStaticPlusStaticUnknown;
	uchar	OrangeRedYellowGreenBluePedalStaticStatic;
} drumData_t;

#define WII_DRUM_ORANGE	wiiData->OrangeRedYellowGreenBluePedalStaticStatic &= ~(1<<7);
#define WII_DRUM_RED	wiiData->OrangeRedYellowGreenBluePedalStaticStatic &= ~(1<<6);
#define WII_DRUM_YELLOW	wiiData->OrangeRedYellowGreenBluePedalStaticStatic &= ~(1<<5);
#define WII_DRUM_GREEN	wiiData->OrangeRedYellowGreenBluePedalStaticStatic &= ~(1<<4);
#define WII_DRUM_BLUE	wiiData->OrangeRedYellowGreenBluePedalStaticStatic &= ~(1<<3);
#define WII_DRUM_PEDAL	wiiData->OrangeRedYellowGreenBluePedalStaticStatic &= ~(1<<3);
#define WII_DRUM_MINUS	wiiData->UnknownStaticStaticMinusStaticPlusStaticUnknown &= ~(1<<4);
#define WII_DRUM_PLUS	wiiData->UnknownStaticStaticMinusStaticPlusStaticUnknown &= ~(1<<2);
#define WII_DRUM_UP		wiiData->NaNaSx |= 0b00111111;
#define WII_DRUM_DOWN	wiiData->NaNaSx &= 0b11000000;
#define WII_DRUM_LEFT	wiiData->NaNaSy &= 0b11000000;
#define WII_DRUM_RIGHT	wiiData->NaNaSy |= 0b00111111;

void resetDrumData() {
	drumData_t* wiiData = (drumData_t*)data;

	wiiData->NaNaSx = 0b00100000;
	wiiData->NaNaSy = 0b00100000;
	wiiData->HhpVelocityTypeUnknown = 0xFF;
	wiiData->SoftnessStaticUnknown = 0xFF;
	wiiData->UnknownStaticStaticMinusStaticPlusStaticUnknown = 0xFF;
	wiiData->OrangeRedYellowGreenBluePedalStaticStatic = 0xFF;
}

void readInputWiiDrum() {
	drumData_t* wiiData = (drumData_t*)data;

	resetDrumData();

	if (!Stick_Up)
		WII_DRUM_UP
	else if (!Stick_Down)
		WII_DRUM_DOWN

	if (!Stick_Left)
		WII_DRUM_LEFT
	else if (!Stick_Right)
		WII_DRUM_RIGHT

	// Buttons
	if(!Stick_LP)
		WII_DRUM_ORANGE

	if(!Stick_LK)
		WII_DRUM_YELLOW

	if(!Stick_MP)
		WII_DRUM_PEDAL

	if(!Stick_MK)
		WII_DRUM_RED

	if(!Stick_HP)
		WII_DRUM_BLUE

	if(!Stick_HK)
		WII_DRUM_GREEN

	if(!Stick_Start)
		WII_DRUM_MINUS

	if(!Stick_Select)
		WII_DRUM_PLUS
}

#define WII_CLASSIC_CONTROLLER 1

#if WII_CLASSIC_CONTROLLER

void wiimote_extension_controller() {
	/* LED DEBUG */
	//PORTD &= ~(1<<4); // S3 low
	//DDRD  |= (1<<4); // S3 output

	resetClassicControllerData();
	wm_init((uchar *)classicControllerDeviceID, (uchar *)&data, (uchar *)classicControllerCalibrationData, sizeof(classicControllerCalibrationData));
	wm_newaction((void*)&data);

	while(1) {
		readJoystickSwitch();
		readInputWiiClassicController();
		wm_newaction((void*)&data);
	}
}

#else

void wiimote_extension_controller() {
	/* LED DEBUG */
	//PORTD &= ~(1<<4); // S3 low
	//DDRD  |= (1<<4); // S3 output

	resetDrumData();
	wm_init((uchar*)drumDeviceID, (uchar*)&data, (uchar*)drumCalibrationData, sizeof(drumCalibrationData));
	wm_newaction((void*)&data);

	while(1) {
		readJoystickSwitch();
		readInputWiiDrum();
		wm_newaction((uchar*)&data);
	}
}

#endif
