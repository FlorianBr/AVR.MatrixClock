/*
 * Hardware used:
 *  - Arduino Nano
 *  - RTC DS3231
 *  - AM2302
 */

#pragma GCC optimize ("Os")

//#define WITH_SERIAL                                                     //< Enable serial support. WARNING: The Nano does not have enough memory!

#include <Wire.h>                                                       //< Basic I2C functionality
#include <FastLED.h>                                                    //< For the LED Matrix
#include <DHT_U.h>                                                      //< Unified DHT Lib
#include <RtcDS3231.h>                                                  //< RTC DS3231 Lib

// RTC
RtcDS3231<TwoWire>  RTC(Wire);                                          //< The RTC Object
RtcDateTime         RTC_Now;                                            //< Current RTC-Time 
RtcTemperature      RTC_Temp;                                           //< Current RTC-Temperature
// DHT22/AM2302
#define             DHT_PIN                         3                   //< DHT Data-Pin : D3
#define             DHT_TYPE                        DHT22               //< Sensortype: DHT22 / AM2302
#define             DHT_CYCLE                       5000                //< Update-Time in ms
DHT_Unified         Sensor_DHT(DHT_PIN, DHT_TYPE);                      //< The DHT Object
float               DHT_Temp;                                           //< Current DHT Temperature
uint8_t             DHT_Hum;                                            //< Current DHT Humidity
// LED Matrix 
#define             LED_PIN                         6                   //< LED Data Pin: D6
#define             LED_TYPE                        NEOPIXEL            //< Matrix Chipset: Neopixel
#define             LED_COLUMNS                     32                  //< Number of Columns
#define             NUM_LEDS                        ((uint16_t)32*8)    //< 32 Cols with 8 LEDs each
#define             BRIGHTNESS                      20                  //< Basic Matrix Brightness
#define             POS_DIGIT1                      (LED_COLUMNS-2)     //< Position of Digit 1: Leftmost position minus 2 colums
#define             POS_DIGIT2                      (POS_DIGIT1-8)      //< Position of Digit 2: Pos1 minus 8 colums
#define             POS_DIGIT3                      (POS_DIGIT2-8)      //< Position of Digit 3: Pos2 minus 8 colums
#define             POS_DIGIT4                      (POS_DIGIT3-8)      //< Position of Digit 4: Pos3 minus 8 colums
#define             POS_COLON                       (LED_COLUMNS/2)     //< Position of Colon: Middle of the display
#define             DISP_COL_BACK                   (CRGB::Black)       //< Background color
#define             DISP_COL_DAY                    (CRGB::White)       //< Daytime color
#define             DISP_COL_NIGHT                  (CRGB::Red)         //< Nighttime color

CRGB                leds[NUM_LEDS];                                     //< The LED Matrix, with fixed column ordering
uint8_t             ColumnData[LED_COLUMNS];                            //< The LED Matrix, with regular column ordering
// Font
#define             FONT_BPC                        5                   //< Bytes per Char
#define             FONT_OFFSET                     32                  //< Ascii-Position of the first entry
const unsigned char Char_C[5] 		= { 0x02,0x05,0x72,0x88,0x50 };	 	// C
const unsigned char Char_Perc[5] 	= { 0xC6,0x26,0x10,0xC8,0xC6 }; 	// %
const unsigned char Char_Colon 	    = 0x48; 	                        // :
const unsigned char Char_Dot 	    = 0x80; 	                        // .

const unsigned char Char_Nums[][5] = {
	{0x7c,0x82,0x92,0x82,0x7c}, // 0
	{0x00,0x84,0xfe,0x80,0x00}, // 1
	{0xc4,0xa2,0x92,0x92,0x8c}, // 2
	{0x44,0x82,0x92,0x92,0x6c}, // 3
	{0x30,0x28,0x24,0xfe,0x20}, // 4
	{0x5e,0x92,0x92,0x92,0x62}, // 5
	{0x78,0x94,0x92,0x92,0x60}, // 6
	{0x02,0x82,0x62,0x1a,0x06}, // 7
	{0x6c,0x92,0x92,0x92,0x6c}, // 8
	{0x0c,0x92,0x92,0x52,0x3c}  // 9
};
const uint8_t       ModeTimes[] = { 5, 1, 1, 1};                       //< The Times for displaying every Mode in seconds

