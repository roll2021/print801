/**
  ******************************************************************************
  * @file    Barcode2D.c
  * @author  Application Team  
  * @version V0.0.1
  * @date    2012-April-16
  * @brief   二维条码相关的程序.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2016</center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
//#define EXTSPIFLASH

#include "barcodepdf417.h"
#include "extgvar.h"		//全局外部变量声明

#define MAX_CHAR_LEN 1024			//定义PDF417最大处理字符数

extern TypeDef_StructPrtCtrl			g_tPrtCtrlData;		//打印控制
/*******************************************************************************
* Function Name	: u16 *encode_text ( u16 *dst , u8 *src , int len )
* Description	: 准备开始新的一行操作
* Input			: 
* Output		: 
* Return		: 
*******************************************************************************/
u16 *encode_text ( u16 *dst , u8 *src , int len )
{
	char m_o,m_n;
	u8 ch;
	int total;
	u16 dat;

	if(pdf_mode != TEXT_MODE)
	{
		pdf_mode = TEXT_MODE;
		*dst++ = TEXT_MODE;
	}
	
	m_o=UPPER;
	total=0;
	while(len)
	{
		ch=*src++;
		//转换数据并获取类型
		text_get(m_n,ch)

		if(m_o&m_n)//新数据在老模式中（不用切换）
			text_add(ch)
		else
		{	//需要模式切换
			//为了更好的压缩（少出现转意府），需要参考后面2-3个数据所在的子模式
			//为了简单暂不考虑那么多
			switch(m_o)
			{
				case UPPER:
					if(m_n&LOWER)
					{	//转成小写
						text_add(LL);
						m_o=LOWER;
					}
					else if(m_n&MIXED)
					{	//转成混合
						text_add(ML);
						m_o=MIXED;
					}
					else
					{	//标点不能直接转换
						//text_add(PS);
						text_add(ML);
						text_add(PL);
						m_o=PUNCT;
					}
					text_add(ch);
					break;
				case LOWER:
					if(m_n&UPPER)
					{	//不能直接转换
						text_add(ML);
						text_add(AL);
						m_o=UPPER;
					}
					else if(m_n&MIXED)
					{	//转成混合
						text_add(ML);
						m_o=MIXED;
					}
					else
					{	//标点不能直接转换
						text_add(ML);
						text_add(PL);
						m_o=PUNCT;
					}
					text_add(ch);
					break;
				case MIXED:
					if(m_n&UPPER)
					{
						text_add(AL);
						m_o=UPPER;
					}
					else if(m_n&LOWER)
					{
						text_add(LL);
						m_o=LOWER;
					}
					else
					{	//标点
						text_add(PL);
						m_o=PUNCT;
					}
					text_add(ch);
					break;
				case PUNCT:
					if(m_n&UPPER)
					{	//标点
						text_add(PAL);
						m_o=UPPER;
					}
					if(m_n&LOWER)
					{	//转成小写
						text_add(PAL);
						text_add(LL);
						m_o=LOWER;
					}
					if(m_n&MIXED)
					{	//转成混合
						text_add(PAL);
						text_add(ML);
						m_o=MIXED;
					}
					text_add(ch);
					break;
			}
		}
		len--;
		if(!len)
		{
			text_end();
			break;
		}
    }
	return dst;
}
/*******************************************************************************
* Function Name	: u16 *ch10to900 ( u16 *dst , u8 *src , int len )
* Description	: 数字模式
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
u16 *ch10to900 ( u16 *dst , u8 *src , int len )
{
    u16* ret = dst;
    int last = len / 3;
    int ni, k;

    memset(ret, 0, (last + 1) * sizeof(u16));
    ret[last] = 1;
    for (ni = 0; ni < len; ++ni) {
        // multiply by 10
        for (k = last; k >= 0; --k)
            ret[k] *= 10;
        // add the digit
        ret[last] += src[ni] - '0';
        // propagate carry
        for (k = last; k > 0; --k) {
            ret[k - 1] += ret[k] / 900;
            ret[k] %= 900;
        }
    }
	return dst+last+1;
}
/*******************************************************************************
* Function Name	: u16 *encode_num ( u16 *dst , u8 *src , int len )
* Description	: 将源地址处的字符压缩存放于目的地址处。
* Input			: dst：目的地址，src：源地址，len：数据长度
* Output		: 改变目的地址内容
* Return		: 返回目的地址
*******************************************************************************/
u16 *encode_num ( u16 *dst , u8 *src , int len )
{
	pdf_mode=NUME_MODE;
	*dst++=NUME_MODE;
	
	while(len)
	{
		if(len <=44)
		{
	        dst = ch10to900(dst, src, len);
			return dst;
		}
		else
			dst = ch10to900(dst, src, 44);
		src +=44;
		len -=44;
    }
	return dst;
}
/*******************************************************************************
* Function Name	: u16 *ch256to900 ( u16 *dst , u8 *src )
* Description	: 字节模式
* Input			: 
* Output		: None
* Return		: 
//总是6字节转5字节,固定
*******************************************************************************/
u16 *ch256to900 ( u16 *dst , u8 *src )
{
    u32 ret[5];
    int ni, k;

    memset(ret, 0, sizeof(ret));
    for (ni = 0; ni < 6; ++ni) {
        // multiply by 256
        for (k = 4; k >= 0; --k)
            ret[k] *= 256;
        // add the digit
        ret[4] += src[ni] ;
        // propagate carry
        for (k = 4; k > 0; --k) {
            ret[k - 1] += ret[k] / 900;
            ret[k] %= 900;
        }
    }
	for(ni=0;ni<5;ni++)
		*dst++=ret[ni];
	return dst;
}
/*******************************************************************************
* Function Name	: u16 *encode_byte(u16 *dst,u8 *src,int len)
* Description	: 
* Input			: 
* Output		: None
* Return		: 
*******************************************************************************/
u16 *encode_byte(u16 *dst,u8 *src,int len)
{
	if(len%6)
	{
		pdf_mode=BYTE_MODE;
		*dst++=BYTE_MODE;
		while(len>=6)
		{
	        dst=ch256to900(dst, src);
			src+=6;
			len-=6;
		}
		while(len--)
			*dst++=*src++;
	}
	else
	{
		pdf_mode=BYTE_MODE_6;
		*dst++=BYTE_MODE_6;
		while(len>=6)
		{
	        dst=ch256to900(dst, src);
			len-=6;
			src+=6;
		}
	}
	return dst;
}
/*******************************************************************************
* Function Name	: void encode ( PDF417_EN *pdf , pdf417 *p )
* Description	: 
* Input			: 
* Output		: 改变pdf结构内容
* Return		: None
*******************************************************************************/
void encode ( PDF417_EN *pdf , pdf417 *p )
{
	u8 *src;
	u16 *dst;
	u8 *tail;
	u8 *num_p;		//连续13个及以上数字的头指针
	u8 *txt_p;		//连续5个及以上文本的头指针
	int num_n;
	int txt_n;
	int i;
	u8 ch;

	pdf_mode = TEXT_MODE;

	//编码相关数据
	
	src = p->in;
	dst = pdf->buf +1;
	tail = src+ p->len;		
	num_p = txt_p = tail;	//设置头指针

	while(src < tail)
	{
		u8 *p;

		//按照压缩程度进行优先数据考虑
		//先找寻连续超过13个数据的头部位置(在当前未编码位置到全文结束内搜寻)
		p =src;		//从当前开始查找
		num_n =0;
		while( p < tail)
		{
			ch = *p;
			if(ch>='0'&&ch<='9')
				num_n++;	//记录连续个数
			else
			{	//如果不足13丢弃,超过13则同时n值记录有效数据长度
				if(num_n>=13)
					break;
				num_n=0;
			}
			p++;
		}
		if(num_n >=13)
			num_p = p -num_n;	//保存起始位置
		else
			num_p = tail;	//设置头指针

		//中间区域编码
		while(src < num_p)
		{
			//再找寻连续超过5字节文本(在当前未编码位置和前面找寻结果的头部之间搜寻)
			p =src;
			txt_n =0;
			while(p <num_p)
			{
				ch=*p;
    	        if((ch>=' '&&ch<127)||ch=='\r'||ch=='\n'||ch=='\t')
					txt_n++;
				else
				{
					if(txt_n>=5)	//如果不足5丢弃,超过5则同时n值记录有效数据长度
						break;
					txt_n=0;
				}
				p++;
			}
			if(txt_n >=5)
				txt_p = p-txt_n;	//保存起始位置
			else
				txt_p =num_p;

			//最后将剩余数据按字节模式编码(在当前未编码位置和前面找寻结果的头部之间搜寻)
			i= txt_p - src;
			if(i)
			{
				dst = encode_byte(dst, src, i);
				src +=i;
			}

			if(txt_n>=5)
			{
				dst =encode_text(dst, txt_p, txt_n);
				src = txt_p + txt_n;
			}
		}

		if(num_n >=13)
		{
			dst = encode_num(dst, num_p, num_n);
			src = num_p+num_n;
		}
	}
	pdf->len = dst- pdf->buf;		//压缩后数据
}
//(17*(wide+4)):(high*3)=17:9	w=sqrt((num+12)/3)-2;
//					  	1,	2, 3, 4, 5, 6,   7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18
//const u16 WIDE_TBL[30]={15,36,63,96,135,180,231,288,351,420,495,576,663,756,855,960,1071,0};
/*******************************************************************************
* Function Name	: struct_make ( PDF417_EN *pdf , pdf417 *p )
* Description	: 数据排版
* Input			: 
* Output		: None
* Return		: 正常返回0，错误返回1
*******************************************************************************/
u8 struct_make ( PDF417_EN *pdf , pdf417 *p )
{
	int len;	//数据
	int max;	//最宽能显示数据
	int i, j;

	//纠错码相关数据初始化
#ifdef EXTFLASH

	pdf->err = 0;
	if(p->level >8)			 //纠错等级修正
		p->level =0;
#else
	u16 *ERROR_LEVEL_ADDR[9] = {(u16 *)ERROR_LEVEL0, (u16 *)ERROR_LEVEL1, (u16 *)ERROR_LEVEL2, (u16 *)ERROR_LEVEL3,
							  	(u16 *)ERROR_LEVEL4, (u16 *)ERROR_LEVEL5, (u16 *)ERROR_LEVEL6, (u16 *)ERROR_LEVEL7,
							  	(u16 *)ERROR_LEVEL8};
	if(p->level >8)
		p->level =0;

	pdf->err = ERROR_LEVEL_ADDR[p->level];	//纠错多项式地址
#endif
	pdf->err_l = p->level;			//纠错等级
	pdf->err_n = 2 << p->level;		//纠错字长度


	//确认矩形宽高
	//1先确认打印机目前能输出的最大宽度
	max = p->max / PDF417_MODE_WIDE;	//每个字宽占用17（结束为18）
	if(!(p-> max%PDF417_MODE_WIDE))
	{	//能整除，由于结束为18，需要舍去
		if(max)
			max--;
	}
	
	if(max <5 )
	{						//宽度太小无法输出
//		pdf->wide =0;
		return 1;
	}
	
	max -=4;//除去起始、左指、右指、结束符后的有效数据宽
	
	len = pdf->len + pdf->err_n;		//总的需编码数据长度

	pdf->wide =max;						//宽度字节数

	//计算高度,并修正
	pdf->high = len/max;
	if(len % max )
		pdf->high++;
	
	if(pdf->high >PDF417_HIGH_MAX)
	{									//高度太大无法输出
//		pdf->high =0;
		return 1;
	}

/**** 2012.9.6 加入缓冲容量限制 ****************/	
	//计算需要缓冲区容量,如果不够，则返回错误
	i =(((pdf->wide +4)*PDF417_MODE_WIDE+1)+7)/8;	//横向字节数
	len = (MAX_CHAR_LEN) + ( pdf->len + pdf->err_n )*(sizeof(u16)) + i* pdf->high;  
	if(len > BARCODE2D_SIZE)
		return 1;
/**** 2012.9.6 ****************/	

	len= max * pdf->high;			//在此宽高下可容纳总数据长度
	i = len - pdf->err_n;			//编码有效数据长度
	*pdf->buf =i;					//更新数据长度
	len =i - pdf->len;				//需要填充的数据长度
	j = pdf->len;					//
	pdf->len =i;					//更新数据长度

		
	while(len--)
		pdf->buf[j++] = TEXT_MODE;	//填充数据
	return 0;
}
/*******************************************************************************
* Function Name	: correct_append ( PDF417_EN *pdf )
* Description	: 生成纠错数据并直接添加到数据尾
* Input			: 条码数据结构
* Output		: None
* Return		: None
*******************************************************************************/
void correct_append ( PDF417_EN *pdf )
{
	int t1 = 0;
	int t2 = 0;
	int t3 = 0;
	int err_n;
	u16 *p;
	u16 *buf;
	int len;
	int i, e;
	u16 *err;				//err必须保留，用于二维码参数存放于内部FLASH中

	#ifdef EXTFLASH
		u32	Flashaddr;
		u8	Data[2];
		u16 ERROR_LEVEL_ADDR[9]={0,4,12,28,60,124,252,508,1020};	//纠错多项式首地址，参数在外部用
	#endif
	
	//确认多项式
	buf = pdf->buf;							//输入地址
	len = pdf->len;							//数据长度
	p = buf + len;							//指向待添加位置
	err = pdf->err;							//纠错多项式地址
	err_n = pdf->err_n;						//纠错字长度	
	memset(p, 0, err_n * (sizeof(u16)+1));	//初始化纠错数据
	
	for(i=0; i<len; i++)
	{
		t1 = buf[i] + p[0];
		for(e =0; e<err_n; e++)
		{
			#ifdef EXTFLASH
			 	Flashaddr =PDF417_ERROR_LEVEL_ADDR + (ERROR_LEVEL_ADDR[	pdf->err_l] + 2*(err_n - e-1));
				SPI_FLASH_BufferRead(Data, Flashaddr , 2 );
				t2=(t1 * (*(u16*)Data)) %MOD;
			#else
				t2=(t1 * err[err_n - e -1]) %MOD;
			#endif

			t3=MOD - t2;
			p[e] = (p[e+1] + t3) %MOD;
		}
	}
	for (i = 0; i < err_n; i++)
		p[i] = (MOD - p[i]) % MOD;
}

