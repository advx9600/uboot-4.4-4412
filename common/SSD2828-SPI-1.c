/*********************************************************************
;* Project Name : s3c2443x
;*
;* Copyright 2006 by Samsung Electronics, Inc.
;* All rights reserved.
;*
;* Project Description :
;* This software is only for verifying functions of the s3c2443x
;* Anybody can use this code without our permission.
;**********************************************************************/


//#define SDO GPE11
//#define SDI GPE12
//#define SCL GPE13
//#define CSX GPE14
//#define RST GPE15

#include <common.h>
#include <asm/io.h>

#define U32 unsigned int
#define U16 unsigned short
#define U8 unsigned char
#define uint16 U16

#define Outp32(addr, data) (*(volatile u32 *)(addr) = (data))

static void Delay_us(U32 us)
{
//	while(us--) Delay(56);	
	udelay(us);
}

static void Delay_ms(U32 ms)
{
	//while(ms--) Delay(59360);	
	Delay_us(ms*1000);
}


static void Delay_clk_time()
{
	Delay_us(10);
}


//uint16 * pFrameBuf = NULL;

static void Set_RST(U32 index)
{
	// 151 pin
	{
	    U32 addr = 0x11000C60;
	    U32 val=readl(addr);
	    if ((val &0xF00) != 0x100){
	        Outp32(addr,(val&(~0x100))|0x100);
	    }
	}
	
	{
	    U32 data= 0x11000C64;
	    U32 val = readl(data);
	    val=(val&(index?0xFF:0xF7));
	    if (index) val |= 0x08;
	    else  val &= 0xF7;
	    Outp32(data,val);
	}
}

static U32 GPBCON =	0x11400040;
static U32 GPBDATA=	0x11400044;
static U32 GPBPUD =	0x11400048;

static void put_gpb_data(U32 addrIndex,U32 setval)
{
	U32 val = readl(GPBDATA);
	if (setval) val = val|(1<<addrIndex);
	else val = val&(~(1<<addrIndex));
	Outp32(GPBDATA,val);
}
static void Set_2805_CS(U32 index)
{
	put_gpb_data(5,index);
}

#if 0
void Set_CSX(U32 index)
{
	if(index) rGPEDAT |= 0x4000;
	else      rGPEDAT &= ~0x4000;	
}
#endif

static void Set_SCL(U32 index)
{
	put_gpb_data(4,index);
}

static void Set_SDI(U32 index)
{
	put_gpb_data(7,index);
}


void SPI_Init(void)
{
	{
	    U32 val = readl(GPBCON);
	    val = val&0x0000FFFF|0x1011FFFF;
	    Outp32(GPBCON,val);

	    val =readl(GPBPUD);
	    val = val|0x3F00;
	    Outp32(GPBPUD,val);

	}

	Set_RST(1);
	Set_2805_CS(1);
//	Set_CSX(1);
	Set_SCL(0);	
	Set_SDI(0);
	printf("GPBCON:0x%x\n",readl(GPBCON));
	printf("GPBDATA:0x%x\n",readl(GPBDATA));
	printf("GPBPUD:0x%x\n",readl(GPBPUD));
	
}

static void SPI_3W_SET_Cmd(U8 cmd)
{
	U32 kk;
	
	Set_SDI(0);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Delay_clk_time();
	Set_SCL(1);
	Delay_clk_time();
	

	Set_SCL(0);
	Delay_clk_time();
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
		cmd = cmd<<1;	
	}
}

