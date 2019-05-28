# sim5320
library Arduino for internet modul 3G, SIM5320E,
only http function and https function(maybe will devolope),
Support by mikroavr.com

below example

#include <sim5320.h>

sim5320 sim(39, 4800); // rst_pin, baudrate

unsigned long cur_time, old_time;

String host = "example.com";



void setup() {

  // put your setup code here, to run once:

  Serial.begin(115200);

  sim.rstSIM();

  delay(1000);

  String txt = sim.getModemInfo();

  Serial.print("modem: ");

  Serial.println(txt);

  delay(1000);

  sim.hasSSL();delay(1000);

  sim.netReg();delay(1000);      

}

void loop() {

  // put your main code here, to run repeatedly:

  cur_time = millis();

  if ( cur_time - old_time >= 20000){

    delay(1000);

    if(!sim.netReg()){

      Serial.println(F("network fail"));

    }

    Serial.println(F("network ok"));

    delay(1000);

    if(!sim.conAPN()){

      Serial.println(F("no internet"));

    }

    Serial.println(F("internet ok"));

    

    delay(1000);

    String url = "/pln/i.php?temp=30&hum=78\r"; 

    while ( !sim.pushHTTP(host, link )){
        serialMon.println(F("fail to server"));
        sim.waiting(20000);
        sim.pushHTTP(host,link); link = "";
      }
      serialMon.println(F("ok to server"));

  }

}
