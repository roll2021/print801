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

#include	"barcode2D.h"
#include	"extgvar.h"		//全局外部变量声明
#include	"timconfig.h"
#include	"feedpaper.h"
#include	"print.h"
#include	"heat.h"
#include	"charmaskbuf.h"
#include	"ean.h"


/* ---------------二维码相关函数-----------------------*/

void generate_gf(int codelen)
{
	register int i, mask ;
	for (i = 0; i < 256; i++)
	{
		alpha_to[i] = 0;
		index_of[i] = 0;
	}
	mask = 1 ;
	alpha_to[mm] = 0 ;
	for (i=0; i<mm; i++)
	{
		alpha_to[i] = mask ;
		index_of[alpha_to[i]] = i ;
		if (pp[i]!=0)
			alpha_to[mm] ^= mask ;
		mask <<= 1 ;
	}
	index_of[alpha_to[mm]] = mm ;
	mask >>= 1 ;
	for (i=mm+1; i<= nn ; i++)
	{
		if (alpha_to[i-1] >= mask)
			alpha_to[i] = alpha_to[mm] ^ ((alpha_to[i-1]^mask)<<1) ;
		else
			alpha_to[i] = alpha_to[i-1]<<1 ;
		index_of[alpha_to[i]] = i ;
	}
	index_of[1] = 0 ;
}

unsigned char rs_mul(unsigned char poly1, unsigned char poly2)
{
	unsigned int tmp;
	tmp = index_of[poly1] + index_of[poly2];
	tmp = tmp % nn;
	return	alpha_to[tmp];
}

void gen_poly(int codelen, int datlen)
{
	int i,index;
	u8	Temp;

	for (i = 0; i < maxgen; i ++)
	{
		#ifdef EXTFLASH
			SPI_FLASH_BufferRead(&Temp, g_poly_ADDR+70*i , 1);
		#else
			Temp= g_poly[i][0];
		#endif
		if ( Temp== (codelen - datlen))
		{
			index = i;
			break;
		}
	}
	for (i=0; i<=codelen-datlen; i++)
	{
		#ifdef EXTFLASH
			SPI_FLASH_BufferRead(&gg[codelen - datlen -i], (g_poly_ADDR+70*index)+(i+1) , 1);
		#else
			gg[codelen - datlen -i] = g_poly[index][i+1];
		#endif
	}
	for (i=0; i<=codelen-datlen; i++)
		gg[i] = alpha_to[gg[i]] ;

}

void encode_rs(unsigned char *cor, int codelen, int datlen)
{
	int i, j ;
	unsigned char tmp, index, index1;

	alpha_to = pTMP;
	index_of = alpha_to +256;
	gg = index_of + 256;

	generate_gf(codelen);
	gen_poly(codelen, datlen);
	
	index1=codelen-1;
	for (i=0; i<datlen; i++)
	{
		tmp=cor[index1];
		index=codelen-datlen;
		if (tmp==0x00)
		{
			index1--;
			continue;
		}
		for (j=0; j<=codelen-datlen; j++)
		{
			cor[index1-j]=cor[index1-j] ^ rs_mul(tmp, gg[index]);
			index--;
		}
		index1--;
	}

}

u32 bch_code(u32 dat, u32 gx, u8 codelen, u8 datlen)
{
	u32	div, msk = 0;
	u32	i;

	for (i = 0; i < datlen; i++)
	{
		msk = msk << 1;
		msk |= 1;
	}
	dat &= msk;
	dat = dat << (codelen - datlen);

	msk=1<<(codelen+1);
	div=gx<<(datlen+1);

	for (i=0; i<=datlen; i++)
	{
		msk=msk>>1;
		div=div>>1;
		if ((msk&dat)>0)
			dat^=div;
	}
	return	dat;
}

/*******************************************************************************
* Function Name  : put_pixel(u8 *ptr, u16 x, u16 y, u16 dots, u8 value)
* Description    : 放置像索点数据，点阵中数据为纵向排列，
* Input          : ptr：基地址，x,Y：坐标点，dots：横向宽度，value：像索点数据
* Output         : None
* Return         : None
*******************************************************************************/
void put_pixel(u8 *ptr, u16 x, u16 y, u16 dots, u8 value)
{
	u8	mod, div;
	u8	*ptrt;
	u16	org;
	div = y >> 3;
	mod = y - (div << 3) & 0xFF;
	org = dots * div + x;
	ptrt = ptr +org;
	mod = 7 - mod;
	*ptrt &= (~(0x01 << mod));
	if (value > 0)
		*ptrt |= (0x01 << mod);
}

void put_array(u8 *des, u8 *src, u8 xlen, u8 ylen, u16 maxdot, u16 x0, u16 y0)
{
	u8	i, j, tmp;
	for (i = 0; i < xlen; i++)
	{
		tmp = *(src + i);
		for (j = 0; j < ylen; j++)
		{
			put_pixel(des, x0 + i, y0 + j, maxdot, tmp & 0x80);
			tmp = tmp << 1;
		}			
	}
}

void put_pos_find()
{
	u16	i, j;
	u8	mask, md, *ptr;
	ptr = pCanvas;
	for (i = 0; i < width /2; i++)
	{
		*ptr = 0x02;
		ptr += 2;
	}
	ptr = pCanvas + 6;
	for (i = 0; i < bytes; i++)
	{
		if (((width% 8)!=0) && (i == (bytes-1)))
		{
			md = width % 8;
			mask = 0;
			for (j = 0; j < md; j++)
			{
				mask >>= 1;
				mask |= 0x80;
			}
			*ptr = mask & 0xAA;
		}
		else
			*ptr = 0xAA;
		ptr += width;
	}
}

