#include <Wire.h>
#include <PololuLedStrip.h>

PololuLedStrip<12> ledStrip;
#define LED_COUNT 40
rgb_color colors[LED_COUNT];
rgb_color color;
bool led_on = false;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

//USB VALUES
byte computerByte;           //used to store data coming from the computer
byte USB_Byte;               //used to store data coming from the USB stick
int LED = 13;                //the LED is connected to digital pin 13 
int timeOut = 2000;          //TimeOut is 2 seconds. This is the amount of time you wish to wait for a response from the CH376S module.
String wrData = "temp";     //We will write this data to a newly created file.
unsigned long time;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.print("here");

  writeFile("TEST.TXT", "Time, AccelX, AccelY, AccelZ, GyroX, GyroY, GyroZ \r\n");
}

void loop(){
  
  Serial.println("hello");
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  
  AcX = Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcX = (AcX / 16384);   
  AcY = Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcY = (AcY / 16384);
  AcZ = Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  AcZ = (AcZ / 16384);

  // look into how to remove tempurature reading to save power and processor time
  Tmp = Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  
  GyX = Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyX = GyX / 131;
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyY = GyY / 131;
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  GyZ = GyZ / 131;

  // ****** Should be removed from final code ******
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | GyY = "); Serial.print(GyY);
  Serial.print(" | GyZ = "); Serial.println(GyZ);
  // ***********************************************
  
  if(AcX >= 0.3 && AcX < 0.5){
    color.red = 255;
    color.green = 0;
    color.blue = 0;
    // Update the colors buffer.
    for(uint16_t i = 0; i < LED_COUNT/4; i++)
    {
      colors[i] = color;
    }
    ledStrip.write(colors, LED_COUNT/4);
    led_on = true;
  }else if(AcX >= 0.5 && AcX < 0.7){
    
    color.red = 255;
    color.green = 0;
    color.blue = 0;
    // Update the colors buffer.
    for(uint16_t i = 0; i < LED_COUNT/2; i++)
    {
      colors[i] = color;
    }

    ledStrip.write(colors, LED_COUNT/2);
    led_on = true;

  }else if(AcX >= 0.7 && AcX < 0.9){ 
    color.red = 255;
    color.green = 0;
    color.blue = 0;
    // Update the colors buffer.
    for(uint16_t i = 0; i < 30; i++)
    {
      colors[i] = color;
    }

    ledStrip.write(colors, 30);
    led_on = true;
  }else if(AcX >= 0.9){ 
    color.red = 255;
    color.green = 0;
    color.blue = 0;
    // Update the colors buffer.
    for(uint16_t i = 0; i < LED_COUNT; i++)
    {
      colors[i] = color;
    }

    ledStrip.write(colors, LED_COUNT);
    delay(1000); //If there is an emergency break level of Decceleration we want it to stay fully lit for a moment longer?
    led_on = true;
  }else{
    if(led_on){
      color.red = 0;
      color.green = 0;
      color.blue = 0;
      // Update the colors buffer.
      for(uint16_t i = 0; i < LED_COUNT; i++)
      {
        colors[i] = color;
      }
    
      ledStrip.write(colors, LED_COUNT);
      led_on = false;
    }
  }
  //write to USB
  time = millis();  
  wrData = String(time) + ", " + AcX + ", " + AcY + ", " + AcZ + ", " + GyX + ", " + GyY + ", " + GyZ + "\r\n";

  appendFile("TEST.TXT", wrData);
//  appendFile("TEST.TXT", "\r\n");
  
  delay(100);
  
}

//set_USB_Mode=====================================================================================
//Make sure that the USB is inserted when using 0x06 as the value in this specific code sequence
void set_USB_Mode (byte value){
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x15);
  Serial1.write(value);
  
  delay(20);
  
  if(Serial1.available()){
    USB_Byte=Serial1.read();
    
    //Check to see if the command has been successfully transmitted and acknowledged.
    if(USB_Byte==0x51){                                   // If true - the CH376S has acknowledged the command.
        Serial.println("set_USB_Mode command acknowledged"); //The CH376S will now check and monitor the USB port
        USB_Byte = Serial1.read();
        
        //Check to see if the USB stick is connected or not.
        if(USB_Byte==0x15){                               // If true - there is a USB stick connected
          Serial.println("USB is present");
        } else {
          Serial.print("USB Not present. Error code:");   // If the USB is not connected - it should return an Error code = FFH
          Serial.print(USB_Byte, HEX);
          Serial.println("H");
        }
        
    } else {
        Serial.print("CH3765 error!   Error code:");
        Serial.print(USB_Byte, HEX);
        Serial.println("H");
    }   
  }
  delay(20);
}

