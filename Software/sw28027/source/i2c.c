 /*! \file i2c.c
  *  \brief Data collecting for the recorder function.
  *  \author Originally Gal Istvan; further development: Bilau Zoltan Tamas (IQ, float, shifting, time, now, SPI)
  *  \version 2.0
  *  \date    2010-06-25
 */
/**************************************************************************
																i2c.c
GpioDataRegs.GPBDAT.bit.GPIOB6 volt
GpioDataRegs.GPBDAT.bit.GPIO32 lett

 **************************************************************************/
#include "DSP2802x_Device.h"
#include "i2c.h"

//Debug
Uint16 tmp_ora[7];


asm(" .c28_amode ");


Uint16 I2cWrite(Uint16 DevAddr, Uint16 * Source, Uint16 Num) ;

Uint16 ora_hiba_cnt, Clock100ms;
//Uint16 iic_hiba_cnt;
Uint16 eeprom_i2c_hiba_cnt;
Uint16 eeprom_iras_ellen_hiba;
Uint16 eeprom_iras_ellen_hiba_cnt;
Uint16 eeprom_iras_ellen_hiba_cim;
Uint16 i2cHibajelzesekLehetnek;
Uint16 eeprom_write_counter;
Uint16 I2c(Uint16 write, Uint16 cim, Uint16 * ptr, Uint16 num);

Uint16 PVC_System_Year;
Uint16 PVC_System_Month;
Uint16 PVC_System_Day;
Uint16 Thet_napjai;
Uint16 PVC_System_Hour;
Uint16 PVC_System_Minute;
Uint16 PVC_System_Second;

#define SDA_READ GpioDataRegs.GPBDAT.bit.GPIO32

//-----------------------------------------------------------------------------
// max. 400kHz busz frekvencia

// @GI az RPT utasítas nem megszakíthato, ez IT kotyogashoz vezet!!!
#define dly() { asm(" RPT #200 || NOP "); }	// 600ns  2808 max 100 MIPS, de az FBANKWAIT !!!!
/*
void dly() // 2us
{
  Uint16 endOfTime = T2CNT + (T0_1MS / 50); // 2us

  while ((signed)(T2CNT - endOfTime) < 0);
}
*/
//-----------------------------------------------------------------------------
void SDA_hi(void){
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 0; // SDA input (high impedance)
}
//-----------------------------------------------------------------------------
void SDA_lo(void){
	GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1; //SDA low
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1; //SDA output
}
//-----------------------------------------------------------------------------
void SCL_hi(void){
	GpioDataRegs.GPBSET.bit.GPIO33 = 1; //SCL high
}

//-----------------------------------------------------------------------------
void SCL_lo(void){
	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1; //SCL low
}
//-----------------------------------------------------------------------------
void start(void)
{
	dly(); SCL_hi(); dly(); SDA_hi(); dly(); dly(); SDA_lo(); dly(); SCL_lo(); dly();
}
//-----------------------------------------------------------------------------
void stop(void)
{
	dly(); SCL_hi(); dly(); SDA_hi(); dly();
	SCL_lo();	// Így a CLK alacsony lesz a kommunikacio vegen.
}
//-----------------------------------------------------------------------------
void ack(void)
{
	SDA_lo(); dly(); SCL_hi(); dly(); SCL_lo(); dly();	/* ACK bit */
}
//-----------------------------------------------------------------------------
void nack(void)
{
	SDA_hi(); dly(); SCL_hi(); dly(); SCL_lo(); dly(); SDA_lo() ;	/* NACK bit */
}
//=============================================================================
Uint16 byte_rd(void)
{
	Uint16 j, adat, mask;

	SCL_lo() ;
	dly();
	SDA_hi() ;
	dly();
	/* SDA bemenet, SCL=0 kimenet */

	for (mask=0x80,adat=0,j=0; j<8; j++,mask>>=1)
	{
		SCL_hi();
		dly();
		if (SDA_READ)
				adat |= mask;
		SCL_lo();
		dly();
	}

	SDA_lo() ; /* SDA=0 kimenet, SCL=0 kimenet */

	return adat;
	}
 /**************************************************************************/
