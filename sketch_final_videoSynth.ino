
#include <ESP32Lib.h>
#include <Ressources/Font6x8.h>
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden

//#################################
//INITIALISE PINS FOR RASTER POTI
const int COM_PIN = 2;
const int SWITCH_PIN = 12;
const int SWITCH_PIN2 = 34;
const int SWITCH_PIN3 = 26;
String currentSwitch = "START";
//#################################
//INIT VGA PINS
const int redPin = 14;
const int greenPin = 19;
const int bluePin = 27;
const int hsyncPin = 4;
const int vsyncPin = 5;

//#################################
//AUDIO JACK
int AUDIO_PIN = 15;
//pin
//#################################
//BUTTON
int BUTTON_PIN = 13;
//#################################
//ARRAY FOR RASTER ANIMATION
const int maxCircles = 10; 
int numCircles = 10;
int circlePositions[maxCircles]  = {0,20,40,60,80,100,120,140,160,180};
bool forward = true;
//##########################
//POTIS --> first value is poti, second is mapped &or smoothed 
int potisWithMap[6][2] = {{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}};
//#########################
//DIGITL OUTS MULTIPLEXER
int MULTI_A_PIN = 33;
int MULTI_B_PIN = 32;
int MULTI_C_PIN = 18;
int MULTI_IN_PIN = 25;
int bit0 = 0;
int bit1 = 0;
int bit2 = 0;
int poticounter = 0;
//##########################
//LCD DISPLAY 
LiquidCrystal_I2C lcd(0x27, 16, 2);
//currentPoti for lcd
int currentPotiForLCD = 0;
//#################################
//VGA Device
VGA3Bit vga;


//animation stuff
int radius = 10;
int radius1 = 11;
//poti
int potiWert = 0;
int potiMap = 0;

int counter = 1;
//circleValue
//color values primary
int r = 0;
int g = 0;
int b = 0;
//color values secondary
int r1 = 0;
int g1 = 0;
int b1 = 0;
//
int r2 = 0;
int g2 = 0;
int b2 = 0;
//colorValues
bool rot =true;
bool gruen = false;
bool blau = true; 

int flashCounter = 0;



