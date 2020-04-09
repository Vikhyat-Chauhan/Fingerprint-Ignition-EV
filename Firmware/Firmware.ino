#include "FPS_GT511C3.h"
#include "SoftwareSerial.h"

FPS_GT511C3 fps(4, 5); // (Arduino SS_RX = pin 4, Arduino SS_TX = pin 5)

int finger_counter = 0, relay_1 = 0, relay_2 = 0, relay_3 = 0; // 0: turn car off 1:turn car on

int relay_pin1 =8, relay_pin2 = 9, relay_pin3 = 10, relay_pin4 = 11;
int led_blue = 2,led_green = 3, led_red = 12;

String command = "";

void setup()
{
	Serial.begin(115200); //set up Arduino's hardware serial UART
  pinMode(relay_pin1,OUTPUT);
  pinMode(relay_pin2,OUTPUT);
  pinMode(relay_pin3,OUTPUT);
  pinMode(relay_pin4,OUTPUT);
  pinMode(led_blue,OUTPUT);
  pinMode(led_green,OUTPUT);
  pinMode(led_red,OUTPUT);
  digitalWrite(relay_pin1,LOW);
  digitalWrite(relay_pin2,LOW);
  digitalWrite(relay_pin3,LOW);
  digitalWrite(relay_pin4,LOW);
  digitalWrite(led_blue,HIGH);
  digitalWrite(led_green,LOW);
  digitalWrite(led_red,LOW);
	delay(100);
	fps.Open();         //send serial command to initialize fps
	fps.SetLED(true);   //turn on LED so fps can see fingerprint
}

void loop()
{
  initilizer();
}

void initilizer(){
  Fingercheck();
  bluetooth();
  digitalWrite(relay_pin4, finger_counter);
  digitalWrite(relay_pin1, relay_1);
  digitalWrite(relay_pin2, relay_2);
  digitalWrite(relay_pin3, relay_3);
  digitalWrite(led_blue,HIGH);
  digitalWrite(led_green,LOW);
  digitalWrite(led_red,LOW);
}

void bluetooth(){
  if(Serial.available()>0){
    char c = Serial.read();
    if(c == '.'){
      if(command == "add"){
        Enroll();
      }
      if(command == "start"){
        if(finger_counter == 0){
          finger_counter = 1;
      }
      else{
          finger_counter = 0;
      }
      }
      if(command == "one"){
        if(relay_1 == 0){
          relay_1 = 1;
      }
      else{
          relay_1  = 0;
      }
      }
      if(command == "two"){
        if(relay_2 == 0){
          relay_2 = 1;
      }
      else{
          relay_2  = 0;
      }
      }
      if(command == "three"){
        if(relay_3 == 0){
          relay_3 = 1;
      }
      else{
          relay_3  = 0;
      }
      }
      command = "";
      break;
    }
    command+=c;
  }
}
void Fingercheck(){
  // Identify fingerprint test
  if (fps.IsPressFinger())
  {
    digitalWrite(led_blue,HIGH);
    digitalWrite(led_green,HIGH);
    digitalWrite(led_red,HIGH);
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    if (id <200) //<- change id value depending model you are using
    {//if the fingerprint matches, provide the matching template ID
      digitalWrite(led_blue,LOW);
      digitalWrite(led_green,HIGH);
      digitalWrite(led_red,LOW);
      Serial.print("Verified ID:");
      Serial.println(id);
      if(finger_counter == 0){
        finger_counter = 1;
      }
      else{
        finger_counter = 0;
      }
    }
    else
    {//if unable to recognize
      digitalWrite(led_blue,LOW);
      digitalWrite(led_green,LOW);
      digitalWrite(led_red,HIGH);
      Serial.println("Finger not found");
    }
  }
  else{
    //Serial.println("Press Finger");
  }
  delay(1000);
}

void Enroll()
{
  // Enroll test

  // find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid==true) enrollid++;
  }
  fps.EnrollStart(enrollid);

  // enroll
  digitalWrite(led_blue,LOW);
  digitalWrite(led_green,HIGH);
  digitalWrite(led_red,LOW);
  Serial.print("Press finger to Enroll #");
  Serial.println(enrollid);
  while(fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    digitalWrite(led_blue,LOW);
    digitalWrite(led_green,LOW);
    digitalWrite(led_red,HIGH);
    Serial.println("Remove finger");
    fps.Enroll1(); 
    while(fps.IsPressFinger() == true) delay(100);
    digitalWrite(led_blue,LOW);
    digitalWrite(led_green,HIGH);
    digitalWrite(led_red,LOW);
    Serial.println("Press same finger again");
    while(fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      digitalWrite(led_blue,LOW);
      digitalWrite(led_green,LOW);
      digitalWrite(led_red,HIGH);
      Serial.println("Remove finger");
      fps.Enroll2();
      while(fps.IsPressFinger() == true) delay(100);
      digitalWrite(led_blue,LOW);
      digitalWrite(led_green,HIGH);
      digitalWrite(led_red,LOW);
      Serial.println("Press same finger yet again");
      while(fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
       digitalWrite(led_blue,LOW);
       digitalWrite(led_green,LOW);
       digitalWrite(led_red,HIGH);
        Serial.println("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling Successful");
          digitalWrite(led_blue,HIGH);
          digitalWrite(led_green,HIGH);
          digitalWrite(led_red,HIGH);delay(1000);
        }
        else
        {
          Serial.print("Enrolling Failed with error code:");
          Serial.println(iret);
        }
      }
      else Serial.println("Failed to capture third finger");
    }
    else Serial.println("Failed to capture second finger");
  }
  else Serial.println("Failed to capture first finger");
}

