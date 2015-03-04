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
	Delay_ms(200);
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

#define CONFIG_1 0

void Init_SSD2805_SPI(void)
{
	Set_RST(0);// ( rGPFDAT &= (~(1<<3))) ;
	Delay_ms(100);
	Set_RST(1);//  ( rGPFDAT |= (1<<3) ) ;
	Delay_ms(100);

	
	//void SPI_2825_WrCmd(U8)
	//void SPI_2825_WrReg(U8 c,U16 value)
	//Initial code 1: SSD2825 initialization
	//RGB interface configuration 
    SPI_2825_WrReg(0xb1,(LCD_VSPW<<8)|LCD_HSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(LCD_VBPD<<8)|LCD_HBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(LCD_VFPD<<8)|LCD_HFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, LCD_XSIZE_TFT);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, LCD_YSIZE_TFT);		//Vertical active period
	SPI_2825_WrReg(0xb6, 0x2017);				//Video mode and video pixel format 

	//MIPI lane configuration
	//00 - 1 lane mode 
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode 
	SPI_2825_WrReg(0xde, 0x0003);				//MIPI lane select 
	SPI_2825_WrReg(0xd6, 0x0004);				//Color order and endianess
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control
	//Communicate with LCD driver through MIPI 
/*	SPI_2825_WrReg(0xb7, 0x0342);				//DCS mode 0342
	SPI_2825_WrReg(0xb8, 0x0000);				//VC registe
	SPI_2825_WrReg(0xbc, 0x0000);				//Packet size 
	SPI_2825_WrCmd(0x11);					//LCD driver exit sleep mode
//	Delay_ms(100);	
	SPI_2825_WrCmd(0x29);*/					//Set LCD driver display on 
//	PLL configuration 
	SPI_2825_WrReg(0xba, LCD_DSI_CLCK);				//PLL setting,8028   8012
	SPI_2825_WrReg(0xbb, 0x0006);				//LP clock divider
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
	
	GP_COMMAD_PA_READ();
	SPI_2825_WrReg(0xb7, 0x0150);				//Generic mode, HS video mode
	
	Delay_ms(10);
                        



	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x00);
	GP_COMMAD_PA(4);SPI_WriteData(0xff);SPI_WriteData(0x12);SPI_WriteData(0x84);SPI_WriteData(0x01);	//EXTC=1

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x80);	        //Orise mode enable
	GP_COMMAD_PA(3);SPI_WriteData(0xff);SPI_WriteData(0x12);SPI_WriteData(0x84);

//-------------------- panel setting --------------------//
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x80);             //TCON Setting
	GP_COMMAD_PA(10);SPI_WriteData(0xc0);SPI_WriteData(0x00);SPI_WriteData(0x64);SPI_WriteData(0x00);SPI_WriteData(0x10);SPI_WriteData(0x10);SPI_WriteData(0x00);SPI_WriteData

(0x64);SPI_WriteData(0x10);SPI_WriteData(0x10);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90);             //Panel Timing Setting
	GP_COMMAD_PA(7);SPI_WriteData(0xc0);SPI_WriteData(0x00);SPI_WriteData(0x60);SPI_WriteData(0x00);SPI_WriteData(0x01);SPI_WriteData(0x00);SPI_WriteData(0x04);  //Hold Time 3u 0x52

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xa4);             //source pre. 
	GP_COMMAD_PA(2);SPI_WriteData(0xc0);SPI_WriteData(0x09);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb3);             //Interval Scan Frame: 0 frame);SPI_WriteData( column inversion
	GP_COMMAD_PA(3);SPI_WriteData(0xc0);SPI_WriteData(0x00);SPI_WriteData(0x55);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x81);             //frame rate:60Hz
	GP_COMMAD_PA(2);SPI_WriteData(0xc1);SPI_WriteData(0x55);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xB4);             //COLOUM  INVERSION
	GP_COMMAD_PA(2);SPI_WriteData(0xC0);SPI_WriteData(0x55);				

