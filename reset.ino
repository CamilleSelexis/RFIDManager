/*void reset(void) {
  //This functions acts like a reset, it stops everything and restarts it
  Wire.end();
  delay(500);
  
  Serial.println("Goodbye");
  Wire.begin();//Start the I2C communications;
  int nDevices;
  nRFID = 0;
  cc = 0;
  pcf = false;
  rfid = false;
  nDevices = scan_i2c(addresses);
  Serial.print("nDevices = ");Serial.println(nDevices);
  //MFRC522_I2C mfrc522[nDevices] = {0}; //Create the array to contain the instances of MFRC522
  for(int i = 0; i<nDevices; i++){ //Create instances of pcf and rfid depending on what was found on i2c bus
    if(addresses[i] == adr_pcf){
      Serial.println("PCF8575 connected to the I2C Bus");
      Serial.println("Initialize the pcf8575");
      init_pcf8575(adr_pcf); //init the pcf8575 with its adr
      pcf = true;
    }
    else if(addresses[i] == 0x24){
      Serial.println("0x24 adress used");
    }
    else{
      Serial.print("MFRC522 detected at adr ");Serial.println(addresses[i]);
      mfrc522[cc] = MFRC522_I2C(addresses[i],RST_PIN);
      mfrc522[cc].PCD_Init();
      ShowReaderDetails(cc);
      nRFID++;
      cc++;
      rfid  = true;
    }
  }
  //----------------------ETHERNET INITIALIZATION------------------------------------------------
  Serial.println("Initialize the ethernet connection");
  //Ethernet setup
  Ethernet.begin(mac,ip);  //Start the Ethernet coms

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1500); // do nothing, no point running without Ethernet hardware
      Serial.println("No ethernet shield connected");
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }


  // Start the server
  server.begin();           //"server" is the name of the object for comunication through ethernet
  Serial.print("Ethernet server connected. Server is at ");
  Serial.println(Ethernet.localIP());         //Gives the local IP through serial com
  digitalWrite(LEDB,LON);
}*/
