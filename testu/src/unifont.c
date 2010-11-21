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

gcc -nostdlib -fno-zero-initialized-in-bss -fno-function-cse -fno-jump-tables -Wl,-N -fPIE unifont.c

 * disassemble:			objdump -d a.out 
 * confirm no relocation:	readelf -r a.out
 * generate executable:		objcopy -O binary a.out unifont
 *
 */
/*
 * to get this source code & binary: http://grub4dos-chenall.google.com
 * For more information.Please visit web-site at http://chenall.net
 * 2010-03-07
 * last modify 2010-10-24
 */
#include "font/font.h"
#include "grub4dos.h"

//#include "fontfile.h"
#define A_NORMAL        0x7
#define A_REVERSE       0x70
#define VIDEOMEM 0xA0000
/* ��������ʱʹ�õ��ڴ� 4M-6M֮��
 * ����ǰ��4KB(0x400000-0x401000)��Ϊȫ�ֱ������λ��
 * 0x401000 ��ʼ����ֿ�+��������
 */
#define VARIABLE_ADDR			0x400000	//���������ʼ��ַ
#define BASE_FONT_ADDR			0x401000	//�ֿ��ļ���ŵ�ַ
#define UNIFONT_LOAD_FLAG		0xBDC6AECB	//�������ڴ����Ѿ����صı�־

#define ushFontReaded			(*(WORD *)(VARIABLE_ADDR + 0x10))	//����ɹ����ر�־
#define g_dwCharInfo			(*(DWORD *)(VARIABLE_ADDR + 0x14))	// �洢��ǰ�ַ��ļ�����Ϣ��  bit0��bit25����ŵ�����Ϣ����ʼ��ַ�� bit26��bit31��������ؿ�ȡ�
#define FONT_WIDTH				(*(DWORD *)(VARIABLE_ADDR + 0x18))
#define ALL_FONT				(*(DWORD *)(VARIABLE_ADDR + 0x1C))
/*ԭʼ���ݱ���*/
#define graphics_cursor_bak		(*(DWORD *)(VARIABLE_ADDR + 0x100))
#define term_backup_orig_addr	((struct term_entry **)(VARIABLE_ADDR + 0x104))
#define term_backup				((struct term_entry *)(VARIABLE_ADDR + 0x108))

#define _fl_header				((PFL_HEADER)(VARIABLE_ADDR + 0x400))


#define VSHADOW VSHADOW1
#define VSHADOW_SIZE 60000
#define VSHADOW1 ((unsigned char *)0x3A0000)
#define VSHADOW2 (VSHADOW1 + VSHADOW_SIZE)
#define VSHADOW4 (VSHADOW2 + VSHADOW_SIZE)
#define VSHADOW8 (VSHADOW4 + VSHADOW_SIZE)
//#define text ((unsigned long *)0x3C5800)
#define TEXT ((unsigned long (*)[x1])0x3FC000)

static int fontfile_func (char *arg ,int flags);
static void graphics_cursor (int set);
static void graphics_setxy (int col, int row);
static void graphics_scroll (void);
static int unifont_unload(void);
void graphics_putchar (int ch);


//static unsigned char mask[16];
//static int graphics_old_cursor;
static int no_scroll = 0;

/* color state */
//static int graphics_standard_color = A_NORMAL;
//static int graphics_normal_color = A_NORMAL;
static int graphics_highlight_color = A_REVERSE;
//static int graphics_helptext_color = A_NORMAL;
//static int graphics_heading_color = A_NORMAL;
//static int graphics_current_color = A_NORMAL;

//int outline = 0;

//int disable_space_highlight = 0;
#define x0 0
#define x1 current_term->chars_per_line
#define y0 0
#define y1 current_term->max_lines

//static char *g_prf = 0x400000;


int GRUB = 0x42555247;/* this is needed, see the following comment. */
/* gcc treat the following as data only if a global initialization like the
 * above line occurs.
 */
