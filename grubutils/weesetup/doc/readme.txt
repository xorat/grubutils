wee ��һ��΢�͵�grub4dos���ڰ�װ��Ӳ��mbr�ϣ�������������֮ǰ��grldr.mbr������

֧�ֵ��ļ�ϵͳ�� FAT12/16/32/NTFS EXT2/3/4

weesetup��Ϊ�˷��㰲װwee63.mbr��д�ĳ���֧���Զ���ű���

ע�⣺ ������biod��֧��EBIOS�벻Ҫ��װ��һ������½��µĵ��Զ���֧�֣�������ԭ���뿴�����WEE˵����

   WEE access disk sectors only using EBIOS(int13/AH=42h), and never using
   CHS mode BIOS call(int13/AH=02h). So, if the BIOS does not support EBIOS
   on a drive, then WEE will not be able to access that drive.

   WEE supports FAT12/16/32, NTFS and ext2/3/4, and no other file systems are
   supported.

   WEE can boot up IO.SYS(Win9x), KERNEL.SYS(FreeDOS), VMLINUZ(Linux), NTLDR/
   BOOTMGR(Windows), GRLDR(grub4dos). And GRUB.EXE(grub4dos) is also bootable
   because it is of a valid Linux kernel format.

   Any single sector boot record file(with 55 AA at offset 0x1FE) can boot
   as well.

   Besides, WEE can run 32-bit programs written for it.

2011-02-17
	1.��������wee63.mbr�����°�
	2.���-l��������ʾ�����б�
	3.��ֹ��ʹ����fbinst�Ĵ�����ʹ�øó���
	

weesetup v1.2.
Usage:
        weesetup [OPTIONS] DEVICE

OPTIONS:
        -i wee63.mbr            Use a custom wee63.mbr file.

        -o outfile              Export new wee63.mbr to outfile.

        -s scriptfile           Import script from scriptfile.

        -m mbrfile              Read mbr from mbrfile(must use with option -o).

        -f                      Force install.
        -u                      Update.
        -b                      Backup mbr to second sector(default is nt6mbr).
	-l			List all disks in system and exit.

Report bugs to website:
        http://code.google.com/p/grubutils/issues

Thanks:
        wee63.mbr (minigrub for mbr by tinybit)
                http://bbs.znpc.net/viewthread.php?tid=5838
        wee63setup.c by SvOlli,xdio.c by bean


wee ��װ����v1.2
�÷�:
        weesetup [����] ����
����:
        -i wee63.mbr            ʹ�����õ�wee63.mbr��

        -o outfile              �����µ�WEE63.MBR�ļ���outfile.

        -s scriptfile           ����wee�ű��ļ�.

        -m mbrfile              ��mbrfile��ȡmbr��Ϣ��������ϲ���-oʹ�ã�.

        -f                      ǿ�ư�װ.
	-u			����wee.
        -b                      ���ݾɵ�mbr���ڶ�������Ĭ�ϲ����ݣ�����ֱ��ʹ�����õ�nt6mbr��.
	-l			��ʾ����Ӳ���б�

�뵽������ַ����BUG:
        http://code.google.com/p/grubutils/issues

��л:
        wee63.mbr (minigrub for mbr by tinybit)
                http://bbs.znpc.net/viewthread.php?tid=5838
        wee63setup.c by SvOlli,xdio.c by bean

��������������ҵĲ���:
	http://chenall.net/post/weesetup/

���뷽����

1.���ȴ�SVN����GRUBUTILSԴ��
svn co svn://svn.gna.org/svn/grubutil grubutil

2.��grubutilĿ¼���½�һ����Ŀ¼weesetup��Ȼ���weesetup��Դ�븴�ƹ�ȥ.

��weesetupĿ¼��ִ��make���Ϳ�����binĿ¼������һ��weesetup.exe


��:���õ�wee63.mbrĬ�Ͻű�����
find --set-root /boot/grub/grldr
/boot/grub/grldr
find --set-root /boot/grldr
/boot/grldr
timeout 1
default 0
title 1.GRUB4DOS
find --set-root /BOOT/GRUB/GRLDR
/BOOT/GRUB/GRLDR
find --set-root /BOOT/GRLDR
/BOOT/GRLDR
find --set-root /BOOT/GRUB.EXE
/BOOT/GRUB.EXE
find --set-root /grldr
/grldr

title 2.Windows
chainloader (hd0)1+1
find --set-root /bootmgr
/bootmgr
find --set-root /ntldr
/ntldr

title 3.Plop Boot Manager 
find --set-root /BOOT/GRUB/PLPBT.BIN
/BOOT/GRUB/PLPBT.BIN