void	put_pos_align()
{
	u8	x0, y0, i, j;
	
	for (i = 0; i < 7; i++)
	{
		x0 = VerCapVerid.chkpos[i];
		if (x0 == 0)
			break;
		for (j = 0; j < 7; j++)
		{
			y0 = VerCapVerid.chkpos[j];
			if (((x0 == 6) && (y0 == 6)) || ((x0 == 6) && (y0 == width - 7)) ||((x0 == width - 7) && (y0 == 6)))
				continue;
			if (y0 == 0)
				break;
			put_array(pCanvas, (u8 *)GraphAlg, 5, 5, width, x0 - 2, y0 - 2);

		}
	}
}

u16 press_byte_head(u8 *des, u16 srclen)
{
	u16	templen;
	*des = 0x40;
	templen = srclen;
	if (templen >253)
	{
		*des |= 0x0F;
		des++;
		*des &= 0x00;
		*des |=0xD0;
		templen -=253;
	}
	else
	{
		*des |= ((templen >> 4) & 0x0F);
		des++;
		*des &= 0x00;
		*des |= ((templen << 4) & 0xF0);
		templen = 0;
	}
	return	templen;
}

u16	press_byte(u8 *des, u8 *src, u16 srclen)
{
	u8	tmp;
	u16 i;
	u16	deslen;
	*des = 0x40;		//模式指示符，固定为8位字节模式

	//填充字符读数指示符，10版本及以上为两字节，其下为一字节
	if (verid >= 10)
	{
		*des++ |= (srclen >> 12) & 0x0F;
		*des++ = (srclen >> 4) & 0xFF;
		*des = (srclen << 4) & 0xF0;
		deslen = 3;
	}
	else
	{
		*des++ |= ((srclen >> 4) & 0x0F);
		*des = ((srclen << 4) & 0xF0);
		deslen = 2;
	}
	
	//将数据连接
	for (i = 0; i <srclen; i++)
	{
		*des++ |=((*src >> 4) & 0x0F);
		*des = (*src << 4) & 0xF0;
		src++;
		deslen++;
	}
	des++;
	tmp = 0xEC;
	if (deslen < VerCapVerid.Cors[corid - 1].aCodesize)
	{
		for (i=0; i< VerCapVerid.Cors[corid - 1].aCodesize -deslen; i++)
		{
			*des++=tmp;
			if (tmp==0xEC)
				tmp=0x11;
			else
				tmp=0xEC;
		}
	}

	return	deslen;
}

u8	dot_in_pos(u16 x, u16 y)
{
	u8	i, j, dotin = 0x00;
	u8	x0, y0;
	for (i = 0; i < 7; i++)
	{
		x0 = VerCapVerid.chkpos[i];
		if (x0 == 0)
			break;
		for (j = 0; j < 7; j++)
		{
			y0 = VerCapVerid.chkpos[j];
			if (((x0 == 6) && (y0 == 6)) || ((x0 == 6) && (y0 == width - 7)) ||((x0 == width - 7) && (y0 == 6)))
				continue;
			if (y0 == 0)
				break;
			if ((x >= x0 - 2) && (x <=x0 + 2) && (y >= y0 - 2) && (y <= y0 + 2))
				return	0;
		}
	}
	x0 = y0 = width;
	
	if ((x <= 8) && (y <= 8))
		return	0;
	if ((x <= 8) && (y >= y0 - 8) && (y <= y0 - 1))
		return	0;
	if ((x >= x0 - 8) && (x <= x0 - 1) && (y <= 8))
		return	0;
	
	if (verid >= 7)
	{
		if ((x <= 5) && (y >= y0 - 11) && (y <= y0 - 9))
			return	0;
		if ((x >= x0 - 11) && (x <= x0 - 9) && (y <= 5))
			return	0;	
	}

	if (dotin > 0)
		return	0x00;
	else
		return	0x01;
}

void	mov_one_pix(pMovePix pPix, u8 maxdot)
{
	if (pPix->left == 0)
	{
		pPix->p0.x --;
		pPix->left = 1;			
	}
	else
	{
		pPix->left = 0;
		if (pPix->up == 1)
		{
			if (pPix->p0.y == 0)
			{
				pPix->up = 0;
				pPix->left = 0;
				pPix->p0.x--;
				if (pPix->p0.x == 0x06)
					pPix->p0.x--;
			}
			else
			{
				pPix->left = 0;
				pPix->p0.x++;
				pPix->p0.y--;
				if (pPix->p0.y == 0x06)
					pPix->p0.y--;
			}
		}
		else
		{
			if (pPix->p0.y == maxdot-1)
			{
				pPix->up = 1;
				pPix->left = 0;
				pPix->p0.x--;
				if (pPix->p0.x == 0x06)
					pPix->p0.x--;
			}
			else
			{
				pPix->left = 0;
				pPix->p0.x++;
				pPix->p0.y++;
				if (pPix->p0.y == 0x06)
					pPix->p0.y++;
			}
		}
	}
}

void Put_Byte(u8 dat, pMovePix pix)
{
	u8	k;
	for (k = 0; k < 8; k++)
	{
		put_pixel(pCanvas, pix->p0.x, pix->p0.y, width, (dat & 0x80));
		do
		{
			mov_one_pix(pix, width);
		}while ((dot_in_pos(pix->p0.x, pix->p0.y)==0) || ((pix->p0.x+pix->p0.y) == 0));
		dat = dat << 1;			
	}	
}