asm(".long 0x534F4434");

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

	while(arg[0]==' ' || arg[0]=='\t') arg++;
	if (memcmp(arg,"--unload",8) == 0)
	{
		unifont_unload();
		return 1;
	}
	if (memcmp(arg,"--all-font",10) == 0)
	{
		ALL_FONT = 1;
		arg = skip_to (0,arg);
	} else ALL_FONT = 0;
	if (strlen(arg) > 256)
	return !(errnum = ERR_WONT_FIT);

	if (open(arg))
	{
		if (filemax > 0x1FD000)
			return 0;
		if ((*(DWORD *)VARIABLE_ADDR) == UNIFONT_LOAD_FLAG)
		{
			unifont_unload();
		}
		read ((unsigned long long)(unsigned int)(char *) RAW_ADDR (BASE_FONT_ADDR), -1, 0xedde0d90);
		close();
		if (InitFont())
		{
			memmove((char *)BASE_FONT_ADDR + filemax ,p,(int)&GRUB - (int)p );
			ushFontReaded = 1;
			FONT_WIDTH = 10;
			return fontfile_func(arg,flags);
		}
		return 0;

	}
	errnum = 0;
	printf ("UNIFONT %s\nUse:\nUNIFONT UNIFONT_FILE\tLoad unicode font file.\nUNIFONT --unload\t\tUnload.",__DATE__);
	return 1;
}

static inline void outb(unsigned short port, unsigned char val)
{
    __asm __volatile ("outb %0,%1"::"a" (val), "d" (port));
}

static void MapMask(int value) {
    outb(0x3c4, 2);
    outb(0x3c5, value);
}

/* Chinese Support by Gandalf
 *    These codes used for defining a proper fontfile
 */
static void
graphics_cursor (int set)
{

    unsigned char *pat, *mem, *ptr;
    int i, ch, offset, n, ch1, ch2;
    int invert = 0;
	static int dbcs_ending_byte;
	static unsigned char by[16][2], chsa[16], chsb[16];
	static unsigned char chr[16 << 2];
    if (set && no_scroll)
        return;

    ch = TEXT[fonty][fontx];

	invert = (ch & 0xffff0000) != 0;

	dbcs_ending_byte = 0;
	offset = cursorY * x1 + fontx;
	pat = font8x16;
	ch1 = ch & 0xff;
	pat += (ch1 << 4);

	if (ushFontReaded && ((ch & 0xff00) || (ALL_FONT && ch1 > 0x20 && ch1 != 218 && ch1 != 191 && ch1 != 192 && ch1 != 217 && ch1 != 196 && ch1 != 179)))
	{
		WORD bytesPerLine = 0;
		int dotpos;
		ReadCharDistX(ch &= 0xffff);
		dotpos = ReadCharDotArray(ch, &bytesPerLine);
		if (dotpos)
		{
			dbcs_ending_byte = GET_FONT_WIDTH(g_dwCharInfo) > FONT_WIDTH;
			memmove(by, (unsigned char *) RAW_ADDR (BASE_FONT_ADDR + dotpos), 32);
			pat = chsa;
			for (i = 0;i < 16; i++)
			{
				chsa[i] = by[i][0];
				chsb[i] = by[i][1];
			}
		}
	}

	mem = (unsigned char*)VIDEOMEM + offset;

    if (set)
    {
        MapMask(15);
        ptr = mem;
        for (i = 0; i < 16; i++, ptr += x1)
       	{
//            cursorBuf[i] = pat[i];
            *ptr = ~pat[i];
        }
        return;
    }

write_char:

	for (i = 0; i < 16; i++, offset += x1)
	{
		unsigned char m, p, c1, c2, c4, c8;

		p = pat[i];

		if (invert)
		{
			p = ~p;
			chr[i     ] = p;
			chr[16 + i] = p;
			chr[32 + i] = p;
			chr[48 + i] = p;
			continue;
		}

		chr[i     ] = ((unsigned char*)VSHADOW1)[offset] | p;
		chr[i + 16] = ((unsigned char*)VSHADOW2)[offset] | p;
		chr[i + 32] = ((unsigned char*)VSHADOW4)[offset] | p;
		chr[i + 48] = ((unsigned char*)VSHADOW8)[offset] | p;
	}

    offset = 0;
    for (i = 1; i < 16; i <<= 1, offset += 16)
    {
        int j;

        MapMask(i);
        ptr = mem;
        for (j = 0; j < 16; j++, ptr += x1)
            *ptr = chr[j + offset];
    }

    MapMask(15);
    
	if (dbcs_ending_byte)
	{
		dbcs_ending_byte = 0;

		/* reset the mem position */
		mem++;//mem += 1;//mem = (unsigned char*)VIDEOMEM + offset;
		offset = cursorY * x1 + fontx + 1;
		pat = chsb;
		graphics_setxy(fontx + 1, fonty);
		goto write_char;
	}
}

