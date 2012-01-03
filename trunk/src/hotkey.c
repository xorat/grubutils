/*
 *  GRUB Utilities --  Utilities for GRUB Legacy, GRUB2 and GRUB for DOS
 *  Copyright (C) 2007 Bean (bean123ch@gmail.com)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "grub4dos.h"
static int my_app_id = 0;/* this is needed, see the following comment. */
typedef struct
{
  unsigned short code[4];
  char name[10];
} key_tab_t;

typedef struct 
{
	unsigned short key_code;
	unsigned short title_num;
} hotkey_t;
#define HOTKEY_MAGIC 0X79654B48
#define HOTKEY_PROG_MEMORY	0x2000000-0x200000
#define HOTKEY_FUNC *(int*)0x827C
static key_tab_t key_table[90] = {
//  {{0x011b, 0x011b, 0x011b, 0x0100}, "esc"},
  {{0x0231, 0x0221, 0x0000, 0x7800}, "1"},
  {{0x0332, 0x0340, 0x0300, 0x7900}, "2"},
  {{0x0433, 0x0423, 0x0000, 0x7a00}, "3"},
  {{0x0534, 0x0524, 0x0000, 0x7b00}, "4"},
  {{0x0635, 0x0625, 0x0000, 0x7c00}, "5"},
  {{0x0736, 0x075e, 0x071e, 0x7d00}, "6"},
  {{0x0837, 0x0826, 0x0000, 0x7e00}, "7"},
  {{0x0938, 0x092a, 0x0000, 0x7f00}, "8"},
  {{0x0a39, 0x0a28, 0x0000, 0x8000}, "9"},
  {{0x0b30, 0x0b29, 0x0000, 0x8100}, "0"},
  {{0x0c2d, 0x0c5f, 0x0c1f, 0x8200}, "-"},
  {{0x0d3d, 0x0d2b, 0x0000, 0x8300}, "="},
//  {{0x0e08, 0x0e08, 0x0e7f, 0x0e00}, "backspace"},
//  {{0x0f09, 0x0f00, 0x9400, 0xa500}, "tab"},
  {{0x1071, 0x1051, 0x1011, 0x1000}, "q"},
  {{0x1177, 0x1157, 0x1117, 0x1100}, "w"},
  {{0x1265, 0x1245, 0x1205, 0x1200}, "e"},
  {{0x1372, 0x1352, 0x1312, 0x1300}, "r"},
  {{0x1474, 0x1454, 0x1414, 0x1400}, "t"},
  {{0x1579, 0x1559, 0x1519, 0x1500}, "y"},
  {{0x1675, 0x1655, 0x1615, 0x1600}, "u"},
  {{0x1769, 0x1749, 0x1709, 0x1700}, "i"},
  {{0x186f, 0x184f, 0x180f, 0x1800}, "o"},
  {{0x1970, 0x1950, 0x1910, 0x1900}, "p"},
  {{0x1a5b, 0x1a7b, 0x1a1b, 0x1a00}, "["},
  {{0x1b5d, 0x1b7d, 0x1b1d, 0x1b00}, "]"},
//  {{0x1c0d, 0x1c0d, 0x1c0a, 0xa600}, "enter"},
  {{0x1e61, 0x1e41, 0x1e01, 0x1e00}, "a"},
  {{0x1f73, 0x1f53, 0x1f13, 0x1f00}, "s"},
  {{0x2064, 0x2044, 0x2004, 0x2000}, "d"},
  {{0x2166, 0x2146, 0x2106, 0x2100}, "f"},
  {{0x2267, 0x2247, 0x2207, 0x2200}, "g"},
  {{0x2368, 0x2348, 0x2308, 0x2300}, "h"},
  {{0x246a, 0x244a, 0x240a, 0x2400}, "j"},
  {{0x256b, 0x254b, 0x250b, 0x2500}, "k"},
  {{0x266c, 0x264c, 0x260c, 0x2600}, "l"},
  {{0x273b, 0x273a, 0x0000, 0x2700}, ";"},
  {{0x2827, 0x2822, 0x0000, 0x0000}, "'"},
  {{0x2960, 0x297e, 0x0000, 0x0000}, "`"},
  {{0x2b5c, 0x2b7c, 0x2b1c, 0x2600}, "\\"},
  {{0x2c7a, 0x2c5a, 0x2c1a, 0x2c00}, "z"},
  {{0x2d78, 0x2d58, 0x2d18, 0x2d00}, "x"},
  {{0x2e63, 0x2e43, 0x2e03, 0x2e00}, "c"},
  {{0x2f76, 0x2f56, 0x2f16, 0x2f00}, "v"},
  {{0x3062, 0x3042, 0x3002, 0x3000}, "b"},
  {{0x316e, 0x314e, 0x310e, 0x3100}, "n"},
  {{0x326d, 0x324d, 0x320d, 0x3200}, "m"},
  {{0x332c, 0x333c, 0x0000, 0x0000}, ","},
  {{0x342e, 0x343e, 0x0000, 0x0000}, "."},
//  {{0x352f, 0x352f, 0x9500, 0xa400}, "key-/"},
  {{0x352f, 0x353f, 0x0000, 0x0000}, "/"},
//  {{0x372a, 0x0000, 0x9600, 0x3700}, "key-*"},
//  {{0x3920, 0x3920, 0x3920, 0x3920}, "space"},
  {{0x3b00, 0x5400, 0x5e00, 0x6800}, "f1"},
  {{0x3c00, 0x5500, 0x5f00, 0x6900}, "f2"},
  {{0x3d00, 0x5600, 0x6000, 0x6a00}, "f3"},
  {{0x3e00, 0x5700, 0x6100, 0x6b00}, "f4"},
  {{0x3f00, 0x5800, 0x6200, 0x6c00}, "f5"},
  {{0x4000, 0x5900, 0x6300, 0x6d00}, "f6"},
  {{0x4100, 0x5a00, 0x6400, 0x6e00}, "f7"},
  {{0x4200, 0x5b00, 0x6500, 0x6f00}, "f8"},
  {{0x4300, 0x5c00, 0x6600, 0x7000}, "f9"},
  {{0x4400, 0x5d00, 0x6700, 0x7100}, "f10"},
/*
  {{0x4700, 0x4737, 0x7700, 0x9700}, "home"},
  {{0x4800, 0x4838, 0x8d00, 0x9800}, "up"},
  {{0x4900, 0x4939, 0x8400, 0x9900}, "pgup"},
  {{0x4a2d, 0x4a2d, 0x8e00, 0x4a00}, "key--"},
  {{0x4b00, 0x4b34, 0x7300, 0x9b00}, "left"},
  {{0x4c00, 0x4c35, 0x0000, 0x0000}, "key-5"},
  {{0x4d00, 0x4d36, 0x7400, 0x9d00}, "right"},
  {{0x4e2b, 0x4e2b, 0x0000, 0x4e00}, "key-+"},
  {{0x4f00, 0x4f31, 0x7500, 0x9f00}, "end"},
  {{0x5000, 0x5032, 0x9100, 0xa000}, "down"},
  {{0x5100, 0x5133, 0x7600, 0xa100}, "pgdn"},
*/
  {{0x5200, 0x5230, 0x9200, 0xa200}, "ins"},
  {{0x5300, 0x532e, 0x9300, 0xa300}, "del"},
  {{0x8500, 0x8700, 0x8900, 0x8b00}, "f11"},
  {{0x8600, 0x8800, 0x8a00, 0x8c00}, "f12"},
/*
  {{0x0000, 0x0000, 0x7200, 0x0000}, "prtsc"},
*/
  {{0x0000, 0x0000, 0x0000, 0x0000}, 0}
};

