#include <MadgwickAHRS.h>
#include <Arduino_LSM6DS3.h>
#include <WiFiNINA.h>
Madgwick filter;

//GOLD CHALLENGE

char ssid[] = "Virgin Macs";
char pass[] = "0870541630gmC";
WiFiClient client;
WiFiServer server(5200);
char c; //for client input

//US sensors
const int US_TRIGGER = A0;
const int US_ECHO = A1;
//motors
const int motor_1A = 9; //buggy's left motor
const int motor_2A = 10; 
const int motor_3A = 11; //buggy's right motor
const int motor_4A = 12; 
const int PWM1 = 16; //PWM for left motor
const int PWM2 = 17; //PWM for right motor
int motor_speed = 0;

//IMU variables
unsigned long micros_per_reading, micros_previous;
float accl_scale, gyro_scale;
float ax, ay, az; //acclerometer values in g/s
float gx, gy, gz; //gyroscope values in degrees/s
float roll, pitch, yaw;

int yaw_direction = 180;



//NOTE
/*Yaw is the measuremeant of rotation about the vertical axis (ie buggy rotating left or right)
 * Here we are using the readings from the gyroscope to calculate yaw
 * This allows the buggy to autocorrect itself depending on far its rotated 
 * This negates the need for IR sensors as now the buggy can follow any direction without deviating
 * Yaw = 0 at initial position (ie facing forward)
 * Yaw = 90 when the buggy has rotated 90 degrees to its left
 * Yaw = -90 when the buggy has rotated 90 degrees to its right
 */

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  IPAddress IP = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(IP);
  server.begin();
 
 //initialising all pins
  pinMode(motor_1A, OUTPUT);
  pinMode(motor_2A, OUTPUT);
  pinMode(motor_3A, OUTPUT);
  pinMode(motor_4A, OUTPUT);
  pinMode(US_TRIGGER, OUTPUT);
  pinMode(US_ECHO, INPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  
  if(!IMU.begin()) {
    Serial.println("Failed to initialise IMU");
    while (true);
  }
  Serial.println("IMU initialised");

  filter.begin(119);
  delay(1000);

  micros_per_reading = 1000000/119;
  micros_previous = micros();

  while (!client.connected()) { //connect to Processing client
  client = server.available();
  if (client.connected()) {
    client.write("Connected");
    }
  }

}

void drive_forward() {
  digitalWrite(motor_1A, HIGH);
  digitalWrite(motor_2A, LOW); 
  digitalWrite(motor_3A, HIGH);
  digitalWrite(motor_4A, LOW);
}

void drive_backward() {
  digitalWrite(motor_1A, LOW);
  digitalWrite(motor_2A, HIGH); 
  digitalWrite(motor_3A, LOW);
  digitalWrite(motor_4A, HIGH);
}

void stop_driving() {
  digitalWrite(motor_1A, LOW);
  digitalWrite(motor_2A, LOW); 
  digitalWrite(motor_3A, LOW);
  digitalWrite(motor_4A, LOW);
}

bool IMU_available() {
  if (IMU.gyroscopeAvailable() && IMU.accelerationAvailable()) {
    return true;
  }
  return false;
}

void IMU_calculations() {
  unsigned long micros_now = micros();
  if (micros_now - micros_previous >= micros_per_reading) {
    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);
    
    filter.updateIMU(gx, gy, gz, ax, ay, az);
    roll = filter.getRoll();
    pitch = filter.getPitch();
    yaw = filter.getYaw();
    micros_previous = micros_previous + micros_per_reading;
  }
}

void correct_direction() {
  if (yaw <= yaw_direction + 4 && yaw >= yaw_direction - 4) { //allow for a little error otherwise the buggy will start twitching
    analogWrite(PWM1, motor_speed);
    analogWrite(PWM2, motor_speed); 
    drive_forward();
  }
  else if (yaw > yaw_direction + 4) { //if the buggy has rotated left, rotate the buggy towards the right
    analogWrite(PWM1, 190);
    analogWrite(PWM2, 190); 
    digitalWrite(motor_1A, HIGH);
    digitalWrite(motor_2A, LOW);
    digitalWrite(motor_3A, LOW);
    digitalWrite(motor_4A, HIGH);
  }
  else { //if the buggy has rotated right, rotate the buggy towards the left
    analogWrite(PWM1, 190);
    analogWrite(PWM2, 190);
    digitalWrite(motor_1A, LOW); 
    digitalWrite(motor_2A, HIGH);
    digitalWrite(motor_3A, HIGH);
    digitalWrite(motor_4A, LOW);
  }
}

void send_data(float calculated_yaw) {
  String heading, yaw_data;
  yaw_data = String(calculated_yaw, 2);
  heading = String("heading: " + yaw_data + " " + "&");
  char data[50];
  heading.toCharArray(data, 50);
  client.write(data);
}

void reset_position() {
  gz = 0;
  yaw = 0;
}

void loop() {
  if (IMU_available()) {
    if (client.available()) {
      c = client.read();
    }

    if (c == 'g') {
      motor_speed = 120; //allows buggy to drive forward
    }

    if (c == 'b') {
      motor_speed = 0; //stops buggy
    }

    if (c == 'r') {
      reset_position(); //resets heading drift
    }

    //buggy directions up, left, down and right
    if (c == 'w') {
      yaw_direction = 180; //initial position
    }

    if (c == 'a') {
      yaw_direction = 245; //to the buggy's left
    }

    if (c == 's') {
      yaw_direction = 315; //to the south of the buggy
    }

    if (c == 'd') {
      yaw_direction = 115; //to the buggy's right
    }

    IMU_calculations();
    correct_direction();
    send_data(yaw);
    
  }
}
