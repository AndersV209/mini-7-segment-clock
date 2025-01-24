//get time from NTP
// Do not alter this function, it is used by the system 
int getTimeAndDate() {
  int flag=0;
  if(!use_rtc){
     WiFi.hostByName(ntpServerName, timeServerIP); 
     
     udp.begin(localPort);
     Serial.println(timeServerIP);
     sendNTPpacket(timeServerIP);
     delay(1000);
     if (udp.parsePacket()){
       udp.read(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer
       unsigned long highWord, lowWord, epoch;
       highWord = word(packetBuffer[40], packetBuffer[41]);
       lowWord = word(packetBuffer[42], packetBuffer[43]); 
       epoch = highWord << 16 | lowWord;
       epoch = epoch - 2208988800 + timeZoneOffset;
       flag=1;
       //setTime(epoch);  // set time without time zone libary 
       setTime(myTZ.toUTC(epoch));  // set time with timezone libary 
       
       ntpLastUpdate = now();
     }

  }else{
    Serial.println("uses rtc");
    DateTime nowrtc = rtc.now();
    unsigned long epoch;
    epoch = nowrtc.unixtime();
    setTime(myTZ.toUTC(epoch));  // set time with timezone libary 
    ntpLastUpdate = now();
  }

  time_is_set = flag;
  return flag;
}


// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}