void put_dat(u8 *ptr)
{
	u16	i;
	MovePix	pix;
	u8	j, temp;
	u8	*pTmp0, *pTmp1;

	pix.p0.x =pix.p0.y = width - 1;
	pix.up = 1;
	pix.left = 0;

	for (i = 0; i < VerCapVerid.Cors[corid - 1].Codesize; i ++)
	{
		pTmp0 = pCodeBuf + VerCapVerid.Size - 1;
		pTmp0 -= i;
		for (j = 0; j < VerCapVerid.Cors[corid - 1].blocks0; j++)
		{
			pTmp1 = pTmp0 - (VerCapVerid.Cors[corid - 1].Codesize) * j;
			temp = *pTmp1;
			Put_Byte(temp, &pix);
		}
		pTmp0 = pTmp0 -  (VerCapVerid.Cors[corid - 1].Codesize) *  (VerCapVerid.Cors[corid - 1].blocks0) ;
		
		for (j = 0; j < VerCapVerid.Cors[corid - 1].blocks1; j++)
		{
			pTmp1 = pTmp0 - (VerCapVerid.Cors[corid - 1].Codesize + 1) * j;
			temp = *pTmp1;
			Put_Byte(temp, &pix);
		}
	}

	if (VerCapVerid.Cors[corid - 1].blocks1 > 0)
	{
		pTmp0 = pCodeBuf + VerCapVerid.Size - 3 * (VerCapVerid.Cors[corid - 1].Codesize) - 1;
		for (j = 0; j < VerCapVerid.Cors[corid - 1].blocks1; j++)
		{
			pTmp1 = pTmp0 - (VerCapVerid.Cors[corid - 1].Codesize + 1) * j;
			temp = *pTmp1;
			Put_Byte(temp, &pix);
		}	
	}
	
	for (i = 0; i < VerCapVerid.Cors[corid - 1].CorSize; i ++)
	{
		pTmp0 = pCodeBuf + VerCapVerid.Cors[corid - 1].aCorSize - 1;
		pTmp0 -= i;
		for (j = 0; j < VerCapVerid.Cors[corid - 1].blocks0 + VerCapVerid.Cors[corid - 1].blocks1; j++)
		{
			pTmp1 = pTmp0 - (VerCapVerid.Cors[corid - 1].CorSize) * j;
			temp = *pTmp1;
			Put_Byte(temp, &pix);
		}
	}

}

u8 get_dot(u8 *ptr, u16 bitH, u16 bitV, u16 bitA)
{
	u8	mod, div, value;
	u8	*ptrt;
	u16	org;
	div = bitV >> 3;
	mod = bitV - (div << 3) & 0xFF;
	org = bitA * div + bitH;
	ptrt = ptr +org;
	mod = 7 - mod;
	value = (*ptrt >> mod) & 0x01;
	return	value;
}

void	mask_dat(u8 *tmap, u8 ref)
{
	u16	i, j;
	u8	msk, value;
	for (i = 0; i < width; i++)
	{
		if (i == 6)
			continue;
		for (j = 0; j < width; j++)
		{
			if (j == 6)
				continue;
			if (dot_in_pos(j, i)==0)
				continue;
			if (ref == 0)
				msk = (i + j) % 2;
			else if (ref == 1)
				msk = i % 2;
			else if (ref == 2)
				msk = j % 3;
			else if (ref == 3)
				msk = (i + j) % 3;
			else if (ref == 4)
				msk = (i  / 2 + j  / 3) % 2;
			else if (ref == 5)
				msk = ((i * j) % 2 + (i * j) % 3);
			else if (ref == 6)
				msk = ((i * j) % 2 + (i * j) % 3) & 0x01;
			else if (ref == 7)
				msk = ((i * j) % 3 + (i + j) & 0x01) & 0x01;
			if (msk == 0)
				msk = 1;
			else
				msk = 0;
			value = get_dot(tmap, j, i, width);
			if (msk == value)
				put_pixel(tmap, j, i, width, 0x00);
			else
				put_pixel(tmap, j, i, width, 0x01);			
		}
	}
}

void put_fmtinfo(u8 mskno)
{
	u32	codeinfo, tmp;
	u8	i;
	if (corid == 0x01)
		codeinfo = 1;
	else if (corid == 0x02)
		codeinfo = 0;
	else if (corid == 0x03)
		codeinfo = 3;
	else if (corid == 0x04)
		codeinfo = 2;
	else
		codeinfo = 1;
	codeinfo = codeinfo << 3;
	mskno &= 0x07;
	codeinfo |= mskno;
	tmp = codeinfo;
	tmp = bch_code(codeinfo, 1335, 15, 5);
	codeinfo = codeinfo << 10;
	codeinfo |= tmp;
	codeinfo ^= 21522;

	for (i = 0; i < 7; i++)
	{
		if (i >= 6)
			put_pixel(pCanvas, i+1, 8, width, (codeinfo >> (14 - i)) & 0x01);
		else
			put_pixel(pCanvas, i, 8, width, (codeinfo >> (14 - i)) & 0x01);
		put_pixel(pCanvas, 8, width - i - 1, width, (codeinfo >> (14 - i)) & 0x01);
	}
	
	for (i = 0; i < 8; i++)
	{
		if (i >= 6)
			put_pixel(pCanvas, 8, i + 1, width, (codeinfo >> i) & 0x01);
		else
			put_pixel(pCanvas, 8, i, width, (codeinfo >> i) & 0x01);
		put_pixel(pCanvas, width - i - 1, 8, width, (codeinfo >> i) & 0x01);
	}	
}

void put_verinfo(void)
{
	u32	codeinfo, tmp;
	u8	i, j;
	if (verid < 7)
		return;
	codeinfo = verid;
	tmp = bch_code(codeinfo, 7973, 18, 6);
	codeinfo = codeinfo << 12;
	codeinfo |= tmp;
	for (j = 0; j <6; j++)
		for (i = 0; i < 3; i++)
		{
			put_pixel(pCanvas, 5 - j , width - 9 - i, width, (codeinfo >> (17 - (j * 3) - i)) & 0x01);
			put_pixel(pCanvas, width - 9 - i , 5 - j, width, (codeinfo >> (17 - (j * 3) - i)) & 0x01);		//2012.12.10，右上角版本信息填充错误
		}	
}