//-------------------- power setting --------------------//
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xa0);             //dcdc setting
	GP_COMMAD_PA(15);SPI_WriteData(0xc4);SPI_WriteData(0x05);SPI_WriteData(0x10);SPI_WriteData(0x06);SPI_WriteData(0x02);SPI_WriteData(0x05);SPI_WriteData(0x15);SPI_WriteData

(0x10);SPI_WriteData(0x05);SPI_WriteData(0x10);SPI_WriteData(0x07);SPI_WriteData(0x02);SPI_WriteData(0x05);SPI_WriteData(0x15);SPI_WriteData(0x10);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb0);             //clamp voltage setting
	GP_COMMAD_PA(3);SPI_WriteData(0xc4);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x91);             //VGH=15V);SPI_WriteData( VGL=-10V);SPI_WriteData( pump ratio:VGH=6x);SPI_WriteData( VGL=-5x
	GP_COMMAD_PA(3);SPI_WriteData(0xc5);SPI_WriteData(0x46);SPI_WriteData(0x42);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x00);             //GVDD=5.008V);SPI_WriteData( NGVDD=-5.008V
	GP_COMMAD_PA(3);SPI_WriteData(0xd8);SPI_WriteData(0xc7);SPI_WriteData(0xc7);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x00);             //VCOMDC=-1.62
	GP_COMMAD_PA(2);SPI_WriteData(0xd9);SPI_WriteData(0x80);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb3);             //VDD_18V=1.7V);SPI_WriteData( LVDSVDD=1.6V
	GP_COMMAD_PA(2);SPI_WriteData(0xc5);SPI_WriteData(0x84);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xbb);             //LVD voltage level setting
	GP_COMMAD_PA(2);SPI_WriteData(0xc5);SPI_WriteData(0x8a);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x82);             //chopper
	GP_COMMAD_PA(2);SPI_WriteData(0xc4);SPI_WriteData(0x0a);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xc6);		        //debounce 
	GP_COMMAD_PA(2);SPI_WriteData(0xB0);SPI_WriteData(0x03); 

	//GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xc2);             //precharge disable
	//GP_COMMAD_PA(2);SPI_WriteData(0xf5);SPI_WriteData(0x40);             //0x40 Disable 

	//GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xc3);             //sample hold gvdd
	//GP_COMMAD_PA(2);SPI_WriteData(0xf5);SPI_WriteData(0x85);

//-------------------- control setting --------------------//
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x00);             //ID1
	GP_COMMAD_PA(2);SPI_WriteData(0xd0);SPI_WriteData(0x40);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x00);             //ID2);SPI_WriteData( ID3
	GP_COMMAD_PA(3);SPI_WriteData(0xd1);SPI_WriteData(0x00);SPI_WriteData(0x00);

//-------------------- panel timing state control --------------------//
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x80); 
	GP_COMMAD_PA(12);SPI_WriteData(0xcb);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90); 
	GP_COMMAD_PA(16);SPI_WriteData(0xcb);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xa0); 
	GP_COMMAD_PA(16);SPI_WriteData(0xcb);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb0);  
	GP_COMMAD_PA(16);SPI_WriteData(0xcb);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xc0); 
	GP_COMMAD_PA(16);SPI_WriteData(0xcb);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x00);SPI_WriteData(0x05);SPI_WriteData(0x05);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xd0); 
	GP_COMMAD_PA(16);SPI_WriteData(0xcb);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xe0);
	GP_COMMAD_PA(15);SPI_WriteData(0xcb);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x00);SPI_WriteData(0x05);SPI_WriteData

(0x05);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xf0);
	GP_COMMAD_PA(12);SPI_WriteData(0xcb);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData

(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);

