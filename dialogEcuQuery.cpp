/*/
	dialogEcuQuery.cpp (2006.01.24)
/*/

#include "dialogEcuQuery.h"
#include "definitionError.h"
#include "handlerError.h"
#include "ecuExplorer.h"

extern void	nuke();
extern void	OutputString(CString sMessage,unsigned short usMessageType = 0);

BEGIN_MESSAGE_MAP(dialogEcuQuery, CDialog)
	ON_COMMAND(IDCLOSE,OnClose)
	ON_COMMAND(IDCAPTURE,OnCapture)
	ON_EN_KILLFOCUS(IDC_ECUQUERY_START,OnKillFocus)
	ON_EN_KILLFOCUS(IDC_ECUQUERY_STOP,OnKillFocus)
END_MESSAGE_MAP()

dialogEcuQuery::dialogEcuQuery() : CDialog(dialogEcuQuery::IDD)
{
	bQuery = FALSE;
	lpParent = NULL;
	lpFont = NULL;
	iAddress_start = 131072;
	iAddress_stop = 139264;
}

void dialogEcuQuery::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX,IDC_ECUQUERY_START,iAddress_start);
	DDX_Text(pDX,IDC_ECUQUERY_STOP,iAddress_stop);
}

BOOL dialogEcuQuery::OnInitDialog()
{
	lpFont = new CFont;
	LOGFONT lf;

	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 0xfffffff5;
	lf.lfWeight = FW_REGULAR;
	lf.lfOutPrecision = OUT_STROKE_PRECIS;
	lf.lfClipPrecision = CLIP_STROKE_PRECIS;
	lf.lfQuality = DRAFT_QUALITY;
	lf.lfPitchAndFamily = 0x22;
	lf.lfCharSet = 0;
	strcpy(lf.lfFaceName,"Lucida Console");

	lpFont->CreateFontIndirect(&lf);

	CEdit* lpEdit = NULL;
	if((lpEdit = (CEdit*)GetDlgItem(IDC_ECUQUERY_PROGRESS)) != NULL)
		lpEdit->SetFont(lpFont);

	CString sError;
	sError.Format("Start Address - 0x%03X",iAddress_start);
	SetDlgItemText(IDC_ECUQUERY_STATICSTART,sError);
	sError.Format("Stop Address - 0x%03X",iAddress_stop);
	SetDlgItemText(IDC_ECUQUERY_STATICSTOP,sError);

	return CDialog::OnInitDialog();
}

void dialogEcuQuery::OnKillFocus()
{
	CDialog::OnKillFocus(this);	

	UpdateData(TRUE);

	CString sError;
	sError.Format("Start Address - 0x%03X",iAddress_start);
	SetDlgItemText(IDC_ECUQUERY_STATICSTART,sError);
	sError.Format("Stop Address - 0x%03X",iAddress_stop);
	SetDlgItemText(IDC_ECUQUERY_STATICSTOP,sError);
}

void dialogEcuQuery::OnClose()
{
	s.close();
	bQuery = FALSE;
	Sleep(100);

	if(lpFont != NULL)
		delete lpFont;

	CDialog::OnOK();
}

void dialogEcuQuery::UpdateProgress(LPCTSTR cBuffer)
{
	CString sError;
	CEdit* lpEdit = NULL;

	CString sNew = cBuffer;

	GetDlgItemText(IDC_ECUQUERY_PROGRESS,sError);
	sNew.Replace("\n","\r\n");
	sError += sNew;
	SetDlgItemText(IDC_ECUQUERY_PROGRESS,sError);

	if((lpEdit = (CEdit*)GetDlgItem(IDC_ECUQUERY_PROGRESS)) != NULL)
		lpEdit->LineScroll(lpEdit->GetLineCount());
}

void dialogEcuQuery::OnCapture()
{
	CString sError;
	FILE* fpDump = NULL;
	int iCol = 1;
	long lResult = 0;
	int iLength = 0;

	if(iAddress_stop <= iAddress_start)
	{
		UpdateProgress("incorrect address values!\n");
		return;
	}

	sError.Format("\\\\.\\COM%i",((ecuExplorer*)lpParent)->lpProtocolSSM->iCommPort);
	s.portname = (LPCTSTR)sError;
	s.baudrate = 4800;

	SetDlgItemText(IDC_ECUQUERY_PROGRESS,"");
	if(!ready_port())
	{
		UpdateProgress("unable to connect to port.\n");
		return;
	}

	if(!((ecuExplorer*)lpParent)->BrowseForFile(&sError,OFN_OVERWRITEPROMPT))
	{
		UpdateProgress("!! PROCESS ABORTED !!\n");
		s.close();
		return;
	}

	dump_memory_to_file((LPCTSTR)sError,iAddress_start,iAddress_stop);
	s.close();
}

bool dialogEcuQuery::ready_port()
{
	if(!s.open())
	{
		s.set_status(SETDTR);
		s.flush();
		return false;
	}
	return true;
}

