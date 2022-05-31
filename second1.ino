#include <painlessMesh.h>
#include <ArduinoJson.h>

// mesh credentials has to be same for all!
#define id "AQUABOTICS"
#define pass "Caretta2"
#define port 5555

#define button D3
#define LED D5

int buttonState;

Scheduler myScheduler;
painlessMesh mesh;

void sendCallback();
Task sendTask(TASK_SECOND * 1, TASK_FOREVER, &sendCallback);  


void sendCallback(){
  digitalWrite(LED, HIGH);
  Serial.println();
  Serial.println("Start Sending ...");
  buttonState = digitalRead(button);
  Serial.print("Button State  = "); Serial.println(buttonState);
  /*
   * { // Json format
   *  "ADCValue": adcReading
   * }
   */
   String msg;
   DynamicJsonDocument doc(1024);
   doc["Button"] = buttonState;
   serializeJson(doc, msg);
   mesh.sendBroadcast(msg);
   Serial.println("Message Sent");
   digitalWrite(LED, LOW) ;
}

void receivedCallback(uint32_t from, String msg){
  Serial.println();
  Serial.print("Message = "); Serial.println(msg);
  String json = msg.c_str();
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, json);
  if(error){
    Serial.print("Failed Deserializing ... ");
    Serial.println(error.c_str());
  }
 //LEDState = doc["Button"];
 //digitalWrite(LED, LEDState);
}

void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT);
  pinMode(LED, OUTPUT);

  mesh.init(id, pass, &myScheduler, port); 
  mesh.onReceive(&receivedCallback);
  myScheduler.addTask(sendTask);
  sendTask.enable();
}

void loop() {
  mesh.update();
}