static void SPI_3W_SET_PAs(U8 value)
{
	U32 kk;

	Set_SDI(1);			//Set DC=1, for writing to Data register
	Set_SCL(0);
	Delay_clk_time();
	Set_SCL(1);
	Delay_clk_time();
//	Delay_us(1);	
	Set_SCL(0);
	Delay_clk_time();
	for(kk=0;kk<8;kk++)
	{
		if((value&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
		value = value<<1;	
	}	
}
//-----------------------------------------------------
static void Write_com(U16 vv)
{
	Set_2805_CS(0);
	SPI_3W_SET_Cmd(vv&0xff);	
}

static void Write_register(U16 vv)
{
	SPI_3W_SET_PAs(vv&0xff);
	SPI_3W_SET_PAs((vv>>8)&0xff);	
}
//-----------------------------------------------------

static void writec(U8 cmd)
{
		U32 kk;
	
	Set_2805_CS(0);
	
	Set_SDI(0);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Delay_clk_time();
	Set_SCL(1);
	Delay_clk_time();
	

	Set_SCL(0);
	Delay_clk_time();
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
		cmd = cmd<<1;	
	}
	
	Set_2805_CS(1);		
}

static void writed(U8 cmd)
{
		U32 kk;
	
	Set_2805_CS(0);
	
	Set_SDI(1);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Delay_clk_time();
	Set_SCL(1);
	Delay_clk_time();
	

	Set_SCL(0);
	Delay_clk_time();
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
		cmd = cmd<<1;	
	}
	
	Set_2805_CS(1);		
}

static void SPI_2825_WrCmd(U8 cmd)
{
	U32 kk;
	
	Set_2805_CS(0);
	
	Set_SDI(0);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Delay_clk_time();
	Set_SCL(1);
	Delay_clk_time();
	

	Set_SCL(0);
	Delay_clk_time();
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
		cmd = cmd<<1;	
	}
	
	Set_2805_CS(1);	
}

//void SPI_2825_WrCmd(U8)
static void SPI_2825_WrReg(U8 c,U16 value)
{
	Set_2805_CS(0);
	SPI_3W_SET_Cmd(c);
	SPI_3W_SET_PAs(value&0xff);
	SPI_3W_SET_PAs((value>>8)&0xff);	
	Set_2805_CS(1);	
}



static U8 Read_PAs(void)
{
	U32 kk;
	U8  rdValue;

//	Set_SCL(0);
	rdValue = 0;
	for(kk=0;kk<8;kk++)
	{
		rdValue = rdValue<<1;
		
		Set_SCL(1);
		if(readl(GPBDATA)&0x40)  rdValue |= 0x01;
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
	}
//	Set_CSX(0);

	return rdValue;	
}

static U16   SPI_READ(U8 cmd)
{
	U8  rdT;
	U16 reValue;
	U32 kk;
	
	Set_2805_CS(0);

	Set_SDI(0);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Delay_clk_time();
	Set_SCL(1);
	Delay_clk_time();
	

	Set_SCL(0);
	Delay_clk_time();
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
		cmd = cmd<<1;	
	}
	
	Set_SDI(0);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Delay_clk_time();
	Set_SCL(1);
	Delay_clk_time();
	

	cmd = 0xFA;
	Set_SCL(0);
	Delay_clk_time();
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Delay_clk_time();
		Set_SCL(0);
		Delay_clk_time();
		cmd = cmd<<1;	
	}	
	
	rdT=0;
	for(kk=0;kk<8;kk++)
	{
		rdT = rdT<<1;
		Set_SCL(1);
		if(readl(GPBDATA)&0x40) rdT |= 0x01;
		Delay_clk_time();
		Set_SCL(0);				
		Delay_clk_time();
	}
	
	reValue = rdT;
	//reValue = (reValue<<8)&0xFF00;
	
	rdT=0;
	for(kk=0;kk<8;kk++)
	{
		rdT = rdT<<1;
		Set_SCL(1);
		if(readl(GPBDATA)&0x40) rdT |= 0x01;
		Delay_clk_time();
		Set_SCL(0);				
		Delay_clk_time();
	}
	
	reValue += (rdT<<8);
	
	Set_2805_CS(1);
	
	return reValue;			
}

static U16  SPI_READ_ID(U8 addr)
{
	SPI_2825_WrReg(0xd4, 0x00FA);
	return SPI_READ(addr);
}


