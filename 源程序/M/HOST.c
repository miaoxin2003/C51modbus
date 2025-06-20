//FIRMWARE FOR THE HOST
#include <reg52.h>
#include <intrins.h>

#define Uint16 unsigned int
#define Uint32 unsigned long
#define uchar  unsigned char
#define LCD_Data P1
#define Busy    0x80 //ÓÃÓÚ¼ì²âLCD×´Ì¬×ÖÖÐµÄBusy±êÊ¶

//uchar tflag;//ÎÂ¶ÈÕý¸º±êÖ¾

sbit reving=P3^5;
sbit rw=P3^7;
//sbit tcnt=P3^5;
//sbit rcnt=P3^7;

sbit LCD_RS=P3^2;//¶¨ÒåÒý½Å
sbit LCD_RW=P3^3;
sbit LCD_E=P3^4;

sbit P2_0 = P2^0; 
sbit P2_1 = P2^1;
sbit COOL_FAN = P2^5; 
sbit HEAT_FAN = P2^0; 
sbit BUZZER = P2^1;   
sbit RED_LED = P2^2; 
sbit GREEN_LED = P2^3; 
sbit YELLOW_LED = P2^4; 
sbit HUMIDIFIER = P2^7; 


void InitUART(void);
void SendOneByte(unsigned);
void mdproc(uchar);
Uint16 Crc16(Uint16 *puchMsg, Uint16 usDataLen); 

void WriteDataLCD(unsigned char WDLCD);
void WriteCommandLCD(unsigned char WCLCD,BuysC);
unsigned char ReadDataLCD(void);
unsigned char ReadStatusLCD(void);
void LCDInit(void);
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData);
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char *DData);
void Delay5Ms(void);
void Delay400Ms(void);

unsigned char code row1[] = {"MODBUS FUNC:04H"};
unsigned char code row2[] = {"NOW GET START"};
unsigned char code row6[] = {"RH(%)     "};
unsigned char code row7[] = {"TP(C)     "};


/* CRC ¸ßÎ»×Ö½ÚÖµ±í */ 
const Uint16 code auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
} ; 

/* CRCµÍÎ»×Ö½ÚÖµ±í*/ 
const Uint16 code auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
} ;
uchar asknode; // the current node for polling(ask for data in turn)
uchar mtx[8],mrx[10];
uchar send101[]={0x01,0x04,0x00,0x00,0x00,0x03};
uchar timercnt;
uchar revptr;
bit nodeok;
bit procok;
bit crcok;
Uint16 delaycnt;

uchar row4[]={"           "};
uchar row5[]={"           "};

