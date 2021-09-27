#include <PID_v1.h> //PID library
#include <WiFiNINA.h>

//SILVER CHALLENGE

char ssid[] = "Virgin Macs";
char pass[] = "0870541630gmC";
//char ssid[] = "TP-LINK_BCA6_5G";
//char pass[] = "47451797";
WiFiClient client;
WiFiServer server(5200);
char c; //for client input

//IR sensors
const int IRS_left = 2; //left infrared sensor
const int IRS_right = 3; //right infrared sensor
int left_val;
int right_val;
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

//PID control
double Kp = 20; //affects the gain of the PID controller
double Ki = 0.1; //adjusts the steady state error
double Kd = 0.00; //used for steady signals so for this project a PI controller was enough
double input, output;
double setPoint;
PID buggyPID(&input, &output, &setPoint, Kp, Ki, Kd, REVERSE); //function to calculate output using PID constants and input
int control_speed; //speed set by PID controller

//Global variables
long previous_time = 0;

//Note:
//sensor value = 0 when LED is GREEN
//sensor value = 1 when LED is RED
//sensor is GREEN when on white
//sensor is RED when on black

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  IPAddress IP = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(IP);
  server.begin();

  setPoint = 20; //setting the optimal distance to be 20cm
  buggyPID.SetMode(AUTOMATIC);
  buggyPID.SetOutputLimits(0,255); //limit output to value range of 0 - 255

  //initialising all pins
  pinMode(motor_1A, OUTPUT);
  pinMode(motor_2A, OUTPUT);
  pinMode(motor_3A, OUTPUT);
  pinMode(motor_4A, OUTPUT);
  pinMode(IRS_left, INPUT);
  pinMode(IRS_right, INPUT);
  pinMode(US_TRIGGER, OUTPUT);
  pinMode(US_ECHO, INPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);

  stop_driving(); //set all motors to off
  analogWrite(PWM1, 0); //set speed to off
  analogWrite(PWM2, 0);

  while (!client.connected()){ //connect to Processing client
  client = server.available();
  if (client.connected()){
    Serial.println("Client Connected");
    }
  }
}

void drive_forward(){
  digitalWrite(motor_1A, HIGH);
  digitalWrite(motor_2A, LOW);
  digitalWrite(motor_3A, HIGH);
  digitalWrite(motor_4A, LOW);
}

void stop_driving(){
  digitalWrite(motor_1A, LOW); //motors are all off
  digitalWrite(motor_2A, LOW);
  digitalWrite(motor_3A, LOW);
  digitalWrite(motor_4A, LOW);
}

//IR sensor code
void correct_path(int left, int right){
  if (left_val == 0){ 
    analogWrite(PWM1, 10); //slow left motor to allow buggy to correct itself
    analogWrite(PWM2, control_speed); 
    drive_forward();
  } 
  else if (right_val == 0) {
    analogWrite(PWM1, control_speed); 
    analogWrite(PWM2, 10); //slow right motor to allow buggy to correct itself
    drive_forward();
  }
}

void follow_line(int left, int right){
  if((left == 1) && (right == 1)){ //buggy is where it needs to be
    drive_forward();
  }
  else if ((left == 0) && (right == 0)){ //if buggy ends up outside the track
    stop_driving(); 
  }
  else {
    correct_path(left, right); //one of the sensors needs to be corrected
  }
}

//US sensor code
int distance_from_object(){
  long duration;
  digitalWrite(US_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(US_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(US_TRIGGER, LOW);
  duration = pulseIn(US_ECHO, HIGH);
  return((duration/58)-4);//for length of IR-sensors
}

void loop(){
  
  c = client.read(); //read in character from client
  if (c == 'w'){
    while (c == 'w'){ //run this loop repeatedly as long as START has been pressed
      long current_time = millis();
      input = distance_from_object();;
      buggyPID.Compute(); //calculate PID values for the motors
      control_speed = output;
      analogWrite(PWM1, control_speed);
      analogWrite(PWM2, control_speed);
    
      if (output == 0) {
        client.write("Obstacle detected"); //buggy has stopped for an obstacle
        while (output == 0) {
          input = distance_from_object(); //update the distance while the buggy has stopped
          buggyPID.Compute();
        }
        client.write("Obstacle cleared");
      }

       if (current_time - previous_time > 1500) { //send out distance values to Processing every 1.5 secs
        int distance = distance_from_object();
        char data[99];   
        sprintf(data,"Distance in cm: %u", distance); //this allows int values to be read in by Processing
        client.write(data);//otherwise it's sent as bytes
        previous_time = current_time;
      }
      
      left_val = digitalRead(IRS_left);
      right_val = digitalRead(IRS_right);
      follow_line(left_val, right_val); //follow the track using IR sensors
      
      c = client.read(); //check for input from client again
      if (c != 'y'){
        c = 'w';
      }
    }
  }
  
  if (c == 'y') {
    stop_driving(); //STOP button has been pressed
  }
}