////resetALL=========================================================================================
////This will perform a hardware reset of the CH376S module - which usually takes about 35 msecs =====
void resetALL(){
    Serial1.write(0x57);
    Serial1.write(0xAB);
    Serial1.write(0x05);
    Serial.println("The CH376S module has been reset !");
    delay(200);
}

////writeFile========================================================================================
////is used to create a new file and then write data to that file. "fileName" is a variable used to hold the name of the file (e.g TEST.TXT). "data" should not be greater than 255 bytes long. 
void writeFile(String fileName, String data){
  resetALL();                     //Reset the module
  set_USB_Mode(0x06);             //Set to USB Mode
  diskConnectionStatus();         //Check that communication with the USB device is possible
  USBdiskMount();                 //Prepare the USB for reading/writing - you need to mount the USB disk for proper read/write operations.
  setFileName(fileName);          //Set File name
  if(fileCreate()){               //Try to create a new file. If file creation is successful
    fileWrite(data);              //write data to the file.
  } else {
    Serial.println("File could not be created, or it already exists");
  }
  fileClose(0x01);
}

//appendFile()====================================================================================
//is used to write data to the end of the file, without erasing the contents of the file.
void appendFile(String fileName, String data){
    resetALL();                     //Reset the module
    set_USB_Mode(0x06);             //Set to USB Mode
    diskConnectionStatus();         //Check that communication with the USB device is possible
    USBdiskMount();                 //Prepare the USB for reading/writing - you need to mount the USB disk for proper read/write operations.
    setFileName(fileName);          //Set File name
    fileOpen();                     //Open the file
    filePointer(false);             //filePointer(false) is to set the pointer at the end of the file.  filePointer(true) will set the pointer to the beginning.
  
    fileWrite(data);                //Write data to the end of the file
  
    fileClose(0x01);                //Close the file using 0x01 - which means to update the size of the file on close. 
}

////setFileName======================================================================================
////This sets the name of the file to work with
void setFileName(String fileName){
  Serial.print("Setting filename to:");
  Serial.println(fileName);
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x2F);
  Serial1.write(0x2F);         // Every filename must have this byte to indicate the start of the file name.
  Serial1.print(fileName);     // "fileName" is a variable that holds the name of the file.  eg. TEST.TXT
  Serial1.write((byte)0x00);   // you need to cast as a byte - otherwise it will not compile.  The null byte indicates the end of the file name.
  delay(20);
}

////diskConnectionStatus================================================================================
////Check the disk connection status
void diskConnectionStatus(){
  Serial.println("Checking USB disk connection status");
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x30);

  if(waitForResponse("Connecting to USB disk")){       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
    if(getResponseFromUSB()==0x14){               //CH376S will send 0x14 if this command was successful
       Serial.println(">Connection to USB OK");
    } else {
      Serial.print(">Connection to USB - FAILED.");
    }
  }
}

////fileOpen========================================================================================
////opens the file for reading or writing
void fileOpen(){
  Serial.println("Opening file.");
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x32);
  if(waitForResponse("file Open")){                 //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
    if(getResponseFromUSB()==0x14){                 //CH376S will send 0x14 if this command was successful  
       Serial.println(">File opened successfully.");
    } else {
      Serial.print(">Failed to open file.");
    }
  }
}

////USBdiskMount========================================================================================
////initialise the USB disk and check that it is ready - this process is required if you want to find the manufacturing information of the USB disk
void USBdiskMount(){
//  Serial.println("Mounting USB disk");
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x31);

  if(waitForResponse("mounting USB disk")){       //wait for a response from the CH376S. If CH376S responds, it will be true. If it times out, it will be false.
    if(getResponseFromUSB()==0x14){               //CH376S will send 0x14 if this command was successful
       Serial.println(">USB Mounted - OK");
    } else {
      Serial.print(">Failed to Mount USB disk.");
    }
  }
}

