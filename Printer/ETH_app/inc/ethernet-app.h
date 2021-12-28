#ifndef __PRINT_H
#define __PRINT_H
 
//定义串口接收缓冲区结构
#define UART_BUF_MAX 2048			//缓冲区最大值
typedef struct
{
	int in ; 						//放入地址
	int out ;						//读出地址
	u8_t 	buf[ UART_BUF_MAX ] ;		//接收缓冲区
}UARTBUF;

//定义打印输出缓冲区结构
#define PRINT_BUF_MAX	10*1024		//输出缓冲区大小
typedef struct TPRINT {
	int in ; 						//放入地址
	int out ;						//读出地址
	int n ;							//放入的数据个数
	char buf [PRINT_BUF_MAX ] ;		//缓冲区
}PRINT;

//定义SPRT协议头结构
typedef struct {
//	u8_t		bodylenH;				//整包长度
//	u8_t		bodylenL;
	u8_t		CONSTIDH;				//包头
	u8_t		CONSTIDL;				//
	u8_t		packetidH;				//包序号
	u8_t		packetidL;
	u8_t		packetlenH;				//包长度
	u8_t		packetlenL;
	u8_t		chksumH;			  	//包校验和
	u8_t		chksumL;
	u8_t		status;
	u8_t		nextpktidH;			   //下次包序号
	u8_t		nextpktidL;
	u8_t		dat;
}USER_HEADER;

void print_char(char ch);
char print_get ( char *ch );
void print_normal(void);
void print_init ( void );
char print_puts ( u8_t *buf );
u8_t print_putn( u8_t *buf, u16_t len);
void print_test( char flag );
void printer_server ( void );
void get_dat(void);
u8_t	chk_bsy(void);
u8_t	chk_err(void);

#endif

