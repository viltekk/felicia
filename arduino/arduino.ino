#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

/* ---------------------------------------------------------------------- LED */
#define PIN_LED   (  7)
#define LED_COUNT (  1)
#define LED_DIM_T (500)
#define LED_DELTA ( 10)
#define LED_STEPS (LED_DIM_T/LED_DELTA)

Adafruit_NeoPixel _led = Adafruit_NeoPixel(LED_COUNT,
                                           PIN_LED,
                                           NEO_GRB + NEO_KHZ800);

/* -------------------------------------------------------------- hall sensor */
#define PIN_IRQ (3)

bool _interrupted = false;

/* ---------------------------------------------------------------------- led */
void led() {
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

  if(_interrupted) {
    nxt_r = random(255);
    nxt_g = random(255);
    nxt_b = random(255);

    // nxt_r = 255;
    // nxt_g = 255;
    // nxt_b = 255;

    cur    = _led.getPixelColor(0);
    cur_r  = (cur & 0xff0000) >> 16;
    cur_g  = (cur & 0x00ff00) >>  8;
    cur_b  = (cur & 0x0000ff) >>  0;

    Serial.print("c: "); Serial.println(cur);
    Serial.print("r: "); Serial.println(cur_r);
    Serial.print("g: "); Serial.println(cur_g);
    Serial.print("b: "); Serial.println(cur_b);
    
    r_delta = cur_r / LED_STEPS;
    g_delta = cur_g / LED_STEPS;
    b_delta = cur_b / LED_STEPS;

    // dim down
    for(int i = 0; i < LED_STEPS; i++) {
      cur_r -= r_delta;
      cur_g -= g_delta;
      cur_b -= b_delta;
      
      _led.setPixelColor(0, cur_r, cur_g, cur_b);
      _led.show();
      delay(LED_DELTA);
    }

    _led.setPixelColor(0, 0, 0, 0);
    _led.show();
    Serial.println("dimmed down");

    // dim up, re-use variables
    cur_r = cur_g = cur_b = 0;
    
    r_delta = nxt_r / LED_STEPS;
    g_delta = nxt_g / LED_STEPS;
    b_delta = nxt_b / LED_STEPS;

    for(int i = 0; i < LED_STEPS; i++) {
      cur_r += r_delta;
      cur_g += g_delta;
      cur_b += b_delta;
      
      _led.setPixelColor(0, cur_r, cur_g, cur_b);
      _led.show();
      delay(LED_DELTA);
    }

    _led.setPixelColor(0, nxt_r, nxt_g, nxt_b);
    _led.show();
    Serial.print("r: "); Serial.println(nxt_r);
    Serial.print("g: "); Serial.println(nxt_g);
    Serial.print("b: "); Serial.println(nxt_b);
    Serial.println("dimmed up");
    
    _interrupted = false;
  }
}

/* -------------------------------------------------------------------- setup */
void setup() {
  Serial.begin(9600);
  _led.begin();
  _led.setBrightness(64);
  _led.show();

  attachInterrupt( digitalPinToInterrupt(PIN_IRQ), irq, FALLING );
  pinMode(PIN_IRQ, INPUT_PULLUP);
  
  randomSeed( analogRead(0) );
}

/* --------------------------------------------------------------------- loop */
void loop() {
  led();
  delay(1000);
}

/* ---------------------------------------------------------------------- irq */
void irq() {
  Serial.println("Irq!");
  if(!_interrupted) {
    Serial.println("interrupted = true");
    _interrupted = true;
  }
}
