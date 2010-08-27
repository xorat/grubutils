/***********************************************************
������	��c����д��һ����δӵ����ֿ��ж�ȡ�ַ���Ϣ�����ؿ� +������Ϣ��
        �����ݴ��Ժ�Ч�ʷ��滹��ʹ�������и���,лл���Ĳ��ģ�

�ļ���ʽ�� 
		Unicode -- �ļ�ͷ+���ݶ�(section)+������+������Ϣ
		MBCS_Ladin-�ļ�ͷ+������+������Ϣ
		MBCS_CJK-- �ļ�ͷ+������Ϣ
		
���ߣ�  wujianguo
���ڣ� 	20090516
MSN:    wujianguo19@hotmail.com
qq��    9599598
*************************************************************/

#include "font.h"
//#include "unicode\unicode.h"
//#include "mbcs\mbcs.h"

char *g_prf = 0x400000;
FL_HEADER _fl_header;
DWORD g_dwCharInfo = 0;    // �洢��ǰ�ַ��ļ�����Ϣ��  bit0��bit25����ŵ�����Ϣ����ʼ��ַ�� bit26��bit31��������ؿ�ȡ�

static int ReadFontHeader(PFL_HEADER pfl_header)
{	
//	fread(pfl_header, sizeof(FL_HEADER) -4, 1, g_prf);
	memmove(pfl_header,g_prf,sizeof(PFL_HEADER));
	//����ʾͷ
	if((_fl_header.magic[0] != 'U'/* && _fl_header.magic[0] != 'M'*/)
		|| _fl_header.magic[1] != 'F' || _fl_header.magic[2] != 'L')
	{
		printf("Cann't support file format!\n");
		return 0;
	}

	if('U' == pfl_header->magic[0])     //unicode ����
	{
		 _fl_header.pSection = 0x400010;
//		return ReadFontSection();
	}

	return 1;	
}

//static int OpenFontFile(char *pFontFile)
//{
//	if((g_prf = fopen(pFontFile, "rb")) == NULL)
//	{
//		printf("Cann't open : %s\n", pFontFile);
//		return 0;
//	}
	
//	return 1;
//}

/***************************************************************
���ܣ� ��ʼ�����塣 ���������ļ����Ҷ�ȡ��Ϣͷ��
������ pFontFile--�ֿ��ļ���      
***************************************************************/
int InitFont()
{
	memset(&_fl_header, 0, sizeof(FL_HEADER));

//	if(OpenFontFile(pFontFile))
		return ReadFontHeader(&_fl_header);	
//	else
//		return 0;
}

void ExitFont()
{

//	if('U' == _fl_header.magic[0])     //unicode ����
//		ReleaseSection();
}

BYTE GetFontYSize()
{
	return _fl_header.YSize;
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
int ReadCharDotArray(WORD wCode, BYTE *fontArray, WORD *bytesPerLine)
{	
	*bytesPerLine= (WORD)((GET_FONT_WIDTH(g_dwCharInfo))+7)/PIXELS_PER_BYTE;	

	if(g_dwCharInfo > 0)
	{		
		DWORD nDataLen = *bytesPerLine * _fl_header.YSize;			
		DWORD  dwOffset = GET_FONT_OFFADDR(g_dwCharInfo);    //��ȡ�ַ�����ĵ�ַ��Ϣ(��26λ)
		
		fontArray = (BYTE *)dwOffset;
		//fseek(g_prf, dwOffset, SEEK_SET);
		//fread(fontArray, nDataLen, 1, g_prf);	
		
		return 1;
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
	
	offset = _fl_header.pSection[i].OffAddr+ FONT_INDEX_TAB_SIZE*(wCode - _fl_header.pSection[i].First );	
//	fseek(g_prf, offset, SEEK_SET);
//	fread(&g_dwCharInfo, sizeof(DWORD), 1, g_prf);	
	g_dwCharInfo = (*(DWORD *)offset);

	return GET_FONT_WIDTH(g_dwCharInfo);
}
