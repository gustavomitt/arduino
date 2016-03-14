/*
  Matrix
  LED-Matrix displaying image
  
  This example is part of the Fritzing Creator Kit: www.fritzing.org/creator-kit.
*/

int colPins[8] = {8,9,10,11,12,13,14,15};         // matrix rows connected to the Arduino
int rowPins[8] = {7,6,5,4,3,2,1,0};       // matrix columns connected to the Arduino

int estadoSmily = 1;
int estadoDash = 2;
int estadoSad = 3;
int estado = estadoSmily;
int loopNumber = 0;
int sensorPin = A5;    // select the input pin for the sensor
int activatePin = A1;    // select the pin to turn on the sensor
int sensorValue = 0;  // variable to store the value coming from the sensor


int smilyFace[8][8]={                              // the image displayed on the matrix : 1 = LED on, 0 = LED off
{0,0,0,0,0,0,0,0},  
{0,1,1,0,0,1,1,0},
{0,1,1,0,0,1,1,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{1,0,0,0,0,0,0,1},
{0,1,1,1,1,1,1,0},
{0,0,0,0,0,0,0,0}
                };  

int dashFace[8][8]={                              // the image displayed on the matrix : 1 = LED on, 0 = LED off
{0,0,0,0,0,0,0,0},  
{0,1,1,0,0,1,1,0},
{0,1,1,0,0,1,1,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{1,1,1,1,1,1,1,1},
{0,0,0,0,0,0,0,0}
                }; 

int sadFace[8][8]={                              // the image displayed on the matrix : 1 = LED on, 0 = LED off
{0,0,0,0,0,0,0,0},  
{0,1,1,0,0,1,1,0},
{0,1,1,0,0,1,1,0},
{0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0},
{0,1,1,1,1,1,1,0},
{1,0,0,0,0,0,0,1},
{0,0,0,0,0,0,0,0},
                };                      
                

void setup()
{
  for (int i=0; i<8; i++){                       // all pins are declared as outputs 
    pinMode(rowPins[i],OUTPUT);
    pinMode(colPins[i],OUTPUT);
  }
  for (int i=0; i<8; i++){                       // all pins are declared as outputs 
    digitalWrite(rowPins[i],HIGH);            
    digitalWrite(colPins[i],LOW);            
  }
  pinMode(activatePin,OUTPUT);
  digitalWrite(activatePin,LOW);
}

void smile()
{
  for (int y=0; y<8; y++)
  {                       
    // rowwise 
    for (int x=0; x<8; x++)
    {                     
      // check all entries of the array from left to right 
      if (smilyFace[x][y]==1)
      {                       
        // is the entry 1
        digitalWrite(colPins[x],HIGH);           
        // switch on column pin
      } 
      else 
      {                                   
        // else
        digitalWrite(colPins[x],LOW);            
        // switch it off
      }
    }
    
    digitalWrite(rowPins[y],LOW);                
    // switch the row pin to LOW (because it is the cathode of the LED; LOW means ON)
    delayMicroseconds(100);                      
    // stop the program for 100 milliseconds
    digitalWrite(rowPins[y],HIGH);               
    // switch the row pin to HIGH (this means OFF)
  }
}

void sad()
{
  for (int y=0; y<8; y++)
  {                       
    // rowwise 
    for (int x=0; x<8; x++)
    {                     
      // check all entries of the array from left to right 
      if (sadFace[x][y]==1)
      {                       
        // is the entry 1
        digitalWrite(colPins[x],HIGH);           
        // switch on column pin
      } 
      else 
      {                                   
        // else
        digitalWrite(colPins[x],LOW);            
        // switch it off
      }
    }
    
    digitalWrite(rowPins[y],LOW);                
    // switch the row pin to LOW (because it is the cathode of the LED; LOW means ON)
    delayMicroseconds(100);                      
    // stop the program for 100 milliseconds
    digitalWrite(rowPins[y],HIGH);               
    // switch the row pin to HIGH (this means OFF)
  }
}

void dash()
{
  for (int y=0; y<8; y++)
  {                       
    // rowwise 
    for (int x=0; x<8; x++)
    {                     
      // check all entries of the array from left to right 
      if (dashFace[x][y]==1)
      {                       
        // is the entry 1
        digitalWrite(colPins[x],HIGH);           
        // switch on column pin
      } 
      else 
      {                                   
        // else
        digitalWrite(colPins[x],LOW);            
        // switch it off
      }
    }
    
    digitalWrite(rowPins[y],LOW);                
    // switch the row pin to LOW (because it is the cathode of the LED; LOW means ON)
    delayMicroseconds(100);                      
    // stop the program for 100 milliseconds
    digitalWrite(rowPins[y],HIGH);               
    // switch the row pin to HIGH (this means OFF)
  }
}


void loop()
{
  loopNumber++;
  if(loopNumber == 59000){
    digitalWrite(activatePin,HIGH);
  }
  if(loopNumber == 60000){
    sensorValue = analogRead(sensorPin);// reading sensor value
    if (sensorValue <= 500){      
      estado = estadoSmily;
    }else if(sensorValue >= 800){
      estado = estadoSad;
    }else if((sensorValue < 800) && (sensorValue > 500)){
      estado = estadoDash;
    }
    digitalWrite(activatePin,LOW);
    loopNumber = 0;
  }
    
  
  if (estado == estadoSmily){      
    smile();
  }else if(estado == estadoSad){
    sad();
  }else if(estado == estadoDash){
    dash();
  }

}

