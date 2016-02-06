// This #include statement was automatically added by the Particle IDE.
#include "NCD4Relay.h"
#include "SI7020_A20.h"
#include "Si7020.h"
#include "BlynkSimpleParticle.h"
//#include <SimpleTimer.h>

NCD4Relay relayController;
SI7020_A20 tempController;  //  Instance of tempController
Si7020 sensor;  // Humidity sensor Instance
//SimpleTimer timer; // Instance of timer
SYSTEM_MODE(AUTOMATIC);

int triggerRelay(String command);
bool tripped[4];
int debugTrips[4];
int minTrips = 5;
double curtemp = 0;  // tempcontroller

char auth[] = "023932fd10d14565ace851287487933a";  // Blynk authorization code
int number;

WidgetLED led1(V3); // setup virtual pins to widget led #
WidgetLED led2(V4);
WidgetLED led3(V5); // setup virtual pins to widget led #
WidgetLED led4(V6);


/* This function is called once at start up ----------------------------------*/
void setup()
{
    Particle.function("controlRelay", triggerRelay);
    Serial.begin(9600);
//    delay(5000);

    Blynk.begin(auth);
    while (Blynk.connect() == false) {
  // Wait until connected
}


    sensor.begin();
    relayController.setAddress(0,0,0);
  // To change resolution of the sensor use
  // sensor.changeResolution(int i) where i=[0-3],
  //sensor.changeResolution(0);
  Spark.variable("number", &number, INT);// Publish for dashboard
//  timer.setInterval(1000L, getTempHum); // call timer and other func
//    timer.setInterval(200L, fadeLedWidget);
}

/* This function loops forever --------------------------------------------*/
void loop()
{
//      delay(500);

    Blynk.run();  // Run Blynk for Android interface
//    timer.run();
//    getTempHum::getTempHum;
    delay(1000);
//

//void getTempHum()
//
/*  Added tempurature coding below        -------------------------------------*/
    if(curtemp = tempController.temperatureF()){

      Serial.print("F Temp: ");  //  Tempurature
      Serial.println(curtemp);  //  Tempurature test
//      Blynk.virtualWrite(V1, curtemp);

       if (curtemp < 78.0){
        relayController.turnOnRelay(1);
        relayController.turnOffRelay(3);
//        Blynk.virtualWrite(D6, HIGH);
//          led1.setValue(249);
//          led2.setValue(0);
          led1.on();
          led3.off();
      }else{
        relayController.turnOnRelay(3);
        relayController.turnOffRelay(1);
//          led1.setValue(0);
//        led2.setValue(249);
         led1.off();
         led3.on();
      }
    }
    Serial.println("before");
    Blynk.virtualWrite(V1, curtemp);
    Serial.println("after");

/*    int strng_tmp = (curtemp - (int)curtemp) * 100;
int curtemp;
Spark.variable("number", &number, INT);
String data = String(number);
    Spark.publish("event",data);
std::string str = to_string(curtemp);  */
number = curtemp;
String data = String(number);
    Particle.publish("steve",data);
//    Blynk.virtualWrite(7, data);
//    BLYNK_WRITE(7)

//delay(7000);

/* Si2070  added for Humidy                                                 */
// Measure RH
  float rh = sensor.getRH();
  // Measure Temperature
  float t = sensor.getTemp();

  // Temperature is measured every time RH is requested.
  // It is faster, therefore, to read it from previous RH
  // measurement instead with readTemp()
  // float t = sensor.readTemp();

  // Print the result in the JSON format
  Serial.print("{\"rh\":\"");
  Serial.print(rh,1);
  Serial.print("\",\"temp\":\"");
  Serial.print(t,2);
  Serial.println("\"}");
Blynk.virtualWrite(V2, rh);


/*  End of Si7020 add for Humidity                                          */

//delay(7000);
/*
    int status = relayController.readAllInputs();
    int a = 0;
    for(int i = 1; i < 9; i*=2){
        if(status & i){
            debugTrips[a]++;
            if(debugTrips[a] >= minTrips){
                if(!tripped[a]){
                    tripped[a] = true;
                    //set input trip event to true
                    String eventName = "Input_";
                    eventName+=(a+1);
                    Particle.publish(eventName, "ON");
                    Serial.print("eventName: ");
                    Serial.println(eventName);
                    Serial.print("eventContents: ");
                    Serial.println("ON");
                }
            }
        }else{
            debugTrips[a] = 0;
            if(tripped[a]){
                tripped[a] = false;
                //set input trip event to false
                String eventName = "Input_";
                eventName+=(a+1);
                Particle.publish(eventName, "OFF");
                Serial.print("eventName: ");
                Serial.println(eventName);
                Serial.print("eventContents: ");
                Serial.println("OFF");
            }
        }
        a++;
    }
    */
}

int triggerRelay(String command){
    if(command.equalsIgnoreCase("turnonallrelays")){
        relayController.turnOnAllRelays();
        return 1;
    }
    if(command.equalsIgnoreCase("turnoffallrelays")){
        relayController.turnOffAllRelays();
        return 1;
    }
    if(command.startsWith("setBankStatus:")){
        int status = command.substring(14).toInt();
        if(status < 0 || status > 255){
            return 0;
        }
        Serial.print("Setting bank status to: ");
        Serial.println(status);
        relayController.setBankStatus(status);
        Serial.println("done");
        return 1;
    }
    //Relay Specific Command
    int relayNumber = command.substring(0,1).toInt();
    Serial.print("relayNumber: ");
    Serial.println(relayNumber);
    String relayCommand = command.substring(1);
    Serial.print("relayCommand:");
    Serial.print(relayCommand);
    Serial.println(".");
    if(relayCommand.equalsIgnoreCase("on")){
        Serial.println("Turning on relay");
        relayController.turnOnRelay(relayNumber);
        Serial.println("returning");
        return 1;
    }
    if(relayCommand.equalsIgnoreCase("off")){
        relayController.turnOffRelay(relayNumber);
        return 1;
    }
    if(relayCommand.equalsIgnoreCase("toggle")){
        relayController.toggleRelay(relayNumber);
        return 1;
    }
    if(relayCommand.equalsIgnoreCase("momentary")){
        relayController.turnOnRelay(relayNumber);
        delay(300);
        relayController.turnOffRelay(relayNumber);
        return 1;
    }
    return 0;
}
