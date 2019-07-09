#define Photo_Diode A0
#define Speaker 10

struct TimeKeeper {
  long startTime;
  long endTime;
  double rpm;
};

struct Sound {
  int frequency;
  int multiplier;
};

TimeKeeper rpm = {0,0,0};
Sound sound = {100, 5};
void setup() {
  Serial.begin(9600);
  while(!Serial) {}
  
  pinMode(Speaker, OUTPUT);
}

void loop() {
  getRPM();
  if (Serial.available() > 0) {
    Serial.println(Serial.available());
    setTone();
  }
  // One second delay to prevent flooding serial output & to allow time for input
  delay(100);
}

void getRPM() {
  int lightLevel = analogRead(Photo_Diode); //Read voltage from the photo resistor
  
  /* If the laser is not striking the photo resistor and RPM has been calculated
     before or hasn't yet been calculated, set the start time of a rotation of 
     the tube. Else, if the laser is not striking the phot0 resistor and RPM is 
     in the process of being calculated, get the end time of the rotation and
     finish the calculation of the vortex tube's RPM.
  */
  if (lightLevel < 200 && rpm.endTime >= rpm.startTime) {
    // Get the start time of rotation
    rpm.startTime = millis();
  } else if (lightLevel < 200 && rpm.startTime > rpm.endTime) {
    // Get the end time of rotation
    rpm.endTime = millis();
    
    // Divide one by time taken (in minutes) to complete one rotation 
    long timeDifference = rpm.endTime - rpm.startTime;
    double minutes = ((double)timeDifference/60000.0);
    rpm.rpm = (1/minutes);
    // Display RPM to serial output
    Serial.print("RPM: ");
    Serial.println(rpm.rpm);
  }
}

void setTone() {
  // If there is input to take from serial input, read it in.
  int newFreq = 0;
  while (Serial.available() > 0) {
    newFreq = Serial.parseInt();
    Serial.read();
  }
  // Don't accept frequencies lower than 31.
  if (5 < newFreq && newFreq < 31) {
    Serial.println("No tone lower than 31Hz can be generated");
    noTone(Speaker);
    return;
  } else if (0 < newFreq && newFreq <= 5) {
    //Set the multiplier to control amplitude
    sound.multiplier = newFreq;
  } else {
    sound.frequency = newFreq;
    // Stop the old tone in order to create the new one.
    noTone(Speaker);
  }
  // Set the tone to the Speaker PWM output and MATH to controll amplitude
  tone(Speaker, (int)(((double)sound.multiplier/5.0) * (double)sound.frequency));
}