static void
graphics_setxy (int col, int row)
{
    if (col >= x0 && col < x1)
    {
        fontx = col;
        cursorX = col << 3;
    }

    if (row >= y0 && row < y1)
    {
        fonty = row;
        cursorY = row << 4;
    }
}

static void
graphics_scroll (void)
{
	int i, j;

	/* we don't want to scroll recursively... that would be bad */
	if (no_scroll)
		return;
	no_scroll = 1;

	/* move everything up a line */
	for (j = y0 + 1; j < y1; j++)
	{
		gotoxy (x0, j - 1);

		for (i = x0; i < x1; i++)
		{
			graphics_putchar (TEXT[j][i]);
			if ((TEXT[j][i]) & 0xff00)
			{
				if ((TEXT[j][i+1]) == 0) i++;
			}
		}
	}

	/* last line should be blank */
	gotoxy (x0, y1 - 1);

	for (i = x0; i < x1; i++)
		graphics_putchar (' ');

	graphics_setxy (x0, y1 - 1);

	no_scroll = 0;
}

void
graphics_putchar (int ch)
{
    if (ch == '\n') {
        if (fonty + 1 < y1)
            current_term->GOTOXY(fontx, fonty + 1);
        else
	{
	    graphics_cursor(0);
            graphics_scroll();
	    graphics_cursor(1);
	}
        //graphics_cursor(1);
        return;
    } else if (ch == '\r') {
        current_term->GOTOXY(x0, fonty);
        //graphics_cursor(1);
        return;
    } else if (ch & 0xff00)
    {
		if (fontx+1>=x1) graphics_putchar(' ');
    }
    //graphics_cursor(0);

    TEXT[fonty][fontx] = ch;
/*
    if (graphics_current_color == graphics_highlight_color)//if (graphics_current_color & 0xf0)
        text[fonty * 80 + fontx] |= 0x10000;//0x100;
*/
	int w_char=1;
	if (ch &= 0xff00)
    {
		TEXT[fonty][fontx +1] = 0;
//		w_char++;
	}
    graphics_cursor(0);

    if (fontx + w_char >= x1)
    {
        if (fonty + 1 < y1)
            graphics_setxy(x0, fonty + 1);
        else
	{
            graphics_setxy(x0, fonty);
            graphics_scroll();
	}
    } else {
		graphics_setxy(fontx + w_char, fonty);
    }

    graphics_cursor(1);
}

