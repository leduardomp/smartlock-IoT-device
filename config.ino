void resetParameters(){
  DatosConfig datosConfigReset = {0,"",""};
  
  EEPROM.put(0, datosConfigReset);
  EEPROM.commit();
  EEPROM.end();
}

void saveParameters(String ssidA, String passwordA){

  Serial.print("tamaño password:");Serial.println(passwordA.length());
  Serial.print("antes de datosConfig password:");Serial.println(passwordA);
  
  datosConfig.modoConfig = 1; 
  ssidA.toCharArray(datosConfig.ssid, ssidA.length()+1);
  passwordA.toCharArray(datosConfig.password, passwordA.length()+1);

  Serial.print("en datosConfig password:");Serial.println(datosConfig.password);
  
  EEPROM.put(0, datosConfig);
  Serial.println("");
  if (EEPROM.commit()) {
    Serial.println("Data successfully committed");
  } else {
    Serial.println("ERROR! Data commit failed");
  }
  EEPROM.end();
}

void blinkLed(int vueltas){

  int contador = 0;

  while(contador < vueltas){
    
    digitalWrite(LED_BUILTIN, LOW);   
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    
    if(vueltas > 2){
      delay(500);
    }
    
    contador++;
  }
  
}