void main(void)
{
	unsigned char temp8;
	unsigned char RH=0,TP=0;
	Uint16 temp16;//temp16_2;
    int i;
	reving=1;
	nodeok=0;
	P2_0 = 1;
	timercnt=0x00;
	asknode=0x00;
	//temp16_2=250;
	for(i=0;i<10;i++){mrx[i]=0x00;}
	for(i=0;i<8;i++){mtx[i]=0x00;}
//	tcnt=0;
//	rcnt=0;

	Delay400Ms(); //Æô¶¯µÈ´ý£¬µÈLCD½²Èë¹¤×÷×´Ì¬
	LCDInit(); //LCM³õÊ¼»¯
	Delay5Ms(); //ÑÓÊ±Æ¬¿Ì(¿É²»Òª)
	DisplayListChar(1, 0, row1);
	DisplayListChar(2, 1, row2);
	ReadDataLCD();//²âÊÔÓÃ¾äÎÞÒâÒå	

	delaycnt=0xffff;
	while(delaycnt--){}
	delaycnt=0xffff;
	while(delaycnt--){}

	DisplayListChar(0, 0, row6);
	DisplayListChar(0, 1, row7);
	
	InitUART();

/*	for(i=0;i<8;i++)
	{	temp8=send101[i];
		SendOneByte(temp8);
	}*/  //test sending...
	procok=1; // preset the PROCCESS OK FLAG is 1,from the RX to PROCCESS OK the bit is 0.
	crcok=1;
	rw=1;  //accroding to the MAX485 IC, /RE=1 means transfering ,and /RE=0 means receiving ,after INIT ,default state is transfering for HOST
	while(1)
	{	
		BUZZER = 0;
		//P2_0 = 1;
		if((procok==1)&&(crcok==1))
		{				
			//temp16_1=mrx[3]+mrx[4];	//´ÓÍ¨ÐÅÖÐ»ñÈ¡Êý¾Ý²¢×¼±¸ÏÂÒ»Ö¡·¢ËÍ
			//temp16_1*=temp16_2; // get the data from the communication and prepare the next frame for send
			//temp16_1>>=7;
			RH=mrx[3];
			TP=mrx[4];
			switch(mrx[0])          // ?????????? (mrx[0]) ??????????
            {
                case 0x01:
                    {
                        row4[1]=(RH/10)+0x30;
                        row4[2]=(RH%10)+0x30;
                        row5[1]=(TP/10)+0x30;
                        row5[2]=(TP%10)+0x30;
                        // ??:?????????TP?????????
                            // TP>28
                            if (row5[1] > '2' || (row5[1] == '2' && row5[2] >= '9')) {
                                COOL_FAN = 0; 
																BUZZER = 0;
																RED_LED = 0;
																YELLOW_LED = 1;
																GREEN_LED = 1;
                            }
														
														else if ((row5[1] == '2' && row5[2] == '6') ) {
                                COOL_FAN = 1;
																HEAT_FAN = 1;
																BUZZER = 1;
																GREEN_LED = 0;
                            }
														else if ((row5[1] == '2' && row5[2] >= '7')) {
                                COOL_FAN = 0; 
																YELLOW_LED = 0;
																BUZZER = 1;
																GREEN_LED = 1;
                            }
														else if ((row5[1] == '2' && row5[2] <= '5')) {
															HEAT_FAN = 0; 
															YELLOW_LED = 0;
															BUZZER = 1;
															GREEN_LED = 1;
                            }
														else if (row5[1] < '2' || (row5[1] == '2' && row5[2] <= '3')) {
															HEAT_FAN = 0; 
															BUZZER = 0;
															RED_LED = 0;
															YELLOW_LED = 1;
															GREEN_LED = 1;
                            }
														else {
                                COOL_FAN = 1;
																HEAT_FAN = 1;
																BUZZER = 1;
																//HUMIDIFIER = 0;
															
														
                            }
														if((row4[1] == '6' && row4[2] == '0') ){
															HUMIDIFIER = 1;
														}
														else if ((row4[1] == '5' && row4[2] <= '8')||(row4[1] == '6' && row4[2] >= '2')){
														HUMIDIFIER = 0;
														}
														else {
														HUMIDIFIER = 1;
														}
                        
                        // ****************************************
                        break; // ??switch??
                    }
                case 0x02:
                    {
                        // ??LCD??????2?????
                        row4[5]=(RH/10)+0x30;
                        row4[6]=(RH%10)+0x30;
                        // ??LCD??????2?????
                        row5[5]=(TP/10)+0x30;
                        row5[6]=(TP%10)+0x30;
												if (row5[5] > '2' || (row5[5] == '2' && row5[6] >= '9')) {
                                COOL_FAN = 0; 
																BUZZER = 0;
																RED_LED = 0;
																YELLOW_LED = 1;
																GREEN_LED = 1;
                            }
														
														else if ((row5[5] == '2' && row5[6] == '6') ) {
                                COOL_FAN = 1;
																HEAT_FAN = 1;
																BUZZER = 1;
																GREEN_LED = 0;
                            }
														else if ((row5[5] == '2' && row5[6] >= '7')) {
                                COOL_FAN = 0; 
																YELLOW_LED = 0;
																BUZZER = 1;
																GREEN_LED = 1;
                            }
														else if ((row5[6] == '2' && row5[6] <= '5')) {
															HEAT_FAN = 0; 
															YELLOW_LED = 0;
															BUZZER = 1;
															GREEN_LED = 1;
                            }
														else if (row5[5] < '2' || (row5[5] == '2' && row5[6] <= '3')) {
															HEAT_FAN = 0; 
															BUZZER = 0;
															RED_LED = 0;
															YELLOW_LED = 1;
															GREEN_LED = 1;
                            }
														else {
                                COOL_FAN = 1;
																HEAT_FAN = 1;
																BUZZER = 1;
																//HUMIDIFIER = 0;
														
                            }
														if((row4[5] == '6' && row4[6] == '0') ){
															HUMIDIFIER = 1;
														}
														else if ((row4[5] == '5' && row4[6] <= '8')||(row4[5] == '6' && row4[6] >= '2')){
														HUMIDIFIER = 0;
														}
														else {
														HUMIDIFIER = 1;
														}
                        // ** ????2???,??P2.0???? **
                        break; // ??switch??
                    }
                case 0x03:
                    {
                        // ??LCD??????3?????
                        row4[9]=(RH/10)+0x30;
                        row4[10]=(RH%10)+0x30;
                        // ??LCD??????3?????
                        row5[9]=(TP/10)+0x30;
                        row5[10]=(TP%10)+0x30;
												if (row5[9] > '2' || (row5[9] == '2' && row5[10] >= '9')) {
                                COOL_FAN = 0; 
																BUZZER = 0;
																RED_LED = 0;
																YELLOW_LED = 1;
																GREEN_LED = 1;
                            }
														
														else if ((row5[9] == '2' && row5[10] == '6') ) {
                                COOL_FAN = 1;
																HEAT_FAN = 1;
																BUZZER = 1;
																GREEN_LED = 0;
                            }
														else if ((row5[9] == '2' && row5[10] >= '7')) {
                                COOL_FAN = 0; 
																YELLOW_LED = 0;
																BUZZER = 1;
																GREEN_LED = 1;
                            }
														else if ((row5[9] == '2' && row5[10] <= '5')) {
															HEAT_FAN = 0; 
															YELLOW_LED = 0;
															BUZZER = 1;
															GREEN_LED = 1;
                            }
														else if (row5[9] < '2' || (row5[9] == '2' && row5[10] <= '3')) {
															HEAT_FAN = 0; 
															BUZZER = 0;
															RED_LED = 0;
															YELLOW_LED = 1;
															GREEN_LED = 1;
                            }
														else {
                                COOL_FAN = 1;
																HEAT_FAN = 1;
																BUZZER = 1;
																//HUMIDIFIER = 0;
														
                            }
														if((row4[9] == '6' && row4[10] == '0') ){
															HUMIDIFIER = 1;
														}
														else if ((row4[9] == '5' && row4[10] <= '8')||(row4[9] == '6' && row4[10] >= '2')){
														HUMIDIFIER = 0;
														}
														else {
														HUMIDIFIER = 1;
														}
                         // ** ????3???,??P2.0???? **
                        break; // ??switch??
                    }
                default:
                    {
                        // ?????????,??????
                        break; // ??switch??
                    }
            }
			
			DisplayListChar(5, 0, row4); //ÔÚÒº¾§ÏÔÊ¾Æ÷µÄÊÊµ±ÇøÓòÏÔÊ¾Êý¾Ý display the data at the proper zone on the LCD
			DisplayListChar(5, 1, row5); //ÔÚÒº¾§ÏÔÊ¾Æ÷µÄÊÊµ±ÇøÓòÏÔÊ¾Êý¾Ý display the data at the proper zone on the LCD

			delaycnt=0x3ff;
			while(delaycnt--){}
			rw=1;
			reving=0;
			send101[0]=(asknode++)+0x01;  // rolling the node of SLAVE for polling
			asknode%=3;
			temp16=Crc16(send101,6);  // generate the CRC and save it in an 16bit var
			for(i=0;i<6;i++){mtx[i]=send101[i];}
			mtx[6]=(temp16>>8)&0x00ff;
			mtx[7]=temp16&0x00ff;
			delaycnt=0x3ff;
			while(delaycnt--){}
			for(i=0;i<8;i++)
			{	temp8=mtx[i];
				SendOneByte(temp8);
			}
			//tcnt=~tcnt;
			//tcnt=~tcnt; // generate a pulse to the counter in proteus when send an data asking frame
			procok=0;
			crcok=0;
			delaycnt=0x3ff;
			while(delaycnt--){}
			rw=0;
			reving=1; // blink the led when sent a data asking frame
		}
	}
}

