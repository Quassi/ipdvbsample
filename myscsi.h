bool			SCSIInitStream(unsigned char, unsigned char, unsigned int, unsigned int, unsigned int);
bool			SCSIInitMStream(unsigned char, unsigned char, unsigned char, unsigned char *, unsigned long, unsigned long);
bool			SCSIInitMAC(unsigned char, unsigned char, unsigned char[12]);
bool			SCSIStopStream(unsigned char, unsigned char);
bool			SCSIRezero(unsigned char, unsigned char);
unsigned char	SCSIHosts();
//bool			SCSIRead(unsigned char, unsigned char);
bool			SCSIRead(unsigned char, unsigned char, unsigned char, unsigned char *, unsigned long, unsigned long);
bool			SCSIReadBig(unsigned char, unsigned char, unsigned char, unsigned char *, unsigned long, unsigned long);
bool			SCSIWriteFW(unsigned char, unsigned char, unsigned char *, unsigned long, unsigned long);
bool			SCSIWrite(unsigned char, unsigned char, unsigned char, unsigned char *, unsigned long, unsigned long);
bool			SCSIWriteBig(unsigned char, unsigned char, unsigned char, unsigned char *, unsigned long, unsigned long);
unsigned char	SCSISpeed(unsigned char, unsigned char, unsigned char *);
bool			SCSIInquiry(unsigned char, unsigned char, unsigned char *);
void			SCSIChangeChannel(unsigned char, unsigned char, int);
unsigned char	SCSISetup();
void			SCSISpecialCommand(unsigned char, unsigned char, unsigned char, CString, unsigned char);