int bits;

void bits_begin ( void )
{
	bits=0;
}
/*******************************************************************************
* Function Name	: void bits_add ( u8 *buf , u16 dat )
* Description	: 
* Input			: 
* Output		: 
* Return		: None
*******************************************************************************/
void bits_add ( u8 *buf , u16 dat )
{
	int i;
	int bit;
	u8 *p;
	
	p=&buf[bits>>3];
	bit=bits&0x07;
	i=dat|0x10000;

	//17位数据始终要填充到3个字节里
	*p|=i>>(17-8+bit);
	p++;
	*p++=i>>(17-16+bit);
	*p=i<<(7-bit);
	bits+=17;
}

/*******************************************************************************
* Function Name	: int bits_end ( u8 *buf )
* Description	: 
* Input			: 
* Output		: 
* Return		: 
*******************************************************************************/
int bits_end ( u8 *buf )
{
	int bit;
	u8 *p;
	
	p = &buf[bits >> 3];
	bit = bits&0x07;
	*p |= 1<<(7-bit);
	bits++;

	return bits;
}
/*******************************************************************************
* Function Name	: bitmap_make ( pdf417 *p , PDF417_EN *pdf )
* Description	: 将数据转换成位图
* Input			: 
* Output		: 
* Return		: None
*******************************************************************************/
void bitmap_make ( pdf417 *p , PDF417_EN *pdf )
{
	int w,h;
	u16 ch;
	u8 *dst;
	u16 *src;
	
#ifdef EXTFLASH
	u32	ClusterAddress, FlashAddress;
	u8  Data[2];
#else
	u16 *tbl;		
#endif

	src = pdf->buf;
	dst = p->out;
	
	for(h=0; h < pdf->high; h++)
	{
		bits_begin();

		//add head
		bits_add(dst, PDF417_START_FLAG);
#ifdef EXTFLASH
		ClusterAddress = PDF417_CLUSTERS_ADDR + 2*929*(h %3);	//获取族号首地址
#else
		{
			tbl=(u16 *)CLUSTERS[ h % 3];	//获取族号
		}
#endif

		//add L
		switch(h %3)
		{
			case 0: 
				ch= (pdf->high- 1)/3;
				break;
			case 1:
				ch= pdf->err_l *3+((pdf->high -1) %3);
				break;
			default:
				ch= pdf->wide -1;
			break;
		}

		ch= 30*(h/3) + ch;

#ifdef EXTFLASH
		FlashAddress = ClusterAddress + 2*ch;					//获取族号(族号为2字节)
		SPI_FLASH_BufferRead(Data, FlashAddress, 2 );
		bits_add(dst,*(u16*)Data);
#else
		bits_add(dst, tbl[ch]);
#endif
		//add data
		for(w=0; w < pdf->wide; w++)
		{
			#ifdef EXTFLASH
					FlashAddress = ClusterAddress + 2*(*src++);	//获取族号
					SPI_FLASH_BufferRead(Data, FlashAddress, 2 );
					bits_add(dst, *(u16*)Data);
			#else
					bits_add(dst, tbl[*src++]);
			#endif
		}
		//add R
		switch(h%3)
		{
			case 0:
				ch =pdf->wide -1;
				break;
			case 1:
				ch =(pdf->high -1)/3;
				break;
			default:
				ch =pdf->err_l*3 + ((pdf->high -1)%3);
				break;
		}
		ch =30*(h/3)+ch;

#ifdef EXTFLASH
		FlashAddress = ClusterAddress + 2*ch;	//获取族号
		SPI_FLASH_BufferRead(Data, FlashAddress, 2 );
		bits_add(dst,*(u16*)Data);
#else
		bits_add(dst,tbl[ch]);
#endif
		//add tail
		bits_add(dst,PDF417_STOP_FLAG);
		ch = bits_end(dst);
		dst +=((ch-1)>>3)+1;
	}
	p->wide = ch;		//修正宽度，点宽，占用空间以字节对齐
	p->high = pdf->high;
}
/*******************************************************************************
* Function Name	: pdf417_encode ( pdf417 *p )
* Description	: 完成编码、位图生成
* Input			: p：输入结构
* Output		: 影响结构p的内容
* Return		: 正常返回0，错误返回1
*******************************************************************************/
u8 pdf417_encode ( pdf417 *p )
{
	encode(&pdf_en, p);			//编码
	
	if(struct_make(&pdf_en, p))	//排版
		return 1;				//有错误，则返回 2012.9.6
	
	correct_append(&pdf_en);	//添加纠错码
	p->out = (u8*)(pdf_en.buf + pdf_en.len + pdf_en.err_n);		//位图输出位置（字节中最高位表示图形的最左边）
	bitmap_make(p, &pdf_en);	//生成位图
	
	return 0;
}
/*******************************************************************************
* Function Name	: u32 Pdf417_Print(u8 *pBuf, u8 Width, u8 CorID, u16 CharLen)
* Description	: 处理PDF417条码
* Input			: pBuf：数据缓冲区，Width：每行字符数，CorID：纠错等级，CharLen：数据长度
* Output		: None
* Return		: 有效字符数，高16位为横向点数，低16位为纵向字节数。0x0000，表示错误
*******************************************************************************/
u32 Pdf417_Print(u8 *pBuf, u8 Width, u8 CorID, u16 CharLen)
{
	pdf417 pdf;
	u8	*dst,  row, col,  value, *pT0, *pT1, i, j;
	u16	height, width, bitsw;						//2012.7.12,原bitsw为u8
	u32 res;
	
	if(CharLen > MAX_CHAR_LEN)		//如果字符数据超长，返回错误
		return 0x0000;
	
	dst = pBuf + MAX_CHAR_LEN;		//

	memset(dst, 0x00, MAX_CHAR_LEN);
	
	pdf.in  = pBuf;					//输入地址
	pdf.len = CharLen;				//数据长度
	pdf.wide = Width;				//每行字符数
	if(pdf.wide > PDF417_WIDE_MAX) 	//修正一行字符数
		pdf.wide = PDF417_WIDE_MAX;

	pdf.max= (pdf.wide + 4) * PDF417_MODE_WIDE+1;	//最大输出宽度(结束符为18点宽)，加入左右栏、左右结束符

	if(pdf.max > (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex )/2)	//横向固定为2倍	,如果横向字符数超过了打印机行宽，自动调整为最大字符数
		pdf.max= (g_tPrtCtrlData.PrtLength - g_tPrtCtrlData.PrtDataDotIndex )/2;	//2012.6.27 修正宽度限制为实际设定宽度
	
	pdf.level=CorID;				//纠错等级

	pdf_en.buf = (u16*)dst;
	
	if(pdf417_encode(&pdf))			//如图形转换， 
		return 0x0000;				//有错误，则返回 2012.9.6

	dst = (u8*)(pdf_en.buf + pdf_en.len + pdf_en.err_n);	//位图输出位置（字节中最高位表示图形的最左边）

	width = pdf.wide;
	height = pdf.high/8;
	if ((pdf.high % 8) != 0)
		height++;

	res = (width <<16 ) + height;

	width = width / 8;
	if ((pdf.wide % 8) != 0)
		width ++;

	//为了保证纵向不为整字节情况的转换正确
	memset(dst + width * pdf.high, 0x00, width*8);	//2012.12.10 清除地址不对
	
	//横向排列转换为纵向排列，左边为最高位
	pT0 = pBuf;
	for (row = 0; row < height; row++)
	{
		bitsw = 0;
	
		pT1 = dst + row * width * 8;
		for (col = 0; col < width; col++)
		{
			for (i = 0; i < 8; i++)
			{
				bitsw += 1;
				value = 0;
				
				for (j = 0; j < 8; j++)
					value |= ((*(pT1 + width * j)<< i) & 0x80)>>j;
				*pT0++ =value;
				
				if (bitsw >= pdf.wide)
					break;
			}
			if (bitsw >= pdf.wide)
				break;
			pT1++;
		}
	}
	return	 res;
}


/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
