// This file will go through many iterations
// current goals are to get LED controls from C# app
// then be able to pass signal commands & data back and forth

const long BUAD_RATE = 115200;
const byte led_gpio = 33; // Define the GPIO pin for the LED

void setup() {
  Serial.begin(BUAD_RATE);
  pinMode(led_gpio, OUTPUT); // Set the pin as an output
}

void loop() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.equals("green_light_on")) {
      digitalWrite(led_gpio, HIGH); // Turn the LED on
    } else if (command.equals("green_light_off")){
      digitalWrite(led_gpio, LOW);
    }
  }
}
