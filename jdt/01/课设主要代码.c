main函数：
#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"
#include "bh1750.h"


//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"
//c库
#include "string.h"

#include"timer.h"
	



//dht11添加变量
u8 humidityH;	  //湿度整数部分
u8 humidityL;	  //湿度小数部分
u8 temperatureH;   //温度整数部分
u8 temperatureL;   //温度小数部分


DHT11_Data_TypeDef DHT11_Data;

float Light = 0; //光照度
void DHT11_OLED(void)
{

if( Read_DHT11(&DHT11_Data)==SUCCESS)               //读取DHT11温湿度成功
	{
		
		
		//oled显示温湿度
			OLED_ShowString(8,0,"TEMP:"); 
			OLED_ShowNum(48,0,DHT11_Data.temp_int,2,16);
			OLED_ShowChar(70,0,'.');
			OLED_ShowNum(85,0,DHT11_Data.temp_deci,1,16);
				
			OLED_ShowString(8,2,"HUM:"); 
			OLED_ShowNum(48,2,DHT11_Data.humi_int,2,16);
			OLED_ShowChar(70,2,'%'); 
			delay_ms(100);
				
		//将其赋值上传云平台时使用的参数
				humidityH=DHT11_Data.humi_int;	  //湿度整数部分
				humidityL=DHT11_Data.humi_deci;	  //湿度小数部分
				temperatureH=DHT11_Data.temp_int;   //温度整数部分
				temperatureL=DHT11_Data.temp_deci;   //温度小数部分
				printf("hum temp=%d .%d %d .%d\r\n",humidityH,humidityL,temperatureH,temperatureL);
				
		
		printf("temp:%d,hum:%d\r\n",DHT11_Data.temp_int,DHT11_Data.humi_int);	
//		u2_printf("temp:%d,hum:%d,light:%d,led_sta:%d\r\n",DHT11_Data.temp_int,DHT11_Data.humi_int,(int)Light,Led_switch_STA);


	}

}



