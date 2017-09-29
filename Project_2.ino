//include libraries
#include <RFID.h>
#include <SPI.h>
#include <RFID.h>

//define pins for VMA405
#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN,RST_PIN);

//register variables
int led = 8;    //led pin
int id_scanned[5];    //the scanned serialnumber
boolean already_scanned = false;    //check if already scanned
boolean button_removed;     //check if button is removed
boolean pay_button_pressed = false;   //check if payment button is pressed
int total_price = 0;    //total price of all products scanned
int db[][7] = { {132241131091173, 10000,100}, {101182118008173, 5000,200} };   //the database with all the products [id, price(ct.), amount]
int scanned_items[50][3][5];    //the scanned items
int tot_scanned_items = 0;    //the total of the scanned items

//setup
void setup() {
  
    Serial.begin(9600);
    SPI.begin();
    rfid.init();

    //registers outputs & sets them to 0
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
}

void loop() {
  
    //Check for scan
    if(rfid.isCard()){
        Serial.println("We scanned an item!");
        checkForMatch();
    
  if(pay_button_pressed){
    initPayment();
    }
  }
}

void checkForMatch(){
  for(int x = 0; x < sizeof(db[])-2; x++){
              for(int y = 0; y < sizeof(rfid.id_scanned); y++ ){
                  if(rfid.id_scanned[y] != cards[x][y]) {
                      access = false;
                      break;
                  } else {
                      access = true;
                  }
              }
              if(access) break;
            }
}

void match(){
    /*
       * If there is a match:
       *  if item is allready in the scanned items add 1
       *  else make a new item in the scanned_items array for the newly scanned item
       */
  for(int i = 0; i < sizeof(db), i++;){
      if(id_scanned == db[i][0]){
        Serial.println("Scanned item recognized");
        if(scanned_items > 0){
          for (int j = 0; j < sizeof(scanned_items); j++){
            if(id_scanned == scanned_items[j][0]){
              scanned_items[6]++;
              already_scanned = true;
              break;
              }
            }
          }else if (scanned_items == 0 || !already_scanned){
          for(int k = 0; k < sizeof(scanned_items){
            }
          }
          scanned_items[tot_scanned_items] = db[i];
          scanned_items[tot_scanned_items][2][0] = 1;
          id_scanned = {0,0,0,0,0};
          tot_scanned_items++;
          allready_scanned = false;
          }
        db[i][2]--;
        //calculate the new price
        for(int l = 0; l < sizeof(scanned_items); l++;){
          
        }
    break;
    }
}

void magnet(){
  //power magnet
  while true{
    if (button_removed){
      break;
    }
  }
}

void initPayment(){
  //payment
  //ask for receipt
  if(payment_succesfull){
    //open gate
    total_price = 0;
  }
}