Uint16 byte_wr(Uint16 adat)	// 0 -> OK
	{
	Uint16 j, mask, error_=0;

	dly();
	for ( mask=0x80, j=0; j<8; j++, mask>>=1 )
		{
		if (adat & mask)	SDA_hi();
		else				SDA_lo();
	dly();
		SCL_hi();	dly(); SCL_lo();
	dly();
		}

	SDA_hi() ; /* SDA bemenet, SCL=0 kimenet */	dly();

	SCL_hi();  dly();
	//if (PFDATDIR & SDA_BIT)
	if (SDA_READ)	// SDA
		error_ = 1;
	SCL_lo();
	dly();
	SDA_lo(); /* SDA=0 kimenet, SCL=0 kimenet */	dly();

	return error_;
	}

//--------------------------------------------------------------------------//

/*!
 *  \brief
 *
 * Reset utan az i2c-n addig adunk orajelet, amig stoppot nem
 * tudunk adni neki. (Ha esetleg valamilyen folyamat, íras vagy
 * olvasas nem fejezodott be, akkor most megallítjuk.)
 * Bekapcsolas utan az orajelet magasba allítjuk es megnezzuk,
 * hogy az adat magasban van-e!
 * Ha magasban van akkor startot es stoppot adunk.
 * Ha alacsonyban van az adat akkor orajelet adunk es ismet
 * megvizsgaljuk, hogy az adat magasban van-e es így tovabb.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 05.
 */
void i2c_force_stop(void)
{

	Uint16 i;

	for(i=0; i<16; i++)
	{
		SCL_hi();  dly();	// I2C SCL magasba es varunk
		if(SDA_READ)
		{
			// Az adat magasban van. Lehet startot es stoppot adni.
			start();
			stop();
			return;
		}
		SCL_lo();
		dly();
	}
	// Nem sikerult kiadnia a stop parancsot!
//	iic_hiba_cnt++;

}

/*!
 *  \brief Ha hiba tortent az íras soran, akkor igaz ertekkel ter vissza
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 *  \param[in] mod
 *  \param[in] cim
 *  \param[in] *ptr
 *  \param[in] num
 */
