/*
 * sam21d18a_register.h
 *
 * Created: 19-03-2018 14:32:55
 *  Author: Qtra
 */ 


#ifndef SAM21D18A_REGISTER_H_
#define SAM21D18A_REGISTER_H_

struct SERUSART_CTRLA_T{
	unsigned int SWRST: 1;
	unsigned int ENABLE: 1;
	unsigned int MODE : 3;
	unsigned : 2;
	unsigned int RUNDSTBY : 1;
	unsigned int IBON : 1;
	unsigned : 4;
	unsigned int SAMPR : 3;
	unsigned int TXPO : 2;
	unsigned : 2;
	unsigned int RXPO : 2;
	unsigned int SAMPA : 2;
	unsigned int FORM : 4;
	unsigned int CMODE : 1;
	unsigned int CPOL : 1;
	unsigned int DORD : 1;
	unsigned int : 1;
	
};

struct SERUSART_CTRLB_T{
	unsigned int CHSIZE : 3;
	unsigned : 3;
	unsigned int SBMODE : 1;
	unsigned : 1;
	unsigned int COLDEN : 1;
	unsigned int SFDE : 1;
	unsigned int ENC : 1;
	unsigned : 2;
	unsigned int PMODE : 1;
	unsigned : 2;
	unsigned int TXEN : 1;
	unsigned int RXEN : 1;
	unsigned : 14;
};

struct SERUSART_INTENCLR_T{
	unsigned int DRE : 1;
	unsigned int TXC : 1;
	unsigned int RXC : 1;
	unsigned int RXS : 1;
	unsigned int CTSIC : 1;
	unsigned int RXBRK : 1;
	unsigned : 1;
	unsigned int ERROR : 1;
};

struct SEUSART_STATUS_T{
	unsigned int PERR : 1;
	unsigned int FERR : 1;
	unsigned int BUFOVF : 1;
	unsigned int CTS : 1;
	unsigned int ISF : 1;
	unsigned int COLL : 1;
};

#endif /* SAM21D18A_REGISTER_H_ */