void light_data()
{
		if (!i2c_CheckDevice(BH1750_Addr))//检测iic总线设备
		{
				Light = LIght_Intensity();//获取光照强度
				
		}
		printf("当前光照度为：%.1f lx\n", Light);		
		
		OLED_ShowString(8,4,"Light:"); 
		OLED_ShowNum(56,4,Light*10/10,3,16);
		OLED_ShowString(100,4,"lx"); 
		
}

 int main(void)
 {	

	unsigned char *dataPtr = NULL;
	 
	unsigned short timeCount = 0;	//发送间隔变量
	 
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
 	LED_Init();			     //LED端口初始化
	 
	oled_init(); 
	OLED_Fill(0x00);
 	LED_Init();			     //LED端口初始化    
	 
	uart_init(115200);//串口1初始化
	uart2_init(115200);//串口2初始化
	 
	 TIM3_PWM_Init(899,0);//不分频内部时钟是72MHZ，pwm频率=72000000/900=80khz 
	 
	//初始化温湿度传感器检测引脚
	DHT11_GPIO_Config();  
  BH1750_Init();	 //光照强度传感器
	ESP8266_Init();					//初始化ESP8266
	 

	
	while(OneNet_DevLink())			//接入OneNET(1-成功，0-失败)
	delay_ms(500);
	GPIO_SetBits(GPIOC,GPIO_Pin_13);
	printf("接入onenet成功\r\n");
	while(1) 
	{		
		if(++timeCount >= 500)		//间隔时间
		{
				
			DHT11_OLED();
			light_data();
			  printf( "OneNet_will_SendData\r\n");//通过串口1发送数据
					
				OneNet_SendData();//发送数据给onenet
				
				printf("send_data_end\n");
			
				timeCount = 0;
				
				ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(0);//获取平台返回的数据（不等待）
		if(dataPtr != NULL)//如果返回数据不为空
			OneNet_RevPro(dataPtr);//平台返回数据检测
		
		delay_ms(10);
		
	  
		
}

}







Onenet部分：
//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "esp8266.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
//#include "sht20.h"

//C库
#include <string.h>
#include <stdio.h>

//json库
//#include"cjson.h"
#include "timer.h"

#include"led.h"

#define PROID		"578907"

#define AUTH_INFO	"111222"

#define DEVID		"1055178146"


extern unsigned char esp8266_buf[128];
extern u8 humidityH;	  //湿度整数部分
extern u8 humidityL;	  //湿度小数部分
extern u8 temperatureH;   //温度整数部分
extern u8 temperatureL;   //温度小数部分
extern float Light;

 u8 LED0_FLAG;
 u8 LED1_FLAG;
 
 
 int PWM_value;
//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	//打印连接信息
   printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
	
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		//参数1数据，参数2数据长度
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//上传平台(发送数据)
		
		//+IPD,x:yyy"	x代表数据长度，yyy是数据内容
		dataPtr = ESP8266_GetIPD(250);									//等待平台响应
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))//根据链接消息包返回的数据进行匹配打印信息
				{
					case 0:printf( "Tips:	连接成功\r\n");status = 0;break;
					
					case 1:printf(  "WARN:	连接失败：协议错误\r\n");break;
					case 2:printf(  "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:printf(  "WARN:	连接失败：服务器失败\r\n");break;
					case 4:printf(  "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:printf(  "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					
					default:printf(  "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		printf( "WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

unsigned char OneNet_FillBuf(char *buf)
{
	
	char text[32];//暂时存放数据用
	
	LED0_FLAG=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);
	
	LED1_FLAG=GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
	
	
	
	memset(text, 0, sizeof(text));//清空text数据
	
	strcpy(buf, ",;");//将，；复制进去
	
	memset(text, 0, sizeof(text));//清空text数据
	sprintf(text, "temp,%d.%d;",temperatureH,temperatureL);//将温度格式化后放入text中
	strcat(buf, text);//将温度拼接在后面
	
	memset(text, 0, sizeof(text));//清空text数据
	sprintf(text, "humi,%d.%d;", humidityH,humidityL);//将湿度格式化后放入text中
	strcat(buf, text);//将湿度拼接在后面
	
	//memset(text, 0, sizeof(text));
	//sprintf(text, "light,%d;", (int)Light);//led0的开关状态
	//strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "LED0,%d;", LED0_FLAG);//led0的开关状态
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "LED1,%d;", LED1_FLAG);//led1的开关状态
	strcat(buf, text);
	
	//memset(text, 0, sizeof(text));
	//sprintf(text, "PWM,%d;", PWM_value);//led1的开关状态
	//strcat(buf, text);
	
	printf("send_buf_data=%s\r\n",buf);//打印一下将要上传的数据
	
	return strlen(buf);//返回数据长度

}

//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_SendData(void)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												//协议包
	
	char buf[128];
	
	short body_len = 0, i = 0;
	
	printf( "Tips:	OneNet_SendData-MQTT\r\n");//打印提示信息
	
	memset(buf, 0, sizeof(buf));
	
	body_len = OneNet_FillBuf(buf);																	//获取当前需要发送的数据流的总长度
	
	if(body_len)//如果长度不是0（非空）
	{
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0)							//封包0-成功，1-失败
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			
			ESP8266_SendData(mqttPacket._data, mqttPacket._len);									//上传数据到平台
			printf( "Send %d Bytes\r\n", mqttPacket._len);//打印发送数据的长度
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			printf(  "WARN:	EDP_NewBuffer Failed\r\n");
	}
	
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================

void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	
	char *req_payload = NULL;//存放返回的数据
	char *cmdid_topic = NULL;//存放topic
	
	unsigned short req_len = 0;//数据长度
  unsigned char type = 0;//数据接收类型

	short result = 0;

	char *dataPtr = NULL;

	char numBuf[10];

	int num = 0;
	char a,b,c;

	type = MQTT_UnPacketRecv(cmd);//MQTT数据接收类型判断
	switch(type)
	{
		case MQTT_PKT_CMD:															//命令下发
			
			//参数1收到的
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
				//打印收到的信息，参数2数据，参数3数据长度
				printf(  "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

				if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
				{
					printf( "Tips:	Send CmdResp\r\n");
					
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);//回复命令（给云端发送数据）
					MQTT_DeleteBuffer(&mqttPacket);									//删包
				}
			}
		break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				printf(  "Tips:	MQTT Publish Send OK\r\n");	
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//清空缓存
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, ':');					//搜索':'

	if(dataPtr != NULL && result != -1)					//如果找到了
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		
		numBuf[num] = 0;
		
		num = atoi((const char *)numBuf);				//转为数值形式
		
		
		
		if(strstr((char *)req_payload, "re"))		//搜索"redled"
		{
			if(num == 1)								//控制数据如果为1，代表开
			{
				 GPIO_SetBits(GPIOC,GPIO_Pin_13);
			}
			else if(num == 0)							//控制数据如果为0，代表关
			{
				 GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			}
		}
														//下同
		else if(strstr((char *)req_payload, "gr"))
		{
			if(num == 1)
			{
				GPIO_SetBits(GPIOC,GPIO_Pin_10);
			}
			else if(num == 0)
			{
				GPIO_ResetBits(GPIOC,GPIO_Pin_10);
			}
		}
		else if(strstr((char *)req_payload, "PWM"))
		{
//			printf("pw_value%s\r\n",(char *)req_payload);
			sscanf((char *)req_payload, "{\"%c%c%c\":%d", &a,&b,&c,&PWM_value); //取需要的字符串 
			printf("pwm_get_value=%d\r\n",PWM_value);
			TIM_SetCompare2(TIM3, PWM_value);// 修改 TIM3_CCR2 来控制占空比
		}
		
	}
	
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