static unsigned short allow_key[9] = {
/*KEY_ENTER       */0x1C0D,
/*KEY_HOME        */0x4700,
/*KEY_UP          */0x4800,
/*KEY_PPAGE       */0x4900,
/*KEY_LEFT        */0x4B00,
/*KEY_RIGHT       */0x4D00,
/*KEY_END         */0x4F00,
/*KEY_DOWN        */0x5000,
/*KEY_NPAGE       */0x5100
};
static char keyname_buf[16];
static char* str_upcase (char* str);
static int get_keycode (char* key);
static int get_key(void);
static int check_hotkey(char **title);
static int check_f11(void);
static int (*_checkkey_)(void);
static char *get_keyname (unsigned short code);
static int check_allow_key(unsigned short key);
/* gcc treat the following as data only if a global initialization like the
 * above line occurs.
 */
asm(".string \"GRUB4DOS\"");
asm(ASM_BUILD_DATE);
/* a valid executable file for grub4dos must end with these 8 bytes */
asm(".long 0x03051805");
asm(".long 0xBCBAA7BA");

/* thank goodness gcc will place the above 8 bytes at the end of the b.out
 * file. Do not insert any other asm lines here.
 */

static int main(char *arg,int flags)
{
	int i;
	char *base_addr;
	char *magic;
	static hotkey_t *hotkey;
	unsigned long hotkey_flags;
	unsigned long *p_hotkey_flags;
	base_addr = (char *)(init_free_mem_start+512);
	hotkey = (hotkey_t*)base_addr;
	p_hotkey_flags = (unsigned long*)(base_addr + 508);
	if (flags == -1)
	{
		int c;
		if (my_app_id != HOTKEY_MAGIC || !hotkey->key_code)
		{
			return getkey();
		}
		hotkey_flags = *p_hotkey_flags;
		c = _checkkey_();
		if (!c || check_allow_key(c))
			return c;
		for (;hotkey->key_code;++hotkey)
		{
			if (hotkey->key_code == c)
				return hotkey_flags|(hotkey->title_num<<16);
		}
		if (hotkey_flags & (1<<29))
			return 0;
		return c;
	}
	else if (flags == 0)
	{
		char **titles;
		titles = (char **)(base_addr + 512);
		for(i=0;i<126;++i)
		{
			if (!*titles)
				break;
			if (hotkey->key_code = check_hotkey(titles))
			{
				hotkey->title_num = i;
				++hotkey;
			}
			++titles;
		}
		hotkey->key_code = 0;
		if (!_checkkey_)
			_checkkey_ = get_key;
		return 1;
	}
	printf("Hotkey for grub4dos by chenall,%s\n",__DATE__);
	hotkey_flags = 1<<31;
	while (*arg == '-')
	{
		++arg;
		if (*(unsigned short*)arg == 0x626E) //nb not boot
			hotkey_flags |= 1<<30;
		else if (*(unsigned short*)arg == 0x636E) //nc not control
			hotkey_flags |= 1<<29;
		else if (*arg == 'u')
		{
			HOTKEY_FUNC = 0;
			return builtin_cmd("delmod","hotkey",flags);
		}
		arg = wee_skip_to(arg,0);
	}
   *p_hotkey_flags = hotkey_flags;
	if (!HOTKEY_FUNC)
	{
		int buff_len;
		char *p;
		p = (char *)&main;
		buff_len = *(int*)(p-20);
		if (buff_len > 0x4000)//文件太大加载失败。限制hotkey程序不可以超过16KB。
			return 0;
		if (check_f11())//检测BIOS是否支持F11,F12热键，如果有支持直接使用getkey函数取得按键码
		{
			_checkkey_ = getkey;
			printf("Current BIOS supported F11,F12 key.\n");
		}
		else
		{
			_checkkey_ = 0;
			printf("Current BIOS Does not support F11,F12 key,try to hack it.\n");
		}
		//HOTKEY程序驻留内存，直接复制自身代码到指定位置。
		my_app_id = HOTKEY_MAGIC;
		memmove((void*)HOTKEY_PROG_MEMORY,p,buff_len);
		//开启HOTKEY支持，即设置hotkey_func函数地址。
		HOTKEY_FUNC = HOTKEY_PROG_MEMORY;
		//获取程序执行时的路径的文件名。
		p = p-*(int*)(p-16);
		strncat(p," hotkey",-1);
		builtin_cmd("insmod",p,flags);
		printf("Hotkey Installed!\n");
	}
	return 0;
}
/*
	检测当前的按键码是否方向键或回车键
*/
static int check_allow_key(unsigned short key)
{
	int i;
	for (i=0;i<9;++i)
	{
		if (key == allow_key[i])
			return key;
	}
	return 0;
}

