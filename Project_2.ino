/*
 * Steenbrink
 * 27-09-2017
 * "Automated check-out project"
 */

//include libraries
#include <RFID.h>
#include <SPI.h>
#include <RFID.h>

//define pins for VMA405
#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN,RST_PIN);

//register variables
String serial;   //storage of the scanned serial code

boolean already_bought = false;   //check to see if item was allready bought

String product_serial[] = {"101182118008173", "132241131091173", "227179198250108"};    //the database with all the serialcodes of the tags
int product_data[][2] = { {100, 50}, {50, 20}, {25, 3} };   //the price(€ct) and amount of products corresponding to above array
int product_amount = 3;   //the amount of items in the array above

String bought_product[100];     //all the serialcodes of the bought products
int data_bought[100][2];   //the price(€ct) and amount of products bought corresponding to the above array
int bought_products = 0;    //the amount of bought products

int total = 0;    //the total price

void setup() {
  // register lcd, sixteen characters across 2 lines
  
  Serial.begin(9600);

  //init libraries for reader
  SPI.begin();
  rfid.init();
}

void loop() {
    // if a card is detected
    if(rfid.isCard()){
        //if we can read the card
        if(rfid.readCardSerial()){
            Serial.println("Card registered");
            registerSerial();
            Serial.println("Serial = " + serial);
            checkSerial();
        }
    }
}

/*
 * First clears the current serial
 * Then adds the 5 3numbercodes together to one code
 * if the code is smaller than 10 add 00 if smaller than 100 add 0 to keep the 3 numbers
 */
void registerSerial(){
    serial = "";
    for(int i = 0; i < 5; i++){
        if(rfid.serNum[i] < 10){
            serial = serial + "00"+rfid.serNum[i];
        }else if (rfid.serNum[i] < 100){
            serial = serial + "0"+rfid.serNum[i];
        }else{
            serial = serial + rfid.serNum[i];
        }
    }
}

void checkSerial(){
    for(int j = 0; j < product_amount; j++){
        if(product_serial[j] == serial){
            Serial.println("check done");
            handleProduct(j);
            break;
        }
    }
}

void handleProduct(int number_in_array){
    Serial.println("handle");
    
    if(bought_products != 0){
        Serial.println("bought");
        /*
        for(int k = 0; k < bought_products; k++){
            if(bought_product[k] == serial){
                data_bought[k][1]++;
                product_data[number_in_array][1]--;
                already_bought = true;
                break;
            }
        }
        */
    }else if (bought_products == 0 || !already_bought ){
        bought_product[bought_products] = product_serial[number_in_array];
        int temp = product_data[number_in_array][0];
        data_bought[bought_products][0] = temp;
        data_bought[bought_products][1] = 1;
        product_data[number_in_array][1]--;
        bought_products++;
    }
    calculatePrice();
}

void calculatePrice(){
    for(int m = 0; m < bought_products; m++){
        total += data_bought[m][0]*data_bought[m][1];
    }
    Serial.println("Totaalprijs: €"+String(total));
}

