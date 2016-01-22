/* Author: Benjamin Low
 *  
 * Description: Prototype for Teck Wah gallery NeoPixels.
 *
 *   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
 *   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
 *   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
 *   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
 *   
 * IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
 * pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
 * and minimize distance between Arduino and first pixel.  Avoid connecting
 * on a live circuit...if you must, connect GND first.
 * 
 * Last updated: 21 Jan 2016
 */

#include <Adafruit_NeoPixel.h>

//USER DEFINED SETTINGS
//See setup() for defining Color1 and Color2
const int NUM_PIXELS = 30; //number of pixels in the strip
const int PORT_NUM = 2; //Arduino pin for the strip
const int INTERVAL = 2; //lower number is higher speed for fade

// Pattern types supported:
enum  pattern { NONE, FADE };
// Pattern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:
 
    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use 
    uint32_t myColor1, myColor2; //placeholders for Color1 and Color2
    uint32_t noColor = Color(0,0,0);
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t  pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }
    
    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            
            switch(ActivePattern)
            {
                case FADE:
                    FadeUpdate();
                    break;
                default:
                    break;
            }
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
           Index++;
           if (Index > TotalSteps)
            {   
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
    }
    
    // Initialize for a Fade
    void Fade(uint16_t steps, uint8_t interval, int state) //state is 1 for turn on, 0 for turn off
    {   
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Index = 0;
        
        if (state == 1) //turn on 
        {
            myColor1 = Color2;
            myColor2 = Color1;
            Direction = FORWARD;
            
        } else if (state == 0) //turn off
        {
            myColor1 = Color1;
            myColor2 = Color2;
            Direction = REVERSE;
        }
    }
    
    // Update the Fade Pattern
    void FadeUpdate()
    {
        // Calculate linear interpolation between Color1 and Color2
        // Optimise order of operations to minimize truncation error

        uint8_t red = ((Red(myColor1) * (TotalSteps - Index)) + (Red(myColor2) * Index)) / TotalSteps;
        uint8_t green = ((Green(myColor1) * (TotalSteps - Index)) + (Green(myColor2) * Index)) / TotalSteps;
        uint8_t blue = ((Blue(myColor1) * (TotalSteps - Index)) + (Blue(myColor2) * Index)) / TotalSteps;
            
        ColorSet(Color(red, green, blue));
        show();
        Increment();
    }
 
    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
        for (int i = 0; i < numPixels(); i++)
        {
            setPixelColor(i, color);
        }
        show();
    }
 
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
        return (color >> 16) & 0xFF;
    }
 
    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
        return (color >> 8) & 0xFF;
    }
 
    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
        return color & 0xFF;
    }
    
}; //end of class definition

// ------------------------------------------------------
// Function prototypes for completion callback routines 
// ------------------------------------------------------
void Strip1Complete();

// -----------------------------
// Object declarations
// -----------------------------
  NeoPatterns Strip1(NUM_PIXELS, PORT_NUM, NEO_GRB + NEO_KHZ800, &Strip1Complete);

//------------------------------
// setup
//------------------------------
void setup() {
  
  Serial.begin(9600);

  Strip1.begin();

  //USER DEFINED SETTINGS
  Strip1.Color1 = Strip1.Color(255,255,255);
  Strip1.Color2 = Strip1.Color(0,0,0);
}

//-------------------------
// The main loop
//-------------------------

void loop() {

  Strip1.Update();
  
  read_from_serial();
}

//-----------------------
// supporting functions
//-----------------------

void read_from_serial() {

  unsigned char incomingbyte = 0;

  if (Serial.available() > 0) {
        
    incomingbyte = Serial.read();

    if (incomingbyte == '0') {

        Strip1.ActivePattern = NONE;
        Strip1.ColorSet( Strip1.noColor );
        
    } else if (incomingbyte == '1') {

        Strip1.Fade( 128, INTERVAL, 1); //num of steps, interval, state
        
    } else if (incomingbyte == '2') {

        Strip1.Fade( 128, INTERVAL, 0);
        
    } else if (incomingbyte == '3') {
        
    } else if (incomingbyte == '4') {
         
    } else if (incomingbyte == '5') {
    
    }
  }
}

// -----------------------------
// Completion callback routines 
// -----------------------------

void Strip1Complete() {

    Serial.println("Strip 1 Complete");

    if (Strip1.Direction == FORWARD) 
    {
        Strip1.Fade( 128, INTERVAL*5, 0);
    } else 
    {
        Strip1.Fade( 128, INTERVAL*5, 1);
    }
    
//    Strip1.ActivePattern = NONE; //prevent the fadeUpdate
}








