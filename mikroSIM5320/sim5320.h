
/*
    create by: Jimmi Kardo Sitepu
    date: 20-may-2019
    place: Medan
    www.mikroavr.com
*/


#ifndef sim5320_h
#define sim5320_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define serialAT Serial3
#define serialMON Serial

class sim5320
{

    private:
        byte RST;
        unsigned int BAUD;

    public:
        sim5320(byte rst, unsigned int baud);
        void begin(byte rst, unsigned int baud);
        void rstSIM();
        String getModemInfo();
        bool hasSSL();
        bool netReg();
        bool conAPN();
        void pushData(String host, String ulr);
        String gTime();

};

#endif