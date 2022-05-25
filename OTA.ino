void startOTA(const char* HOST_NAME){
 
  ArduinoOTA.setHostname(HOST_NAME);
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  
  ArduinoOTA.begin();
  Serial.println("OTA listo!!!!");
}

void startHTTP(const char* HOST_NAME){

  MDNS.begin(HOST_NAME);
  
  httpUpdater.setup(&httpServerOTA);
  httpServerOTA.on("/", []() {

    if (!httpServerOTA.authenticate(usernameWEB, passwordWEB)){
        return httpServerOTA.requestAuthentication();
    }
    httpServerOTA.send(200, "text/html", "<html><body><a href='/update'><button>Update</button></a></body></html>");
  });
  
  httpServerOTA.begin();
  MDNS.addService("http", "tcp", 8080);
  
  Serial.printf("HTTPUpdateServer listo! abrir http://%s.local/ \n", HOST_NAME);
}


void autoUpdate(){

  Serial.println(" ******** Buscando actualizacion automatica ******** ");
  
  t_httpUpdate_return ret = ESPhttpUpdate.update(clientOTA, URL_UPDATE, "1.0");
 
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
  
}