void Timer0Interrupt(void) interrupt 1
{
    //TH0 = 0x3C;
    //TL0 = 0x0B0;
	if(++timercnt==5)
	{	procok=1;
		timercnt=0;
	}

}

void UARTInterrupt(void) interrupt 4
{	uchar byterev;
    if(RI)
    {
        RI = 0;
        byterev=SBUF;
		mdproc(byterev);
    }
    else
        TI = 0;
}


void mdproc(uchar b)
{	Uint16 temp16_1,temp16_2;
	if(nodeok==0)
	{	if(b==send101[0])
		{	nodeok=1;
			revptr=1;
			mrx[0]=b;
		}
	}
	else if(revptr<11)
	{	mrx[revptr++]=b;
	}
	else
	{
		revptr=0;
		nodeok=0;
		temp16_1=Crc16(mrx,9);
		temp16_2=mrx[9];
		temp16_2<<=8;
		temp16_2|=mrx[10];
		if(temp16_1==temp16_2)
		{	crcok=1;
			//rcnt=~rcnt;
			//rcnt=~rcnt; // generate a pulse to the counter in proteus when received correct frame
		}
						
	}
}

void SendOneByte(unsigned char c)
{
    SBUF = c;
    while(!TI);
    TI = 0;
}


void InitUART(void)
{
    TMOD = 0x20;
    SCON = 0x50;
    TH1 = 0xFE;
    TL1 = TH1;
    PCON = 0x00;
    EA = 1;
    ES = 1;
    TR1 = 1;
	
	TMOD |= 0x01;
    //TH0 = 0x3C;
    //TL0 = 0x0B0;  // timer sets at 20ms
    ET0 = 1;
    TR0 = 1;


}


