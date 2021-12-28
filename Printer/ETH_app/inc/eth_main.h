#ifndef	__MAIN_H
#define	__MAIN_H

extern void my_appcall(void);
extern void my_udp_appcall(void);

struct Port_state{
  u16_t datalen;			  //发送数据长度
  u16_t olddatalen;			//正在发送数据长度,重发时用
  u16_t count;				  //连接计数器
  u8_t state;				    //等待状态
  u8_t *dataptr;			  //发送数据地址
  
};

#endif