//-------------------- panel pad mapping control --------------------//

   GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x80); 
	GP_COMMAD_PA(16);SPI_WriteData(0xcc);SPI_WriteData(0x0E);SPI_WriteData(0x10);SPI_WriteData(0x0A);SPI_WriteData(0x0C);SPI_WriteData(0x02);SPI_WriteData(0x04);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x2E);SPI_WriteData(0x2D);SPI_WriteData(0x00);SPI_WriteData(0x29);SPI_WriteData(0x2A);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90);    
	GP_COMMAD_PA(16);SPI_WriteData(0xcc);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x0D);SPI_WriteData(0x0F);SPI_WriteData(0x09);SPI_WriteData(0x0B);SPI_WriteData(0x01);SPI_WriteData(0x03);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xa0); 
	GP_COMMAD_PA(15);SPI_WriteData(0xcc);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x2E);SPI_WriteData(0x2D);SPI_WriteData(0x00);SPI_WriteData(0x29);SPI_WriteData

(0x2A);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);




	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb0); 
	GP_COMMAD_PA(16);SPI_WriteData(0xcc);SPI_WriteData(0x0B);SPI_WriteData(0x09);SPI_WriteData(0x0F);SPI_WriteData(0x0D);SPI_WriteData(0x03);SPI_WriteData(0x01);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x2D);SPI_WriteData(0x2E);SPI_WriteData(0x00);SPI_WriteData(0x29);SPI_WriteData(0x2A);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xc0); 
	GP_COMMAD_PA(16);SPI_WriteData(0xcc);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x0C);SPI_WriteData(0x0A);SPI_WriteData(0x10);SPI_WriteData(0x0E);SPI_WriteData(0x04);SPI_WriteData(0x02);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xd0);  
	GP_COMMAD_PA(15);SPI_WriteData(0xcc);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x2D);SPI_WriteData(0x2E);SPI_WriteData(0x00);SPI_WriteData(0x29);SPI_WriteData

(0x2A);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);
    

   
   
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb5);             //TCON_GOA_OUT Setting
	GP_COMMAD_PA(7);SPI_WriteData(0xc5);SPI_WriteData(0x00);SPI_WriteData(0x6f);SPI_WriteData(0xfF);SPI_WriteData(0x00);SPI_WriteData(0x6f);SPI_WriteData(0xfF);

   
	


//-------------------- panel timing setting --------------------//
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x80);             //panel VST setting
	GP_COMMAD_PA(13);SPI_WriteData(0xce);SPI_WriteData(0x8B);SPI_WriteData(0x03);SPI_WriteData(0x18);SPI_WriteData(0x8A);SPI_WriteData(0x03);SPI_WriteData(0x18);SPI_WriteData

(0x89);SPI_WriteData(0x03);SPI_WriteData(0x18);SPI_WriteData(0x88);SPI_WriteData(0x03);SPI_WriteData(0x18);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90);             //panel VEND setting
	GP_COMMAD_PA(15);SPI_WriteData(0xce);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);


	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xa0);             //panel CLKA1/2 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xce);SPI_WriteData(0x38);SPI_WriteData(0x07);SPI_WriteData(0x05);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData

(0x00);SPI_WriteData(0x38);SPI_WriteData(0x06);SPI_WriteData(0x05);SPI_WriteData(0x01);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData(0x00);
   

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb0);             //panel CLKA3/4 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xce);SPI_WriteData(0x38);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x02);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData

(0x00);SPI_WriteData(0x38);SPI_WriteData(0x04);SPI_WriteData(0x05);SPI_WriteData(0x03);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData(0x00);
   

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xc0);             //panel CLKb1/2 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xce);SPI_WriteData(0x38);SPI_WriteData(0x03);SPI_WriteData(0x05);SPI_WriteData(0x04);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData

(0x00);SPI_WriteData(0x38);SPI_WriteData(0x02);SPI_WriteData(0x05);SPI_WriteData(0x05);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData(0x00);
   

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xd0);             //panel CLKb3/4 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xce);SPI_WriteData(0x38);SPI_WriteData(0x01);SPI_WriteData(0x05);SPI_WriteData(0x06);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData

