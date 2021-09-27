import processing.serial.*;
import controlP5.*;
import processing.net.*;
ControlP5 cp5;
ControlP5 p5;
ControlP5 theFont;
Client myClient;
PFont font;
Textarea textbox; 
Println console;
float yaw;
long previous_time = 0;

void setup() {
  //server stuff
  myClient=new Client(this,"192.168.0.249", 80);
 
  //GUI stuff
  size(1000, 600);

  cp5 = new ControlP5(this);
  font = createFont("Verdana Bold Italic",15); //_ select a font
  textFont(font); 
  
  //buttons
  cp5.addButton("Forward")
  .setPosition(190, 50)
  .setSize(125, 80)
  .setFont(font);
  
  cp5.addButton("Down")
  .setPosition(190, 212)
  .setSize(125, 80)
  .setFont(font);
  
  cp5.addButton("Left")
  .setPosition(125, 131)
  .setSize(125, 80)
  .setFont(font);
  
  cp5.addButton("Right")
  .setPosition(251, 131)
  .setSize(125, 80)
  .setFont(font);
  
  cp5.addButton("GO")
  .setPosition(150, 330)
  .setColorBackground(color(52, 255, 39))
  .setSize(100, 100)
  .setFont(createFont("SansSerif.plain", 20));
  
  cp5.addButton("STOP")
  .setPosition(251, 330)
  .setColorBackground(color(255, 39, 39))
  .setSize(100, 100)
  .setFont(createFont("SansSerif.plain", 20));
  
  cp5.addButton("Reposition")
  .setPosition(635, 495)
  .setColorBackground(color(#DE2B4E))
  .setSize(170,70)
  .setFont(font);
 
  cp5.enableShortcuts(); //create a textbox to display telemetry
  frameRate(50);
  textbox = cp5.addTextarea("txt")
               .setPosition(105, 470)
               .setSize(290, 90)
               .setFont(font)
               .setLineHeight(25)
               .setColor(color(0))
               .setColorBackground(color(255))
               .setColorForeground(color(0));
  
  console = cp5.addConsole(textbox); //set console as the textbox
               
}

public void draw() {
  long current_time = millis();
  
  background (#FFBFE5); //background colour
  fill(0,0,0);
  text("Buggy Control Panel GUI, Group W05", 650, 40); 
  
  fill(255);
  rect(100, 465, 300, 100);
  
  fill(255);
  rect(550, 80, 340, 380);
  
  readInput();
  
  drawBuggy(yaw - 180);
  
  if (current_time - previous_time > 1500) {
  print("Yaw/heading value: "); //print out yaw values every 1.5 seconds
  println(yaw*-1);
  previous_time = current_time;
  }
    
}

void readInput() { //read in heading from Arduino server
  int at_symbol = 64; // @ character in ASCII
    String input = myClient.readStringUntil(at_symbol); // read from Arduino until @
    if (input != null) {
      String[] list = split(trim(input), " ");
      if (list[0].equals("heading:")) {
        yaw = float(list[1])*(-1); // convert to float yaw
      }
   }
}

void drawBuggy(float heading) { //draw 2D visual of buggy rotating
   pushMatrix();
   fill(255);
   rectMode(CENTER);
   translate(720, 280);
   rotate(radians(heading));
   
   strokeWeight(4);
   fill(234, 234, 234);
   rect(0, 0, 150, 220);
   
   strokeWeight(3);
   fill(#FCDA2E);
   ellipse(90, -60, 20, 150);
   ellipse(-90, -60, 20, 150);
   
   fill(0, 90, 90);
   rect(-15, 65, 90, 30);
   
   popMatrix();
   rectMode(CORNER);
}

//buttons
public void Forward(int theValue) {
  if (myClient.active()){
    myClient.write("w");
    println("Going forward");
  }
}

public void Right(int theValue) {
  if (myClient.active()){
    myClient.write("d");
    println("Going right");
  }
}

public void Left(int theValue) {
  if (myClient.active()){
    myClient.write("a");
    println("Going left");
  }
}

public void Down(int theValue) {
  if (myClient.active()){
    myClient.write("s");
    println("Going in reverse");
  }
}
  
public void GO(int theValue) {
  if (myClient.active()){
    myClient.write("g");
    println("Driving...");
  }
}

public void STOP(int theValue) {
  if (myClient.active()){
    myClient.write("b");
    println("Stopping...");
  }
}

public void Reposition(int theValue) {
  if (myClient.active()){
    myClient.write("r");
    println("Position reset successfully");
  }
}
