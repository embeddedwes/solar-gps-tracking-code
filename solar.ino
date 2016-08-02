#include <Servo.h>

/*
 * TimeGPS.pde
 * example code illustrating time synced from a GPS
 * 
 */

#include <TimeLib.h>
#include <TimeAlarms.h>
#include <TinyGPS.h>       // http://arduiniana.org/libraries/TinyGPS/
#include <SoftwareSerial.h>
// TinyGPS and SoftwareSerial libraries are the work of Mikal Hart

SoftwareSerial SerialGPS = SoftwareSerial(10, 11);  // receive on pin 10
TinyGPS gps; 

Servo myservo;

const int mornAngle = 70;
const int evenAngle = 110;

// To use a hardware serial port, which is far more efficient than
// SoftwareSerial, uncomment this line and remove SoftwareSerial
//#define SerialGPS Serial1

// Offset hours from gps time (UTC)
const int offset = -4;   // Central European Time
//const int offset = -5;  // Eastern Standard Time (USA)
//const int offset = -4;  // Eastern Daylight Time (USA)
//const int offset = -8;  // Pacific Standard Time (USA)
//const int offset = -7;  // Pacific Daylight Time (USA)

// Ideally, it should be possible to learn the time zone
// based on the GPS position data.  However, that would
// require a complex library, probably incorporating some
// sort of database using Eric Muller's time zone shape
// maps, at http://efele.net/maps/tz/

time_t prevDisplay = 0; // when the digital clock was displayed

void setup()
{
  myservo.attach(9);
  myservo.write(mornAngle);
  Serial.begin(115200);
  while (!Serial) ; // Needed for Leonardo only
  SerialGPS.begin(9600);
  Serial.println("Waiting for GPS time ... ");
}

void loop()
{
  while (SerialGPS.available()) {
    char data = SerialGPS.read();
    //Serial.print(data);
    if (gps.encode(data)) { // process gps messages
      // when TinyGPS reports new data...
      unsigned long age;
      int Year;
      byte Month, Day, Hour, Minute, Second;
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);
      }
    }
  }
  if (timeStatus()!= timeNotSet) {
    if (now() > prevDisplay) { //update the display only if the time has changed
      prevDisplay = now();

      //digitalClockDisplay();
      //servoCheck();
      
      //servo check
      if(second() == 0) {
        digitalClockDisplay();
        servoCheck();
      }
    }
  }
}

void servoCheck() {
  //Serial.println("check servo");
  int hourR = hour(now());
  int minuteR = minute(now());

  int currentFrame = hourR * 60 + minuteR;

  const int keyFrameMorn = 7 * 60 + 45;
  const int keyFrameNight = 20 * 60 + 30;

  if(currentFrame >= keyFrameMorn && currentFrame <= keyFrameNight) {
    //calculate ratio of angles and times and set servo
    double ratio = (currentFrame - (double)keyFrameMorn) / (keyFrameNight - keyFrameMorn);

    myservo.write(mornAngle + (ratio * (evenAngle - mornAngle)));
  }
  else if(currentFrame == 0) {
    //go to morning angle
    myservo.write(mornAngle);
  }
  Serial.print(" - ");
  Serial.print(myservo.read());
  Serial.println();
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  //printDigits(second());
  //Serial.println();
  /*Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); */
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