static int fontfile_func (char *arg, int flags)
{
	int i;
	/* get rid of TERM_NEED_INIT from the graphics terminal. */
	for (i = 0; term_table[i].name; i++) {
		if (strcmp (term_table[i].name, "graphics") == 0) {
			term_table[i].flags &= ~(1 << 16);
			break;
		}
	}
	//graphics_set_splash (splashimage);
	if ((*(DWORD *)VARIABLE_ADDR) != UNIFONT_LOAD_FLAG)
	{
		*term_backup_orig_addr = &term_table[i];
		graphics_cursor_bak = graphics_CURSOR;
		memmove(term_backup, &term_table[i], sizeof(struct term_entry));
	}
	term_table[i].PUTCHAR = (void *)BASE_FONT_ADDR + filemax + (int)&graphics_putchar - (int)&main;
	graphics_CURSOR = BASE_FONT_ADDR + filemax + (int)&graphics_cursor - (int)&main;
	if (graphics_inited) {
		term_table[i].SHUTDOWN ();
		if (! term_table[i].STARTUP())
		{
			graphics_CURSOR = graphics_cursor_bak;
			return !(errnum = ERR_EXEC_FORMAT);
		}
		cls();
	}
	(*(DWORD *)VARIABLE_ADDR) = UNIFONT_LOAD_FLAG;
	/* FIXME: should we be explicitly switching the terminal as a 
	 * side effect here? */
	builtin_cmd("terminal", "graphics", flags);

	return 1;
}

/***************************************************************
���ܣ� ��ʼ�����塣 ���������ļ����Ҷ�ȡ��Ϣͷ��
������ pFontFile--�ֿ��ļ���      
***************************************************************/
int InitFont()
{
	memset(_fl_header , 0, sizeof(FL_HEADER));
	memmove(_fl_header,(char *)BASE_FONT_ADDR,sizeof(FL_HEADER));
	//����ͷ
	if(_fl_header->YSize != 16 || _fl_header->magic[0] != 'U' || _fl_header->magic[1] != 'F' || _fl_header->magic[2] != 'L')
	{
		printf("Cann't support file format!\n");
		return 0;
	}

	_fl_header->pSection = (FL_SECTION_INF *)(BASE_FONT_ADDR + 0x10);

	return 1;	
//		return ReadFontHeader(_fl_header);	
}

/********************************************************************
���ܣ� ��ȡ��ǰ�ַ������ؿ��, �ҽ�������Ϣ����һ��ȫ�ֱ�����g_dwCharInfo��
        ����������Ϣ����ͬʱ�ܻ�ȡ��ǰ�ַ��ĵ�����Ϣ����ʼ��ַ��
������ wCode -- ���ֿ�Ϊunicode�����ʽʱ����wCode��unicode���봦��
                ����֮��MBCS)��
********************************************************************/
int ReadCharDistX(WORD wCode)
{
	if('U' == _fl_header->magic[0])     //unicode ����
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
//	*bytesPerLine= (WORD)((GET_FONT_WIDTH(g_dwCharInfo))+7)/PIXELS_PER_BYTE;	
	*bytesPerLine= (WORD)((GET_FONT_WIDTH(g_dwCharInfo))+7) >> 3;	

	if(g_dwCharInfo > 0)
	{
//		DWORD nDataLen = *bytesPerLine * _fl_header->YSize;			
		/*��Ϊֻ֧��16X16��ʽ����*/
		DWORD nDataLen = *bytesPerLine << 4;
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
	
	for(i = 0;i<_fl_header->nSection;i++)
	{
		if(wCode >= _fl_header->pSection[i].First && wCode <= _fl_header->pSection[i].Last)
			break;		
	}
	if(i >= _fl_header->nSection)	
		return 0;	

	offset = BASE_FONT_ADDR + _fl_header->pSection[i].OffAddr+ FONT_INDEX_TAB_SIZE*(wCode - _fl_header->pSection[i].First );	
	g_dwCharInfo = (*(DWORD *)offset);

	return GET_FONT_WIDTH(g_dwCharInfo);
}

static int unifont_unload(void)
{
	if ((*(DWORD *)VARIABLE_ADDR) != UNIFONT_LOAD_FLAG)
		return 0;
	memmove(*term_backup_orig_addr, term_backup, sizeof(struct term_entry));
	if (graphics_cursor_bak) graphics_CURSOR = graphics_cursor_bak;
	(*(DWORD *)VARIABLE_ADDR) = 0;
	ushFontReaded = 0;
	return printf("UNIFONT Unload success!\n");
}