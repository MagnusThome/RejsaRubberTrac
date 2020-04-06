// RANDOM DUMMY DATA

void dummyloop(void) {

  randomSeed(2);          // The exact same sequence of semi random data will playback if this number isn't changed
                           // You can change it to get other semi random sequences
                           
  int8_t stepchange = 10;
  unsigned long dtimer = 0;
  unsigned long ttimer = 0;
  unsigned long ctimer = 0;
  unsigned long cdelay = 9000;
  unsigned long btimer = 0;

  datapackOne.distance = 150;
  datapackTwo.voltage = 4220;
  for(uint8_t i=0;i<8;i++){
    datapackOne.temps[i] = 600;
    datapackTwo.temps[i] = 600;
  }
  

  
  // ENDLESS LOOP
  while(1) {

    unsigned long now = millis();


    // - - D I S T A N C E - -
    if (now - dtimer >= 870) {
      dtimer = now;
      datapackOne.distance = random(130,190);
    }
    else {
      datapackOne.distance = datapackOne.distance + random(-20,21);
    }
    
    
    
    // - - T E M P S - -
    if (now - ttimer >= 3200) {
      ttimer = now;
      for(uint8_t i=0;i<8;i++){
        datapackOne.temps[i] += stepchange + random(-5,6);
        datapackTwo.temps[i] += stepchange + random(-5,6);
        if (datapackOne.temps[i] > 1200) {
          datapackOne.temps[i] = 1140;
        }
        if (datapackTwo.temps[i] > 1200) {
          datapackTwo.temps[i] = 1140;
        }
        if (datapackOne.temps[i] < 600) {
          datapackOne.temps[i] = 640;
        }
        if (datapackTwo.temps[i] < 600) {
          datapackTwo.temps[i] = 640;
        }
        datapackThr.temps[i] = (int16_t) (datapackOne.temps[i] + datapackTwo.temps[i]) / 2;    // Mean value of even numbered and uneven numbered sensor values => all 16 temp spots averaged together in pairs of two and two into 8 temp values (degrees Celsius x 10)
      }
    }
    if (now - ctimer >= cdelay) {
      ctimer = now;
      cdelay = 1500 + random(2500);
      stepchange = random(-40,41);
    }
  

  
    // - - B A T T E R Y - -
    static unsigned long btimer = 60000;  // check every 60 seconds
    if (now - btimer >= 60000) {
      btimer = now;
      datapackTwo.voltage--;
      datapackTwo.charge = lipoPercent(datapackTwo.voltage);
    }
  
    
    if ( Bluefruit.connected() ) {
      GATTone.notify(&datapackOne, sizeof(datapackOne));
      GATTtwo.notify(&datapackTwo, sizeof(datapackTwo));
      GATTthr.notify(&datapackThr, sizeof(datapackThr));
    }


    blinkOnTempChange(datapackOne.temps[4]);    // Use one single temp in the middle of the array
    blinkOnDistChange(datapackOne.distance);
     
    printStatus();
    delay(80);
    
  }
}
