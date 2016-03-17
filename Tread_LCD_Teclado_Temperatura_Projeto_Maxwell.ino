
// Sensor de temperatura - DS18B20 "1-Wire" digital
// Sensor de Temperatura Duplo - Projeto da cervejaria.
// Controle de Teclado - Projeto da cervejaria.
#include <DallasTemperature.h>
#include <OneWire.h>
#include <LiquidCrystal.h>
#include <Thread.h>
#include <ThreadController.h>


LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // Cria um objeto lcd e atribui os pinos
byte a[8]= {B00110,B01001,B00110,B00000,B00000,B00000,B00000,B00000,}; //Array que desenha o simbolo de grau

// Dados do medidor de Temperatura "Data wirento" São lido e conectdos no pino 2
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices // Ativa a instância oneWire para comunicar  dispositivos medidores de temperatura
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Endereços dos sensores de temperatura  DS18B20. 
DeviceAddress insideThermometer = { 0x28, 0xE3, 0x62, 0x09, 0x06, 0x00, 0x00, 0xBE };
DeviceAddress outsideThermometer = { 0x28, 0x8B, 0x0F, 0x09, 0x06, 0x00, 0x00, 0x21 };

void medirtemperatura(){
  // tentativa de Thread: O objetivo dessa função será medir o tempo
  // Essa função é chamada pela Thread medirtemp
}  
void mostrartempo(){
  // tentativa de Thread: O objetivo dessa função será mostrar o tempo
  // Essa função é chamada pela Thread mostrartemp
}
void lerteclado(){
  // tentativa de Thread: O objetivo dessa função será ler tecclas apertadas
  // Essa função é chamada pela Thread lertecl
}

ThreadController cpu; // A ThreadController vai verificar e acionar as thread criadas no programa
Thread medirtemp;    // Cria Thread medirtemp
Thread mostrartemp;   // Cria Thread mostrartemp
Thread lertecl;       // Cria Thread lertecl


void setup(void)
{
    // Start up the library - Inicia a biblioteca
    sensors.begin();

    // set the resolution to 10 bit (good enough?)
    sensors.setResolution(insideThermometer, 10);
    sensors.setResolution(outsideThermometer, 10);

    //pinMode(backLight, OUTPUT);
    //digitalWrite(backLight, HIGH); // turn backlight on. Replace 'HIGH' with 'LOW' to turn it off.
     lcd.begin(16,2); // columns, rows. use 16,2 for a 16x2 LCD, etc.
     lcd.clear(); // start with a blank screen
     lcd.begin(16, 2);              // start the library
     lcd.setCursor(0,0);
     lcd.print("INICIAR PROCESSO"); // print a simple message
     
     medirtemp.setInterval(500);      // Determnina o intervalo que a Thread vai ser lida
     medirtemp.onRun(medirtemp);     // Chama a função medirtemp
     
     mostrartemp.setInterval(400);    // Determnina o intervalo que a Thread vai ser lida
     mostrartemp.onRun(medirtemp);   // Chama a função mostrartemp
     
     lertecl.setInterval(300);        // Determnina o intervalo que a Thread vai ser lida
     lertecl.onRun(lertecl);         // Chama a função lertecl
     
     cpu.add(&medirtemp);            // Gerencia as threads criadas
     cpu.add(&mostrartemp);          // Gerencia as threads criadas
     cpu.add(&lertecl);              // Gerencia as threads criadas
}




void printTemperature(DeviceAddress deviceAddress)
{
    float tempC = sensors.getTempC(deviceAddress);
    if (tempC == -127.00) {
    lcd.print("Error");
    } else {
        lcd.print(tempC);
        //Atribui a "1" o valor do array "A", que desenha o simbolo de grau
        lcd.createChar(1, a); 
        //lcd.setCursor(7,1); //Coloca o cursor na coluna 7, linha 1
        //lcd.write(1); //Escreve o simbolo de grau
        lcd.setCursor(14,0);
        lcd.write(1);
        lcd.print("C");
        lcd.setCursor(14,1);
        lcd.write(1);
        lcd.print("C");
        //lcd.print("/");
        //lcd.print(DallasTemperature::toFahrenheit(tempC));
    }
}

// Define valores usados pelo painel e botões
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
 
// Leitura dos Botões
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;
 return btnNONE;  // when all others fail, return this...
}
 

void loop()
{
 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons
 
 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
        lcd.clear(); // start with a blank screen
        delay(200);
        sensors.requestTemperatures();
        lcd.setCursor(0,0);
        lcd.print("Temp 1: ");
        printTemperature(insideThermometer);
        lcd.setCursor(0,1);
        lcd.print("Temp 2: ");
        printTemperature(outsideThermometer);
        break;        
     }
   
    case btnLEFT:
     {       
        lcd.clear(); // start with a blank screen
        delay(200); 
        lcd.setCursor(0,0); // move cursor to second line "0" and 0 spaces over
        lcd.print("Tempo Decorrido  ");
        lcd.setCursor(0,1); // move cursor to second line "0" and 0 spaces over
        lcd.print("Contador  ");    
      
          //'lcd_key = read_LCD_buttons();  
           
           
           //lcd.setCursor(13,1);
           //lcd.print( lcd_key ); // Indica o valor da tecla apertada up=1  
            
        
        
       do { 
           lcd.setCursor(9,1); // move cursor to second line "1" and 9 spaces over       
           lcd.print(millis()/1000);      // display seconds elapsed since power-up   
           lcd_key = read_LCD_buttons();    
           
      
           } while(lcd_key == 3);  
        break;
     
     }     
      
   case btnUP:
     {
     lcd.clear(); // start with a blank screen 
     lcd.setCursor(0,1); 
     lcd.print("UP    ");
     lcd.setCursor(5,1);
     lcd.print( lcd_key ); // Indica o valor da tecla apertada up=1
     lcd.setCursor(9,1);
     lcd.print(adc_key_in );
     break;
     }
   case btnDOWN:
     {
     lcd.clear(); // start with a blank screen 
     lcd.setCursor(0,1); 
     lcd.print("DOWN  "); 
     lcd.setCursor(5,1);
     lcd.print( lcd_key ); //Indica o valor da tecla apertada up=2
     lcd.setCursor(9,1);
     lcd.print(adc_key_in );
     break;
     }
   case btnSELECT:
     {
     lcd.clear(); // start with a blank screen
     lcd.setCursor(0,1);  
     lcd.print("SELE");
     lcd.setCursor(5,1);
     lcd.print( lcd_key ); //Indica o valor da tecla apertada up=4
     lcd.setCursor(9,1);
     lcd.print(adc_key_in );
     break;
     }
     case btnNONE:
     {
     //lcd.print("Contador  ");
     break;
     }
 
     
}
}
    
     
     
     
 




