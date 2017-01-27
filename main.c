#include <Adafruit_NeoPixel.h>

// Proximity sensor #1
#define trigPin1 13
#define echoPin1 12
// Proximity sensor #2
#define trigPin2 9
#define echoPin2 11
// Proximity sensor #3
#define trigPin3 8
#define echoPin3 10

// light section #1
#define PIN            6
#define NUMPIXELS      30

// button
#define button 4

// lightsensor
int lightPin = 0;  //define a pin for Photo resistor

float light_mapped;

long distance, distance1, distance2, distance3, duration; // variables for proximity sensors
long distance1_c = 0;
long distance2_c = 0;
long distance3_c = 0;

// filter for proximity sensors
bool a1 = false;
bool b1 = false;
bool c1 = false;

bool a2 = false;
bool b2 = false;
bool c2 = false;

bool a3 = false;
bool b3 = false;
bool c3 = false;
bool total = false;

long true1 = 999.99;
long true2 = 999.99;
long true3 = 999.99;

void SonarSensor(int trigPin, int echoPin);
void ColorChange(int speed_of_change);
void ModeOn();
void ModeOff();

// Colors
struct RGB {
  float r;
  float g;
  float b;
};
float temp_r, temp_g, temp_b;
RGB temp = {0, 0, 0};
RGB old_color = {0, 0, 0};
RGB new_color = {0, 0, 0};
RGB green = {0, 255, 0};
RGB light_green = {120, 255, 0};
RGB yellow = { 255, 255, 0};
RGB orange = {255, 150, 0};
RGB brown = {155, 84, 0};
RGB beige = {255, 230, 140};
RGB white = {255, 255, 255};
RGB red = {255, 0, 0};
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin (9600);

  // setup proximity sensor #1
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);

  pinMode(button, INPUT);

  pixels.begin();
  temp = yellow;
  light_mapped = 100;
  ColorChange(2);
  delay(150);
  ColorChange(2);
  delay(150);
  ColorChange(2);
  delay(150);
}

long old_distance1 = 0;
void loop() {
  if (digitalRead(button)) {
    Serial.println("ON");
    if (!total) {
      ModeOn();
      total = true;
    }
  }
  else {
    if (total) {
      ModeOff();
      total = false;
    }
    Serial.println("OFF");
    //read light sensor
    int light = analogRead(lightPin);
    light_mapped = map(light, 400, 960, 100, 0);
    if (light_mapped < 3) light_mapped = 3;

    Serial.print("Real light: ");
    Serial.print(light); //Write the value of the photoresistor to the serial monitor.
    Serial.print(" Mapped light: ");
    Serial.print(light_mapped);

    SonarSensor(trigPin1, echoPin1);
    distance1 = distance;
    SonarSensor(trigPin2, echoPin2);
    distance2 = distance;
    SonarSensor(trigPin3, echoPin3);
    distance3 = distance;

    // Distance measurement
    Serial.print(" #1: ");
    if (distance1 >= 200 || distance1 <= 0) {
      distance1 = 999.99;
      Serial.print("Out of range");
    }
    else Serial.print(distance1);
    Serial.print(" #2: ");
    if (distance2 >= 200 || distance2 <= 0) {
      distance2 = 999.99;
      Serial.print("Out of range");
    }
    else Serial.print(distance2);
    Serial.print(" #3: ");
    if (distance3 >= 200 || distance3 <= 0) {
      distance3 = 999.99;
      Serial.print("Out of range");
    }
    else Serial.print(distance3);
    int threshold = 40;
    if ((distance1 > distance1_c - 20) && (distance1 < distance1_c + 20) && (distance1 < threshold)) {
      a1 = true;
    }
    else a1 = false;
    if (distance2 > distance2_c - 20 && distance2 < distance2_c + 20  && (distance2 < threshold)) {
      a2 = true;
    }
    else a2 = false;
    if (distance3 > distance3_c - 20 && distance3 < distance3_c + 20  && (distance3 < threshold)) {
      a3 = true;
    }
    else a3 = false;

    // first sensor
    if (a1 && b1 && c1) {
      ColorChange(2);
      a1 = false;
      b1 = false;
      c1 = false;
    }
    c1 = b1;
    b1 = a1;

    //second
    if (a2 && b2 && c2) {
      ColorChange(2);
      a2 = false;
      b2 = false;
      c2 = false;
    }
    c2 = b2;
    b2 = a2;

    //third
    if (a3 && b3 && c3) {
      ColorChange(2);
      a3 = false;
      b3 = false;
      c3 = false;
    }
    c3 = b3;
    b3 = a3;

    // remembering last measurement
    distance1_c = distance1;
    distance2_c = distance2;
    distance3_c = distance3;

    int i = 0;
    for (i; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(temp.r * (light_mapped / 100), temp.g * (light_mapped / 100), temp.b * (light_mapped / 100))); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
    old_distance1 = distance1;
    delay(50);
    Serial.println();
  }
}