u32 count_score(u8 *tmap)
{
	u16	i, j, cntb = 0, cntw = 0, cnt = 0;
	u32	score = 0;
	u8	tmp, pix = 0xFF;
	for	(i = 0; i < width; i++)
	{
		for (j = 0; j < width; j++)
		{
			tmp = get_dot(tmap, i, j, width);
			if (tmp == 0x00)
				cntw++;
			else
				cntb++;
			
			if (pix == tmp)
			{
				cnt++;
			}
			else
			{	
				pix = tmp;
				if (cnt >= 5)
					score += 3+cnt -5;
				cnt = 1;
			}
			if ((i + 6) < width)
			{
				if ((tmp == 0x01) && (get_dot(tmap, i, j+1, width) == 0x00) &&  (get_dot(tmap, i, j+2, width) == 0x01) &&
					(get_dot(tmap, i, j+3, width) == 0x01) && (get_dot(tmap, i, j+4, width) == 0x01) &&
					(get_dot(tmap, i, j+5, width) == 0x00) && (get_dot(tmap, i, j+6, width) == 0x01))
					score += 40;
			}
			if ((i + 1) < width)
			{
				if ((cnt > 1) && (get_dot(tmap, i + 1, j, width) == tmp) && (get_dot(tmap, i + 1, j  - 1, width) == tmp))
					score += 3;
			}
		}
	}
	
	cntb = cntb * 100 /(cntb + cntw);

	if (cntb >50)
		cntb -= 50;
	else
		cntb = 50 - cntb;
	cntb = cntb / 5;
	score += 10 * cntb;

	for	(j = 0; j < width; j++)
	{
		for (i = 0; i < width; i++)
		{
			tmp = get_dot(tmap, i, j, width);
			if (pix == tmp)
			{
				cnt++;
			}
			else
			{	
				pix = tmp;
				if (cnt >= 5)
					score += 3+cnt -5;
				cnt = 1;
			}
			if ((i + 6) < width)
			{
				if ((tmp == 0x01) && (get_dot(tmap, i, j+1, width) == 0x00) &&  (get_dot(tmap, i, j+2, width) == 0x01) &&
					(get_dot(tmap, i, j+3, width) == 0x01) && (get_dot(tmap, i, j+4, width) == 0x01) &&
					(get_dot(tmap, i, j+5, width) == 0x00) && (get_dot(tmap, i, j+6, width) == 0x01))
					score += 40;
			}
		}
	}
	return	score;
}