(0x00);SPI_WriteData(0x38);SPI_WriteData(0x00);SPI_WriteData(0x05);SPI_WriteData(0x07);SPI_WriteData(0x00);SPI_WriteData(0x18);SPI_WriteData(0x00);
   


	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x80);             //panel CLKc1/2 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xcf);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90);             //panel CLKc3/4 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xcf);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xa0);             //panel CLKd1/2 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xcf);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb0);             //panel CLKd3/4 setting
	GP_COMMAD_PA(15);SPI_WriteData(0xcf);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData(0x00);

	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xc0);             //panel ECLK setting
	GP_COMMAD_PA(12);SPI_WriteData(0xcf);SPI_WriteData(0x3D);SPI_WriteData(0x02);SPI_WriteData(0x15);SPI_WriteData(0x20);SPI_WriteData(0x00);SPI_WriteData(0x00);SPI_WriteData

(0x01);SPI_WriteData(0x81);SPI_WriteData(0x00);SPI_WriteData(0x03);SPI_WriteData(0x08); //gate pre. ena.


     
                                                                                                                           
                                                                                                                           
//-------------------- for Power IC --------------------//                                                                 
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90);             //Mode-3                                                                
	GP_COMMAD_PA(5);SPI_WriteData(0xf5);SPI_WriteData(0x02);SPI_WriteData(0x11);SPI_WriteData(0x02);SPI_WriteData(0x11);                                                                      
                                                                                                                                                                                   
                                                                                                                           
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90);             //2xVPNL                                                                
	GP_COMMAD_PA(2);SPI_WriteData(0xc5);SPI_WriteData(0x50);                                                                                     
                                                                                                                           
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x94);             //Freq.                                                                 
	GP_COMMAD_PA(2);SPI_WriteData(0xc5);SPI_WriteData(0x66);                                                                                     
                                                                                                                           
                                                                                                                           
                                                                                                                           
                                                                                                                           
//------------------VGLO1/O2 disable----------------                                                                       
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb2);             //VGLO1                                                                 
	GP_COMMAD_PA(3);SPI_WriteData(0xf5);SPI_WriteData(0x00);SPI_WriteData(0x00);                                                                                
                                                                                                                           
	//GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb4);             //VGLO1_S                                                               
	//GP_COMMAD_PA(3);SPI_WriteData(0xf5);SPI_WriteData(0x00);SPI_WriteData(0x00);                                                                                
                                                                                                                           
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb6);             //VGLO2                                                                 
	GP_COMMAD_PA(3);SPI_WriteData(0xf5);SPI_WriteData(0x00);SPI_WriteData(0x00);                                                                                
                                                                                                                           
	//GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb8);             //VGLO2_S                                                               
	//GP_COMMAD_PA(3);SPI_WriteData(0xf5);SPI_WriteData(0x00);SPI_WriteData(0x00);                                                                                
                                                                                                                           
	//GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x94);  		//VCL on  	                                                                  
	//GP_COMMAD_PA(2);SPI_WriteData(0xF5);SPI_WriteData(0x02);
	                                                                                     
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x94);  		//VCL off  	                                                                  
	GP_COMMAD_PA(3);SPI_WriteData(0xF5);SPI_WriteData(0x00);SPI_WriteData(0x00);    
	                                                                                 
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xd2);             //VCL reg. en
	GP_COMMAD_PA(3);SPI_WriteData(0xf5);SPI_WriteData(0x06);SPI_WriteData(0x15);
                                                                                                                           
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xBA);  		//VSP on   	                                                                  
	GP_COMMAD_PA(2);SPI_WriteData(0xF5);SPI_WriteData(0x03);                                                                                     
                                                                                                                           
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0xb4);       //VGLO1/2 Pull low setting                                              
   GP_COMMAD_PA(2);SPI_WriteData(0xc5);SPI_WriteData(0xc0);		   //d[7] vglo1 d[6] vglo2 => 0: pull vss);SPI_WriteData( 1: pull vgl                              
                  
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x93); //CD err sel. 
	GP_COMMAD_PA(2);SPI_WriteData(0xB0);SPI_WriteData(0x8C);
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x90); 
	GP_COMMAD_PA(2);SPI_WriteData(0xB6);SPI_WriteData(0xB6); //sleep out re-load
	GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x92); 
	GP_COMMAD_PA(2);SPI_WriteData(0xB3);SPI_WriteData(0x02); //HS auto Disable CMD1

   GP_COMMAD_PA(2);SPI_WriteData(0x00);SPI_WriteData(0x00);             //Orise mode disable
   GP_COMMAD_PA(4);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);SPI_WriteData(0xff);



 
	  	   