static void SPI_WriteData(U8 value)
{
//	printf("-%2x",value);
	Set_2805_CS(0);
	SPI_3W_SET_PAs(value);
	Set_2805_CS(1);	
}


static void GP_COMMAD_PA(U16 num)
{


/*    if(num<3)
   {
  	SPI_2825_WrReg(0xb7, 0x0210);
    }
    else
    {
    	SPI_2825_WrReg(0xb7, 0x0610);
    }*/

 

         	SPI_2825_WrReg(0xbc, num);
	    	Write_com(0x00bd);
			SPI_2825_WrReg(0xbe, num);
        	Write_com(0x00bf);
        	Set_2805_CS(1);	
}

static U16 GP_COMMAD_PA_READ()
{
	U32 i,count;
	SPI_2825_WrReg(0xB7,0x382);	
	SPI_2825_WrReg(0xBC,0x01);
	SPI_2825_WrReg(0xBF,0x0a);

	Set_2805_CS(1);
	Delay_ms(10);
	printf("C6:0x%X\n",SPI_READ_ID(0xC6));
	if ((SPI_READ_ID(0xC6)&0x19) !=0x19){
	   printf("read 0xC6 not ready failed C3:0x%X\n",SPI_READ_ID(0xC3));
	   return 0xFFFF;
	}
	
	count = SPI_READ_ID(0xC2);
	printf("readnum:0x%X ",count);
	if (count >100) count =100;
	for (i=0;i<count;i++)
	{
	    printf("data[%d]:0x%X ",i,SPI_READ_ID(0xFF));
	} printf("\n");

	return 0xFFFF;
//	return SPI_READ_ID(cmd);
}

#define VBP        		(0x10)
#define LCD_VBPD		VBP
#define VFP        		(0x10)
#define LCD_VFPD		VFP
#define VPW        		(2)
#define LCD_VSPW		VPW

#define HBP        		(0x24)
#define LCD_HBPD		HBP
#define HFP        		(0x18)
#define LCD_HFPD		HFP
#define HPW        		(2)
#define LCD_HSPW		HPW

#define LCD_XSIZE_TFT		(1024)
#define LCD_YSIZE_TFT		(600)
#define LCD_WIDTH 		LCD_XSIZE_TFT
#define LCD_HEIGHT		LCD_YSIZE_TFT
#define LCD_DSI_CLCK		(0x801E)

#define MDELAY Delay_ms
static void init_lcm_registers(void)
{
	//LCM_DEBUG("[LCM************]: init_lcm_registers. \n");
	Write_com(0x00B7);
	Write_register(0x0302);	
	//0xB1--B5 ¿¿¿¿¿¿RGB timing
	Write_com(0x00B1);
	Write_register((VPW<<8)+HPW);
	Write_com(0x00B2);
	Write_register(((VPW+VBP)<<8)+(HPW+HBP));
	Write_com(0x00B3);
	Write_register((VFP<<8)+HFP);
	Write_com(0x00B4);
	Write_register(LCD_WIDTH);
	Write_com(0x00B5);
	Write_register(LCD_HEIGHT);
	
	Write_com(0x00B6);
	Write_register(0x000B);
	Write_com(0x00DE);
	//Write_register(0x0001);//2 Lane MIPI
	//Write_register(0x0002);//3 Lane MIPI
	Write_register(0x0003);//4 Lane MIPI
	Write_com(0x00D6);
	Write_register(0x0004);//Color order:RGB
	//Write_register(0x0005);//Color order:BGR
	Write_com(0x00B9);
	Write_register(0x0000);
	
	Write_com(0x00BA);
	Write_register(LCD_DSI_CLCK);//12MHz
	Write_com(0x00BB);
	Write_register(0x0009);
	
	Write_com(0x00B9);
	Write_register(0x0001);
	
	Write_com(0x00B7);
	Write_register(0x0302);//0x034b
	Write_com(0x00B8);
	Write_register(0x0000);
	
	Write_com(0x00BC);
	Write_register(0x0001);
	Write_com(0x00BF);
	Write_register(0x0011);//LCD Sleep out
	
  MDELAY(100);//Delay¿¿¿¿driver ic¿¿¿¿

	Write_com(0x00BC);
	Write_register(0x0001);
	Write_com(0x00BF);
	Write_register(0x0029);//LCD display on

	Write_com(0x00B7);
	Write_register(0x030b);//video mode on
	
}

