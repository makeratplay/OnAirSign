#define DEBUG_FAUXMO             Serial
#define DEBUG_FAUXMO_VERBOSE_TCP true
#define DEBUG_FAUXMO_VERBOSE_UDP true

#include <fauxmoESP.h>
#include <ESPmDNS.h>

fauxmoESP fauxmo;


#define REDPIN 21
#define GREENPIN 23
#define BLUEPIN 22
#define FADESPEED 5     // make this higher to slow down

// setting PWM properties
const int freq = 5000;
const int resolution = 8;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";


void setup() {

    // configure LED PWM functionalitites
    ledcSetup(1, freq, resolution);
    ledcSetup(2, freq, resolution);
    ledcSetup(3, freq, resolution);

    // attach the channel to the GPIO to be controlled
    ledcAttachPin(REDPIN, 1);
    ledcAttachPin(GREENPIN, 2);
    ledcAttachPin(BLUEPIN, 3);


    Serial.begin(115200);
    delay(500);

    WiFi.begin(ssid, pass);
    Serial.println("");
  
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println("Setup MDNS for onair");
    if (!MDNS.begin("onair")) {
        Serial.println("Error setting up MDNS responder!");
        while(1){
            delay(1000);
        }
    }

    // setup device names for Amazon Echo
    fauxmo.addDevice("on air");
    fauxmo.addDevice("on air red");
    fauxmo.addDevice("on air green");
    fauxmo.addDevice("on air blue");

    fauxmo.setPort(80); // required for gen3 devices
    fauxmo.enable(true);

    //Handle command from Amazon Echo
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
        uint8_t light = 1;
        String name = (const char *) device_name;

        if ( name.indexOf("red") > 0 )
        {
          light = 1;
        }
        else if ( name.indexOf("green") > 0 )
        {
          light = 2;
        }
        else if ( name.indexOf("blue") > 0 )
        {
          light = 3;
        }

        if ( state ){
          ledcWrite(light, value);
        }
        else
        {
          ledcWrite(light, 0);
        }      
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
    });


    // cycle the lights during bootup
    for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
      // changing the LED brightness with PWM
      ledcWrite(1, dutyCycle);
      delay(15);
    }
    ledcWrite(1, 0);

    for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
      // changing the LED brightness with PWM
      ledcWrite(2, dutyCycle);
      delay(15);
    } 
    ledcWrite(2, 0);

    for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
      // changing the LED brightness with PWM
      ledcWrite(3, dutyCycle);
      delay(15);
    }     
    ledcWrite(3, 0);
}

void loop() {
    fauxmo.handle();  // check for command from Amazon Echo
}
