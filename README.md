# ipdvbsample
IP / DVB2000 Example to read the DVB Data link layer protocol MPE (Multiprotocol Encapsulation), for the TCP and IP packets, which were in that time, around 1996-2000 unencrypted send over satellites to the users.

https://en.wikipedia.org/wiki/Multiprotocol_Encapsulation

Setup: Nokia Mediamaster 9500 connected with a SCSI-cable to the PC. 

Thanks to Dr. Overflow (Uli Herrmann) for the DVB2000 software, the creator of the MM9500 firmware to make this all possible! It was fun and instructive creating this piece together with you!

v0.0
  This is Visual C++ 5.0 code, it'll show you how to handle with SCSI 
  and the 'orange' MAC screen.

  - OnInitDialog()    :  find the MediaMaster
  - OnButtonTofile()  :  init the 'orange' MAC-screen
  - OnButtonStop()    :  stop the MAC-screen

  - OnTimer()         :  poll the Mediamaster and get data into buf[]

Jarno
