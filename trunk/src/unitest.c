/*
 *  GRUB  --  GRand Unified Bootloader
 *  Copyright (C) 1999,2000,2001,2002,2003,2004  Free Software Foundation, Inc.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * compile:

gcc -nostdlib -fno-zero-initialized-in-bss -fno-function-cse -fno-jump-tables -Wl,-N -fPIE unitest.c

 * disassemble:			objdump -d a.out 
 * confirm no relocation:	readelf -r a.out
 * generate executable:		objcopy -O binary a.out unitest
 *
 */
/*
 * to get this source code & binary: http://grub4dos-chenall.google.com
 * For more information.Please visit web-site at http://chenall.net
 * 2010-03-05
 */
#include "font/font.h"
#include "grub4dos.h"

static FL_HEADER _fl_header;
static DWORD g_dwCharInfo = 0;    // �洢��ǰ�ַ��ļ�����Ϣ��  bit0��bit25����ŵ�����Ϣ����ʼ��ַ�� bit26��bit31��������ؿ�ȡ�
static void DisplayChar(int nYSize, int nBytesPerLine, unsigned char *pFontData);
#define		BASE_FONT_ADDR		0x600000

int GRUB = 0x42555247;/* this is needed, see the following comment. */
/* gcc treat the following as data only if a global initialization like the
 * above line occurs.
 */
asm(".long 0x534F4434");
asm(ASM_BUILD_DATE);
/* a valid executable file for grub4dos must end with these 8 bytes */
asm(".long 0x03051805");
asm(".long 0xBCBAA7BA");

/* thank goodness gcc will place the above 8 bytes at the end of the b.out
 * file. Do not insert any other asm lines here.
 */

int
main ()
{
	void *p = &main;
	char *arg = p - (*(int *)(p - 8));
	int flags = (*(int *)(p - 12));
		if (strlen(arg) <= 0)
	{
		return 1;
	}

	while(arg[0]==' ' || arg[0]=='\t') arg++;

	if (strlen(arg) > 256)
	return !(errnum = ERR_WONT_FIT);

	if (open(arg))
	{
		read ((unsigned long long)(unsigned int)(char *) RAW_ADDR (BASE_FONT_ADDR), -1, 0xedde0d90);
		close();

		if (InitFont())
		{
				unsigned long long ch;
				arg=skip_to(0,arg);
				if (! safe_parse_maxint(&arg,&ch))
					return 0;
				ch &= 0xffff;
			if (ch)
			{
				ReadCharDistX(ch);
				int dwOffset;
				WORD bytesPerLine = 0;
				dwOffset = ReadCharDotArray(ch, &bytesPerLine);
				DisplayChar(_fl_header.YSize, bytesPerLine, (char *)BASE_FONT_ADDR + dwOffset);
				return 0;
			}
		}
		return 1;
	}
	else
	{
		printf ("load fontfile failed!\n");
		return 0;
	}
}

static int ReadFontHeader(PFL_HEADER pfl_header)
{	
	memmove(pfl_header,(char *)BASE_FONT_ADDR,sizeof(FL_HEADER));
	//����ʾͷ
	if(_fl_header.magic[0] != 'U' && _fl_header.magic[1] != 'F' && _fl_header.magic[2] != 'L')
	{
		printf("Cann't support file format!\n");
		return 0;
	}

	_fl_header.pSection = (FL_SECTION_INF *)(BASE_FONT_ADDR + 0x10 );

	return 1;	
}

/***************************************************************
���ܣ� ��ʼ�����塣 ���������ļ����Ҷ�ȡ��Ϣͷ��
������ pFontFile--�ֿ��ļ���      
***************************************************************/
int InitFont()
{
	memset(&_fl_header , 0, sizeof(FL_HEADER));

	return ReadFontHeader(&_fl_header);	
}

/********************************************************************
���ܣ� ��ȡ��ǰ�ַ������ؿ��, �ҽ�������Ϣ����һ��ȫ�ֱ�����g_dwCharInfo��
        ����������Ϣ����ͬʱ�ܻ�ȡ��ǰ�ַ��ĵ�����Ϣ����ʼ��ַ��
������ wCode -- ���ֿ�Ϊunicode�����ʽʱ����wCode��unicode���봦��
                ����֮��MBCS)��
********************************************************************/
int ReadCharDistX(WORD wCode)
{
	if('U' == _fl_header.magic[0])     //unicode ����
		return ReadCharDistX_U(wCode);
	else
		return 0;//ReadCharDistX_M(wCode);
}

/**********************************************************************
���ܣ� ��ȡ������Ϣ
������ wCode ������Ԥ������Ҫ����Ϊǰ���б���һ��ȫ�ֵ�g_dwCharInfo��Ҳ��֪���˸��ַ�����Ӧ��Ϣ(���+������Ϣ����ʼ��ַ)��
       fontArray ��ŵ�����Ϣ
	   bytesPerLine ÿһ��ռ���ٸ��ֽڡ�
**********************************************************************/
int ReadCharDotArray(WORD wCode, WORD *bytesPerLine)
{	
	*bytesPerLine= (WORD)((GET_FONT_WIDTH(g_dwCharInfo))+7)/PIXELS_PER_BYTE;	

	if(g_dwCharInfo > 0)
	{		
		DWORD nDataLen = *bytesPerLine * _fl_header.YSize;			
		DWORD  dwOffset = GET_FONT_OFFADDR(g_dwCharInfo);    //��ȡ�ַ�����ĵ�ַ��Ϣ(��26λ)
		return dwOffset;
	}
	
	return 0;
}

// ��ȡ�ַ������ؿ��
DWORD ReadCharDistX_U(WORD wCode)
{	
	DWORD offset ;
	int   i;
	g_dwCharInfo = 0;
	
	for(i = 0;i<_fl_header.nSection;i++)
	{
		if(wCode >= _fl_header.pSection[i].First && wCode <= _fl_header.pSection[i].Last)
			break;		
	}
	if(i >= _fl_header.nSection)	
		return 0;	

	offset = BASE_FONT_ADDR + _fl_header.pSection[i].OffAddr+ FONT_INDEX_TAB_SIZE*(wCode - _fl_header.pSection[i].First );
	g_dwCharInfo = (*(DWORD *)offset);

	return GET_FONT_WIDTH(g_dwCharInfo);
}

/*****************************************************************
���ܣ����ݵ�����Ϣ��ʾһ����ģ
������
	  nYSize--�ַ��ĸ߶�
	  nBytesPerLine--ÿ��ռ���ֽ�������ʽΪ�������ؿ���������������
      pFontData--�ַ��ĵ�����Ϣ��
*****************************************************************/
static void DisplayChar(int nYSize, int nBytesPerLine, unsigned char *pFontData)
{
	int i, j;
	printf("bytesPerLine = %d\n", nBytesPerLine);

	for(i = 0; i < nYSize; i++)	
	{
		for(j = 0; j < nBytesPerLine; j++)
		{
			int x=0;
			for(x=0;x<8;x++)
				putchar( (pFontData[i * nBytesPerLine + j] & (1 << 7 - x))?'*':' ');
		}
		printf("\n");
	}
}

