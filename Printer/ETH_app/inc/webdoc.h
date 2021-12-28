//-----------------------------------------------------------------------------
// Copyright (c) 2002 Jim Brady
// Do not use commercially without author's permission
// Last revised August 2002
// Net WEBDOC.C
//
// This module contains web pages and headers
//-----------------------------------------------------------------------------
//#include "includes.h"

//#ifdef WEB_SERVER
// This is the header for web text pages. LENGTH is the body
// length, needed because in HTTP 1.1 we leave connection open
// Includes control statements to prevent browser from caching page
#ifndef	__WEBSVR
#define __WEBSVR

const char html_header[] ={
	"HTTP/1.1 200 OK\r\n"
	"Cache-control: no-cache\r\n"
	"Connection: Keep-Alive\r\n"
	"Content-Length: TAG:LEN1\r\n"
	"Content-Type: text/html\r\n\r\n"
};

const char language_htm_ch[]={"<HTML><HEAD><TITLE>Login</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>" 
  "<Script language='javascript'>function vl(){document.tk.languageset.value=language;}</Script></HEAD><BODY onload='vl();' style=background:#6495ED;margin-left:5;margin-top:0;><table width=100%% height=70%% cellspacing=0>" 
  "<tr height=30 style=background:#EEEE00><td></td></tr><tr height=70><td></td></tr><tr height=70><td align=middle style=font-family:宋体;font-size:26pt;width:60%%><BR><BR><B>打印机</B></td></tr><tr><td align=center>" 
  "<BR><BR><BR><TABLE borderColor=#6495ED width=299 height=79 border=1><TR><FORM name=tk action=languageset method=POST>"
  "<TR><TD align=middle><B>语言选择</B></TD></TR><TR><TD align=middle><select name='languageset'><option value=0>English<option value=1>中文简体"
  "<option value=2>中文繁体</select></TD></TR><TR><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>" 
  "&nbsp;<input class=button type=submit name=upload value='提 交'></TD></TR><TR><TD align=middle><FONT>选择语言并提交后进入设置界面！</TD></TR></TR></BODY></HTML>"};

const char menu_htm_ch[]={"<HTML><HEAD><TITLE>menu</TITLE><META http-equiv=content-type content='text/html; charset=gb2312'></HEAD><BR><BR><BR><BR><BR><BR><BR><BR>"
  "<BODY style='background: #6495ED;margin-left:4;margin-top:0;margin-bottom:0;margin-right:0;'><TABLE class=font9 cellSpacing=0 cellPadding=0 width=118 border=0 style='cursor:hand'>"
	"<TD valign=top bgColor=#EEEE00 height=20><P align=center><BR><A href='port' target=bf>网口IP<BR><BR><A href='sp' target=bf>串口参数<BR><BR>"
	"<A href='usb' target=bf>USB 参数<BR><BR><A href='print' target=bf>打印机参数<BR><BR>"
	"<A href='prtset' target=bf>设置参数<BR><BR>"   //2017.10.11
	"<A href='symbol' target=bf>制造商内容<BR><BR>"
	"<A href='upload' target=bf>固件升级<BR><BR></P></TD></TR></TABLE></BODY></HTML>"};
	
