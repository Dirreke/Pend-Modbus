#include "modbus.h"
#include "modbus_uart.h"
#include "modbusCRC.h"

MODBUS modbus;
extern u16 Reg[];

/*
��Ϊ������ 9600
1λ���ݵ�ʱ��Ϊ 1000000us/9600bit/s=104us
һ���ֽ�Ϊ    104us*10λ  =1040us
���� MODBUSȷ��һ������֡��ɵ�ʱ��Ϊ   1040us*3.5=3.64ms  ->10ms
*/

void Mosbus_Init()
{
	modbus.myadd = 4;  //�����豸�ĵ�ַ
	modbus.timrun = 0; //MODbus��ʱ��ֹͣ��ʱ
	my_usart_Init();
}

void Modbud_fun3() //3�Ź����봦��  ---����Ҫ��ȡ���ӻ��ļĴ���
{
	u16 Regadd;
	u16 Reglen;
	u16 byte;
	u16 i, j;
	u16 crc;
	Regadd = modbus.rcbuf[2] * 256 + modbus.rcbuf[3]; //�õ�Ҫ��ȡ�ļĴ������׵�ַ
	Reglen = modbus.rcbuf[4] * 256 + modbus.rcbuf[5]; //�õ�Ҫ��ȡ�ļĴ���������
	i = 0;

	modbus.Sendbuf[i++] = modbus.myadd; //���豸��ַ
	modbus.Sendbuf[i++] = 0x03;			//������
	byte = Reglen * 2;					//Ҫ���ص������ֽ���
										//modbus.Sendbuf[i++]=byte/256;  //
	modbus.Sendbuf[i++] = byte % 256;

	for (j = 0; j < Reglen; j++)
	{
		modbus.Sendbuf[i++] = Reg[Regadd + j] / 256;
		modbus.Sendbuf[i++] = Reg[Regadd + j] % 256;
	}
	crc = crc16(modbus.Sendbuf, i);
	modbus.Sendbuf[i++] = crc / 256; //
	modbus.Sendbuf[i++] = crc % 256;

	for (j = 0; j < i; j++)
	{
		my_usart_byte(modbus.Sendbuf[j]);
	}
}

void Modbud_fun6() //6�Ź����봦��
{
	u16 Regadd;
	u16 val;
	u16 i, crc, j;
	i = 0;
	Regadd = modbus.rcbuf[2] * 256 + modbus.rcbuf[3]; //�õ�Ҫ�޸ĵĵ�ַ
	val = modbus.rcbuf[4] * 256 + modbus.rcbuf[5];	  //�޸ĺ��ֵ
	Reg[Regadd] = val;								  //�޸ı��豸��Ӧ�ļĴ���

	//����Ϊ��Ӧ����

	modbus.Sendbuf[i++] = modbus.myadd; //���豸��ַ
	modbus.Sendbuf[i++] = 0x06;			//������
	modbus.Sendbuf[i++] = Regadd / 256;
	modbus.Sendbuf[i++] = Regadd % 256;
	modbus.Sendbuf[i++] = val / 256;
	modbus.Sendbuf[i++] = val % 256;
	crc = crc16(modbus.Sendbuf, i);
	modbus.Sendbuf[i++] = crc / 256; //
	modbus.Sendbuf[i++] = crc % 256;

	for (j = 0; j < i; j++)
	{
		my_usart_byte(modbus.Sendbuf[j]);
	}
}

void Modbud_fun10() //10�Ź����봦��  ---����Ҫ��ȡ���ӻ��ļĴ���
{
	u16 Regadd;
	u16 Reglen;
	u16 byte;
	u16 i, j;
	u16 crc;
	Regadd = modbus.rcbuf[2] * 256 + modbus.rcbuf[3]; //�õ�Ҫд��ļĴ������׵�ַ
	Reglen = modbus.rcbuf[4] * 256 + modbus.rcbuf[5]; //�õ�Ҫд��ļĴ���������

	for (j = 0; j < Reglen; j++)
	{
		Reg[Regadd + j] = modbus.rcbuf[6+j*2]*256 + modbus.rcbuf[7+j*2];
	}

	i = 0;
	modbus.Sendbuf[i++] = modbus.myadd; //���豸��ַ
	modbus.Sendbuf[i++] = 0x10;			//������
	modbus.Sendbuf[i++] = modbus.rcbuf[2];
	modbus.Sendbuf[i++] = modbus.rcbuf[3];
	modbus.Sendbuf[i++] = modbus.rcbuf[4];
	modbus.Sendbuf[i++] = modbus.rcbuf[5];

	crc = crc16(modbus.Sendbuf, i);
	modbus.Sendbuf[i++] = crc / 256; //
	modbus.Sendbuf[i++] = crc % 256;

	for (j = 0; j < i; j++)
	{
		my_usart_byte(modbus.Sendbuf[j]);
	}
}

void Mosbus_Event()
{
	u16 crc;
	u16 rccrc;
	if (modbus.reflag == 0) //û���յ�MODbus�����ݰ�
	{
		return;
	}
	if(modbus.recount <= 2)
	{
		modbus.recount = 0;
		modbus.reflag=0;	
		return;
	}
	crc = crc16(&modbus.rcbuf[0], modbus.recount - 2);								   //����У����
	rccrc = modbus.rcbuf[modbus.recount - 2] * 256 + modbus.rcbuf[modbus.recount - 1]; //�յ���У����
	if (crc == rccrc)																   //���ݰ�����CRCУ�����
	{
		if (modbus.rcbuf[0] == modbus.myadd) //ȷ�����ݰ��Ƿ��Ƿ������豸��
		{
			switch (modbus.rcbuf[1]) //����������
			{
			case 0:
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				Modbud_fun3();
				break; //3�Ź����봦��
			case 4:
				break;
			case 5:
				break;
			case 6:
				Modbud_fun6();
				break; //6�Ź����봦��
			case 7:
				break;
				//....
			case 16:
				Modbud_fun10();//10�Ź�������
			}
		}
		else if (modbus.rcbuf[0] == 0) //�㲥��ַ
		{
		}
	}

	modbus.recount = 0; //
	modbus.reflag = 0;
}