/************************************************************/
// ****************************** Display
/************************************************************/
// Fix order of LEDs and send it to the Display
// The LEDs in the display are wired "zig-zag", so we need to 
// revert the led order in every second column
void Disp_FixAndShow() {
    bool bDaytime = true;

    if ( (RTC_Now.Hour()<=6) || (RTC_Now.Hour()>=23)) bDaytime = false; // up to 7:00 and from 23:00 on is nighttime

    // Generate LED-Matrix
    for (uint16_t Led=0;Led<NUM_LEDS;Led++) {
        bool        BitIsSet = false;
        uint8_t     Column = Led/8;
        uint8_t     Bit = Led - (Column*8);

        if (Column<LED_COLUMNS) {
            if ((ColumnData[Column]&(0x01<<Bit))>0) BitIsSet=true;
        }

        if (BitIsSet&bDaytime) {
            leds[Led] = DISP_COL_DAY;
        } else if (BitIsSet&!bDaytime) {
            leds[Led] = DISP_COL_NIGHT;
        } else {          
            leds[Led] = DISP_COL_BACK;
        }
    } // for Led

    // Fix column order
    for (uint8_t Col=0;Col<32;Col+=2) {
        CRGB LedBuffer[8];
        memcpy(&LedBuffer[0], &leds[Col*8], 8*sizeof(CRGB) );                   // Column into Buffer
        for (uint8_t Pos=0;Pos<8;Pos++) leds[Col*8+Pos] = LedBuffer[7-Pos];     // Write back in opposite order
    }

    if (bDaytime) {
        FastLED.setBrightness( BRIGHTNESS );
    } else {
        FastLED.setBrightness( 1 );
    }

    FastLED.show();
    return;
} // Disp_FixAndShow
/************************************************************/
// Adds a Byte to the Matrix in the given color
void Disp_AddByte(uint16_t Position, uint8_t Byte ) {
    ColumnData[Position] = Byte;
} // Disp_AddByte
/************************************************************/
// Sends a numeric (0..9) to the display
void Disp_SetDigit(uint16_t Position, uint8_t Digit) {
    if (Digit>9) return; // Boundary check
    for (uint8_t i=0;i<FONT_BPC;i++) {
        Disp_AddByte( (Position-i ), Char_Nums[Digit][i] );
    }
} // Disp_SetDigit
/************************************************************/
void Disp_Setup() {
    FastLED.addLeds<LED_TYPE,LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness( BRIGHTNESS );
} // Disp_Setup
/************************************************************/
// Display Worker Loop
void Disp_Worker() {
    static uint8_t DispMode = 0;        // Current display mode
    static uint8_t OldSecond = 0xFF;    // Old second value
    static uint8_t StateTime = 0;       // Time in the currend state
    uint8_t DispModeNew;                // Next display mode

    // Worker is only executed once a second
    if (OldSecond == RTC_Now.Second()) return;
    OldSecond = RTC_Now.Second();

    DispModeNew = DispMode;
    StateTime++;

    if (DispMode==0) {                  // Mode 0: Hour&Minute
        uint8_t Digit1 = 0;
        uint8_t Digit2 = 0;

        // Hour
        Digit1 = RTC_Now.Hour()/10;
        Digit2 = RTC_Now.Hour() - (10*Digit1);
        Disp_SetDigit(POS_DIGIT1, Digit1 );
        Disp_SetDigit(POS_DIGIT2, Digit2 );

        // Minute
        Digit1 = RTC_Now.Minute()/10;
        Digit2 = RTC_Now.Minute() - (10*Digit1);
        Disp_SetDigit(POS_DIGIT3, Digit1 );
        Disp_SetDigit(POS_DIGIT4, Digit2 );

        // Pulsing Colon for Second
        // Only one column is used, this may be adjusted for different fonts
        if ( ((RTC_Now.Second())%2)==0) {
            Disp_AddByte( POS_COLON,    Char_Colon );
        } else {
            Disp_AddByte( POS_COLON,    0x00 );
        }

        if (StateTime > ModeTimes[0]) DispModeNew = DispMode + 1;
    }
    else if (DispMode==1) {             // Mode 1: Date
        uint8_t Digit1 = 0;
        uint8_t Digit2 = 0;

        // Day
        Digit1 = RTC_Now.Day()/10;
        Digit2 = RTC_Now.Day() - (10*Digit1);
        Disp_SetDigit(POS_DIGIT1, Digit1 );
        Disp_SetDigit(POS_DIGIT2, Digit2 );

        // Month
        Digit1 = RTC_Now.Month()/10;
        Digit2 = RTC_Now.Month() - (10*Digit1);
        Disp_SetDigit(POS_DIGIT3, Digit1 );
        Disp_SetDigit(POS_DIGIT4, Digit2 );

        Disp_AddByte( POS_COLON,    Char_Dot );

        if (StateTime > ModeTimes[1]) DispModeNew = DispMode + 1;
    }
    else if (DispMode==2) {             // Mode 2: Temperature
        uint8_t Digit1 = 0;
        uint8_t Digit2 = 0;
        uint8_t Digit3 = 0;

        Digit1 = (uint8_t)((uint16_t)DHT_Temp)/10;
        Digit2 = ((uint8_t)(uint16_t)DHT_Temp) - (10*Digit1);
        Digit3 = (uint8_t)(((uint16_t)(10*DHT_Temp)) - (100*Digit1) - (10*Digit2));

        Disp_SetDigit(POS_DIGIT1, Digit1 );
        Disp_SetDigit(POS_DIGIT2, Digit2 );
        Disp_SetDigit(POS_DIGIT3, Digit3 );
        Disp_AddByte( POS_COLON,    Char_Dot );

        for (uint8_t i=0;i<FONT_BPC;i++) {
            Disp_AddByte( (POS_DIGIT4-i ), Char_C[i] );
        }

        if (StateTime > ModeTimes[2]) DispModeNew = DispMode + 1;
    }
    else if (DispMode==3) {             // Mode 2: Humidity
        uint8_t Digit1 = 0;
        uint8_t Digit2 = 0;

        Digit1 = (uint8_t)(DHT_Hum)/10;
        Digit2 = (uint8_t)(DHT_Hum) - (10*Digit1);

        Disp_SetDigit(POS_DIGIT2, Digit1 );
        Disp_SetDigit(POS_DIGIT3, Digit2 );

        for (uint8_t i=0;i<FONT_BPC;i++) {
            Disp_AddByte( (POS_DIGIT4-i ), Char_Perc[i] );
        }

        if (StateTime > ModeTimes[3]) DispModeNew = DispMode + 1;

    }
    else {                              // End of cycle / illegal DispMode
        DispModeNew=0;
        OldSecond=0xFF; // Force immediately refresh
    }

    Disp_FixAndShow();

    if (DispModeNew != DispMode) { // Mode change?
        for (uint16_t i=0;i<LED_COLUMNS;i++) ColumnData[i] = 0x00;
        DispMode = DispModeNew;
        StateTime = 0;
    }

    return;
} // Disp_Worker
/************************************************************/
// ****************************** RTC
/************************************************************/
void RTC_Setup() {
    RTC.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

    if (!RTC.IsDateTimeValid()) {               // RTC contains illegal datetime
        if (RTC.LastError() != 0) {
#ifdef WITH_SERIAL
            Serial.print("[RTC] ComError = ");
            Serial.println(RTC.LastError());
#endif
        } else {
#ifdef WITH_SERIAL
            Serial.println("[RTC] DateTime Error!");
            RTC.SetDateTime(compiled);          // Set RTC to build-DateTime
#endif
        }
    }
    if (!RTC.GetIsRunning()) RTC.SetIsRunning(true);

    RtcDateTime now = RTC.GetDateTime();
    if (now < compiled) {
#ifdef WITH_SERIAL
        Serial.println("[RTC] DateTime too old, updating to Build-DateTime");
#endif
        RTC.SetDateTime(compiled);
    } else if (now > compiled) {
        // Nothing, this is okay
    } else if (now == compiled)  {
        // Nothing, this is odd but okay
    }
    RTC.Enable32kHzPin(false);
    RTC.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

    return;
} // RTC_Setup
/************************************************************/
void RTC_Worker() {
    if (!RTC.IsDateTimeValid())  {
        if (RTC.LastError() != 0) {
#ifdef WITH_SERIAL
            Serial.print("[RTC] ComError = ");
            Serial.println(RTC.LastError());
#endif
            // Errorcodes: https://www.arduino.cc/en/Reference/WireEndTransmission
        }
    }
    RTC_Now  = RTC.GetDateTime();
    RTC_Temp = RTC.GetTemperature();
} // RTC_Worker
/************************************************************/
// ****************************** DHT
/************************************************************/
void DHT_Setup() {
    Sensor_DHT.begin();
    return;
} // DHT_Setup
/************************************************************/
void DHT_Worker() {
    static uint8_t CallCount = 0;
    sensors_event_t event;

    CallCount++;
    if (CallCount<10) return; // Requesting too fasten may crash the sensor
    CallCount=0;

    Sensor_DHT.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
#ifdef WITH_SERIAL
        Serial.println(F("[DHT] Error reading temperature!"));
#endif
    } else {
        DHT_Temp = event.temperature;
    }

    Sensor_DHT.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
