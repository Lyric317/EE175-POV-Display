#include <IRremote.h>
#include <SoftwareSerial.h>
#include "alphabet.h"
#include "heart.h"

//Pin connected to ST_CP of 74HC595(pin12)
int latchPin = 8;
//Pin connected to SH_CP of 74HC595(pin11)
int clockPin = 12;
////Pin connected to DS of 74HC595(pin14)
int dataPin = 11;

SoftwareSerial softSerial(13,7);

int interruptPin = 2;
IRrecv irrecv(interruptPin);

const long taketime = 1000;
unsigned long time4count;
unsigned long previousTime;
unsigned long modelTime;
unsigned long Time;
float Val = 0;

void setup() {
  //set pins to output because they are addressed in the main loop
  Serial.begin(9600);
  softSerial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(interruptPin),count,FALLING);
  time4count = millis() + taketime;
  previousTime = micros();
  
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

//count the time of interrupt
void count(){      
  Val += 1;
}


void led16(byte high_data,byte low_data){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST,high_data);
  //shiftout the first eight digits
  shiftOut(dataPin, clockPin, MSBFIRST,low_data); 
  //shiftout the last eight digits
  digitalWrite(latchPin, HIGH);
}

//reverse the 16-bit output due to hardware connection
unsigned long BitReverse(unsigned long number){
  unsigned long res = 0;
  for(int i=1; i <= 8; i++){
  res |= (number&1) << (8-i);
  number >>= 1;
  }
  return res;
};

//display the letter
void display(char letter,int det){
  //address the letter in alphabet.h
  int index;
  if(letter <= 'Z'){
    index = letter-'A';
  }
  else if(letter >= 'a' && letter <= 'z'){
    index = letter-'a'+26;
  }
  else{
    index = letter-'0'+52;
  }

  //display the letter in a given time
  for(int i = 0; i < 8; i++){
    int low_data = BitReverse(alphabet[index*16 + i*2]);
    int high_data = BitReverse(alphabet[index*16 + i*2 +1]);
    while(micros() - previousTime < det){
        led16(high_data, low_data);
    }
    previousTime = micros();
  }
}


//diaplay the image
void display_img(int i, int det){
    int low_data = BitReverse(heart[i*2]);
    int high_data = BitReverse(heart[i*2 +1]);
    while(micros() - previousTime < det){
      led16(high_data, low_data);
    }
    previousTime = micros();
}

//check whether the rotational speed is stable
void isStable(){
  //consider it as stable if 6 interrupts are counted in a second
  while(Val < 5){
    led16(0x00,0x00);
    if (millis() >= time4count){
      time4count = millis() + taketime;
      Val = 0;
    }
  }
}

float motor_speed = 50;
float det = 600;

String text;
int i = 0;
int j = 0;
int pre_index = 0;

char model = '0';
//0: text model
//1: image model
unsigned long display_time = 15;

//read data from serial port and extract information from it
void readData(){
  //read data from serial port
  String data = "";
  while(1){
    char input;
    if (softSerial.available()) { 
      input = softSerial.read();
      if(input == '@'){
        break;
      }
      else{
        data += input;
      }
    }
  }
  
  //extract information from it
  //the sent data is in the format: *mode#display_time$<text>@
  for(int index = 0; index<data.length(); index++){
    if(data[index] == '*'){model = data[index+1];}
    if(data[index] == '#'){
      int index_num = index + 1;
      display_time = 0;
      for(index_num;data[index_num] != '$'; index_num++){
        display_time = display_time * 10 + (data[index_num]-'0');
      }
    }
    if(data[index] == '<'){
      int index_text = index + 1;
      text = "";
      for(index_text;data[index_text] != '>'; index_text++){
        text += data[index_text];
      }
    }
  }
}

void loop() {
  
  readData();//read data from serial port

  isStable();//check if stable

  modelTime = millis();

switch(model){

  case '0':{
    //text mode
  while(millis()-modelTime < display_time*1000){
    //display_time is set as second and modelTime is set as millisecond
    
while(text[i] != '&'){
  //the text is end with '&'
  
  for(j = pre_index; (text[j] != ' ') && (text[j] != '&'); j++ );
  //get the index of last letter of the word to display
  
  int len = (j-pre_index);
  Time = millis();

  if (len >= 6){det = 800;}
  else {det = 1100;}
  //different interval for each column is set according to length of word
  
  while(millis()-Time < 5000){
    //display each word for 5 seconds
    
      previousTime = micros();
      while(i != j){
        display(text[i++],det);
      }      
      if (i == j){
        while(micros() - previousTime < (15/len-1)*det*len*8){
          led16(0x00, 0x00);
        }
      }
      i = pre_index;
     }
     
     i = j;
     if(text[i] != '&'){
     pre_index = j + 1;}
     else{break;}
}
i = 0;
j = 0;
pre_index = 0;

  }
  led16(0x00,0x00);
  break;
  }
  
case '1':{
  //image mode
  while(millis() - modelTime < display_time*1000){
    det = 1500;
    previousTime = micros();
    for(int i=0; i<16; i++){
      display_img(i, det);
    }
    while(micros() - previousTime < (15/2-1)*det*16){
      led16(0x00, 0x00);
    }
  }
  led16(0x00,0x00);
  break;
}

}

}
