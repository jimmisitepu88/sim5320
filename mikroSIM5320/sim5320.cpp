/*
    create by: Jimmi Kardo Sitepu
    date: 20-may-2019
    place: Medan
    www.mikroavr.com

*/

#include <Arduino.h>
#include <sim5320.h>

int8_t sentAT(char* ATcommand, char* expected_answer, unsigned int timeout);
void wRespon(unsigned long wkt);
unsigned long nMillis, oMillis;

#if ARDUINO_VERSION <= 106
    void yield(void) {};
#endif

sim5320::sim5320(byte rst,  unsigned int baud){
    begin(rst, baud);
}

void sim5320::begin(byte rst,  unsigned int baud){
    RST = rst;
    BAUD = baud;
    pinMode(RST, OUTPUT);
    serialAT.begin(BAUD);
}

String sim5320::getModemInfo(){
    String txt;
    serialAT.println(F("ATI"));
    while(!serialAT.available()){}
    while(serialAT.available()){
        txt = serialAT.readString();
    }
    return txt;
}

bool sim5320::hasSSL(){
    bool i;
    i = sentAT("AT+CGPSSSL=1","OK", 2000);
    return i;
}


bool sim5320::netReg(){
    bool i, j; 
    byte k, sts;

    delay(2000);
    i = sentAT("AT+CREG?", "+CREG: 0,1", 2000);
    j = sentAT("AT+CFUN?", "+CFUN: 1", 2000);
    if( j != true){
        sentAT("AT+CFUN=1", "OK", 2000);
    }
    k = i + j;
    if ( k == 2){
        sts = true;
    }else{
        sts = false;
    }
    return sts;
    
}

bool sim5320::conAPN(){
    bool i, j;
    delay(2000);
    j = sentAT("AT+CGATT?", "+CGATT: 1", 2000);    
    if (j != true){
        delay(1000);
        sentAT("AT+CGATT=1", "OK", 2000);
    }
    delay(2000);
    i = sentAT("AT+CGSOCKCONT=1,\"IP\",\"Internet\"","OK", 2000);
    return i;
}

bool sim5320::pushData(String host, String url){
   bool sts;
   String answer; char buf[30]; int len = 0; 
   delay(3000);
   serialAT.println(String("AT+CHTTPACT=") + "\"" + host + "\",80");
   delay(500);
   wRespon(3000);
   serialAT.print(String("GET ") + "http://" + host + url + " HTTP/1.1\r\n");
   serialAT.print(String(host) + "\r\n");
   serialAT.print("Connection: close\r\n\r\n");
   serialAT.print(char(0x1A));

   nMillis = millis();
   oMillis = nMillis;
   memset(buf, '\0', 30);
   while ( nMillis - oMillis < 16000){
        nMillis = millis();
        
        while(serialAT.available()){
        serialMON.println(serialAT.readString());
        len++;
        }
    }
    serialMON.println(String("i: ") + len);
        if( len <= 2){
            sts = true;
        }
        if (len > 2){
        sts = false;
        }
    return sts;
}

String sim5320::gTime(){
    String Twkt; int len; char buf[55];
    delay(500);
    sentAT("AT+CTZR=1", "OK", 1000);
    //wRespon(2000);
    delay(500);
    sentAT("AT+CTZU=1", "OK", 1000);
    //wRespon(2000);
    
    delay(1500);
    serialAT.println("AT+CCLK?");
    nMillis = millis();
    oMillis = nMillis;
    while(nMillis - oMillis < 3000){
        nMillis = millis();
        while (serialAT.available())
        {
            Twkt =  serialAT.readString();
        }  
    }
    len = Twkt.length();
    memset(buf, '\0', len);
    Twkt.toCharArray(buf, len);
    Twkt = "";
    for(int j = 21; j<len-11 ; j++){
        Twkt += String(buf[j]);
    }
    Twkt = String("20") + Twkt;
    //serialMON.println(Twkt);
    oMillis = nMillis;
    return Twkt;
}


int8_t sentAT(char* ATcommand, char* expected_answer, unsigned int timeout){
  uint8_t x=0, answer=0;
  char response[100];
  unsigned long prevMillis;
  memset(response, '\0', 100);
  delay(100);

   while(serialAT.available()>0)serialAT.read();
   serialAT.println(ATcommand);

   prevMillis = millis();

   do{
    if(serialAT.available() != 0){
      response[x] = serialAT.read();
      x++;
      if(strstr(response, expected_answer) != NULL){
        answer = 1;
      }
    }
   }while((answer == 0) && ((millis() - prevMillis) < timeout));
   return answer;
}

void sim5320::rstSIM(){
    String answer; char buf[30]; int len;
    serialMON.println(F("rst begin"));
    digitalWrite(RST, LOW);
    delay(3000);
    digitalWrite(RST, HIGH);
    delay(1000);

    while(answer != "PB DONE"){
        nMillis = millis();
        if(nMillis - oMillis > 40000){
            memset(buf, '\0', 30); 
            serialMON.println("rst again");
            digitalWrite(RST, LOW);
            delay(3000);
            digitalWrite(RST, HIGH);
            delay(1000);
            oMillis = nMillis;
        }
            memset(buf, '\0', 30);    
            while(serialAT.available()){
                answer = serialAT.readString();
            }
            serialMON.println(answer);
            len = answer.length();
            answer.toCharArray(buf, len); answer = "";
            if(strstr(buf, "PB DONE")){
                break;
            }
            delay(1000);
    }

    serialMON.println("rst ok");delay(1000);
    //serialAT.println("AT+IPREX=4800");
    wRespon(2000);
    return answer;
}

void sim5320::baudCheck(){
    delay(1000);
    serialAT.println("AT+IPREX=?");
    wRespon(2000);
    delay(1000);
    serialAT.println("AT+IPREX?");
    wRespon(2000);
    serialAT.println("AT+IPR?");
    wRespon(2000);
    
}

void wRespon(unsigned long wkt){
   nMillis = millis();
   oMillis = nMillis;
   while ( nMillis - oMillis < wkt){
    nMillis = millis();
    while(serialAT.available()){
      serialMON.println(serialAT.readString());
    }
 }
}

void sim5320::waiting(unsigned long wkt){
   serialMON.println(F("waiting loop"));
   nMillis = millis();
   oMillis = nMillis;
   while ( nMillis - oMillis < wkt){
    nMillis = millis();
    while(serialAT.available()){
      serialMON.println(serialAT.readString());
    }
 }
}