#ifdef WITH_SERIAL
        Serial.println(F("[DHT] Error reading humidity!"));
#endif
    } else {
        DHT_Hum = (uint8_t)event.relative_humidity;
    }

    return;
} // DHT_Worker
/************************************************************/
// ****************************** Setup
/************************************************************/
void setup() {
    delay(3000);
#ifdef WITH_SERIAL
    Serial.begin(115200);
    Serial.println("[SYS] Build: " __DATE__ " - " __TIME__);
    Serial.println("[SYS] Starting Setup...");
#endif
    RTC_Setup();    // Setup RTC
    DHT_Setup();    // Setup DHT-Sensor
    Disp_Setup();   // Setup Matrix Display
#ifdef WITH_SERIAL
    Serial.println("[SYS] Setup complete!");
#endif
} // Setup
/************************************************************/
// ****************************** Loop
/************************************************************/
void loop() {
    static uint8_t SecOld = 0;

#ifdef WITH_SERIAL
    Serial.println("[SYS] Entering Loop...");
#endif
    while (1) {
        RTC_Worker();
        Disp_Worker();

        // Sync to Second!
        if (SecOld != RTC_Now.Second() ) {
            DHT_Worker();
#ifdef WITH_SERIAL
            Serial.print("[Inf] RTC: ");

            Serial.print(RTC_Now.Day(), DEC);   Serial.print(".");
            Serial.print(RTC_Now.Month(), DEC); Serial.print(".");
            Serial.print(RTC_Now.Year(), DEC);  Serial.print(" ");

            Serial.print(RTC_Now.Hour(), DEC);      Serial.print(":");
            Serial.print(RTC_Now.Minute(), DEC);  Serial.print(":");
            Serial.print(RTC_Now.Second(), DEC);  Serial.print(":");

            Serial.print(" - ");
            Serial.print(RTC_Temp.AsFloatDegC());
            Serial.print("dC");
            Serial.print(" / DHT: ");
            Serial.print(DHT_Temp,2);
            Serial.print("dC - ");

            Serial.print(DHT_Hum);
            Serial.print("%");
            Serial.println();
#endif
            SecOld = RTC_Now.Second();
        }
        delay(25);
    } // while (1)
}
