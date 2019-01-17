// RANDOM DUMMY DATA

void dummyloop(void) {

  randomSeed(100);
  int8_t stepchange = 3;
  unsigned long dtimer = 0;
  unsigned long ttimer = 0;
  unsigned long ctimer = 0;
  unsigned long btimer = 0;

  datapackOne.distance = 150;
  datapackTwo.voltage = 4220;
  for(uint8_t i=0;i<8;i++){
    datapackOne.temps[i] = 70;
    datapackTwo.temps[i] = 70;
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
    if (now - ttimer >= 320) {
      ttimer = now;
      for(uint8_t i=0;i<8;i++){
        datapackOne.temps[i] += stepchange + random(-1,2);
        datapackTwo.temps[i] += stepchange + random(-1,2);
        if (datapackOne.temps[i] > 130) {
          datapackOne.temps[i] = 120;
        }
        if (datapackTwo.temps[i] > 130) {
          datapackTwo.temps[i] = 120;
        }
        if (datapackOne.temps[i] < 60) {
          datapackOne.temps[i] = 64;
        }
        if (datapackTwo.temps[i] < 60) {
          datapackTwo.temps[i] = 64;
        }
      }
    }
    if (now - ctimer >= 1500 + random(2500)) {
      ctimer = now;
      stepchange = random(-5,5);
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
    }


    
    printStatus();
    delay(80);
    
  }
}