//ccmoff
//ccmrun

GP_COMMAD_PA(1);   SPI_WriteData(0x11);
Delay_ms(100);
GP_COMMAD_PA(1);   SPI_WriteData(0x29);
Delay_ms(200); 
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------	
	Delay_ms(200);
	GP_COMMAD_PA_READ();
#if CONFIG_1
	
	//Cmd code 3: Access video mode 
//	//RGB interface configuration 
	SPI_2825_WrReg(0xb1,(LCD_VSPW<<8)|LCD_HSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(LCD_VBPD<<8)|LCD_HBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(LCD_VFPD<<8)|LCD_HFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, LCD_XSIZE_TFT);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, LCD_YSIZE_TFT);		//Vertical active period
	SPI_2825_WrReg(0xb6, 0x001b);				//Video mode and video pixel format //0x2007
	//MIPI lane configuration
	//00 - 1 lane mode 
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode
	SPI_2825_WrReg(0xde, 0x0003);				//MIPI lane select, 4chl
	SPI_2825_WrReg(0xd6, 0x0004);				//Color order and endianess
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control
	
	Delay_ms(200);
	//PLL configuration
	//FR: bit15~14
	//00 ¨C 62.5 < OUT f  < 125 
	//01 ¨C 126 < OUT f  < 250 
	//10 ¨C 251 < OUT f  < 500  
	//11 ¨C 501 < OUT f  < 1000 
	
	//MS: bit12~8
	//Fpre = fin/MS
	
	//NS: bit7~0
	//Fout = Fpre*NS
	
	SPI_2825_WrReg(0xba, LCD_DSI_CLCK);		//0x8012-PLL setting, select TX_CLK=24MHZ, (24/8)*250 = 750MHZ(8012:24*18=432M;866E:24/6*110=440M,8337:24/3*55=440) 844b
										//844b:24/4*75=450
										//8446:24/4*70=420
										//8332:24/3*50=400
										//8440:24/4*64=384
										//843c:24/4*60=360
	//
	//
	//
	//
	
	//SPI_2825_WrReg(0xba, 0x8A95);		//PLL setting, select PCLK = 33384960, 0x8A7C
	SPI_2825_WrReg(0xbb, 0x0003);				//LP clock divider,·ÖÆµf/£¨1+1£©,750MHZ/2 = //////24/6=4M
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable //0xb9,0x0001
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
	SPI_2825_WrReg(0xb7, 0x030B | 0x0020);		//Generic mode, HS video mode	
	//SPI_2825_WrReg(0xb7, 0x030B | 0x0020);//select PCLK
		Delay_ms(200);
	printf("0xB0:0x%04X\n",SPI_READ_ID(0xB0));
	printf("0xB6:0x%04X\n",SPI_READ_ID(0xB6));
	printf("0xB7:0x%04X\n",SPI_READ_ID(0xB7));
	printf("0xB9:0x%04X\n",SPI_READ_ID(0xB9));
#else
	lcm_init();
#endif
}
