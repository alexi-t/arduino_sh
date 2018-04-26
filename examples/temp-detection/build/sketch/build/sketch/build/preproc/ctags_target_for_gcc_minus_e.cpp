# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino"
# 1 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino"



# 5 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino" 2
# 6 "c:\\Users\\cankr\\Documents\\Arduino\\sh\\examples\\temp-detection\\app.ino" 2





DHT dht(D11, 11);

void setup()
{
  Serial.begin(115200);
  pinMode(D11, 0x00);

  dht.begin();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.printf("H: %f\r\n", h);
  Serial.printf("T: %f\r\n", t);

  delay(1000);
}