char *
str_upcase (char *str)
{
  int i;

  for (i = 0; str[i]; i++)
    if ((str[i] >= 'a') && (str[i] <= 'z'))
      str[i] -= 'a' - 'A';

  return str;
}

static char inb(unsigned short port)
{
	char ret_val;
	__asm__ volatile ("inb %%dx,%%al" : "=a" (ret_val) : "d"(port));
	return ret_val;
}

static void outb(unsigned short port, char val)
{
	__asm__ volatile ("outb %%al,%%dx"::"a" (val), "d"(port));
}

/*检查BIOS是否支持F11作为热键*/
static int check_f11(void)
{
	int i;
	//要发送按键的指令。
	outb(0x64,0xd2);
	//发送一个按键扫描码
	outb(0x60,0x57);
	//发送一个按键中断码  扫描码（scan_code）+ 0x80 = 中断码（Break Code）
	outb(0x60,0xD7);
	//判断是否可以接收到这个按键，有的话就是有支持，否则不支持。
	for(i=0;i<3;++i)
	{
		if (checkkey() == 0x8500)
		{
			getkey();
			return 1;
		}
	}
	return 0;
}

static int get_key(void)
{
	while(checkkey()<0)
	{
	//没有检测到按键，可能是用户没有按键或者BIOS不支持
		unsigned char c = inb(0x60);
		//扫描码低于0X57的不处理，直接丢给BIOS。
		if (c < 0x57)
		{
			outb(0x64,0xd2);
			outb(0x60,c);
			break;
		}
		switch(c |= 0x80)//取中断码
		{
			case 0xd7: //F11
				return 0x8500;
			case 0xd8: //F12
				return 0x8600;
		}
	}
	//bios检测到按键，
	return getkey();
}

