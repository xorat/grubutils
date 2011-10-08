#include "include/grub4dos.h"
#include "include/pc_slice.h"
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

gcc -nostdlib -fno-zero-initialized-in-bss -fno-function-cse -fno-jump-tables -Wl,-N -fPIE diskid.c

 * disassemble:			objdump -d a.out 
 * confirm no relocation:	readelf -r a.out
 * generate executable:		objcopy -O binary a.out diskid
 *
 */
/*
 * to get this source code & binary: http://grub4dos-chenall.google.com
 * For more information.Please visit web-site at http://chenall.net
 * 2010-02-25
	2010-08-07 �������ܵ��¼����޷�Ӧ������bug.
	2010-08-08 ��� gid=����,�������õ�ǰroot��ָ����id.
	diskid gid=1:1 �л�����һ�����̵�һ������
 */
#define RET_VAR 0x4FF00
#define MAX_PART 26
int GRUB = 0x42555247;/* this is needed, see the following comment. */
/* gcc treat the following as data only if a global initialization like the
 * above line occurs.
 */
static int diskid_func (char *arg,int flags);
typedef struct part_info {unsigned long id, part, start;} partinfo;
asm(".long 0x534F4434");
asm(ASM_BUILD_DATE);
/* a valid executable file for grub4dos must end with these 8 bytes */
asm(".long 0x03051805");
asm(".long 0xBCBAA7BA");

/* thank goodness gcc will place the above 8 bytes at the end of the b.out
 * file. Do not insert any other asm lines here.
 */

static int
main ()
{
	void *p = &main;
	char *arg = p - (*(int *)(p - 8));
	int flags = (*(int *)(p - 12));
	return diskid_func (arg , flags);
}

static int get_partinfo(partinfo *PART_INFO)
{
	char mbr[512];
	unsigned long part = 0xFFFFFF;
	unsigned long start, len, offset, ext_offset1;
	unsigned long type, entry1;
	partinfo *PI;
	unsigned long id;
	/* Look for the partition.  */
  PI = PART_INFO;
  id = 0UL;
  while ((	next_partition_drive		= current_drive,
		next_partition_dest		= 0xFFFFFF,
		next_partition_partition	= &part,
		next_partition_type		= &type,
		next_partition_start		= &start,
		next_partition_len		= &len,
		next_partition_offset		= &offset,
		next_partition_entry		= &entry1,
		next_partition_ext_offset	= &ext_offset1,
		next_partition_buf		= mbr,
		next_partition ()))
	{
	  if (type != PC_SLICE_TYPE_NONE
		&& ! IS_PC_SLICE_TYPE_BSD (type)
		&& ! IS_PC_SLICE_TYPE_EXTENDED (type))
	  {
		PI->id = ++id;
		PI->part = part;
		PI->start = start;
		++PI;
		if (id >= MAX_PART) break;
	  }
	  errnum = ERR_NONE;
	}
	PI->id = 0;
	errnum = ERR_NONE;

	long i,j;
	partinfo t_pi;
/*�ӷ���λ������(Ghost Style)*/
	for (i = 0; i < id; i++)
	  for (j = i+1; j < id; j++)
		if (PART_INFO[i].start > PART_INFO[j].start)
		{
			t_pi = PART_INFO[j];
			PART_INFO[j] = PART_INFO[i];
			PART_INFO[i] = t_pi;
		}

	return id;
}

