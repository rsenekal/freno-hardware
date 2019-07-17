// SENG 499 - Engineering Capstone Project
// Freno, The Smart Break Monitor
// July 17th, 2019
// Arshi, Eli, Shreyas, Adam, Sajan, Rickus

#include <Wire.h>
#include <PololuLedStrip.h>

PololuLedStrip<12> ledStrip;
#define LED_COUNT 40
rgb_color colors[LED_COUNT];
rgb_color color;
bool led_on = false;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
}
void loop(){
  
  
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
      
  delay(100);

  
}