Uint16 Crc16(uchar *puchMsg, Uint16 usDataLen) 
{ 
	Uint16 uchCRCHi = 0xFF ;              /* ¸ßCRC×Ö½Ú³õÊ¼»¯  */ 
	Uint16 uchCRCLo = 0xFF ;              /* µÍCRC ×Ö½Ú³õÊ¼»¯ */ 
	Uint16 temp16;
	Uint32 uIndex ;                      /* CRCÑ­»·ÖÐµÄË÷Òý  */ 
	while (usDataLen--)                  /* ´«ÊäÏûÏ¢»º³åÇø   */ 
	{ 
		temp16=*puchMsg++;
		uIndex = uchCRCHi ^ temp16 ; /* ¼ÆËãCRC          */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return (uchCRCHi << 8 | uchCRCLo) ; 
}

//Ð´Êý¾Ý
void WriteDataLCD(unsigned char WDLCD)
{
 ReadStatusLCD(); //¼ì²âÃ¦
 LCD_Data = WDLCD;
 LCD_RS = 1;
 LCD_RW = 0;
 LCD_E = 0; //Èô¾§ÕñËÙ¶ÈÌ«¸ß¿ÉÒÔÔÚÕâºó¼ÓÐ¡µÄÑÓÊ±
 LCD_E = 0; //ÑÓÊ±
 LCD_E = 0;
 LCD_E = 1;
}

//Ð´Ö¸Áî
void WriteCommandLCD(unsigned char WCLCD,BuysC) //BuysCÎª0Ê±ºöÂÔÃ¦¼ì²â
{
 if (BuysC) ReadStatusLCD(); //¸ù¾ÝÐèÒª¼ì²âÃ¦
 LCD_Data = WCLCD;
 LCD_RS = 0;
 LCD_RW = 0; 
 LCD_E = 0;
 LCD_E = 0;  
 LCD_E = 1; 
}

//¶ÁÊý¾Ý
unsigned char ReadDataLCD(void)
{
 LCD_RS = 1; 
 LCD_RW = 1;
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1;
 return(LCD_Data);
}

//¶Á×´Ì¬
unsigned char ReadStatusLCD(void)
{
 LCD_Data = 0xFF; 
 LCD_RS = 0;
 LCD_RW = 1;
 LCD_E = 0;
 LCD_E = 0;
 LCD_E = 1;
 while (LCD_Data & Busy); //¼ì²âÃ¦ÐÅºÅ
 return(LCD_Data);
}

void LCDInit(void) //LCM³õÊ¼»¯
{
 LCD_Data = 0;
 WriteCommandLCD(0x38,0); //Èý´ÎÏÔÊ¾Ä£Ê½ÉèÖÃ£¬²»¼ì²âÃ¦ÐÅºÅ
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 
 WriteCommandLCD(0x38,0);
 Delay5Ms(); 

 WriteCommandLCD(0x38,1); //ÏÔÊ¾Ä£Ê½ÉèÖÃ,¿ªÊ¼ÒªÇóÃ¿´Î¼ì²âÃ¦ÐÅºÅ
 WriteCommandLCD(0x08,1); //¹Ø±ÕÏÔÊ¾
 WriteCommandLCD(0x01,1); //ÏÔÊ¾ÇåÆÁ
 WriteCommandLCD(0x06,1); // ÏÔÊ¾¹â±êÒÆ¶¯ÉèÖÃ
 WriteCommandLCD(0x0C,1); // ÏÔÊ¾¿ª¼°¹â±êÉèÖÃ
}

//°´Ö¸¶¨Î»ÖÃÏÔÊ¾Ò»¸ö×Ö·û
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
 Y &= 0x1;
 X &= 0xF; //ÏÞÖÆX²»ÄÜ´óÓÚ15£¬Y²»ÄÜ´óÓÚ1
 if (Y) X |= 0x40; //µ±ÒªÏÔÊ¾µÚ¶þÐÐÊ±µØÖ·Âë+0x40;
 X |= 0x80; // Ëã³öÖ¸ÁîÂë
 WriteCommandLCD(X, 0); //ÕâÀï²»¼ì²âÃ¦ÐÅºÅ£¬·¢ËÍµØÖ·Âë
 WriteDataLCD(DData);
}

//°´Ö¸¶¨Î»ÖÃÏÔÊ¾Ò»´®×Ö·û
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char *DData)
{
 unsigned char ListLength;

  ListLength = 0;
 Y &= 0x1;
 X &= 0xF; //ÏÞÖÆX²»ÄÜ´óÓÚ15£¬Y²»ÄÜ´óÓÚ1
 while (DData[ListLength]>=0x20) //Èôµ½´ï×Ö´®Î²ÔòÍË³ö
  {
   if (X <= 0xF) //X×ø±êÓ¦Ð¡ÓÚ0xF
    {
     DisplayOneChar(X, Y, DData[ListLength]); //ÏÔÊ¾µ¥¸ö×Ö·û
     ListLength++;
     X++;
    }
  }
}

//5msÑÓÊ±
void Delay5Ms(void)
{
 unsigned int TempCyc = 5552;
 while(TempCyc--);
}

//400msÑÓÊ±
void Delay400Ms(void)
{
 unsigned char TempCycA = 5;
 unsigned int TempCycB;
 while(TempCycA--)
 {
  TempCycB=7269;
  while(TempCycB--);
 };
}