static int
diskid_func (char *arg,int flags)
{
  unsigned long long ret = 0;
  partinfo PART_INFO[MAX_PART];
  unsigned long id;

  errnum = ERR_BAD_ARGUMENT;
  if (*arg && (memcmp(arg,"ret=",4) == 0))
  {
	arg =skip_to(1,arg);
	safe_parse_maxint(&arg,&ret);
	arg = skip_to (0,arg);
  }
  if (! *arg || *arg == ' ' || *arg == '\t')
  {
	current_drive = saved_drive;
	current_partition = saved_partition;
  }
	else if (memcmp(arg,"info",4) == 0)
	{
		unsigned long tmp_drive = saved_drive;
		unsigned long tmp_partition = saved_partition;
		char tmp[20];
		for (id=0;id<(*((char *)0x475));)
		{
			saved_drive = current_drive = 0x80 + id;
			current_partition = 0xffffff;
			open_device();
			ret = sprintf(tmp,"%ld",part_length<<9);
			printf(" Disk: %d\t",++id);
			if (ret > 9)
				printf("%.*s GB\n",(int)(ret-9),tmp);
			else
				printf("%.*s MB\n",(int)(ret-6),tmp);
			partinfo *pi = PART_INFO;
			get_partinfo(PART_INFO);
			while (pi->id)
			{
				current_partition = saved_partition = pi->part;
				open_device();
				if (current_drive == tmp_drive && current_partition == tmp_partition)
				{
					if (current_term->SETCOLORSTATE)
						current_term->SETCOLORSTATE (COLOR_STATE_HIGHLIGHT);
					printf("   * %d:%d\t",id,pi->id);
				}
				else
					printf("     %d:%d\t",id,pi->id);
				if (part_length>>22)
					printf("%d GB\t",part_length>>21);
				else
					printf("%d MB\t",part_length>>11);
				builtin_cmd("root","", 1);
				if (current_term->SETCOLORSTATE)
					current_term->SETCOLORSTATE (COLOR_STATE_STANDARD);
				++pi;
			}
		}
		saved_drive = tmp_drive;
		saved_partition = tmp_partition;
		return 1;
	}
	else if (memcmp(arg, "gid=", 4) == 0) /*gid x:y */
	{
		int dd,dp;
		arg =skip_to(1,arg);
		if (*arg=='*')/*�������*��ͷ��,���ȡ��Ӧ�ڴ��ַ��ֵ*/
		{
			arg++;
			if (! safe_parse_maxint(&arg,&ret))
				return 0;
			arg =(char *)(unsigned int)ret;
		}

		if (! safe_parse_maxint(&arg,&ret))
			return 0;
		dd = (int)ret;
		arg++;
		if (! safe_parse_maxint(&arg,&ret))
			return 0;
		++arg;
		dp = (int)ret;
		if (dd == 0)
			dd = saved_drive - 0x7f;
		else if (dd < 0)
			dd = (*((char *)0x475)) + dd + 1;

		if (dd < 1)
			return 0;
		for(current_drive = 0x7f + dd;dd && dd<=(*((char *)0x475));++arg)
		{
			id = get_partinfo(PART_INFO);

			if (dp < 0)
				dp += id + 1;
			if (dp > id || dp <= 0)
				break;
			if (*arg == '+')
			{
				if (dp >= id)
				{
					dp = 1;
					if (dd < *((char *)0x475))
						++dd;
					else
						dd = 1;
					continue;
				}
				++dp;
			}
			else if (*arg == '-')
			{
				if (dp <= 1)
				{
					dp = -1;
					if (dd > 1)
						--dd;
					else
						dd = *((char *)0x475);
					continue;
				}
				--dp;
			}
			saved_drive = current_drive;
			saved_partition = PART_INFO[dp-1].part;
			sprintf((char *)RET_VAR,"%d:%d\r\r",dd,dp);
			sprintf(((char*)0x4CA00),"%d.%d",dd,dp);
			if (debug > 0)
				printf(" %d:%d",dd,dp);
			return builtin_cmd("root","()", 1);
		}

		return !(errnum = ERR_NO_PART);
	}
	else if (memcmp(arg, "uid=", 4) == 0)
  {
	ret = 0x20000;
  }
  else if (! set_device (arg))
    return 0;

  /* The drive must be a hard disk.  */
  if (! (current_drive & 0x80))
    {
       errnum = ERR_BAD_ARGUMENT;
      return 0;
    }
  
  /* The partition must be a PC slice.  */
  if ((current_partition >> 16) == 0xFF
      || (current_partition & 0xFFFF) != 0xFFFF)
    {
      errnum = ERR_BAD_ARGUMENT;
      return 0;
    }
	/* Look for the partition.  */
	id = get_partinfo(PART_INFO);
/*��ȡָ��������Ӧ��ID*/	
	long i;
	for (i=0UL;i<id;++i)
		if (PART_INFO[i].part == current_partition)
		{
			if (ret) /*ָ����ret����,��ID��Ϣд�뵽ָ���ڴ��и�ʽ:0xXXYYAABB */
			{
				int *P = (int *)(int)ret;
				*P = (current_drive - 0x80) & 0xff; /*XX ��0��ʼ�Ĵ��̱��*/
				*P <<= 8;
				*P |= PART_INFO[i].id;				/*YY ��1��ʼ�ķ������,�Ը÷��ڷ������е�λ������*/
				*P <<= 8;
				*P |= (current_drive - 0x7F);		/*AA ��1��ʼ�Ĵ��̱��*/
				*P <<= 8;
				*P |= i + 1;						/*BB ��1��ʼ�ķ������,�Ը÷����ڴ��̵�λ������*/
			}
			sprintf((char *)RET_VAR,"%d:%d\r\r",(unsigned char)(current_drive - 0x7F),i+1);
			sprintf(((char*)0x4CA00),"%d.%d",(unsigned char)(current_drive - 0x7F),i+1);
			if (debug > 0)
				printf(" (hd%d,%d) in Ghost Style is: %d:%d\n",(unsigned int)(current_drive-0x80),(unsigned int)(current_partition >> 16),(unsigned int)(current_drive - 0x7F),i+1);
			return 1;
		}

	return 0;
}



