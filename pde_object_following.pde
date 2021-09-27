import processing.serial.*;
import controlP5.*;
import processing.net.*;
import meter.*;
ControlP5 cp5;
ControlP5 p5;
ControlP5 theFont;
Client myClient;
PFont font;
String input;
int dist;
int Speed=0;
Meter m;
Textarea textbox; 
Println console;

void setup() {
  //server stuff
  myClient=new Client(this,"192.168.0.249", 5200);
 
  //GUI stuff
  size(800,500);

  cp5 = new ControlP5(this);
  font = createFont("Verdana Bold Italic",15); //_ select a font
  textFont(font); 
  
  cp5.addButton("Start")
  .setValue(0)
  .setPosition(110,145)
  .setSize(200,100)
  .setFont(font);
  
  cp5.addButton("Stop")
  .setValue(0)
  .setPosition(110,250)
  .setSize(200,100)
  .setFont(font);
    
  noStroke();
 
  cp5.enableShortcuts(); //create a textbox to display telemetry
  frameRate(50);
  textbox = cp5.addTextarea("txt")
               .setPosition(405, 85)
               .setSize(290, 90)
               .setFont(font)
               .setLineHeight(25)
               .setColor(color(0))
               .setColorBackground(color(255))
               .setColorForeground(color(0));
  
  console = cp5.addConsole(textbox); //set console as the textbox
               
  
  p5 = new ControlP5(this);
  p5.addSlider("Speed")
     .setPosition(400, 400)
     .setSize(300, 30)
     .setRange(0, 100)
     //.setValue(Speed)
     .setColorCaptionLabel(color(20,20,20));
    
    //Distance Meter 
  m = new Meter(this,400,200,false);
        m.setMeterWidth(300);
        m.setMinScaleValue(0);
        m.setMaxScaleValue(50);
        m.setArcColor(color(141, 113, 178));
        m.setArcThickness(10);
        m.setDisplayDigitalMeterValue(false);
        m.setTitleFontSize(20);
        m.setTitleFontName("Verdana Bold Italic");
        m.setTitle("Distance to Obstacle");
        String[] scaleLabels = {"0", "10", "20", "30", "40", "50+"};
        m.setScaleLabels(scaleLabels);
        m.setShortTicsBetweenLongTics(9);
        m.setNeedleThickness(3);
        
        m.setMinInputSignal(0);
        m.setMaxInputSignal(50);
        
}


public void draw() {
  background (#FFBFE5);
  fill (0,0,0);       // text color (r, g, b)
  text("Buggy Control Panel GUI, Group W05", 450, 40); 
  m.updateMeter(dist);
  
  fill(0,0,0);
  text("Kp = 20, Ki = 0.1, Kd = 0.0", 100, 420);
  
  rect(400,80,300,100);
  fill(255);
    
  if (myClient.available() > 0) {
    input = myClient.readString(); //Processing will read in values as before and display them on the GUI
    println(input);
  }
 
}

public void Start(int theValue) {
  if (myClient.active()){
    myClient.write("w");
    println("Start Button Pressed");
  }
}

public void Stop(int theValue) {
  if (myClient.active()){
    myClient.write("y");
    println("Stop Button Pressed");
  }
}
