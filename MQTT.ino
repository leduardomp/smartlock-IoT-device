void onMqttReceived(char* topic, byte* payload, unsigned int length){

  blinkLed(2);
  
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  DeserializationError error = deserializeJson(json, payload);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }
  
  Serial.println();
  const char* numserie = json["numserie"];
  int accion = json["accion"];

  if(strcmp(NUM_SERIE, numserie) == 0 && accion == 1){
    //Abrir Puerta
    Serial.println("A"); 
    digitalWrite(PUERTA, PRENDER); 
    blinkLed(1);
    digitalWrite(PUERTA, APAGAR);
  }
  
}

void startMQTT(){
  clientMQTT.setServer(IP_SERVER_MQTT, 1883);
  clientMQTT.setCallback(onMqttReceived);
}

void reconnect() {
  
  // Loop until we're reconnected
  while (!clientMQTT.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (clientMQTT.connect(NUM_SERIE, MQTT_USER, MQTT_PASS)) {
      Serial.println("connected");
      clientMQTT.subscribe(CANAL_SUBSCRIPCION);
      
    } else {
      Serial.print("failed, rc=");
      Serial.print(clientMQTT.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
