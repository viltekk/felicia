#include <Adafruit_NeoPixel.h>

#define _DEBUG

#ifdef _DEBUG
#include <SoftwareSerial.h>
#define TRACE(S) (Serial.println((S)))
#else
#define TRACE(s)
#endif

/* ---------------------------------------------------------------------- LED */
#define PIN_LED0  (  4)
#define PIN_LED1  (  5)
#define LED_BRGHT ( 64) // led brightness
#define LED_COUNT (  1) // number of leds on strips
#define LED_DIM_T (500) // duration of fade in millis
#define LED_DELTA ( 10) // delay in millis between pixel changes when fading
#define LED_STEPS (LED_DIM_T/LED_DELTA)

Adafruit_NeoPixel _led0 = Adafruit_NeoPixel(LED_COUNT,
                                            PIN_LED0,
                                            NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel _led1 = Adafruit_NeoPixel(LED_COUNT,
                                            PIN_LED1,
                                            NEO_GRB + NEO_KHZ800);

/* -------------------------------------------------------------- hall sensor */
#define PIN_IRQ (3)

bool _interrupted = false;

/* -------------------------------------------------------------------- debug */
#define LON  (digitalWrite(13, HIGH))
#define LOFF (digitalWrite(13, LOW))

/* ---------------------------------------------------------------------- led */
void led() {
  if(_interrupted) {
    uint8_t cur_r;
    uint8_t cur_g;
    uint8_t cur_b;

    uint8_t nxt_r;
    uint8_t nxt_g;
    uint8_t nxt_b;

    uint8_t r_delta;
    uint8_t g_delta;
    uint8_t b_delta;

    uint32_t cur;
    
    nxt_r = random(255);
    nxt_g = random(255);
    nxt_b = random(255);

    cur    = _led0.getPixelColor(0);
    cur_r  = (cur & 0xff0000) >> 16;
    cur_g  = (cur & 0x00ff00) >>  8;
    cur_b  = (cur & 0x0000ff) >>  0;
    
    r_delta = cur_r / LED_STEPS;
    g_delta = cur_g / LED_STEPS;
    b_delta = cur_b / LED_STEPS;

    // dim down
    for(int i = 0; i < LED_STEPS; i++) {
      cur_r -= r_delta;
      cur_g -= g_delta;
      cur_b -= b_delta;
      
      _led0.setPixelColor(0, cur_r, cur_g, cur_b);
      _led0.show();
      delay(LED_DELTA);
    }

    _led0.setPixelColor(0, 0, 0, 0);
    _led0.show();

    // dim up, re-use variables
    cur_r = cur_g = cur_b = 0;
    
    r_delta = nxt_r / LED_STEPS;
    g_delta = nxt_g / LED_STEPS;
    b_delta = nxt_b / LED_STEPS;

    for(int i = 0; i < LED_STEPS; i++) {
      cur_r += r_delta;
      cur_g += g_delta;
      cur_b += b_delta;
      
      _led0.setPixelColor(0, cur_r, cur_g, cur_b);
      _led0.show();

      _led1.setPixelColor(0, cur_r, cur_g, cur_b);
      _led1.show();

      delay(LED_DELTA);
    }

    _led0.setPixelColor(0, nxt_r, nxt_g, nxt_b);
    _led0.show();

    _led1.setPixelColor(0, nxt_r, nxt_g, nxt_b);
    _led1.show();
    
    _interrupted = false;
    LOFF;
  }
}

/* -------------------------------------------------------------------- setup */
void setup() {
  _led0.begin();
  _led0.setBrightness(LED_BRGHT);
  _led0.show();

  _led1.begin();
  _led1.setBrightness(LED_BRGHT);
  _led1.show();

  attachInterrupt( digitalPinToInterrupt(PIN_IRQ), irq, FALLING );
  pinMode(PIN_IRQ, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  
  randomSeed( analogRead(0) );

  #ifdef _DEBUG
  Serial.begin(9600);
  #endif

  TRACE("booted");
}

/* --------------------------------------------------------------------- loop */
void loop() {
  led();
  delay(500);
  TRACE(digitalRead(PIN_IRQ));
}

/* ---------------------------------------------------------------------- irq */
void irq() {
  TRACE("irq");
  if(!_interrupted) {
    TRACE("interrupted");
    LON;
    _interrupted = true;
  }
}