/*
	从菜单标题中提取热键代码
	返回热键对应的按键码。
*/
static int check_hotkey(char **title)
{
	char *arg = *title;
	unsigned short code;
	while (*arg && *arg <= ' ')
		++arg;
	if (*arg == '^')
	{
		++arg;
		sprintf(keyname_buf,"%.15s",arg);//最多复制15个字符
		nul_terminate(keyname_buf);//在空格处截断
		if ((code = (unsigned short)get_keycode(keyname_buf)))
		{
			//设置新的菜单标题位置
			arg+=strlen(keyname_buf);
			*arg = *(*title);
			*title = arg;
			return code;
		}
	}
	else if (*arg == '[')
	{
		int i;
		++arg;
		while (*arg == ' ' || *arg == '\t')
			++arg;
		for(i = 0;i<15;++arg)
		{
			if (!*arg || *arg == ' ' || *arg == ']' )
			{
				break;
			}
			keyname_buf[i++] = *arg;
		}
		while (*arg == ' ')
			++arg;
		if (*arg != ']')
			return 0;
		keyname_buf[i] = 0;
		code = (unsigned short)get_keycode(keyname_buf);
		return code;
	}
	return 0;
}

static int
get_keycode (char *key)
{
  int idx, i;
  char *str;

  if (*(unsigned short*)key == 0x7830)/* == 0x*/
    {
      unsigned long long code;
      if (!safe_parse_maxint(&key,&code))
			return 0;
      return (((long)code <= 0) || ((long)code >= 0xFFFF)) ? 0 : (int)code;
    }
  str = key;
  idx = 0;
  if (!strnicmp (str, "shift", 5))
    {
      idx = 1;
      str += 6;
    }
  else if (!strnicmp (str, "ctrl", 4))
    {
      idx = 2;
      str += 5;
    }
  else if (!strnicmp (str, "alt", 3))
    {
      idx = 3;
      str += 4;
    }
  for (i = 0; key_table[i].name[0]; i++)
    {
      if (!stricmp(str,key_table[i].name))
      {
			return key_table[i].code[idx];
		}
    }
  return 0;
}


static char *
get_keyname (unsigned short code)
{
  int i;

  for (i = 0; key_table[i].name; i++)
    {
      int j;

      for (j = 0; j < 4; j++)
	{	 
	  if (key_table[i].code[j] == code)
	    {
	      char *p;
	      
	      switch (j)
		{
		case 0:
		  p = "";
		  break;
		case 1:
		  p = "Shift-";
		  break;
		case 2:
		  p = "Ctrl-";
		  break;
		case 3:
		  p = "Alt-";
		  break;
		}
	      sprintf (keyname_buf, "%s%s", p, key_table[i].name);
	      return keyname_buf;
	    }
	}
    }

  sprintf (keyname_buf, "0x%x", code);
  return keyname_buf;
}