Uint16 I2c(Uint16 mod, Uint16 cim, Uint16 * ptr, Uint16 num)
{

	Uint16 cim_tmp;
	Uint16 *ptr_tmp;
	Uint16 num_tmp;

	Uint16 Ora = cim >= ADDR_CLK_REG;
	  // cim: 0-0x7fff      -> EEPROM 32kbyte (24LC128 16kbyte)
	  // cim: 0x8000-0x803f -> ORA 64 byte (csak az also 8 bitet hasznaljuk)
 	Uint16 eof_time;

	Uint16 next_page_addr;
	GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;
EEPROM_REWRITE:
	// Ujra íras eseten itt volt a hiba, az elozoeket mar beírta es
	// elleorizte is. Csak a hibastol kezdi ujbol beírni a vegeig.
	cim_tmp = cim;
	ptr_tmp = ptr;
	num_tmp = num;

EEPROM_VERIFY:
// eof_time = EvbRegs.T4CNT ;
	eof_time = 1000 ;
//	next_page_addr = (cim + PAGE_SIZE) & (0x10000 - PAGE_SIZE);
	next_page_addr = (cim + PAGE_SIZE) & ~(PAGE_SIZE-1);

NEXT_PAGE_ADDR_SET:
	while(1) // ha volt elozoleg iras, es meg nincs kesz az eeprom, akkor varni kell
	{
//		if ((unsigned)(EvbRegs.T4CNT-eof_time) > T10MS )
		if (--eof_time == 0 )
			goto ERR_I2C; // ha idotullepes volt, kiszall
//		KICK_DOG
		start();
		if (byte_wr(Ora ? CLK_DEV_ADDR : EEPR_DEV_ADDR))
		{ // nem volt acknowladge
			stop();
			continue;
		}
		else
			break; // elfogadta az eeprom a cimzest
	}
	if (!Ora)
	{
		cim *= 2;	// a sw wordos cimeket hasznal, az eeprom bajtos szervezesu
		if (byte_wr(cim>>8))
			goto ERR_I2C;
	}
	if (byte_wr(cim))
		goto ERR_I2C;

	switch(mod)
	{
		case READ:	// olvasas
			start();
			if (byte_wr((Ora ? CLK_DEV_ADDR : EEPR_DEV_ADDR) + 1))	// Olvasas parancs
				goto ERR_I2C;
			for (;;)
			{
				if (Ora)
					*ptr = byte_rd();
				else
				{
					*ptr = byte_rd();
					ack();
					*ptr += 256 * byte_rd();
				}
				ptr++;
				if (--num)	ack();
				else			{ nack(); break; }
//				KICK_DOG();
			}
			break;

		case WRITE:	// iras
			eeprom_write_counter++;
			do
			{
				if (byte_wr(*ptr))
					goto ERR_I2C;
				if (!Ora)
				{
					if (byte_wr(*ptr>>8))
						goto ERR_I2C;
					else
						cim = cim + 2;
				}
				ptr++;
				num--;
				if (!Ora && num && (next_page_addr == (cim >> 1)))
				{
					cim = cim >> 1;
					next_page_addr += PAGE_SIZE;
					eof_time = 1000;

					stop();
					goto NEXT_PAGE_ADDR_SET;
				}
			}	while(num);
			break;

		case VERIFY:
			// Minden adat ki lett írva az EEPROM-ba.
			// Az írt adatot vissza kell olvasni es ellenorizni kell
			start();
			if (byte_wr((Ora ? CLK_DEV_ADDR : EEPR_DEV_ADDR) + 1))	// Olvasas parancs
				goto ERR_I2C;
			for (;;)
			{
				if (Ora)
				{
					if(*ptr != byte_rd())
					{	// Ellenorzesi hiba
						goto VERIFY_ERROR;
					}
				}
				else
				{
					if((*ptr & 0x00ff) != byte_rd())
					{
						goto VERIFY_ERROR;
					}
					ack();
					if((*ptr >> 8) != byte_rd())
					{
						goto VERIFY_ERROR;
					}
				}
				ptr++;
				if (--num)
					ack();
				else
				{
					nack();
					break;
				}
//				KICK_DOG();
			}
			break;
	}	// switch vege

	stop();
	// Ellenorzes
	if(!Ora && mod == WRITE)
	{
		//I2c(VERIFY, cim_tmp, ptr_tmp, num_tmp);
		mod = VERIFY;
		cim = cim_tmp;
		ptr = ptr_tmp;
		num = num_tmp;
		goto EEPROM_VERIFY;
	}
	return 0; // Nem tortent hiba

ERR_I2C:
	stop();
	/*
	if (!err_on)
	{
		// Csak akkor jelzunk EEPROM hibat, ha a tukor parameter keszletbe se sikerult elmenteni
		if (i2cHibajelzesekLehetnek)
			error_type_3(IIC_ERR);
	}
	*/
	eeprom_i2c_hiba_cnt++;
	return 1;	// I2C hiba

VERIFY_ERROR:
	nack();
	stop();

	 if(eeprom_iras_ellen_hiba == 0)
	{
		eeprom_iras_ellen_hiba++;
		eeprom_iras_ellen_hiba_cnt++;

		// Ismetelt íras megkiserlese
		mod = WRITE;
		if(!Ora)	// cim/2, mert at EEPROM bajtos címet hasznal!
			cim /= 2;
		cim = cim + ptr - ptr_tmp;
		eeprom_iras_ellen_hiba_cim = cim;
		goto EEPROM_REWRITE;
	}

	// Ismetelt íras megtortent, tobbet nem probalkozik
	/*
	if (i2cHibajelzesekLehetnek)
		error_type_3(EEPROM_WRITE_ERR);
	*/
	eeprom_iras_ellen_hiba = 0;
	return 2;	// EEPROM íras hiba
}

/**************************************************************************/
/* i2c buszos PVC_System_Hour kezelese */
/**************************************************************************/
Uint16 bcd2bin(Uint16 inp, Uint16 msk)
	{
	return (10 *(inp >>4)) + (inp & msk);
	}
/*!
 *  \brief Reading the RTC.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 05.
 */
void RdClock(void)
{
	I2c(READ, ADDR_CLK_REG, tmp_ora, 7);

	if (tmp_ora[0]>0x59)		goto ora_hiba;
	PVC_System_Second = bcd2bin(tmp_ora[0], 0x0f);

	if (tmp_ora[1]>0x59)		goto ora_hiba;
	PVC_System_Minute = bcd2bin(tmp_ora[1], 0x0f);

	if (tmp_ora[2]>0x23)		goto ora_hiba;
	PVC_System_Hour = bcd2bin(tmp_ora[2], 0x0f);

	if (tmp_ora[3]>7)   		goto ora_hiba;
	Thet_napjai = tmp_ora[3]+VK_NAPOK_HETFOE-1;

	if (tmp_ora[4]>0x31)		goto ora_hiba;
	PVC_System_Day = bcd2bin(tmp_ora[4], 0x0f);

	if (tmp_ora[5]>0x12)		goto ora_hiba;
	PVC_System_Month = bcd2bin(tmp_ora[5], 0x0f);

	PVC_System_Year = 2000 + bcd2bin(tmp_ora[6],0x0f);
	return ;

 ora_hiba:
  ora_hiba_cnt++;

}
/**************************************************************************/
Uint16 bin2bcd(Uint16 inp, Uint16 msk)
	{
  	return (((inp/10) << 4) & msk) + (inp%10);
	}
