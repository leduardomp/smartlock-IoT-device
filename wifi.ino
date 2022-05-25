void wifiConfig(String ssid, String password){
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int contador = 0;

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    blinkLed(1);
    delay(500);
    
    contador++;
    if(contador > 10){
      Serial.println("");
      contador = 0;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void serverStationMode(){
  
  WiFi.softAP(NUM_SERIE, "0987654321");  //Contraseñe default Device
  IPAddress myIP = WiFi.softAPIP(); 
  Serial.print("IP del acces point: ");
  Serial.println(myIP);
  Serial.println("WebServer iniciado...");


  serverConfig.on("/", paginaconf); //esta es la pagina de configuracion

  serverConfig.on("/guardar_conf", guardarConf); //Graba en la eeprom la configuracion

  serverConfig.on("/escanear", escanear); //Escanean las redes wifi disponibles
  
  serverConfig.begin();  
}

void paginaconf() {
  serverConfig.send(200, "text/html", pagina + mensaje + paginafin); 
}

void guardarConf() {
  
  Serial.println(serverConfig.arg("ssid"));//Recibimos los valores que envia por GET el formulario web
  Serial.println(serverConfig.arg("pass"));

  saveParameters(serverConfig.arg("ssid"), serverConfig.arg("pass"));

  mensaje = "Configuracion Guardada...";
  paginaconf();

  delay(2000);
  resetFunc();
}

void escanear() {  
  int n = WiFi.scanNetworks(); //devuelve el número de redes encontradas
  Serial.println("escaneo terminado");
  if (n == 0) { //si no encuentra ninguna red
    Serial.println("no se encontraron redes");
    mensaje = "no se encontraron redes";
  }  
  else
  {
    Serial.print(n);
    Serial.println(" redes encontradas");
    mensaje = "";
    for (int i = 0; i < n; ++i)
    {
      // agrega al STRING "mensaje" la información de las redes encontradas 
      mensaje = (mensaje) + "<p>" + String(i + 1) + ": " + WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ") Ch: " + WiFi.channel(i) + " Enc: " + WiFi.encryptionType(i) + " </p>\r\n";
      //WiFi.encryptionType 5:WEP 2:WPA/PSK 4:WPA2/PSK 7:open network 8:WPA/WPA2/PSK
      delay(10);
    }
    Serial.println(mensaje);
    paginaconf();
  }
}