////fileWrite=======================================================================================
////are the commands used to write to the file
void fileWrite(String data){
//  Serial.println("Writing to file:");
  byte dataLength = (byte) data.length();         // This variable holds the length of the data to be written (in bytes)
  delay(100);
  // This set of commands tells the CH376S module how many bytes to expect from the Arduino.  (defined by the "dataLength" variable)
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x3C);
  Serial1.write((byte) dataLength);
  Serial1.write((byte) 0x00);
  if(waitForResponse("setting data Length")){      // Wait for an acknowledgement from the CH376S module before trying to send data to it
    if(getResponseFromUSB()==0x1E){                // 0x1E indicates that the USB device is in write mode.
      Serial1.write(0x57);
      Serial1.write(0xAB);
      Serial1.write(0x2D);
      Serial1.println(data);                             // write the data to the file
      if(waitForResponse("writing data to file")){   // wait for an acknowledgement from the CH376S module
      }
      Serial.print("Write code (normally FF and 14): ");
      Serial.print(Serial1.read(),HEX);                // code is normally 0xFF
      Serial.print(",");
      Serial1.write(0x57);
      Serial1.write(0xAB);
      Serial1.write(0x3D);                             // This is used to update the file size. Not sure if this is necessary for successful writing.
      if(waitForResponse("updating file size")){   // wait for an acknowledgement from the CH376S module
      }
      Serial.println(Serial1.read(),HEX);              //code is normally 0x14
    }
  }
}

////fileCreate()========================================================================================
////the command sequence to create a file
boolean fileCreate(){
  boolean createdFile = false;
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x34);
  if(waitForResponse("creating file")){       //wait for a response from the CH376S. If file has been created successfully, it will return true.
     if(getResponseFromUSB()==0x14){          //CH376S will send 0x14 if this command was successful
       createdFile=true;
     }
  }
  return(createdFile);
}

////filePointer========================================================================================
////is used to set the file pointer position. true for beginning of file, false for the end of the file.
void filePointer(boolean fileBeginning){
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x39);
  if(fileBeginning){
    Serial1.write((byte)0x00);             //beginning of file
    Serial1.write((byte)0x00);
    Serial1.write((byte)0x00);
    Serial1.write((byte)0x00);
  } else {
    Serial1.write((byte)0xFF);             //end of file
    Serial1.write((byte)0xFF);
    Serial1.write((byte)0xFF);
    Serial1.write((byte)0xFF);
  }
  if(waitForResponse("setting file pointer")){       //wait for a response from the CH376S. 
     if(getResponseFromUSB()==0x14){                 //CH376S will send 0x14 if this command was successful
       Serial.println("Pointer successfully applied");
     }
  }
}

////fileClose=======================================================================================
////closes the file
void fileClose(byte closeCmd){
  Serial.println("Closing file:");
  Serial1.write(0x57);
  Serial1.write(0xAB);
  Serial1.write(0x36);
  Serial1.write((byte)closeCmd);                                // closeCmd = 0x00 = close without updating file Size, 0x01 = close and update file Size

  if(waitForResponse("closing file")){                      // wait for a response from the CH376S. 
     byte resp = getResponseFromUSB();
     if(resp==0x14){                                        // CH376S will send 0x14 if this command was successful
       Serial.println(">File closed successfully.");
     } else {
       Serial.print(">Failed to close file. Error code:");
       Serial.println(resp, HEX);
     }  
  }
}

//waitForResponse===================================================================================
//is used to wait for a response from USB. Returns true when bytes become available, false if it times out.
boolean waitForResponse(String errorMsg){
  boolean bytesAvailable = true;
  int counter=0;
  while(!Serial1.available()){     //wait for CH376S to verify command
    delay(1);
    counter++;
    if(counter>timeOut){
      Serial.print("TimeOut waiting for response: Error while: ");
      Serial.println(errorMsg);
      bytesAvailable = false;
      break;
    }
  }
  delay(1);
  return(bytesAvailable);
}

//getResponseFromUSB================================================================================
//is used to get any error codes or messages from the CH376S module (in response to certain commands)
byte getResponseFromUSB(){
  byte response = byte(0x00);
  if (Serial1.available()){
    response = Serial1.read();
  }
  return(response);
}