#define SET_RESET_PIN	Set_RST
#define Read_Register	SPI_READ_ID
static void lcm_init(void)
{
	unsigned short id;
	unsigned short B6, B7, B9;

	SET_RESET_PIN(1);
	MDELAY(30);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(30);
	init_lcm_registers();
	
	id = Read_Register(0x00b0);
	B6 = Read_Register(0x00b6);
	B7 = Read_Register(0x00b7);
	B9 = Read_Register(0x00b9);
	
        printf("Uboot--SSD2828 id is: %x, B6=%x, B7=%x, b9=%x \n",id, B6, B7, B9);
    
}

static U32 GPX2DATA=0x11000c44;
static void lcd_reset()
{
	// GPX2_1
	U32 GPX2CON=0x11000c40;
	U32 val = readl(GPX2CON);
	val = val&(~0xF0)|0x10;
	Outp32(GPX2CON,val);

	val =readl(GPX2DATA);

	val |= 0x2;
	Outp32(GPX2DATA,val);	
	Delay_ms(100);

	val &= ~0x2;
	Outp32(GPX2DATA,val);	
//	Delay_us(1);

	val |= 0x2;
	Outp32(GPX2DATA,val);	
	Delay_ms(100);

	val &= ~0x2;
	Outp32(GPX2DATA,val);	
	Delay_ms(100);

	val |= 0x2;
	Outp32(GPX2DATA,val);	
	Delay_ms(500);

}