void SonarSensor(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
}


void ColorChange(int speed_of_change) {
  old_color = temp;
  Serial.println(old_color.r);
  Serial.println(old_color.g);
  if (old_color.r > 200 && old_color.g < 10 && old_color.b < 10) new_color = yellow;
  if (old_color.r > 200 && old_color.g > 200 && old_color.b < 10) new_color = green;
  if (old_color.r < 10 && old_color.g > 200 && old_color.b < 10) new_color = red;
  if (old_color.r > 200 && old_color.g > 200 && old_color.b > 200) new_color = red;
  for (float j = 0; j < 100; j++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      temp_r = (old_color.r * (1 - j / 100) + new_color.r * j / 100);
      temp_g = (old_color.g * (1 - j / 100) + new_color.g * j / 100);
      temp_b = (old_color.b * (1 - j / 100) + new_color.b * j / 100);
      temp = {temp_r, temp_g, temp_b};
      pixels.setPixelColor(i, pixels.Color(temp.r * (light_mapped / 100), temp.g * (light_mapped / 100), temp.b * (light_mapped / 100))); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
    delay(speed_of_change);
  }
}

void ModeOn()
{
  old_color = temp;
  Serial.println(old_color.r);
  Serial.println(old_color.g);
  new_color = white;
  float k = (100 - light_mapped) / 100;
  for (float j = 0; j < 100; j++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      temp_r = (old_color.r * (1 - j / 100) + new_color.r * j / 100);
      temp_g = (old_color.g * (1 - j / 100) + new_color.g * j / 100);
      temp_b = (old_color.b * (1 - j / 100) + new_color.b * j / 100);
      temp = {temp_r, temp_g, temp_b};
      pixels.setPixelColor(i, pixels.Color(temp.r * (light_mapped / 100), temp.g * (light_mapped / 100), temp.b * (light_mapped / 100))); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
    light_mapped = light_mapped + k;
    delay(10);
  }
}

void ModeOff()
{
  old_color = temp;
  Serial.println(old_color.r);
  Serial.println(old_color.g);
  new_color = red;
  int light = analogRead(lightPin);
  float light_mapped2 = map(light, 400, 960, 100, 0);
  if (light_mapped2 < 3) light_mapped2 = 3;

  float k = (100 - light_mapped2) / 100;
  float light_mapped3 = 100;
  for (float j = 0; j < 100; j++) {
    for (int i = 0; i < NUMPIXELS; i++) {
      temp_r = (old_color.r * (1 - j / 100) + new_color.r * j / 100);
      temp_g = (old_color.g * (1 - j / 100) + new_color.g * j / 100);
      temp_b = (old_color.b * (1 - j / 100) + new_color.b * j / 100);
      temp = {temp_r, temp_g, temp_b};
      pixels.setPixelColor(i, pixels.Color(temp.r * (light_mapped3 / 100), temp.g * (light_mapped3 / 100), temp.b * (light_mapped3 / 100))); // Moderately bright green color.
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
    light_mapped3 = light_mapped3 - k;
    delay(10);
  }
}