void setup() {
  //LCD
  lcd.init();
  lcd.backlight();
  //const outputs for lcd;
  //upper line ->Poti :<- 
  lcd.setCursor(0,0);
  lcd.print("Poti");
  lcd.setCursor(8, 0);
  lcd.print(": ");
  //second line 
  lcd.setCursor(0, 1);
  lcd.print("Mode");
  lcd.setCursor(8, 1);
  lcd.print(": ");
  //SET RASTER SWITCH
  pinMode(COM_PIN, INPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(SWITCH_PIN2, INPUT_PULLUP);
  pinMode(SWITCH_PIN3, INPUT_PULLUP);
  //SET BUTTON
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //digitalWrite(BUTTON_PIN, HIGH);
  //SET AUDIO JACK
  pinMode(AUDIO_PIN,INPUT_PULLUP);
  //SET MULTIPLEXER 
  pinMode(MULTI_A_PIN,OUTPUT);
  pinMode(MULTI_B_PIN,OUTPUT);
  pinMode(MULTI_C_PIN,OUTPUT);
  
  //set frame buffer (1|2)
  vga.setFrameBufferCount(2);
  //initializing vga at the specified pins
  vga.init(vga.MODE320x240, redPin, greenPin, bluePin, hsyncPin, vsyncPin);
  //setting the font
  vga.setFont(Font6x8);
  //bitrate
  Serial.begin(115200);
  //wait a bit so everything is setted 
  delay(3000);
}

//###################################
//--------ANIMATION FUNCTIONS--------
//--------pick what you like---------
//###################################

//grid function draws a grid with optional width and count
void steadyRaster(int breite){
 
 flashCounter = flashCounter+=1;
 if(flashCounter == 10){
 rot = !rot;
 gruen =!gruen;
 blau = !blau;}

int r;
int g;
int b;

if(rot){r = 255;} else {r = 0;}
if(gruen){g = 255;} else {g = 0;}
if(blau){b = 255;} else {b = 0;}

//r=255;
//g=255;
//b=255;
if(flashCounter >= 10){
if(flashCounter == 20){flashCounter=0;} 
vga.fillRect(0,0,breite,120,vga.RGB(r,0,0));
 
vga.fillRect(160,0,breite,120,vga.RGB(0,g,0));
 
vga.fillRect(0,120,breite,120,vga.RGB(0,0,b));
  
vga.fillRect(160,120,breite,120,vga.RGB(r,g,0));
  }
}

//draws a "zebrastreifen" grid which runs forward or backward 
void rasterRunning(int wert)
{
  
 //vga.fillRect(x,y,w,h,c);

  for( int i  = 1; i <= 8 ; i++){

    vga.fillRect(40*(i-1),0,40,120,vga.RGB(255,0,0));
    delay(wert);
    //vga.fillRect(40*(i-1),0,40,120,vga.RGB(0,0,0));
    vga.clear(0);
  }


}

//draws a animation which blinks a rectangle 
//starts upper left to right to lower left to right
void fourTimesfour2(int wert, int breite)
{
  
 //vga.fillRect(x,y,w,h,c);


vga.fillRect(0,0,breite,120,vga.RGB(255,0,0));
 delay(wert);
 vga.fillRect(0,0,breite,120,vga.RGB(0,0,0));
 
vga.fillRect(80,0,breite,120,vga.RGB(255,0,255));
 delay(wert);
vga.fillRect(80,0,breite,120,vga.RGB(0,0,0));

vga.fillRect(160,0,breite,120,vga.RGB(0,255,0));
delay(wert);
vga.fillRect(160,0,breite,120,vga.RGB(0,0,0));

vga.fillRect(0,120,breite,120,vga.RGB(0,0,255));
delay(wert);
vga.fillRect(0,120,breite,120,vga.RGB(0,0,0));
 
vga.fillRect(160,120,breite,120,vga.RGB(255,255,0));
delay(wert);
vga.fillRect(160,120,breite,120,vga.RGB(0,0,0));
}

//draws a zoom effect of a circle with a inner circle  
void circleZoomEffect(int color){

int  xPos = 160;
int  yPos = 120;

radius += 1;
radius1 += 2;

if(radius >= 100){
  radius = 10;
  radius1 = 11;
}

  vga.fillCircle(xPos,yPos,radius, vga.RGB(color,255,255));
  vga.circle(xPos, yPos, radius1, vga.RGB(color,255,0));


}

//draws a raster with a given count over the whole display
void drawRaster(int anzahl) {
  int gridCols = sqrt(anzahl); // Number of columns in the grid
  int gridRows = anzahl / gridCols; // Number of rows in the grid
  int breite = 320 / gridCols; // Width of each rectangle
  int hoehe = 240 / gridRows; // Height of each rectangle
  int r = 255;
  int g = 255;
  int b = 255;

  for (int row = 0; row < gridRows; row++) {
    for (int col = 0; col < gridCols; col++) {
      int posX = col * breite;
      int posY = row * hoehe;

      vga.fillRect(posX, posY, breite, hoehe, vga.RGB(r, 0, 0));
      vga.rect(posX, posY, breite, hoehe, vga.RGB(r, g, b));
    }
  }
}
//draws a raster with a count and a shape selector , like circle,rectangle  .. .(provide 3 cases at least mapped values with 1,2,3)
void drawRasterWithShape(int anzahl, int shape, int r, int g, int b) {
  
  int gridCols = sqrt(anzahl); // Number of columns in the grid
  int gridRows = anzahl / gridCols; // Number of rows in the grid
  int breite = 320  / gridCols; // Width of each rectangle
  int hoehe = 240  / gridRows; // Height of each rectangle
  
  //int r = 0;
  //int g = 0;
  //int b = 0;
 //use r1,g1,b1 for secondary color
  for (int row = 0; row < gridRows; row++) {
    for (int col = 0; col < gridCols; col++) {
         int posX = col * breite;
         int posY = row * hoehe;

        switch (shape) {
    case 1: 
        //int posXK = (col * breite) + (breite / 2);
        //int posYK = (row * hoehe) + (hoehe / 2);        
        vga.fillRect(posX, posY, breite, hoehe, vga.RGB(r1, g1, b1));
        vga.fillCircle(posX + (breite  / 2), posY + (hoehe / 2), hoehe / 2, vga.RGB(r,g,b));       
        break; 
    case 2:
       // int posX = col * breite;
       // int posY = row * hoehe;
      vga.fillRect(posX, posY, breite, hoehe, vga.RGB(r, g, b));
      vga.rect(posX, posY, breite, hoehe, vga.RGB(r1, g1, b1));
        break;
    case 3: 
          vga.fillRect(0,posY,320,hoehe,vga.RGB(r,g,b));
          vga.rect(0,posY,320,hoehe,vga.RGB(r1, g1, b1));
         
        break;
      }
      
    }
  }
}

//draws a raster with an animation 
void rasterWithAnimation(int numCircles){

  

  for (int i = 0; i < numCircles; i++){
    int xPos = circlePositions[i];
    int yPos = 50;
    int radius = 10; 
    int r = 255;
    int g = 255;
    int b = 0;

    if ((i==9) && (xPos >= 320 - 10)) {
      forward = false;
      } else if ((i==0) && (xPos <= 0 + 10)) {
        forward = true;
        }


    if (forward) {
       xPos += 1;
    } else {
      xPos += -1;
    }

    
    circlePositions[i] = xPos;
    vga.fillRect(xPos, yPos, radius, radius*3, vga.RGB(r,g,b));
    vga.fillRect(xPos, yPos+100, radius, radius*3, vga.RGB(r,g,b));
    
    //vga.fillCircle(xPos,yPos,radius,vga.RGB(r,g,b));
  }


}


//####################################
//------MAIN LOOP---------------------
//################################### 
void loop() {
  //lcd output
    //lcd clear the number, when 100 then 99 
    if (potisWithMap[currentPotiForLCD][0]<100 && potisWithMap[currentPotiForLCD][1]>=100) {
        lcd.setCursor(10, 0);
        lcd.print("   ");
        }   
    //from 2 character to 1 character
    if (potisWithMap[currentPotiForLCD][0]<10 && potisWithMap[currentPotiForLCD][1]>=10) {
        lcd.setCursor(10, 0);
        lcd.print("   ");
        } 


  lcd.setCursor(6, 0);
  lcd.print(currentPotiForLCD);
 
  lcd.setCursor(10, 0);
  lcd.print(potisWithMap[currentPotiForLCD][0]);
  
  lcd.setCursor(10, 1);
  lcd.print(currentSwitch);

  //read in our -raster-poti-switch 
  int comState = digitalRead(COM_PIN);
  int switchState = digitalRead(SWITCH_PIN);
  int switchState2 = digitalRead(SWITCH_PIN2);
  int switchState3 = digitalRead(SWITCH_PIN3);

  
  //check and store potis
  for (counter = 0 ; counter <= 7; counter++ ){
    //lcd.setCursor(6, 0);
    //lcd.print("  ");
    bit0 = bitRead(counter, 0);
    bit1 = bitRead(counter, 1);
    bit2 = bitRead(counter, 2);

    digitalWrite(33,bit0);
    digitalWrite(32,bit1);
    digitalWrite(18,bit2);
    //test
    //potisWithMap[poticounter][0] = analogRead(25);
    //potisWithMap[poticounter][1] = map(potisWithMap[poticounter][0],0,4095,0,1000);  
    //original for different maps we should use the switch case
    int maxMap = 1000;
    int minMap = 1;
    int range = 3;

    switch (counter) {
      //poti for count not higher than 80
      case 0: maxMap = 80;
              minMap = 1;
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1]+2 || potisWithMap[counter][0]<potisWithMap[counter][1]-2 ){
                  currentPotiForLCD = counter;
              }
                
              
        break;
      case 1:  maxMap = 7;
               minMap = 1;
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1] || potisWithMap[counter][0]<potisWithMap[counter][1] ){
                  currentPotiForLCD = counter;
                  
              }

              switch(potisWithMap[counter][0]){
                case 1 :r2 = 255;
                        g2 = 255;
                        b2 = 255;
                        break;
                case 2 :r2 = 0;
                        g2 = 255;
                        b2 = 255;
                        break;
                case 3 :r2 = 0;
                        g2 = 0;
                        b2 = 255;
                        break;
                case 4 :r2 = 255;
                        g2 = 0;
                        b2 = 255;
                        break;
                case 5 :r2 = 255;
                        g2 = 255;
                        b2 = 0;
                        break;
                case 6 :r2 = 255;
                        g2 = 0;
                        b2 = 0;
                        break;       
                case 7 :r2 = 0;
                        g2 = 0;
                        b2 = 0;
                        break;                                        
              }

        break;
      case 2:  maxMap = 6;
               minMap = 1;
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1]+1 || potisWithMap[counter][0]<potisWithMap[counter][1]-1 ){
                  currentPotiForLCD = counter;
              }
              
        break;
      case 3:  maxMap = 160;
              
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, 0, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, 0, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1]+range || potisWithMap[counter][0]<potisWithMap[counter][1]-range ){
                  currentPotiForLCD = counter;
              }
        break;

      case 4:  maxMap = 7; //color poti
               minMap = 1; //1..6 farben
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1]+1 || potisWithMap[counter][0]<potisWithMap[counter][1]-1 ){
                  currentPotiForLCD = counter;
              }
              switch(potisWithMap[counter][0]){
                case 1 :r = 255;
                        g = 255;
                        b = 255;
                        break;
                case 2 :r = 0;
                        g = 255;
                        b = 255;
                        break;
                case 3 :r = 0;
                        g = 0;
                        b = 255;
                        break;
                case 4 :r = 255;
                        g = 0;
                        b = 255;
                        break;
                case 5 :r = 255;
                        g = 255;
                        b = 0;
                        break;
                case 6 :r = 255;
                        g = 0;
                        b = 0;
                        break;       
                case 7 :r = 0;
                        g = 0;
                        b = 0;
                        break;                                        
              }
          
        break;      
      case 5:  maxMap = 7; //secondary color
               minMap = 1;
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1] || potisWithMap[counter][0]<potisWithMap[counter][1] ){
                  currentPotiForLCD = counter;
              }
              switch(potisWithMap[counter][0]){
                case 1 :r1 = 255;
                        g1 = 255;
                        b1 = 255;
                        break;
                case 2 :r1 = 0;
                        g1 = 255;
                        b1 = 255;
                        break;
                case 3 :r1 = 0;
                        g1 = 0;
                        b1 = 255;
                        break;
                case 4 :r1 = 255;
                        g1 = 0;
                        b1 = 255;
                        break;
                case 5 :r1 = 255;
                        g1 = 255;
                        b1 = 0;
                        break;
                case 6 :r1 = 255;
                        g1 = 0;
                        b1 = 0;
                        break;       
                case 7 :r1 = 0;
                        g1 = 0;
                        b1 = 0;
                        break;                                        
              }

        break;
      case 6: 
              /*maxMap = 1000; //
               minMap = 1;
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1] || potisWithMap[counter][0]<potisWithMap[counter][1] ){
                  currentPotiForLCD = counter;
              }*/
            
        break;
      case 7: /*maxMap = 1000;
              minMap = 1;
              if (potisWithMap[counter][0] == 0){
                potisWithMap[counter][1] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              }
              potisWithMap[counter][1] = potisWithMap[counter][0];
              potisWithMap[counter][0] = map(analogRead(MULTI_IN_PIN), 0, 4095, minMap, maxMap); //thats (...) poti
              if ( potisWithMap[counter][0]>potisWithMap[counter][1] || potisWithMap[counter][0]<potisWithMap[counter][1] ){
                  currentPotiForLCD = counter;
              }*/
              
           
        break;  
    }
   

    
 
    
  } 



  if (digitalRead(BUTTON_PIN)==LOW){
    vga.clear();
    vga.show();
  }
 
  if (switchState == LOW) {
    //SCHALTER POSITION 1
    currentSwitch = "CLEAR";
     //vga.fillRect(0,0,40,40,vga.RGB(255,0,0)); 
    vga.clear();
    
   
  }
  if (switchState2 == LOW) {
    //SCHALTER POSITION 2
      currentSwitch = "RASTER"; 
      
        if (analogRead(AUDIO_PIN) >= 50) {
            //vga.fillRect(0, 0, 320, 240, vga.RGB(0,0,0));
            drawRasterWithShape(potisWithMap[0][0],potisWithMap[2][0],0,0,0);
           delay(20);
        } else {
             drawRasterWithShape(potisWithMap[0][0],potisWithMap[2][0],r,g,b);
            // drawRasterWithShape(potisWithMap[0][0],potisWithMap[2][0],r,g,b); 
          }
        

  }
  
  if (switchState3 == LOW) {
    //SCHALTER POSITION 3
    currentSwitch = "CIRCLE";
    //read in our beat   
  	if(analogRead(AUDIO_PIN) >= 50)  {
      circleZoomEffect(0);
      //vga.show();
      delay(30);
      
    } else if (analogRead(AUDIO_PIN) >= 60 && analogRead(AUDIO_PIN) <=100) {
        vga.fillRect(140,100,40,40,vga.RGB(255,255,255));
    }else {

      circleZoomEffect(255);
    
    }
  }
  /*  
  if (switchState4 == LOW) {
    //SCHALTER 4
    vga.clear();
    drawRaster(8);
    vga.show();
  }
  */
  vga.show();
  
}