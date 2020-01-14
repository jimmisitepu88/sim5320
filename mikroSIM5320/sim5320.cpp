/*
    create by: Jimmi Kardo Sitepu
    date: 20-may-2019
    place: Medan
    www.mikroavr.com

*/

#include <Arduino.h>
#include <sim5320.h>

#define RXD1 16
#define TXD1 17

int8_t sentAT(char* ATcommand, char* expected_answer, unsigned int timeout);
void wRespon(unsigned long wkt);
unsigned long nMillis, oMillis;


sim5320::sim5320(byte rst,  unsigned int baud){
    begin(rst, baud);
}

void sim5320::begin(byte rst,  unsigned int baud){
    RST = rst;
    BAUD = baud;
    pinMode(RST, OUTPUT);
    //serialAT.begin(BAUD);
	serialAT.begin(BAUD, SERIAL_8N1, RXD1, TXD1);
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

bool sim5320::pushHTTP(String host, String url){
   bool sts;
   String answer, p_answer; char buf; int len = 0; 
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
   while ( nMillis - oMillis < 5000){
        nMillis = millis();
            while(serialAT.available()){
            buf = serialAT.read();
            if(buf == '\n'){
                len++;
            }
            if(len == 8){
                answer += String(buf);
            }
        }
    }  
    p_answer = answer.substring(10,13);
    serialMON.print("res: ");
    serialMON.println(p_answer);
    if(p_answer == "200"){
        sts = 1;
    }else{
        sts = 0;
    }
    answer = ""; p_answer = "";
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
    String answer; char buf[100]; int len;
    serialMON.println(F("rst begin"));
    digitalWrite(RST, LOW);
    delay(3000);
    digitalWrite(RST, HIGH);
    delay(1000);
	wRespon(20000);
    serialMON.println("rst ok");delay(1000);    
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