bool dialogEcuQuery::dump_memory_to_file(string dumpfile,int dumpstart, int dumpend)
{
	FILE *fpd;

	if (NULL == (fpd = fopen(dumpfile.c_str(),"wb")))
	{
		UpdateProgress("can't open dump file.\n");
		return false;
	}
	bool result = dump_memory_to_file(fpd,dumpstart,dumpend);
	fclose(fpd);
	return result;
}

bool dialogEcuQuery::dump_memory_to_file(FILE *fpd, int dumpstart, int dumpend)
{
	int len;
	unsigned char memblock[1024];
	int col = 0;
	CString sError;

	for (int mp = dumpstart; mp <= dumpend; mp += 16)
	{
		len = min(16,dumpend-mp+1);
		if(readaddresssingle_rsp(memblock,mp,&len))
		{
			dump_hex(&memblock[0],len,'<');
			fwrite(&memblock[5],1,len-6,fpd);
			fflush(fpd);

			col = 0;
			while(col <= 15 && 5+col < len-1)
			{
				if(col == 0)
				{
					sError.Format("%06X - %02X",mp,memblock[5+col]);
					UpdateProgress(sError);
				}
				else if(col == 15)
				{
					sError.Format(" %02X\n",memblock[5+col]);
					UpdateProgress(sError);
				}
				else
				{
					sError.Format(" %02X",memblock[5+col]);
					UpdateProgress(sError);
				}
				col++;
			}
		}
		else
		{
			UpdateProgress("read failure.\n");
			return false;
		}
	}
	return true;
}

bool dialogEcuQuery::readaddresssingle_rsp(unsigned char *data, int addr, int* len)
{
	if(!readaddresssingle_cmd_echo_check(data,addr,*len))
		return false;

	unsigned char rsp;
	if(!read_buffer_varlen(&rsp,data,len))
		return false;
	if(rsp != 0xE8)
		return false;

	return true;
}

bool dialogEcuQuery::readaddresssingle_cmd_echo_check(unsigned char *data,int start,int datalen)
{
	unsigned char buffer[1024];
	unsigned short pointer = 0;

	buffer[pointer++] = 0x80;
	buffer[pointer++] = 0x10;
	buffer[pointer] = 0xF0;
	pointer+=2;
	buffer[pointer++] = 0xA8;
	buffer[pointer++] = 0;

	for(int loop = 0;loop < datalen;loop++)
	{
		*((unsigned long*)&buffer[pointer]) = ntohl(start+loop)>>8;
		pointer+=3;
	}

	buffer[3] = (char)pointer-6+2;
	buffer[pointer++] = generate_checksum(&buffer[0],pointer);

	if(!write_buffer_echo_check(&buffer[0],buffer[4],pointer))
		return false;

	return true;
}

unsigned char dialogEcuQuery::generate_checksum(unsigned char* data,int datalen)
{
	unsigned short index;
	unsigned short chksum = 0;

	for(index = 0;index < datalen;index++)
		chksum += data[index];

	return (unsigned char)LOBYTE(chksum);
}

void dialogEcuQuery::dump_hex(unsigned char* data,unsigned short len,unsigned char header)
{
	int row = 0;
	unsigned int loop = 0;

	printf("%c %04i: ",header,loop);
	for(loop = 0;loop < (int)len;loop++)
	{
		if(row == 16)
		{
			printf("\n%c %04i: ",header,loop);
			row = 0;
		}

		row++;
		printf("%02X ",data[loop]);
	}

	printf("\n\n");

	return;
}

bool dialogEcuQuery::write_buffer_echo_check(unsigned char *data,unsigned char chk,int datalen)
{
	unsigned char rsp;
	int rsplen = datalen;

	dump_hex(data,(unsigned short)datalen,'>');
	bool result = s.write(data,datalen);
	if(result)
	{
		memset(data,0,datalen);
		read_buffer_varlen(&rsp,data,&rsplen);
		dump_hex(data,(unsigned short)rsplen,'<');
		if(rsp != chk || rsplen != datalen)
			result = false;
	}

	return result;
}

bool dialogEcuQuery::read_buffer_varlen(unsigned char* rsp,unsigned char *buffer,int *len)
{
	unsigned char chksum;

	if(!s.read(&buffer[0],1) || buffer[0] != 0x80)
		return false;
	if(!s.read(&buffer[1],1))
		return false;
	if(buffer[1] == 0xF0)
	{
		if(!s.read(&buffer[2],1) || buffer[2] != 0x10)
			return false;
	}
	else if(buffer[1] == 0x10)
	{
		if(!s.read(&buffer[2],1) || buffer[2] != 0xF0)
			return false;
	}
	else
		return false;
	if(!s.read(&buffer[3],1))
		return false;
	if(!s.read(&buffer[4],1))
		return false;

	int szpayload = buffer[3];
	*rsp = buffer[4];

	szpayload--;
	if(szpayload > *len)
		return false;
	*len = szpayload + 6;

	if(!s.read(&buffer[5],szpayload))
		return false;

	if(!s.read(&chksum,1))
		return false;

	if(chksum != (generate_checksum(&buffer[0],*len-1)))
		return false;
	
	return true;
}