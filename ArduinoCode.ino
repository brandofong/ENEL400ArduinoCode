  #include <SPI.h>
  #include <SD.h>
  #include <AudioZero.h>

  volatile bool interruptOccurred = false; // Shared variable between ISR and loop
  bool audioInitialized = false; //Sets the flag if AudioZero has been intialized already so it doesn't reinitialize it
  const int chipSelect = SDCARD_SS_PIN; // SD CARD Input
  const int inputPinSide = 6; // Input pin from the Pi output to see if people are left or right
  const int inputPinDist = 7; // Input pin fed from Pi output to see if they are close or far 
  const int inputPinPeople = 8; // Input pin fed from Pi output and if it is high it means nobody is in the frame
  // Prox stuff

  //Pin numbers for the trigger and echo pins
  const int trigPin = 9;
  const int echoPin = 10;



  void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);

  pinMode(inputPinSide, INPUT); // Set the input pin 6
  pinMode(inputPinDist, INPUT); // Set the input pin 7
  pinMode(inputPinPeople, INPUT); // Set the input pin 8

  //Proximity stuff enabling for the device

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      Serial.println("Card failed, or not present");
      // don't do anything more:
      while (1);
    }

  // 44100kHz stereo => 88200 sample rate

    AudioZero.begin(2*44100); // start sampling
    

  }

  void loop() {

  // 2 input pings correspond to 3-5 and 5<
  long duration, inches, cm;

  // The PING is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);  //10 microsecond trigger pulse
  digitalWrite(trigPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.

  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);


  //  Start speaker code

  if (digitalRead(inputPinPeople) == HIGH || ( cm > 0 && cm < 200)) { // (1) If pin 8 is high, there is nobody in the frame and audio playback will not play (100)

      AudioZero.end(); // "Stops" the DAC0 pin from outputting noise
      audioInitialized = false; // Reset the flag
  
  if (!audioInitialized){
// Do nothing in this if the audio is already unitialized 
  }
else {
        Serial.print(cm);
        Serial.print("cm");
        Serial.println();
    }
}

    

  else if (digitalRead(inputPinSide) == HIGH && digitalRead(inputPinDist) == HIGH) { // (2) If pin on 6 and 7 are high, they are RIGHT side and CLOSE (3m -7m) (011)

    Serial.println("Pin 6 and 7 are high, they are RIGHT side and CLOSE (3m -7m)");
        if (!audioInitialized ) { // If audio hasn't be initialized yet, then do it (To prevent double initializing)

      AudioZero.begin(2* 44100); // Corresponds to the 44.1kHz output of audio sampling at 88.2kHz

      audioInitialized = true; // Set flag to indicate audio initialization

    }
    
    File myFile = SD.open("fast1.wav");     // open wave file from sdcard
    AudioZero.play(myFile);
    myFile.close(); // Close file to prevent leaks

    }

    else if (digitalRead(inputPinSide) == HIGH && digitalRead(inputPinDist) == LOW) { // (3) If pin on 6 is high and 7 is low, they are RIGHT side and FAR 7m< (001)

    Serial.println("If pin on 6 is high and 7 is low, they are RIGHT side and FAR 7m<");
        if (!audioInitialized) { // If audio hasn't be initialized yet, then do it (To prevent double initializing)

      AudioZero.begin(2* 44100); // Corresponds to the 44.1kHz output of audio
     
      audioInitialized = true; // Set flag to indicate audio initialization

    }

    File myFileTwo = SD.open("slow1.wav");     // open wave file from sdcard
    AudioZero.play(myFileTwo);
    myFileTwo.close();

    }


  else if (digitalRead(inputPinSide) == LOW && digitalRead(inputPinDist) == HIGH) { //(4) If pin on 6 is LOW and 7 is HIGH, they are LEFT side and FAR (7m<) 

    Serial.println("If pin on 6 is LOW and 7 is HIGH, they are LEFT side and CLOSE (3m -7m)");
      if (!audioInitialized) { // If audio hasn't be initialized yet, then do it (To prevent double initializing)

      AudioZero.begin(2* 44100); // Corresponds to the 44.1kHz output of audio sampling at 88.2kHz
     
      audioInitialized = true; // Set flag to indicate audio initialization

    }
    
    File myFile = SD.open("faster.wav");     // open wave file from sdcard
      // File myFile = SD.open("fastt.wav");     // open wave file from sdcard

    AudioZero.play(myFile);
    myFile.close();
    }

  else { // 

    Serial.println("Pin 6 and 7 are both LOW. (Left Side and FAR)");
        if (!audioInitialized) { // If audio hasn't be initialized yet, then do it (To prevent double initializing)

      AudioZero.begin(2* 44100); // Corresponds to the 44.1kHz output of audio sampling at 88.2kHz
 
      audioInitialized = true; // Set flag to indicate audio initialization

    }
    
    // File myFile = SD.open("faster.wav");     // open wave file from sdcard
      File myFile = SD.open("fastt.wav");     // open wave file from sdcard

    AudioZero.play(myFile);
    myFile.close();

    }
  }

  long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
  }

