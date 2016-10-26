  #include <EPROM24LC256.h>
  #include <board.h>
  #include <command.h>
  #include <PCA9633.h>
  #include <Adafruit_CAP1188.h>
  #include <BC127.h>
  #include <MSGEQ7.h>
  #include <SoftwareSerial.h>
  #include <Wire.h>
  #include <SPI.h>


  #define DEBUG_PORT_BAUD 57600
  #define BLE_ENABLING_FREQ  HZ_1000
  
  
  byte butColor[3];
  byte c[]={0,0,0,0};

void setup()
{
  
  /* Open Debug serial port */
  Serial.begin(DEBUG_PORT_BAUD);
  Serial.println("DEBUG START !");
  
  /* Init board */
  boardInit();
  
  /* Start BT127 module */
  BC127_ComStart();
  
  
  /* Turn BC127 in ble mode */
  //if(!BC127_BleON()) Serial.println("BC127: ok");
  
  /* Reset command Buffer */
  resetBuffer(&sysBuffer);
   
}
int i=0;
int med;


void loop() // run over and over
{
  
  /* Check button change */
  buttonMonitor();
 
  /* System macchine state*/
  switch (sysReg.state)
  {
    case SL_STARTUP:
       
       butColor[0]=255;
       butColor[1]=0;
       butColor[2]=0;
       setButtonColor(butColor);
       sysReg.state=SL_OFF;
    
       break;
       
    case SL_OFF:
       
       if(button.ntap)//era button.nsltap
       {
         button.nsltap=0;
         butColor[0]=0;
         butColor[1]=0;
         butColor[2]=255;
         setButtonColor(butColor);
         sysReg.state=SL_DISCONNECTED;
       }
    
       break;
    
    case SL_DISCONNECTED:
      while(!BC127_sendCmd("STATUS", 6, "CONNECTED A2DP", 14))
      {
         delay(500);
      }
      Serial.println("Connected");
      sysReg.state = SL_CONNECTED;
      
      /* Enable ble */
      
      delay(5000);
      //if(!BC127_BleON()) Serial.println("BC127: ok");
      //delay(5000);
      
      Serial.print(BC127_sendCmd("ADVERTISING ON", 14, "OK", 2));
      //Serial.print(BC127_sendCmd("SET ENABLE_ANDROID_BLE=ON", 25, "OK", 2));
      break;
    
    case SL_CONNECTED:
    /**
     * Check touch command 
     */
     
      switch(button.ntap)
      {
        case 1:
          BC127_sendCmd("MUSIC PLAY", 10, "OK", 2);
	  Serial.println("Play");
        break;
        case 2:
          BC127_sendCmd("MUSIC FORWARD", 13, "OK", 2);
	  Serial.println("Next");
        break;
        case 3:
          BC127_sendCmd("MUSIC BACKWARD", 14, "OK", 2);
	  Serial.println("Previus");
        break;
        
      }
      button.ntap=0;
      switch (button.nltap)
      {
        case 1:
	  Serial.print(BC127_sendCmd("VOLUME UP", 9, "OK", 2));
	  Serial.println("Volume UP");
        break;
        case 2:
          Serial.print(BC127_sendCmd("VOLUME DOWN", 11, "OK", 2));
	  Serial.println("Volume DOWN");
        break;
      }
      button.nltap=0;
      switch (button.nsltap)
      {
        case 1:
	  Serial.print(BC127_sendCmd("VOLUME DOWN", 11, "OK", 2));
	  Serial.println("Volume DOWN");
        break;  
      }
      button.nsltap=0;
    
    /*end button changes*/
    
    /* Check if there is new command */
    if(CheckNewCommand(&sysBuffer,BC127_GetCom())==COMMAND_NEW)
    { 
        printRecivedCommand(&sysBuffer);
    }
  
//    if((sysReg.bleState==SYSTEM_BLE_OFF)&&(msgeq7.checkBleEnable(BLE_ENABLING_FREQ)))
//    {
//     /* Enable ble function*/
//     Serial.print("abilita ble"); 
//    }

    /* Call speaker manager */
    speakerManager();
    
    /* Process command queue*/
    commandProcessor(&sysBuffer);

    /* Check petals action */
    msgeq7.poll();
    petalsMonitor();
    
    /* Call animation manager */
    animationManager(&sysBuffer);
    
    
    break;
  }
  

}


void showValues()
{
  Serial.print("Current values - ");
  
  Serial.print("63HZ: ");
  Serial.print(msgeq7.getValue(0));
  
  Serial.print(" 160HZ: ");
  Serial.print(msgeq7.getValue(1));

  Serial.print(" 400HZ: ");
  Serial.print(msgeq7.getValue(2));

  Serial.print(" 1000HZ: ");
  Serial.print(msgeq7.getValue(3));

  Serial.print(" 4500HZ: ");
  Serial.print(msgeq7.getValue(4));

  Serial.print(" 6250HZ: ");
  Serial.print(msgeq7.getValue(5));

  Serial.print(" 16000HZ: ");
  Serial.print(msgeq7.getValue(6));

  Serial.println(); 
}