void Init_SSD2805(void)
{
	int i=0;

	lcd_reset();

	Set_RST(0);// ( rGPFDAT &= (~(1<<3))) ;
	Delay_ms(100);
	Set_RST(1);//  ( rGPFDAT |= (1<<3) ) ;
	Delay_ms(1000);

 SPI_2825_WrCmd(0xb7);
			        SPI_WriteData(0x50);//50=TX_CLK 70=PCLK
			        SPI_WriteData(0x00);   //Configuration Register

			        SPI_2825_WrCmd(0xb8);
			        SPI_WriteData(0x00);
			        SPI_WriteData(0x00);   //VC(Virtual ChannelID) Control Register

			        SPI_2825_WrCmd(0xb9);
			        SPI_WriteData(0x00);//1=PLL disable
			        SPI_WriteData(0x00);
                               //TX_CLK/MS should be between 5Mhz to100Mhz
			        SPI_2825_WrCmd(0xBA);//PLL=(TX_CLK/MS)*NS 8228=480M 4428=240M  061E=120M 4214=240M 821E=360M 8219=300M
			        SPI_WriteData(0x14);//D7-0=NS(0x01 : NS=1)
			        SPI_WriteData(0x42);//D15-14=PLL¿¿ 00=62.5-125 01=126-250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1)

			        SPI_2825_WrCmd(0xBB);//LP Clock Divider LP clock = 400MHz / LPD / 8 = 240 / 8 / 4 = 7.5MHz
			        SPI_WriteData(0x03);//D5-0=LPD=0x1 ¿ Divide by 2
			        SPI_WriteData(0x00);

			        SPI_2825_WrCmd(0xb9);
			       	SPI_WriteData(0x01);//1=PLL disable
			       	SPI_WriteData(0x00);
			        //MIPI lane configuration
			            	SPI_2825_WrCmd(0xDE);//¿¿¿
			            	SPI_WriteData(0x03);//11=4LANE 10=3LANE 01=2LANE 00=1LANE
			            	SPI_WriteData(0x00);

			        SPI_2825_WrCmd(0xc9);
			        SPI_WriteData(0x02);
			        SPI_WriteData(0x23);   //p1: HS-Data-zero  p2: HS-Data- prepare  --> 8031 issue
			       // Delay(100);
 
 
//code


 
  
//SSD2825_Initial
SPI_2825_WrCmd(0xb7);
SPI_WriteData(0x50);
SPI_WriteData(0x00);   //Configuration Register

SPI_2825_WrCmd(0xb8);
SPI_WriteData(0x00);
SPI_WriteData(0x00);   //VC(Virtual ChannelID) Control Register

SPI_2825_WrCmd(0xb9);
SPI_WriteData(0x00);//1=PLL disable
SPI_WriteData(0x00);

SPI_2825_WrCmd(0xBA);//PLL=(TX_CLK/MS)*NS 8228=480M 4428=240M  061E=120M 4214=240M 821E=360M 8219=300M 8225=444M 8224=432
SPI_WriteData(0x2b);//D7-0=NS(0x01 : NS=1)   //0X28
SPI_WriteData(0x82);//D15-14=PLL¿¿ 00=62.5-125 01=126-250 10=251-500 11=501-1000  DB12-8=MS(01:MS=1) //0X82

SPI_2825_WrCmd(0xBB);//LP Clock Divider LP clock = 400MHz / LPD / 8 = 480 / 8/ 8 = 7.5MHz
SPI_WriteData(0x07);//D5-0=LPD=0x1 ¿ Divide by 2
SPI_WriteData(0x00);

SPI_2825_WrCmd(0xb9);
SPI_WriteData(0x01);//1=PLL disable
SPI_WriteData(0x00);

SPI_2825_WrCmd(0xc9);
SPI_WriteData(0x02);
SPI_WriteData(0x23);   //p1: HS-Data-zero  p2: HS-Data- prepare  --> 8031 issue
Delay(5);

SPI_2825_WrCmd(0xCA);
SPI_WriteData(0x01);//CLK Prepare
SPI_WriteData(0x23);//Clk Zero

SPI_2825_WrCmd(0xCB); //local_write_reg(addr=0xCB,data=0x0510)
SPI_WriteData(0x10); //Clk Post
SPI_WriteData(0x05); //Clk Per

SPI_2825_WrCmd(0xCC); //local_write_reg(addr=0xCC,data=0x100A)
SPI_WriteData(0x05); //HS Trail
SPI_WriteData(0x10); //Clk Trail
Delay(5);

SPI_2825_WrCmd(0xD0); //local_write_reg(addr=0xCC,data=0x100A)
SPI_WriteData(0x00); //HS Trail
SPI_WriteData(0x00); //Clk Trail
Delay(5);

#include "orgin_config.c"
	
printf("2828:0x%X\n",SPI_READ_ID(0xb0));
GP_COMMAD_PA_READ();
	/**************************************************/
 
   SPI_2825_WrReg(0xb1,0x0204);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,0x0e56);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,0x0a52);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, 720);
	SPI_2825_WrReg(0xb5, 1280);
	SPI_2825_WrReg(0xb6, 0x0003);				//Video mode and video pixel format 
//MIPI lane configuration
SPI_2825_WrCmd(0xDE);//¿¿¿
SPI_WriteData(0x03);//11=4LANE 10=3LANE 01=2LANE 00=1LANE
SPI_WriteData(0x00);

SPI_2825_WrCmd(0xD6);//  05=BGR  04=RGB
SPI_WriteData(0x04);//D0=0=RGB 1:BGR D1=1=Most significant byte sent first
SPI_WriteData(0x00);

SPI_2825_WrCmd(0xB7);
SPI_WriteData(0x4B); //0X4B
SPI_WriteData(0x02);Delay(100);

SPI_2825_WrCmd(0x2C);
}