const char port_htm_ch[]={"<HTML><HEAD><TITLE>port</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.IpAddress.value=IPAddr;document.tk.SubnetMask.value=Netmask;"
	"document.tk.Gateway.value=Gateway;document.tk.MacAddress.value=MacAddr;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=249 width=41%% border=1>"
  "<TR><TD align=middle class=title1 height=18><B>网口IP</TD></TR><TR><TD><FORM name=tk action=portset method=POST>"
  "<TABLE width='100%%'height='100%%' border=1><TR bgcolor=#CAE1FF><TD align='middle'>内容</TD><TD align='middle'>值</TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;打印端口<TD>&nbsp;9100</TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;IP地址<TD>&nbsp;<input size=17 type='text' maxlength=17 name='IpAddress'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;子网掩码<TD>&nbsp;<input size=17 type='text' maxlength=17 name='SubnetMask'></TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;网关<TD>&nbsp;<input size=17 type='text' maxlength=17 name='Gateway'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;MAC地址<TD>&nbsp;<input size=17 type='text' maxlength=17 name='MacAddress'></TD></TR>"
// 	"<TR><TD>&nbsp;&nbsp;&nbsp;DHCP<TD>&nbsp;<select name='dhcp_flag'><option value=0>关闭<option value=1>打开</select></TD></TR>"     //2017.02.07 增加DHCP选项
	"<TR align=center><TD colspan=2><FONT color=#ff0000>注意:修改IP后请重新登录</TD></TR>"
  "<TR bgcolor=#CAE1FF><TD class='submit' height='15%%' align=center colspan=2><input class=button type=button name='rst' value='重 置' onclick='window.location.reload();'>" 
  "<input class=button type=submit name='upload' value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char print_htm_ch[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.print_type.value=printtype;"
	"document.tk.print_speed.value=printspeed;document.tk.print_colour.value=printcolour;document.tk.print_wide.value=printwide;"
	"document.tk.print_direction.value=printdirection;document.tk.print_font.value=printfont;document.tk.cut_control.value=cutcontrol;"
	"document.tk.pne_print.value=pneprint;"       //缺纸重打印                             
	"document.tk.buzzer_control.value=buzzercontrol;"   //蜂鸣器
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>打印机参数</TD></TR><TR><TD><FORM name=tk action=printset method=POST><TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD>&nbsp;打印机型号</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_type></TD></TR>"
	"<TR><TD>&nbsp;打印速度</TD><TD>&nbsp;<select name='print_speed'><option value=0>250mm/s<option value=1>250mm/s<option value=2>250mm/s</select></TD></TR>"
	"<TR><TD>&nbsp;打印浓度</TD><TD>&nbsp;<select name='print_colour'><option value=0>浅色<option value=1>中度<option value=2>深色</select></TD></TR>"
	"<TR><TD>&nbsp;打印宽度</TD><TD>&nbsp;<select name='print_wide'><option value=0>72mm<option value=1>64mm</select></TD></TR>"
	"<TR><TD>&nbsp;打印方向</TD><TD>&nbsp;<select name='print_direction'><option value=0>正常<option value=1>反向</select></TD></TR>"
	"<TR><TD>&nbsp;打印字体</TD><TD>&nbsp;<select name='print_font'><option value=0>12*24<option value=1>9*17</select></TD></TR>"
	"<TR><TD>&nbsp;切刀控制</TD><TD>&nbsp;<select name='cut_control'><option value=0>关闭<option value=1>开启</select></TD></TR>"
	"<TR><TD>&nbsp;缺纸重打印</TD><TD>&nbsp;<select name='pne_print'><option value=0>关闭<option value=1>开启</select></TD></TR>"
	"<TR><TD>&nbsp;蜂鸣器模式</TD><TD>&nbsp;<select name='buzzer_control'><option value=0>关闭<option value=1>错误允许<option value=2>切刀允许<option value=3>错误和切刀允许</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char prtset_htm_ch[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.bill_alarm.value=billalarm;"
	"document.tk.speaker_volume.value=speakervolume;document.tk.blackflag_test.value=blackflagtest;document.tk.paper_sensitivity.value=papersensitivity;"   //2016.10.10 增加未取单报警	
	"document.tk.enter_command.value=entercommand;document.tk.command_assemblage.value=commandassemblage;document.tk.dhcp_flag.value=dhcpflag;document.tk.normal_code.value=normalcode;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>设置参数</TD></TR><TR><TD><FORM name=tk action=prtset method=POST><TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"   //2017.10.12 增加的这一项目 其中的printset改为prtset
	"<TR><TD>&nbsp;打单蜂鸣器报警</TD><TD>&nbsp;<select name='bill_alarm'><option value=0>关闭<option value=1>未取单报警<option value=2>来单报警</select></TD></TR>"			//2016.10.10 增加未取单报警
	"<TR><TD>&nbsp;语音音量</TD><TD>&nbsp;<select name='speaker_volume'><option value=0>低<option value=1>中<option value=2>高<option value=3>关</select></TD></TR>"	
	"<TR><TD>&nbsp;黑标检测</TD><TD>&nbsp;<select name='blackflag_test'><option value=0>关闭<option value=1>开启</select></TD></TR>"
	"<TR><TD>&nbsp;测纸灵敏度</TD><TD>&nbsp;<select name='paper_sensitivity'><option value=0>低度<option value=1>中度<option value=2>高度</select></TD></TR>"
	"<TR><TD>&nbsp;回车指令</TD><TD>&nbsp;<select name='enter_command'><option value=0>关闭<option value=1>开启</select></TD></TR>"
	"<TR><TD>&nbsp;指令集</TD><TD>&nbsp;<select name='command_assemblage'><option value=0>ESC/POS<option value=1>MP<option value=2>IP</select></TD></TR>"
	"<TR><TD>&nbsp;网络DHCP</TD><TD>&nbsp;<select name='dhcp_flag'><option value=0>关闭<option value=1>开启</select></TD></TR>"        //2017.02.07  增加设置DHCP
	"<TR><TD>&nbsp;默认代码页</TD><TD>&nbsp;<select name='normal_code'><option value=0>CP437<option value=1>KataKana<option value=2>PC850"
	"<option value=3>PC860<option value=4>PC863<option value=5>PC865<option value=6>WCP1251<option value=7>CP866<option value=8>MIK<option value=9>CP755"
	"<option value=10>Iran<option value=11>Reserve<option value=12>Reserve<option value=13>Reserve<option value=14>Reserve<option value=15>CP862"
	"<option value=16>WCP1252<option value=17>WCP1253<option value=18>CP852<option value=19>CP858<option value=20>Iran II<option value=21>Latvian"
	"<option value=22>CP864<option value=23>ISO-8859-1<option value=24>CP737<option value=25>WCP1257<option value=26>Thai<option value=27>CP720"
	"<option value=28>CP855<option value=29>CP857<option value=30>WCP1250<option value=31>CP775<option value=32>WCP1254<option value=33>WCP1255"
	"<option value=34>WCP1256<option value=35>WCP1258<option value=36>ISO-8859-2<option value=37>ISO-8859-3<option value=38>ISO-8859-4"
	"<option value=39>ISO-8859-5<option value=40>ISO-8859-6<option value=41>ISO-8859-7<option value=42>ISO-8859-8<option value=43>ISO-8859-9"
	"<option value=44>ISO-8859-15<option value=45>Thai2<option value=46>CP856</select></TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char soft_htm_ch[]={"<HTML><HEAD><TITLE>soft</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>程序信息</TD></TR><TR><TD><FORM name=tk action=softset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD align=middle>当前版本</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR><TR><TD align=middle>选择文件</TD>"
	"<TD>&nbsp;<input size=20 type=file name='sl'></TD></TR><TR><TD align=middle colspan=2><FONT color=#ff0000>注意:升级中不可退出</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

//2016.07.14  增加固件升级
const char upload_htm_ch[]={"<HTML><HEAD><TITLE>upload</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18>打印机固件升级<B></TD></TR><TR><TD><FORM name=tk action=uploadset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD align=middle>当前版本</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR>"
	"<TR><TD align=middle colspan=2><FONT color=#ff0000>注意:升级中不可退出</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2>"
	"<input class=button type=submit name=upload value='进入升级'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};


const char sp_htm_ch[]={"<HTML><HEAD><TITLE>sp</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.sp_baud.value=spbaud;document.tk.sp_bit.value=spbit;document.tk.sp_check.value=spcheck;"
	"document.tk.sp_control.value=spcontrol;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>串口参数</TD></TR><TR><TD><FORM name=tk action=spset method=POST><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR><TR><TD align=middle>波特率</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_baud'><option value=0>1200<option value=1>2400"
	"<option value=2>4800<option value=3>9600<option value=4>19200<option value=5>38400<option value=6>57600<option value=7>115200</select></TD></TR><TR><TD align=middle>数据位数</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_bit'><option value=0>8 位"
	"<option value=1>7 位</select></TD></TR><TR><TD align=middle>校验</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_check'><option value=0>无校验<option value=1>奇校验<option value=3>偶校验</select></TD></TR>"
	"<TR><TD align=middle>流控</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_control'><option value=0>CTS/RTS<option value=1>XON/XOFF</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char start_htm_ch[]={"<HTML><HEAD><TITLE>start</TITLE><META http-equiv=Content-Type content=\\text/html; charset=gb2312\\></HEAD>"
"<frameset cols=129,*><frame name=af marginWidth=4 marginHeight=0 src='menu'><frame name=bf noresize src='port'></FRAMESET></HTML>"};

const char symbol_htm_ch[]={"<HTML><HEAD><TITLE>symbol</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.symbolset.value=FactorID;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=47%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>制造商内容</TD></TR><TR><TD><FORM name=tk action=symbolset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle width=100>内容</TD><TD align=middle>值</TD></TR>"
  "<TR><TD align=middle rowspan=1>制造商内容</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=symbolset>&nbsp;<FONT color=#ff0000>注意:最多20个英文字符</TD></TR>"     
  "<TR bgcolor=#CAE1FF><TD class='submit' height=40%% align=center colspan=2><input type=button name='rst' value='重 置' onclick='window.location.reload();'>&nbsp;&nbsp;"
  "<input type=submit name='upload' value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char usb_htm_ch[]={"<HTML><HEAD><TITLE>usb</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<Script language='javascript'>function vl(){document.tk.print_id.value=printid;document.tk.usb_id.value=usb_id;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>USB ID</TD></TR><TR><TD><FORM name=tk action=usbset method=post><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR><TR><TD>&nbsp;USB序列号</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_id></TD><TR><TR><TD align=middle>USB ID</TD><TD align=middle><select name='usb_id'><option value=0>使能<option value=1>禁止</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
  "<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char ok_htm_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "</HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "网口参数设置成功，请用新参数重新登录！</TD></TR></DIV></BODY></HTML>"};

const char error_htm_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR>"
	"<TR align=middle><TD class=title1 height=18><B>网口参数设置失败，请重新设置！</TD></TR></DIV></BODY></HTML>"};

const char ok_w_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "参数设置成功！</TD></TR></DIV></BODY></HTML>"};

const char ok_s_ch[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "请等待打印进入升级模式并刷新页面！</TD></TR></DIV></BODY></HTML>"};

const char language_htm_e[]={"<HTML><HEAD><TITLE>Login</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>" 
  "<Script language='javascript'>function vl(){document.tk.languageset.value=language;}</Script></HEAD><BODY onload='vl();' style=background:#6495ED;margin-left:5;margin-top:0;><table width=100%% height=70%% cellspacing=0>" 
  "<tr height=30 style=background:#EEEE00><td></td></tr><tr height=70><td></td></tr><tr height=70><td align=middle style=font-family:宋体;font-size:26pt;width:60%%><BR><BR><B>Printer</B></td></tr><tr><td align=center>" 
  "<BR><BR><BR><TABLE borderColor=#6495ED width=299 height=79 border=1><TR><FORM name=tk action=languageset method=POST>"
  "<TR><TD align=middle><B>Language Choose</B></TD></TR><TR><TD align=middle><select name='languageset'><option value=0>English<option value=1>Chinese"
  "<option value=2>Chinese-bi5</select></TD></TR><TR><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>" 
  "&nbsp;<input class=button type=submit name=upload value='Submit'></TD></TR><TR><TD align=middle><FONT>Into the Set interface after Select the language and submit !</TD></TR></TR></BODY></HTML>"};

const char menu_htm_e[]={"<HTML><HEAD><TITLE>menu</TITLE><META http-equiv=content-type content='text/html; charset=gb2312'></HEAD><BR><BR><BR><BR><BR><BR><BR><BR>"
  "<BODY style='background: #6495ED;margin-left:4;margin-top:0;margin-bottom:0;margin-right:0;'><TABLE class=font9 cellSpacing=0 cellPadding=0 width=118 border=0 style='cursor:hand'>"
	"<TD valign=top bgColor=#EEEE00 height=20><P align=center><BR><A href='port' target=bf>Net IP<BR><BR><A href='sp' target=bf>Usart Parameter<BR><BR>"
	"<A href='usb' target=bf>USB Parameter<BR><BR><A href='print' target=bf>Printer Parameter<BR><BR>"
	"<A href='prtset' target=bf>Set Parameter<BR><BR>"    //2017.10.11
	"<A href='symbol' target=bf>Company Content<BR><BR>"
	"<A href='upload' target=bf>UpLoad Firmware<BR><BR></P></TD></TR></TABLE></BODY></HTML>"};  //2016.07.14
	
const char port_htm_e[]={"<HTML><HEAD><TITLE>port</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.IpAddress.value=IPAddr;document.tk.SubnetMask.value=Netmask;"
	"document.tk.Gateway.value=Gateway;document.tk.MacAddress.value=MacAddr;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=249 width=41%% border=1>"
  "<TR><TD align=middle class=title1 height=18><B>Net IP</TD></TR><TR><TD><FORM name=tk action=portset method=POST>"
  "<TABLE width='100%%'height='100%%' border=1><TR bgcolor=#CAE1FF><TD align='middle'>Content</TD><TD align='middle'>Value</TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;Printer Port<TD>&nbsp;9100</TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;IP Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='IpAddress'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;Sub Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='SubnetMask'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;Gate Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='Gateway'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;MAC Address<TD>&nbsp;<input size=17 type='text' maxlength=17 name='MacAddress'></TD></TR>"
// 	"<TR><TD>&nbsp;&nbsp;&nbsp;DHCP<TD>&nbsp;<select name='dhcp_flag'><option value=0>OFF<option value=1>ON</select></TD></TR>"   //2017.02.07
	"<TR align=center><TD colspan=2><FONT color=#ff0000>Please re-login after changing the IP !</TD></TR>"
  "<TR bgcolor=#CAE1FF><TD class='submit' height='15%%' align=center colspan=2><input class=button type=button name='rst' value='Reload' onclick='window.location.reload();'>" 
  "<input class=button type=submit name='upload' value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char print_htm_e[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.print_type.value=printtype;"
	"document.tk.print_speed.value=printspeed;document.tk.print_colour.value=printcolour;document.tk.print_wide.value=printwide;"
	"document.tk.print_direction.value=printdirection;document.tk.print_font.value=printfont;document.tk.cut_control.value=cutcontrol;"
	"document.tk.pne_print.value=pneprint;"       //缺纸重打印
	"document.tk.buzzer_control.value=buzzercontrol;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=369 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>Printer Parameter</TD></TR><TR><TD><FORM name=tk action=printset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD>&nbsp;Printer model</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_type></TD></TR>"
	"<TR><TD>&nbsp;Print Speed</TD><TD>&nbsp;<select name='print_speed'>"
	"<option value=0>250mm/s<option value=1>250mm/s<option value=2>250mm/s</select></TD></TR><TR><TD>&nbsp;Print Density</TD><TD>&nbsp;<select name='print_colour'><option value=0>Light<option value=1>Medium<option value=2>High</select></TD></TR>"
	"<TR><TD>&nbsp;Print Width</TD><TD>&nbsp;<select name='print_wide'><option value=0>72mm<option value=1>64mm</select></TD></TR>"
	"<TR><TD>&nbsp;Print Direction</TD><TD>&nbsp;<select name='print_direction'><option value=0>Normal<option value=1>Reverse</select></TD></TR>"
	"<TR><TD>&nbsp;Print Font</TD><TD>&nbsp;<select name='print_font'><option value=0>12*24<option value=1>9*17</select></TD></TR>"
	"<TR><TD>&nbsp;Auto Cut</TD><TD>&nbsp;<select name='cut_control'><option value=0>Off<option value=1>On</select></TD></TR>"
	"<TR><TD>&nbsp;ChangePaperRePrint</TD><TD>&nbsp;<select name='pne_print'><option value=0>Off<option value=1>On</select></TD></TR>"
	"<TR><TD>&nbsp;Buzzer Enable</TD><TD>&nbsp;<select name='buzzer_control'><option value=0>Off<option value=1>Error On<option value=2>Cut On<option value=3>Error&Cut On</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char prtset_htm_e[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.bill_alarm.value=billalarm;"
	"document.tk.speaker_volume.value=speakervolume;document.tk.blackflag_test.value=blackflagtest;document.tk.paper_sensitivity.value=papersensitivity;"
	"document.tk.enter_command.value=entercommand;document.tk.command_assemblage.value=commandassemblage;document.tk.dhcp_flag.value=dhcpflag;document.tk.normal_code.value=normalcode;"    //增�2017.02.07覦HCP document.tk.dhcp_flag.value=dhcpflag;
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=369 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>Printer Parameter</TD></TR><TR><TD><FORM name=tk action=prtset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD>&nbsp;Bill Buzzer Alarm</TD><TD>&nbsp;<select name='bill_alarm'><option value=0>Off<option value=1>TakeBill On<option value=2>PrintBill On</select></TD></TR>"
	"<TR><TD>&nbsp;Speaker Volume</TD><TD>&nbsp;<select name='speaker_volume'><option value=0>Low<option value=1>Medium<option value=2>Hight<option value=3>OFF</select></TD></TR>"
	"<TR><TD>&nbsp;Black Mark</TD><TD>&nbsp;<select name='blackflag_test'><option value=0>Off<option value=1>On</select></TD></TR>"
	"<TR><TD>&nbsp;Paper Sensor</TD><TD>&nbsp;<select name='paper_sensitivity'><option value=0>Light<option value=1>Medium"
	"<option value=2>High</select></TD></TR><TR><TD>&nbsp;CR_Enable</TD><TD>&nbsp;<select name='enter_command'><option value=0>OFF<option value=1>ON</select></TD></TR>"
	"<TR><TD>&nbsp;Command Set</TD><TD>&nbsp;<select name='command_assemblage'><option value=0>ESC/POS<option value=1>MP<option value=2>IP</select></TD></TR>"
	"<TR><TD>&nbsp;Net DHCP</TD><TD>&nbsp;<select name='dhcp_flag'><option value=0>OFF<option value=1>ON</select></TD></TR>"        //2017.02.07  增加设置DHCP
	"<TR><TD>&nbsp;Code Page</TD><TD>&nbsp;<select name='normal_code'><option value=0>CP437<option value=1>KataKana<option value=2>PC850"
	"<option value=3>PC860<option value=4>PC863<option value=5>PC865<option value=6>WCP1251<option value=7>CP866<option value=8>MIK<option value=9>CP755"
	"<option value=10>Iran<option value=11>Reserve<option value=12>Reserve<option value=13>Reserve<option value=14>Reserve<option value=15>CP862"
	"<option value=16>WCP1252<option value=17>WCP1253<option value=18>CP852<option value=19>CP858<option value=20>Iran II<option value=21>Latvian"
	"<option value=22>CP864<option value=23>ISO-8859-1<option value=24>CP737<option value=25>WCP1257<option value=26>Thai<option value=27>CP720"
	"<option value=28>CP855<option value=29>CP857<option value=30>WCP1250<option value=31>CP775<option value=32>WCP1254<option value=33>WCP1255"
	"<option value=34>WCP1256<option value=35>WCP1258<option value=36>ISO-8859-2<option value=37>ISO-8859-3<option value=38>ISO-8859-4"
	"<option value=39>ISO-8859-5<option value=40>ISO-8859-6<option value=41>ISO-8859-7<option value=42>ISO-8859-8<option value=43>ISO-8859-9"
	"<option value=44>ISO-8859-15<option value=45>Thai2<option value=46>CP856</select></TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
		
const char soft_htm_e[]={"<HTML><HEAD><TITLE>soft</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>Soft Information</TD></TR><TR><TD><FORM name=tk action=softset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD align=middle>Soft Ver</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR><TR><TD align=middle>Choose File</TD>"
	"<TD>&nbsp;<input size=20 type=file name='sl'></TD></TR><TR><TD align=middle colspan=2><FONT color=#ff0000>Attention:Can't exit when upgrading !</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

//2016.07.14  增加固件升级
const char upload_htm_e[]={"<HTML><HEAD><TITLE>upload</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18>Upload Firmware<B></TD></TR><TR><TD><FORM name=tk action=uploadset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR>"
	"<TR><TD align=middle>Soft Ver</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR>"
	"<TR><TD align=middle colspan=2><FONT color=#ff0000>Attention:Can't exit when upgrading !</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2>"
	"<input class=button type=submit name=upload value='Goto Upload'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};


const char sp_htm_e[]={"<HTML><HEAD><TITLE>sp</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.sp_baud.value=spbaud;document.tk.sp_bit.value=spbit;document.tk.sp_check.value=spcheck;"
	"document.tk.sp_control.value=spcontrol;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>Usart Parameter</TD></TR><TR><TD><FORM name=tk action=spset method=POST><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR><TR><TD align=middle>Baud Rate</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_baud'><option value=0>1200<option value=1>2400"
	"<option value=2>4800<option value=3>9600<option value=4>19200<option value=5>38400<option value=6>57600<option value=7>115200</select></TD></TR><TR><TD align=middle>Data bit</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_bit'><option value=0>8 bit"
	"<option value=1>7 bit</select></TD></TR><TR><TD align=middle>Parity</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_check'><option value=0>None<option value=1>Odd<option value=3>Even</select></TD></TR>"
	"<TR><TD align=middle>Handshaking</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_control'><option value=0>CTS/RTS<option value=1>XON/XOFF</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char symbol_htm_e[]={"<HTML><HEAD><TITLE>symbol</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.symbolset.value=FactorID;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=47%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>Company Content</TD></TR><TR><TD><FORM name=tk action=symbolset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle width=100>Content</TD><TD align=middle>Value</TD></TR>"
  "<TR><TD align=middle rowspan=1>Company Content</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=symbolset>&nbsp;<FONT color=#ff0000>Up to 20 characters !</TD></TR>"     
  "<TR bgcolor=#CAE1FF><TD class='submit' height=40%% align=center colspan=2><input type=button name='rst' value='Reload' onclick='window.location.reload();'>&nbsp;&nbsp;"
  "<input type=submit name='upload' value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char usb_htm_e[]={"<HTML><HEAD><TITLE>usb</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<Script language='javascript'>function vl(){document.tk.print_id.value=printid;document.tk.usb_id.value=usb_id;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>USB ID</TD></TR><TR><TD><FORM name=tk action=usbset method=post><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>Content</TD><TD align=middle>Value</TD></TR><TR><TD>&nbsp;USB Serial Number</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_id></TD><TR><TR><TD align=middle>USB ID</TD><TD align=middle><select name='usb_id'><option value=0>Enable<option value=1>Disable</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='Reload' onclick='window.location.reload();'>"
  "<input class=button type=submit name=upload value='Submit'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char ok_htm_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "</HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "Network parameters setting is successful, please re-login with new parameters !</TD></TR></DIV></BODY></HTML>"};

const char error_htm_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR>"
	"<TR align=middle><TD class=title1 height=18><B>Network parameters setting failed, please reset !</TD></TR></DIV></BODY></HTML>"};

const char ok_w_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "Parameter is set successfully !</TD></TR></DIV></BODY></HTML>"};

const char ok_s_e[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "Please wait for Upgrade Mode and Refresh!</TD></TR></DIV></BODY></HTML>"};

const char language_htm_b[]={"<HTML><HEAD><TITLE>Login</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>" 
  "<Script language='javascript'>function vl(){document.tk.languageset.value=language;}</Script></HEAD><BODY onload='vl();' style=background:#6495ED;margin-left:5;margin-top:0;><table width=100%% height=70%% cellspacing=0>" 
  "<tr height=30 style=background:#EEEE00><td></td></tr><tr height=70><td></td></tr><tr height=70><td align=middle style=font-family:宋体;font-size:26pt;width:60%%><BR><BR><B>打印機</B></td></tr><tr><td align=center>" 
  "<BR><BR><BR><TABLE borderColor=#6495ED width=299 height=79 border=1><TR><FORM name=tk action=languageset method=POST>"
  "<TR><TD align=middle><B>語言選擇</B></TD></TR><TR><TD align=middle><select name='languageset'><option value=0>English<option value=1>中文簡體"
  "<option value=2>中文繁體</select></TD></TR><TR><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>" 
  "&nbsp;<input class=button type=submit name=upload value='提 交'></TD></TR><TR><TD align=middle><FONT>選擇語言并提交后進入設置界面！</TD></TR></TR></BODY></HTML>"};

const char menu_htm_b[]={"<HTML><HEAD><TITLE>menu</TITLE><META http-equiv=content-type content='text/html; charset=gb2312'></HEAD><BR><BR><BR><BR><BR><BR><BR><BR>"
  "<BODY style='background: #6495ED;margin-left:4;margin-top:0;margin-bottom:0;margin-right:0;'><TABLE class=font9 cellSpacing=0 cellPadding=0 width=118 border=0 style='cursor:hand'>"
	"<TD valign=top bgColor=#EEEE00 height=20><P align=center><BR><A href='port' target=bf>網口IP<BR><BR><A href='sp' target=bf>串口參數<BR><BR>"
	"<A href='usb' target=bf>USB 參數<BR><BR><A href='print' target=bf>打印機參數<BR><BR>"
	"<A href='prtset' target=bf>設置參數<BR><BR>"      //2017.10.11
	"<A href='symbol' target=bf>製造商内容<BR><BR>"
	"<A href='upload' target=bf>固件升级<BR><BR></P></TD></TR></TABLE></BODY></HTML>"};  //2016.07.14 增加固件升级选项 upload
	
const char port_htm_b[]={"<HTML><HEAD><TITLE>port</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.IpAddress.value=IPAddr;document.tk.SubnetMask.value=Netmask;"
	"document.tk.Gateway.value=Gateway;document.tk.MacAddress.value=MacAddr;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=249 width=41%% border=1>"
  "<TR><TD align=middle class=title1 height=18><B>網口IP</TD></TR><TR><TD><FORM name=tk action=portset method=POST>"
  "<TABLE width='100%%'height='100%%' border=1><TR bgcolor=#CAE1FF><TD align='middle'>内容</TD><TD align='middle'>值</TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;打印端口<TD>&nbsp;9100</TD></TR><TR><TD>&nbsp;&nbsp;&nbsp;IP地址<TD>&nbsp;<input size=17 type='text' maxlength=17 name='IpAddress'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;子網掩碼<TD>&nbsp;<input size=17 type='text' maxlength=17 name='SubnetMask'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;網關<TD>&nbsp;<input size=17 type='text' maxlength=17 name='Gateway'></TD></TR>"
	"<TR><TD>&nbsp;&nbsp;&nbsp;MAC地址<TD>&nbsp;<input size=17 type='text' maxlength=17 name='MacAddress'></TD></TR>"
// 	"<TR><TD>&nbsp;&nbsp;&nbsp;DHCP<TD>&nbsp;<select name='dhcp_flag'><option value=0>關閉<option value=1>開啟</select></TD></TR>"    //2017.02.07 增加DHCP选项
	"<TR align=center><TD colspan=2><FONT color=#ff0000>注意:修改IP后請重新登錄</TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height='15%%' align=center colspan=2><input class=button type=button name='rst' value='重 置' onclick='window.location.reload();'>" 
  "<input class=button type=submit name='upload' value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char print_htm_b[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.print_type.value=printtype;"
	"document.tk.print_speed.value=printspeed;document.tk.print_colour.value=printcolour;document.tk.print_wide.value=printwide;"
	"document.tk.print_direction.value=printdirection;document.tk.print_font.value=printfont;document.tk.cut_control.value=cutcontrol;"
	"document.tk.pne_print.value=pneprint;"       //缺纸重打印
	"document.tk.buzzer_control.value=buzzercontrol;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>打印機參數</TD></TR><TR><TD><FORM name=tk action=printset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD>&nbsp;打印機型號</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_type></TD></TR>"	
	"<TR><TD>&nbsp;打印速度</TD><TD>&nbsp;<select name='print_speed'><option value=0>250mm/s<option value=1>150mm/s<option value=2>250mm/s</select></TD></TR>"
	"<TR><TD>&nbsp;打印濃度</TD><TD>&nbsp;<select name='print_colour'><option value=0>淺色<option value=1>中度<option value=2>深色</select></TD></TR>"
	"<TR><TD>&nbsp;打印寬度</TD><TD>&nbsp;<select name='print_wide'><option value=0>72mm<option value=1>64mm</select></TD></TR>"
	"<TR><TD>&nbsp;打印方向</TD><TD>&nbsp;<select name='print_direction'><option value=0>正常<option value=1>反向</select></TD></TR>"
	"<TR><TD>&nbsp;打印字體</TD><TD>&nbsp;<select name='print_font'><option value=0>12*24<option value=1>9*17</select></TD></TR>"
	"<TR><TD>&nbsp;切刀控制</TD><TD>&nbsp;<select name='cut_control'><option value=0>關閉<option value=1>開啟</select></TD></TR>"
	"<TR><TD>&nbsp;缺紙重打印</TD><TD>&nbsp;<select name='pne_print'><option value=0>關閉<option value=1>開啟</select></TD></TR>"
	"<TR><TD>&nbsp;蜂鳴器模式</TD><TD>&nbsp;<select name='buzzer_control'><option value=0>關閉<option value=1>錯誤允許<option value=2>切刀允許<option value=3>錯誤和切刀允許</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char prtset_htm_b[]={"<HTML><HEAD><TITLE>print</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.bill_alarm.value=billalarm;"
	"document.tk.speaker_volume.value=speakervolume;document.tk.blackflag_test.value=blackflagtest;document.tk.paper_sensitivity.value=papersensitivity;"
	"document.tk.enter_command.value=entercommand;document.tk.command_assemblage.value=commandassemblage;document.tk.dhcp_flag.value=dhcpflag;document.tk.normal_code.value=normalcode;"
	"}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><TABLE borderColor=#6495ED width=350 height=80 border=1><TR align=middle>"
  "<TD class=title1 height=18><B>設置參數</TD></TR><TR><TD><FORM name=tk action=prtset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD>&nbsp;打單蜂鳴器報警</TD><TD>&nbsp;<select name='bill_alarm'><option value=0>關閉<option value=1>未取單報警<option value=2>來單報警</select></TD></TR>"
	"<TR><TD>&nbsp;語音音量</TD><TD>&nbsp;<select name='speaker_volume'><option value=0>低<option value=1>中<option value=2>高<option value=3>關</select></TD></TR>"
	"<TR><TD>&nbsp;黑標檢測</TD><TD>&nbsp;<select name='blackflag_test'><option value=0>關閉<option value=1>開啟</select></TD></TR>"
	"<TR><TD>&nbsp;測紙靈敏度</TD><TD>&nbsp;<select name='paper_sensitivity'><option value=0>低度<option value=1>中度<option value=2>高度</select></TD></TR>"
	"<TR><TD>&nbsp;回車指令</TD><TD>&nbsp;<select name='enter_command'><option value=0>關閉<option value=1>開啟</select></TD></TR>"
	"<TR><TD>&nbsp;指令集</TD><TD>&nbsp;<select name='command_assemblage'><option value=0>ESC/POS<option value=1>WH<option value=2>IP</select></TD></TR>"
	"<TR><TD>&nbsp;網络DHCP</TD><TD>&nbsp;<select name='dhcp_flag'><option value=0>關閉<option value=1>開啟</select></TD></TR>"        //2017.02.07  增加设置DHCP
	"<TR><TD>&nbsp;默認代碼頁</TD><TD>&nbsp;<select name='normal_code'><option value=0>CP437<option value=1>KataKana<option value=2>PC850"
	"<option value=3>PC860<option value=4>PC863<option value=5>PC865<option value=6>WCP1251<option value=7>CP866<option value=8>MIK<option value=9>CP755"
	"<option value=10>Iran<option value=11>Reserve<option value=12>Reserve<option value=13>Reserve<option value=14>Reserve<option value=15>CP862"
	"<option value=16>WCP1252<option value=17>WCP1253<option value=18>CP852<option value=19>CP858<option value=20>Iran II<option value=21>Latvian"
	"<option value=22>CP864<option value=23>ISO-8859-1<option value=24>CP737<option value=25>WCP1257<option value=26>Thai<option value=27>CP720"
	"<option value=28>CP855<option value=29>CP857<option value=30>WCP1250<option value=31>CP775<option value=32>WCP1254<option value=33>WCP1255"
	"<option value=34>WCP1256<option value=35>WCP1258<option value=36>ISO-8859-2<option value=37>ISO-8859-3<option value=38>ISO-8859-4"
	"<option value=39>ISO-8859-5<option value=40>ISO-8859-6<option value=41>ISO-8859-7<option value=42>ISO-8859-8<option value=43>ISO-8859-9"
	"<option value=44>ISO-8859-15<option value=45>Thai2<option value=46>CP856</select></TD></TR><TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
		
const char soft_htm_b[]={"<HTML><HEAD><TITLE>soft</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>程序信息</TD></TR><TR><TD><FORM name=tk action=softset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD align=middle>當前版本</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR><TR><TD align=middle>選擇文件</TD>"
	"<TD>&nbsp;<input size=20 type=file name='sl'></TD></TR><TR><TD align=middle colspan=2><FONT color=#ff0000>注意:升級中不可退出</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
//2016.07.14  增加固件升级
const char upload_htm_b[]={"<HTML><HEAD><TITLE>upload</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.softver.value=softver;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=35%% border=1>"
  "<TR align=middle><TD class=title1 height=18>打印機固件升級<B></TD></TR><TR><TD><FORM name=tk action=uploadset method=POST enctype=multipart/form-data>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD align=middle>當前版本</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=softver></TD></TR>"
	"<TR><TD align=middle colspan=2><FONT color=#ff0000>注意:升級中不可退出</TD></TR><TR bgcolor=#CAE1FF>"
	"<TD class='submit' height=20%% align=center colspan=2>"
	"<input class=button type=submit name=upload value='進入升級'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char sp_htm_b[]={"<HTML><HEAD><TITLE>sp</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.sp_baud.value=spbaud;document.tk.sp_bit.value=spbit;document.tk.sp_check.value=spcheck;"
	"document.tk.sp_control.value=spcontrol;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>串口參數</TD></TR><TR><TD><FORM name=tk action=spset method=POST><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>内容</TD><TD align=middle>值</TD></TR><TR><TD align=middle>波特率</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_baud'><option value=0>1200<option value=1>2400"
	"<option value=2>4800<option value=3>9600<option value=4>19200<option value=5>38400<option value=6>57600<option value=7>115200</select></TD></TR><TR><TD align=middle>數據位數</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_bit'><option value=0>8 bit"
	"<option value=1>7 bit</select></TD></TR><TR><TD align=middle>校驗</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_check'><option value=0>無校驗<option value=1>奇校驗<option value=3>偶校驗</select></TD></TR>"
	"<TR><TD align=middle>流控</TD><TD>&nbsp;&nbsp;&nbsp;<select name='sp_control'><option value=0>CTS/RTS<option value=1>XON/XOFF</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
	"<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char symbol_htm_b[]={"<HTML><HEAD><TITLE>symbol</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<Script language='javascript'>function vl(){document.tk.symbolset.value=FactorID;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED height=80 width=47%% border=1>"
  "<TR align=middle><TD class=title1 height=18><B>製造商内容</TD></TR><TR><TD><FORM name=tk action=symbolset method=POST>"
	"<TABLE id='table' width=100%% height=100%% border=1><TR bgcolor=#CAE1FF><TD align=middle width=100>内容</TD><TD align=middle>值</TD></TR>"
  "<TR><TD align=middle rowspan=1>製造商内容</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=symbolset>&nbsp;<FONT color=#ff0000>注意:最多20個英文字符</TD></TR>"     
  "<TR bgcolor=#CAE1FF><TD class='submit' height=40%% align=center colspan=2><input type=button name='rst' value='重 置' onclick='window.location.reload();'>&nbsp;&nbsp;"
  "<input type=submit name='upload' value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};
	
const char usb_htm_b[]={"<HTML><HEAD><TITLE>usb</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<Script language='javascript'>function vl(){document.tk.print_id.value=printid;document.tk.usb_id.value=usb_id;}</Script></HEAD><BODY onload='vl();'><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TABLE borderColor=#6495ED width=300 height=80 border=1>"
  "<TR align=middle><TD class=title1 height=18><B>USB ID</TD></TR><TR><TD><FORM name=tk action=usbset method=post><TABLE id='table' width=100%% height=100%% border=1>"
	"<TR bgcolor=#CAE1FF><TD align=middle>內容</TD><TD align=middle>值</TD></TR>"
	"<TR><TD>&nbsp;USB序列號</TD><TD>&nbsp;<input size=20 type=text maxlength=20 name=print_id></TD><TR>"
	"<TR><TD align=middle>USB ID</TD><TD align=middle><select name='usb_id'><option value=0>使能<option value=1>禁止</select></TD></TR>"
	"<TR bgcolor=#CAE1FF><TD class='submit' height=20%% align=center colspan=2><input class=button type=button name=rst value='重 置' onclick='window.location.reload();'>"
  "<input class=button type=submit name=upload value='提 交'></TD></TR></TABLE></FORM></TD></TR></TABLE></DIV></BODY></HTML>"};

const char ok_htm_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "</HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "網口參數設置成功，請用新參數重新登錄！</TD></TR></DIV></BODY></HTML>"};

const char error_htm_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
	"<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR>"
	"<TR align=middle><TD class=title1 height=18><B>网口參數设置失敗，請重新設置！</TD></TR></DIV></BODY></HTML>"};

const char ok_w_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "參數設置成功！</TD></TR></DIV></BODY></HTML>"};

const char ok_s_b[]={"<HTML><HEAD><TITLE>ok</TITLE><META http-equiv=Content-Type content='text/html; charset=gb2312'>"
  "<meta http-equiv=\"Refresh\" content=\"2\"></HEAD><BODY><DIV align=center><BR><BR><BR><BR><BR><BR><BR><BR><BR><BR><TR align=middle><TD class=title1 height=18><B>"
  "請等待進入升級模式并刷新頁面！</TD></TR></DIV></BODY></HTML>"};
#endif