u8 do_mask()
{
	u8	i, itmp;
	u32	tmp, cntscore[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	for (i = 0; i < 8; i++)
	{
		memcpy(pMask, pCanvas, bytes * width);
		mask_dat(pMask, i);
		cntscore[i] = count_score(pMask);
	}
	tmp = cntscore[0];
	itmp = 0;
	for (i = 0; i < 8; i++)
	{
		if (tmp > cntscore[i])
		{
			tmp = cntscore[i];
			itmp = i;
		}
	}
	mask_dat(pCanvas, itmp);
	return	itmp;
}

void proc_data(u8 *dat, u8 datlen)
{
	u8	i, tmp, *ptrt;
	tmp = 0xEC;
	if (datlen < VerCapVerid.Size)
	{
		ptrt = dat +datlen;
		for (i=0; i< VerCapVerid.Size -datlen; i++)
		{
			*ptrt++=tmp;
			if (tmp==0xEC)
				tmp=0x11;
			else
				tmp=0xEC;
		}
	}
}

/*****************************/
u8	QR_Ver(u8 Version, u16 len, u8 corlvl)
{
	u8	i;
	u16	Temp;

	#ifdef EXTFLASH
	{
		u8 Data[42];
	#endif

	//数据长度
	datalen = len;

	//纠错等级处理
	if (corlvl ==0 || corlvl >4 )	
		corid = 1;
	else
		corid = corlvl;

	verid =Version;
	
	//按输入的版本读入参数，检查版本容量及内存容量是否满足要求
	if(verid >0  && verid <= 40 )
	{
		#ifdef EXTFLASH					
			SPI_FLASH_BufferRead( &VerCapVerid.Size, VerCap_ADDR + 42*(verid - 1), 42 );
		#else
			VerCapVerid = VerCap[verid - 1];
		#endif

		//2012.12.10，需要的字符容量＝数据长度＋模式指示符＋字符计数指示符
		if ( verid >= 10)					
			Temp =len + 3;
		else
			Temp =len + 2;
		
		//2012.12.10，版本容量大于等于数据量
		if ( VerCapVerid.Cors[corid-1].aCodesize >=Temp)		
		{
			//计算当前版本号下所需缓冲区容量
			width = 17 + verid * 4;				//宽度点数		
			bytes = ((width +7)/8);				//高度字节数
			Temp = width* bytes + VerCapVerid.Size + 1024;	//数据容量+中间临时缓冲区
			
			if (Temp > (BARCODE2D_SIZE))		
				verid =0;						//缓冲区容量不够，
		}
		else
				verid =0;						//版本容量不够，
	}
	else
		verid =0;								//版本号无效
	
	//则自动选择版本号
	if(verid ==0)	
	{
		//按数据长度查表得到合适的版本号，
		for (i = 0; i < 40; i++)		
		{
			verid = i+1;
		
			#ifdef EXTFLASH					
				SPI_FLASH_BufferRead( &VerCapVerid.Size, VerCap_ADDR + 42*(verid - 1), 42 );
			#else
				VerCapVerid = VerCap[verid - 1];
			#endif

			//2012.12.10，需要的字符容量＝数据长度＋模式指示符＋字符计数指示符
			if ( verid >= 10)					
				Temp =len + 3;
			else
				Temp =len + 2;
		
			//2012.12.10，版本容量大于等于数据量
			if ( VerCapVerid.Cors[corid - 1].aCodesize >=Temp )	
			{
				//计算当前版本号下所需缓冲区容量
				width = 17 + verid * 4;				//宽度点数		
				bytes = ((width +7)/8);				//高度字节数
				Temp = width* bytes + VerCapVerid.Size + 1024;	//数据容量+中间临时缓冲区
				
				if (Temp > BARCODE2D_SIZE)			//
					return 0;						//缓冲区容量不够，返回错误
				break;
			}
		}
	}
	if (verid <= 40)
	{
		return	1;
	}
	else
		return	0;
}

/***********************************/
void Get_Code(u8 *pChar, u16 CharLen)
{
	u8	*pTmp0, *pTmp1, tmp;
	u16	i;
	
	memset(pCodeBuf, 0x00, VerCapVerid.Size);
	
	CharLen = press_byte(pCodeBuf, pChar, CharLen);

	pTmp0 = pCodeBuf;
	pTmp1 =pCodeBuf + VerCapVerid.Size - 1;
	
	//变换顺序
	for (i = 0; i < VerCapVerid.Size/2; i ++)	
	{
		tmp = *pTmp0;
		*pTmp0++ = *pTmp1;
		*pTmp1-- = tmp;
	}
	
	pTmp0 = pCodeBuf + VerCapVerid.Size;
	pTmp1 = pCodeBuf + VerCapVerid.Cors[corid - 1].aCorSize;
	for (i = 0; i < VerCapVerid.Cors[corid - 1].blocks0; i++)	//纠错块1
	{
		pTmp0 -= VerCapVerid.Cors[corid - 1].Codesize;
		pTmp1 -= VerCapVerid.Cors[corid - 1].CorSize;
		memset(pRS, 0x00, VerCapVerid.Cors[corid - 1].CorSize + VerCapVerid.Cors[corid - 1].Codesize);
		memcpy(pRS + VerCapVerid.Cors[corid - 1].CorSize, pTmp0, VerCapVerid.Cors[corid - 1].Codesize);
		encode_rs(pRS, VerCapVerid.Cors[corid - 1].CorSize+VerCapVerid.Cors[corid - 1].Codesize, VerCapVerid.Cors[corid - 1].Codesize);
		memcpy(pTmp1, pRS, VerCapVerid.Cors[corid - 1].CorSize);
	}
	
	for (i = 0; i < VerCapVerid.Cors[corid - 1].blocks1; i++)	//纠错块2
	{
		pTmp0 -= VerCapVerid.Cors[corid - 1].Codesize;
		pTmp1 -= VerCapVerid.Cors[corid - 1].CorSize;
		pTmp0 --;
		memset(pRS, 0x00, VerCapVerid.Cors[corid - 1].CorSize+ VerCapVerid.Cors[corid - 1].Codesize);
		memcpy(pRS + VerCapVerid.Cors[corid - 1].CorSize, pTmp0, VerCapVerid.Cors[corid - 1].Codesize + 1);
		encode_rs(pRS, VerCapVerid.Cors[corid - 1].CorSize+VerCapVerid.Cors[corid - 1].Codesize + 1, VerCapVerid.Cors[corid - 1].Codesize + 1);
		memcpy(pTmp1, pRS, VerCapVerid.Cors[corid - 1].CorSize);
	}
}

void sht_dat(void)
{
	u8	i, j, *pSrc, *pDes, *pTmp0, *pTmp1, *pTmp2;
	pSrc = pCanvas;
	pDes = pSrc + (bytes + 1) * (width + 8);
	pTmp0 = pDes+ (bytes + 1) * (width + 8);

	memset(pDes, 0x00, (bytes + 1) * (width + 8));
	pTmp1 = pTmp0;
	memset(pTmp1, 0x00, width+8);
	for (i = 0; i < bytes; i++)
	{
		pTmp2 = pSrc;
		
		pTmp1 += 4;
		
		for (j = 0; j < width; j++)
			*pTmp1++ |= (*pTmp2++ >> 4) & 0x0F;

		memcpy(pDes, pTmp0, width + 8);

		pTmp1 = pTmp0;
		memset(pTmp1, 0x00, width+8);
		
		pTmp2 = pSrc;
		pTmp1 += 4;
		for (j = 0; j < width; j++)
			*pTmp1++ |= (*pTmp2++ << 4) & 0xF0;
		
		pSrc += width;
		pDes += width + 8;
		pTmp1 = pTmp0;
	}
	memcpy(pDes, pTmp1, width + 8);
	
	pDes = pCanvas;
	pSrc = pDes +(bytes + 1) * (width + 8);;
	memcpy(pDes, pSrc, (bytes + 1) * (width + 8));	
	width += 8;
	bytes += 1;
}

u32 QR_Print(u8 *pBuf, u8 Version, u8 CorID, u16 CharLen)
{
	u8	tmsk;
	u32	datlen;
	
	if (QR_Ver(Version, CharLen, CorID) == 0x00)
		return	0x0000;
 	
	pCanvas = pBuf;
	pCodeBuf = pCanvas + width * bytes;		//
	pMask = pCodeBuf;
	pRS = pCodeBuf + VerCapVerid.Size;
	pTMP = pRS + 200;

	Get_Code(pBuf, CharLen);
	memset(pCanvas, 0x00, width * bytes);

	put_pos_find();

	put_array(pCanvas, (u8 *)GraphPos, 8, 8, width, 0, 0);
	put_array(pCanvas, (u8 *)GraphPos, 8, 8, width, width-7, 0);
	put_array(pCanvas, (u8 *)GraphPos, 8, 8, width, 0, width-7);
	put_pos_align();

	put_dat(pCodeBuf);

	tmsk = do_mask();
	put_fmtinfo(tmsk);
	put_verinfo();
	sht_dat();
	
	datlen = width;
	datlen = datlen << 16;
	datlen = datlen | bytes;
	return	datlen;
}

void put_a_finder(u8 *ptr, u8 vWidth, u8 vHeight, u8 xOrg, u8 yOrg, u8 dots)
{
	int	i, inv;
	inv = 1;
	for (i = 0; i < vWidth; i++)
	{
		put_pixel(ptr, xOrg + i, yOrg, dots, inv);
		put_pixel(ptr, xOrg + i, yOrg + vHeight - 1, dots, 1);
		if (inv == 1)
			inv = 0;
		else
			inv = 1;
	}
	inv = 0;
	for (i = 0; i < vHeight; i++)
	{
		put_pixel(ptr, xOrg + vWidth - 1, yOrg + i, dots, inv);
		put_pixel(ptr, xOrg, yOrg + i, dots, 1);
		if (inv == 1)
			inv = 0;
		else
			inv = 1;
	}
}

void put_finder(u8 *ptr)
{
	u8	i, vxOrg = 0, vyOrg = 0;

	for (i = 0; i < MatrixCapDM.blocks; i ++)
	{
		put_a_finder(ptr, MatrixCapDM.subwidth, MatrixCapDM.subheight, vxOrg, vyOrg, MatrixCapDM.mwidth);
		vxOrg += MatrixCapDM.subwidth;
		if (vxOrg == MatrixCapDM.mwidth)
		{
			vxOrg = 0;
			vyOrg += MatrixCapDM.subheight;
		}
	}
}

/*******************************************************************************
* Function Name	: CalculateRamBuffer(void )
* Description	: 计算选择的点阵需要的维修部区容量
* Input			: None
* Output		: None
* Return		: 正常返回1，错误返回0
*******************************************************************************/
u8	CalculateRamBuffer(void)
{
	u16 Len;
	
	ncol = MatrixCapDM.mwidth - MatrixCapDM.xcount * 2;
	nrow = MatrixCapDM.mHeight - MatrixCapDM.ycount * 2;
	width = MatrixCapDM.mwidth;
	bytes = MatrixCapDM.mHeight >> 3;
	if ((MatrixCapDM.mHeight % 8) != 0)
		bytes++;
	
	//计算当前选择的点阵大小需要的缓冲区
	Len = (width * bytes);
	Len += MatrixCapDM.Cor.aCodesize + MatrixCapDM.Cor.aCorSize;
	Len += MatrixCapDM.Cor.Codesize+ MatrixCapDM.Cor.CorSize;
	Len += (2*nrow/8+1) * ncol;
	
	if(Len > BARCODE2D_SIZE)
		return 0;
	else
		return 1;
	}
/*******************************************************************************
* Function Name	: sel_matrx_module(u8 vHeight, u8 vWidth, u16 charlen)
* Description	: 计算选择的点阵需要的维修部区容量
* Input			: vHeight:矩阵高，vWidth：矩阵宽，charlen：数据长度
* Output		: 
* Return		: 正常返回1，错误返回0
*******************************************************************************/
u8	sel_matrx_module(u8 vHeight, u8 vWidth, u16 charlen)
{
	u8	i, verselected = 0;
	
	verid = 0;
	
	//查表校验图形模块的高、宽是否有效
	if ((vHeight != 0) && (vWidth != 0))			  
	{
		for (i = 0; i < 30; i++)
		{	
			#ifdef EXTFLASH
				SPI_FLASH_BufferRead( &MatrixCapDM.mHeight, MatrixCap_ADDR + 16*i , 16);
			#else
				MatrixCapDM = MatrixCap[i]; 
			#endif

			if ((MatrixCapDM.mHeight == vHeight) && (MatrixCapDM.mwidth == vWidth) && (MatrixCapDM.Cor.aCodesize >= charlen))
			{
				if( CalculateRamBuffer())		//如果打印机内存满足选择的矩阵所需的缓冲区，2012.9.6
				{
					verid = i;
					verselected = 1;
					break;
				}
			}
		}
	}
	
	//按数据长度查表合适的图形模块
	if (verselected == 0)		   
	{
		for (i = 0; i < 30; i++)
		{	
			#ifdef EXTFLASH
				SPI_FLASH_BufferRead( &MatrixCapDM.mHeight, MatrixCap_ADDR + 16*i , 16);
			#else
				MatrixCapDM = MatrixCap[i]; 
			#endif
			if (MatrixCapDM.Cor.aCodesize >= charlen)
			{
				if(CalculateRamBuffer())		//如果打印机内存满足选择的矩阵所需的缓冲区，2012.9.6
				{
					verid = i;
					verselected = 1;
					break;
				}
			}
		}	
	}
	return	verselected;
}

u8 getdot(int row, int col, u8 id)
{
	int tmp, bit;
	tmp = (2 * row) / 8;
	bit = 7 - ((2 * row) % 8);
	tmp *= ncol;
	tmp += col;
	return	(*(pTMP+ tmp) >> (bit - id)) & 0x01;
}

void putdot(int row, int col, u8 id, u8 values)
{
	int tmp, bit;
	tmp = (2 * row) / 8;
	bit = 7 - ((2 * row) % 8);
	tmp *= ncol;
	tmp += col;
	*(pTMP + tmp) |= values << (bit - id);
}

void module(int row, int col, u8 chr, u8 bit)
{
	if (row < 0) { row += nrow; col += 4 - ((nrow+4)%8); }
	if (col < 0) { col += ncol; row += 4 - ((ncol+4)%8); }
	putdot(row, col, 0, 1);
	putdot(row, col, 1, (u8)(chr >> (8 - bit) & 0x01));
}

void utah(int row, int col, u8 chr)
{
	module(row-2,col-2,chr,1);
	module(row-2,col-1,chr,2);
	module(row-1,col-2,chr,3);
	module(row-1,col-1,chr,4);
	module(row-1,col,chr,5);
	module(row,col-2,chr,6);
	module(row,col-1,chr,7);
	module(row,col,chr,8);
}

void corner1(u8 chr)
{ 
	module(nrow-1,0,chr,1);
	module(nrow-1,1,chr,2);
	module(nrow-1,2,chr,3);
	module(0,ncol-2,chr,4);
	module(0,ncol-1,chr,5);
	module(1,ncol-1,chr,6);
	module(2,ncol-1,chr,7);
	module(3,ncol-1,chr,8);
}
void corner2(u8 chr)
{
	module(nrow-3,0,chr,1);
	module(nrow-2,0,chr,2);
	module(nrow-1,0,chr,3);
	module(0,ncol-4,chr,4);
	module(0,ncol-3,chr,5);
	module(0,ncol-2,chr,6);
	module(0,ncol-1,chr,7);
	module(1,ncol-1,chr,8);
}

void corner3(u8 chr)
{
	module(nrow-3,0,chr,1);
	module(nrow-2,0,chr,2);
	module(nrow-1,0,chr,3);
	module(0,ncol-2,chr,4);
	module(0,ncol-1,chr,5);
	module(1,ncol-1,chr,6);
	module(2,ncol-1,chr,7);
	module(3,ncol-1,chr,8);
}
void corner4(u8 chr)
{
	module(nrow-1,0,chr,1);
	module(nrow-1,ncol-1,chr,2);
	module(0,ncol-3,chr,3);
	module(0,ncol-2,chr,4);
	module(0,ncol-1,chr,5);
	module(1,ncol-3,chr,6);
	module(1,ncol-2,chr,7);
	module(1,ncol-1,chr,8);
}

void ecc200(void)
{
	int row, col;
	u8	*ptmp;
	ptmp = pTMP;
	
	for (row=0; row<(2*nrow/8+1); row++) 
	{
		for (col=0; col<ncol; col++) 
		{
			*ptmp++ = 0;
		}
	}
	row = 4; col = 0;
	do
	{
		if ((row == nrow) && (col == 0)) corner1(*pCodeBuf++);
		if ((row == nrow-2) && (col == 0) && (ncol%4)) corner2(*pCodeBuf++);
		if ((row == nrow-2) && (col == 0) && (ncol%8 == 4)) corner3(*pCodeBuf++);
		if ((row == nrow+4) && (col == 2) && (!(ncol%8))) corner4(*pCodeBuf++);
		do 
		{
			if ((row < nrow) && (col >= 0) && (getdot(row, col, 0) == 0))
				utah(row,col,*pCodeBuf++);
			row -= 2; col += 2;
		} while ((row >= 0) && (col < ncol));
		row += 1; col += 3;
		do
		{
			if ((row >= 0) && (col < ncol) && (getdot(row, col, 0) == 0))
				utah(row,col,*pCodeBuf++);
			row += 2; col -= 2;
		} while ((row < nrow) && (col >= 0));
		row += 3; col += 1;
	} while ((row < nrow) || (col < ncol));
	if (getdot(row, col, 0) == 0)
	{
		putdot(nrow - 1, ncol - 1, 0, 1);
		putdot(nrow - 1, ncol - 1, 1, 1);
		putdot(nrow - 2, ncol - 2, 0, 1);
		putdot(nrow - 2, ncol - 2, 1, 1);
	}
}

void gen_ploy_dm(u8 codelen, u8 datlen)
{
	int i, j, mask;
	u8	tmp;

	for (i = 0; i < 256; i++)
	{
		alpha_to[i] = 0;
		index_of[i] = 0;
	}

	mask = 1 ;
	alpha_to[mm] = 0 ;

	for (i=0; i<mm; i++)
	{
		alpha_to[i] = mask ;
		index_of[alpha_to[i]] = i ;
		if (ppdm[i]!=0)
			alpha_to[mm] ^= mask ;
		mask <<= 1 ;
	}
	index_of[alpha_to[mm]] = mm ;
	mask >>= 1 ;
	for (i=mm+1; i<= nn ; i++)
	{
		if (alpha_to[i-1] >= mask)
			alpha_to[i] = alpha_to[mm] ^ ((alpha_to[i-1]^mask)<<1) ;
		else
			alpha_to[i] = alpha_to[i-1]<<1 ;
		index_of[alpha_to[i]] = i ;
	}
	index_of[1] = 0 ;

	gg[0] = alpha_to[1] ;
	gg[1] = 1 ;
	for (i=2; i<=codelen-datlen; i++)
	{
		gg[i] = 1 ;
		tmp = alpha_to[i];
		for (j=i-1; j>0; j--)
		{
			if (gg[j] != 0)
			{
				gg[j] = gg[j-1]^ rs_mul(gg[j], tmp) ;
			}
			else
				gg[j] = gg[j-1] ;
		}
		gg[0] = rs_mul(gg[0], tmp) ;
	}

	for (i=0; i<=codelen-datlen; i++)
		gg[i] = index_of[gg[i]];

	for (i=0; i<=codelen-datlen; i++)
		gg[i] = alpha_to[gg[i]] ;
}

void encode_rs_dm(u8 *cor, u8 codelen, u8 datlen)
{
	int i, j ;
	unsigned char tmp, index, index1;

	alpha_to = pTMP+600;
	index_of = alpha_to +256;
	gg = index_of + 256;
	
	gen_ploy_dm(codelen, datlen);
	
	index1=codelen-1;
	for (i=0; i<datlen; i++)
	{
		tmp=cor[index1];
		index=codelen-datlen;
		if (tmp==0x00)
		{
			index1--;
			continue;
		}
		for (j=0; j<=codelen-datlen; j++)
		{
			cor[index1-j]=cor[index1-j] ^ rs_mul(tmp, gg[index]);
			index--;
		}
		index1--;
	}

}

u16 Press_Dat(u8 *pChar, u16 datlen)
{
	u16	i;
	u8	tmp, asc = 1;
	for (i = 0; i < datlen; i++)
		if ((*(pChar + i) >=0x80) || (*(pChar + i) <0x20))
		{
			asc = 0;
			break;
		}
	if (asc == 1)
	{
		for (i = 0; i < datlen; i++)
			*(pChar + i) += 1;
		memcpy(pCodeBuf, pChar, datlen);
	}
	else
	{
		*pCodeBuf = 231;
		if (datlen >= 250)
		{
			tmp = datlen / 250;
			tmp += 249;
			*(pCodeBuf + 1) = tmp;
			tmp = datlen % 250;
			*(pCodeBuf + 2) = tmp;
			memcpy(pCodeBuf + 3, pChar, datlen);
			datlen += 3;
		}
		else
		{
			*(pCodeBuf + 1) = (datlen & 0xFF);
			memcpy(pCodeBuf + 2, pChar, datlen);
			datlen += 2;
		}
		for (i = 1; i < datlen; i++)
		{
			tmp = (149 * (i + 1)) % 255 + 1;
			tmp += *(pCodeBuf + i);
			*(pCodeBuf + i) = tmp;
		}		
	}
	memcpy(pChar, pCodeBuf, datlen);
	return	datlen;
}
/*******************************************************************************
* Function Name	: Build_Code_Word(u8 *pCHar, u16 datlen)
* Description	: 构造代码字
* Input			: pCHar：输入数据缓冲区，datlen：数据长度
* Output		: None
* Return		: None
*******************************************************************************/
void Build_Code_Word(u8 *pCHar, u16 datlen)
{
	u16	i, j;
	u8	*pT0, *pT1, tmp;

	memset(pCodeBuf, 0x00, MatrixCapDM.Cor.aCodesize + MatrixCapDM.Cor.aCorSize);
	memcpy(pCodeBuf, pCHar, datlen);

	if (datlen < MatrixCapDM.Cor.aCodesize)					//添加代码
	{
		for (i = datlen; i < MatrixCapDM.Cor.aCodesize; i++)
			{
				tmp = (149 * (i + 1)) % 255 + 1;
				tmp += *(pCodeBuf + i);
				*(pCodeBuf + i) = tmp;
			}
		*(pCodeBuf + datlen) = 129;
	}

	for (i = 0; i < MatrixCapDM.Cor.blocks0; i++)
	{
		pT0 = pCodeBuf + i;
		pT1 = pRS - 1;

		for (j = 0; j < MatrixCapDM.Cor.aCodesize; j += MatrixCapDM.Cor.blocks0)
		{
			if (i >= 8)
				*(pT1+MatrixCapDM.Cor.Codesize + MatrixCapDM.Cor.CorSize - j / MatrixCapDM.Cor.blocks0 - 1) = *(pT0 + j);
			else
				*(pT1+MatrixCapDM.Cor.Codesize + MatrixCapDM.Cor.CorSize - j / MatrixCapDM.Cor.blocks0) = *(pT0 + j);
		}
		
		memset(pRS, 0x00, MatrixCapDM.Cor.CorSize);
		encode_rs_dm(pRS, MatrixCapDM.Cor.Codesize + MatrixCapDM.Cor.CorSize, MatrixCapDM.Cor.Codesize);

		pT0 += (MatrixCapDM.Cor.aCodesize);
		for (j = 0; j < MatrixCapDM.Cor.CorSize; j++)
			*(pT0 + j * MatrixCapDM.Cor.blocks0) = *(pRS +(MatrixCapDM.Cor.CorSize - j - 1));
	}
}

u32 Matrix_Print(u8 *pBuf, u8 vHeight, u8 vWidth, u16 charlen)
{
	u16	cntbytes, i, j, x, y;
	u32 res;

	pCodeBuf = pBuf + charlen;
	charlen = Press_Dat(pBuf, charlen);
	
	if (sel_matrx_module(vHeight, vWidth, charlen) == 0)	  //选择图形模块
		return	0x0000;
	
	cntbytes = MatrixCapDM.Cor.aCodesize + MatrixCapDM.Cor.aCorSize;

	pCanvas = pBuf;
	pCodeBuf = pCanvas + width * bytes;
	pRS = pCodeBuf + cntbytes;
	memset(pCodeBuf, 0x00, cntbytes);	
	cntbytes = MatrixCapDM.Cor.Codesize+ MatrixCapDM.Cor.CorSize;
	pTMP = pRS + cntbytes;
	Build_Code_Word(pBuf, charlen);

	memset(pCanvas, 0x00, width * bytes);	
	
	ecc200();

	x = 1;
	for (i = 0; i < (MatrixCapDM.mwidth - 2 * MatrixCapDM.xcount); i ++)
	{
		if ((i % (MatrixCapDM.subwidth- 2) == 0) && (i != 0))
			x += 2;
		y = 1;
		for (j = 0;  j < (MatrixCapDM.mHeight- 2 * MatrixCapDM.ycount); j++)
		{
			if ((j % (MatrixCapDM.subheight- 2) == 0) & (j != 0))
				y += 2;
			put_pixel(pCanvas,x,  y, width, getdot(j, i, 1));
			y++;
		}
		x++;
	}
	
	put_finder(pCanvas);
	sht_dat();

	res = width;
	res = res << 16;
	res = res | bytes;
	return	res;
}



/******************* (C) COPYRIGHT 2016 *****END OF FILE****/