/**************************************************************************/
void WrClock(void)
{
	Uint16 tmp[8];

	tmp[0] = bin2bcd(PVC_System_Second, 0xf0);
	tmp[1] = bin2bcd(PVC_System_Minute, 0xf0);
	tmp[2] = bin2bcd(PVC_System_Hour, 0x30);
	tmp[3] = Thet_napjai-VK_NAPOK_HETFOE+1;
	tmp[4] = bin2bcd(PVC_System_Day, 0x30);
	tmp[5] = bin2bcd(PVC_System_Month, 0x10);
	tmp[6] = bin2bcd(PVC_System_Year%100, 0xff);
	tmp[7] = 0; //ctrl. reg, no calibration

	I2c(WRITE, ADDR_CLK_REG, tmp, 8);
}

/*!
 *  \brief Ora leptetes 1 s-al.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 01.
 */
void ClockStep(void)
{
	if (PVC_System_Second < 59)
		PVC_System_Second++;
	else
	{
		PVC_System_Second = 0;
		if (PVC_System_Minute < 59)
			PVC_System_Minute++;
		else
		{
			PVC_System_Minute = 0;
			if (PVC_System_Hour < 23)
				PVC_System_Hour++;
			else
			{
				PVC_System_Hour = 0;
				if (Thet_napjai < VK_NAPOK_VASARNAP)
					Thet_napjai++;
				else
					Thet_napjai = VK_NAPOK_HETFOE;
				if (PVC_System_Month == 1 ||
						PVC_System_Month == 3 ||
						PVC_System_Month == 5 ||
						PVC_System_Month == 7 ||
						PVC_System_Month == 8 ||
						PVC_System_Month == 10 ||
						PVC_System_Month == 12)
				{
					if (PVC_System_Day < 31)
						PVC_System_Day++;
					else
						PVC_System_Day = 0;
				}
				else
				{
					if (PVC_System_Day < 30)
						PVC_System_Day++;
					else
						PVC_System_Day = 0;
				}
				if (PVC_System_Day == 0)
				{
					if (PVC_System_Month < 12)
						PVC_System_Month++;
					else
					{
						PVC_System_Month = 0;
						PVC_System_Year++;
					}
				}
			}
		}
	}
}
/*!
 *  \brief Writing to the DAC.
 *  \author Weitzl Zoltan
 *  \version 1.0
 *	\date 2010. 07. 05.
 *  \param[in] Channel
 *  \param[in] Data
 */
Uint16 WriteDAC(Uint16 Channel, Uint16 Data)
{
	Uint16 buffer[3] ;
	if (Data>255) return 2 ;
	buffer[0] = 0x0010 + Channel*2 ;
	buffer[1] = Data ;
	buffer[2] = 0 ;
	return I2cWrite(DA_DEV_ADDR,buffer,3) ;
}

// Ha hiba tortent az íras soran, akkor (nem nulla) ertekkel ter vissza
Uint16 I2cWrite(Uint16 DevAddr, Uint16 * Source, Uint16 Num)
{
 	Uint16 eof_time = 1000 ;

	while(1) // ha volt elozoleg iras, es meg nincs kesz az eeprom, akkor varni kell
	{
		if (--eof_time == 0 )
			goto ERR_I2cWrite; // Timeout
//		KICK_DOG();
		start();
		if (byte_wr(DevAddr))
		{ // No ACK, try again
			stop();
			continue;
		}
		else
			break; // successful addressing
	}
	do
	{
		if (byte_wr(*Source))
			goto ERR_I2cWrite;
		Source++;
		Num--;
	}	while(Num);

	stop();
	return 0; // Nem tortent hiba

ERR_I2cWrite:
	stop();
	eeprom_i2c_hiba_cnt++;
	return 1;	// I2C hiba
}

