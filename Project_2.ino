/* 
 * Steenbrink
 * 27-09-2017
 * "Automated check-out project"
 */

//include libraries
#include <RFID.h>
#include <SPI.h>
#include <Wire.h>  
#include <LiquidCrystal_I2C.h> // Using version 1.2.1

//register lcd panel
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

//define pins, these are hardwired so no point using variables
#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 2
#define BUTTON_PIN 3

//the set amount of different items available
#define PRODUCT_AMOUNT 3

//register the rfid reader
RFID rfid(SS_PIN,RST_PIN);

//register variables
unsigned char product_serial[PRODUCT_AMOUNT][5] = { {101, 182, 118, 8, 173}, {132, 241, 131, 91, 173}, {227, 179, 198, 250, 108} };    //the database with all the serialcodes of the tags
int product_data[PRODUCT_AMOUNT][2] = { {100, 50}, {50, 20}, {25, 3} };   //the price(€ct) and amount of products corresponding to above array
String product_names[PRODUCT_AMOUNT] = {"JAS    ", "BROEK  ", "T-SHIRT"};   //the names of the products

unsigned char bought_product[PRODUCT_AMOUNT][5];     //all the serialcodes of the bought products, uses exaclty 100 bytes
int data_bought[PRODUCT_AMOUNT][2];   //the price(€ct) and amount of products bought corresponding to the above array, 160 bytes
int bought_products = 0;    //the amount of bought products

int total = 0;    //the total price
boolean already_bought = false;   //check for handleProduct
boolean recognized = false;   //check for checkSerial

void setup() {
    //start the serial monitor
    Serial.begin(115200);

    //init libraries for reader
    SPI.begin();
    rfid.init();
    
    // register lcd, sixteen characters across 2 lines
    lcd.begin(16,2); 
    lcd.backlight();

    //register pins
    pinMode(BUTTON_PIN, INPUT);
    pinMode(SERVO_PIN, OUTPUT);

    //set pin to low output
    digitalWrite(SERVO_PIN, LOW);

    // Print the welcome text on the LCD screen
    lcd.setCursor(0,0);
    lcd.print("WELCOME");
    lcd.setCursor(0,1);
    lcd.print("SCAN PRODUCT");
}

void loop() {
    //checks for a card detection
    if(rfid.isCard()){
        //checks if the card is readable
        if(rfid.readCardSerial()){
            Serial.println("Card registered");
            checkSerial();

            //set the text on the lcd
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("TOTAAL $" + String(total) + ",-");
            lcd.setCursor(0,1);
            lcd.print("SCAN PRODUCT");
        }
    }
    //checks if the payment button is pressed
    if(digitalRead(BUTTON_PIN)){
        checkOut();
    }
}

/*
 * Checks the read serial and compares it agains the database
 * loops through the read serial
 * compares the read 5 bytes to the database
 * if the read serial exists in the database execute handleProduct
 */
void checkSerial(){
    for(int j = 0; j < PRODUCT_AMOUNT; j++){
        if(memcmp(product_serial[j], rfid.serNum, 5) == 0){
            handleProduct(j);
            recognized = true;
            break;
        }
    }
    if(!recognized){
        Serial.println("Error 404: Product not recognized");
    }
    recognized = false;
}

/*
 * Handles the registration of the scanned product
 * When there are bought products loops trough them and compares them to the read serial
 * if the bought product exists add one to the amount
 * if not or no bought products exist write a new one to the array
 * then execute calculatePrice & magnetRead
 */
void handleProduct(int number_in_array){
    already_bought = false;
    
    if(bought_products != 0){
        for(int k = 0; k < bought_products; k++){
            if(memcmp(bought_product[k], rfid.serNum, 5) == 0){
                data_bought[k][1]++;
                product_data[number_in_array][1]--;
                already_bought = true;

                //print the scanned product on the lcd
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print(product_names[k] + " x " + String(data_bought[k][1]));
                break;
            }
        }
    }
    if (bought_products == 0 || !already_bought){
        memcpy(bought_product[bought_products], product_serial[number_in_array], 5);
        data_bought[bought_products][0] = product_data[bought_products][0];
        data_bought[bought_products][1] = 1;
        product_data[number_in_array][1]--;

        //print the scanned product on the lcd
        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(product_names[bought_products] + " x " + String(data_bought[bought_products][1]));
        bought_products++;
    }
    calculatePrice();
    magnetRead();
}

/*
 * loops though the bought products and calculate the total price based on amount and price per product bought
 */
void calculatePrice(){
    total = 0;
    for(int m = 0; m < bought_products; m++){
        total += data_bought[m][0]*data_bought[m][1];
    }

    //print the total price to the lcd
    Serial.println("TOTAAL: $"+String(total));
    lcd.setCursor(0,0);
    lcd.print("TOTAAL $"+String(total) + ",-");
}

/*
 * sets the power to the button we use as replacement
 * wait for the button to be pressed and released
 */
void magnetRead(){
    lcd.setCursor(0,1);
    lcd.print("REMOVE SAFETYPIN");
    while(true){
        if(digitalRead(BUTTON_PIN)){
            while(digitalRead(BUTTON_PIN)){Serial.println("pressed");}
            break;
        }
    }
}

/*
 * handles everything that happends when the payment button is pressed
 * 1. checks if the payment button is pressed and released to simulate payment
 * 2. loops trough the bought products and prints a receipt to the console
 * 3. open the door to let the customer trough
 * 4. reset the system for the next customer
 */

void checkOut(){
    //set the checkout text on the lcd
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TOTAAL $"+String(total) + ",-");
    lcd.setCursor(0,1);
    lcd.print("PLEASE PAY");

    //check to see if the button is released
    while(digitalRead(BUTTON_PIN)){Serial.println("pressed");}

    //handle payment
    while(true){
        if(digitalRead(BUTTON_PIN)){
            while(digitalRead(BUTTON_PIN)){Serial.println("pressed");}
            Serial.println("Payment Succes");
            lcd.setCursor(0,1);
            lcd.print("PAYMENT SUCCES!");
            break;
        }
    }
    
    //print receipt
    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~");
    for(int n = 0; n < bought_products; n++){
        if(data_bought[n][0] == 100){
            Serial.println(product_names[n] + " x " + data_bought[n][1] + " : $" + data_bought[n][0] + " = $" + (data_bought[n][0]*data_bought[n][1]));
        }else{
            Serial.println(product_names[n] + " x " + data_bought[n][1] + " : $" + data_bought[n][0] + "  = $" + (data_bought[n][0]*data_bought[n][1]));
        }
    }
    Serial.println("TOTAAL = $"+String(total));
    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~");
    
    //open door
    digitalWrite(SERVO_PIN, HIGH);
    delay(5000);
    digitalWrite(SERVO_PIN, LOW);
    
    //reset system
    total = 0;
    bought_products = 0;
    for(int o = 0; o < PRODUCT_AMOUNT; o++){
        for(int p = 0; p < 5; p++){
            bought_product[o][p] = 0;
        }
        data_bought[o][0] = 0;
        data_bought[o][1] = 0;
    }
}

