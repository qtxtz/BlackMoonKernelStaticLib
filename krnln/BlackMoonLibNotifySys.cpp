#include "stdafx.h"

#pragma comment(lib,"gdi32.lib") 
#pragma comment(lib,"winspool.lib") 
#pragma comment(lib,"comdlg32.lib") 
#pragma comment(lib,"advapi32.lib")  
#pragma comment(lib,"shell32.lib")  
#pragma comment(lib,"ole32.lib")  
#pragma comment(lib,"oleaut32.lib")  
#pragma comment(lib,"uuid.lib")  
#pragma comment(lib,"odbc32.lib")  
#pragma comment(lib,"odbccp32.lib")  
char strBlackMoonFileName_Path[MAX_PATH];
char strBlackMoonFileName_Name[MAX_PATH];
EXTERN_C INT WINAPI BlackMoonFuncForeLibNotifySys(INT nMsg, DWORD dwParam1, DWORD dwParam2)
{

	switch (nMsg)
	{
	case  NRS_MALLOC:
		// ����ָ���ռ���ڴ棬�������׳��򽻻����ڴ涼����ʹ�ñ�֪ͨ���䡣
		//   dwParam1Ϊ�������ڴ��ֽ�����
		//   dwParam2��Ϊ0�����������ʧ�ܾ��Զ���������ʱ���˳�����
		// �粻Ϊ0�����������ʧ�ܾͷ���NULL��
		//   �����������ڴ���׵�ַ��
		return (INT)E_MAlloc(dwParam1);
	case  NRS_MFREE:
		// �ͷ��ѷ����ָ���ڴ档
		// dwParam1Ϊ���ͷ��ڴ���׵�ַ��
		E_MFree((void*)dwParam1);
		break;
	case  NRS_MREALLOC:
		// ���·����ڴ档
		//   dwParam1Ϊ�����·����ڴ�ߴ���׵�ַ��
		//   dwParam2Ϊ�����·�����ڴ��ֽ�����
		// ���������·����ڴ���׵�ַ��ʧ���Զ���������ʱ���˳�����
		return (INT)E_MRealloc((void*)dwParam1, dwParam2);

	case  NRS_FREE_ARY:
		// �ͷ�ָ���������ݡ�
		// dwParam1Ϊ�����ݵ�DATA_TYPE��ֻ��Ϊϵͳ�������͡�
		// dwParam2Ϊָ����������ݵ�ָ�롣
		switch (dwParam1)
		{
		case SDT_TEXT:
		case SDT_BIN:
			FreeAryElement((void*)dwParam2);
			break;
		case SDT_INT:
		case SDT_BYTE:
		case SDT_SHORT:
		case SDT_FLOAT:
		case SDT_INT64:
		case SDT_DOUBLE:
		case SDT_SUB_PTR:
		case SDT_BOOL:
		case SDT_DATE_TIME:
			E_MFree((void*)dwParam2);
			break;
		default:
			break;

		}
		break;

	case 	NRS_RUNTIME_ERR:
		// ֪ͨϵͳ�Ѿ���������ʱ����
		// dwParam1Ϊchar*ָ�룬˵�������ı���
	{
		char ErrorString[1024];
		wsprintfA(ErrorString, "BlackMoon RunTime Error:\r\n\r\n%s", dwParam1);
		MessageBoxA(0, ErrorString, "ERROR", MB_ICONERROR);
		E_End(0);
	}
	break;
	case 	NRS_EXIT_PROGRAM:
		// ֪ͨϵͳ�˳��û�����
		// dwParam1Ϊ�˳����룬�ô��뽫�����ص�����ϵͳ��
		E_End(dwParam1);
		break;
	case  NRS_GET_PRG_TYPE:
		// ���ص�ǰ�û���������ͣ�ΪPT_DEBUG_RUN_VER�����԰棩��PT_RELEASE_RUN_VER�������棩��
		return PT_RELEASE_RUN_VER;
	case 	NRS_DO_EVENTS:
		// ֪ͨϵͳ�������д������¼���
	{
		MSG Msg;
		while (PeekMessage(&Msg, NULL, NULL, NULL, PM_NOREMOVE))
		{
			if (Msg.message == WM_QUIT)
				break;
			GetMessage(&Msg, NULL, 0, 0);
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	break;
	case  NRS_GET_CMD_LINE_STR:
		// ȡ��ǰ�������ı�
		// �����������ı�ָ�룬�п���Ϊ�մ���
	{
		LPSTR p = GetCommandLineA();
		// �������ó�������
		char ch = ' ';
		if (*p++ == '\"')
			ch = '\"';
		while (*p++ != ch);
		if (ch != ' ' && *p == ' ')  p++;    // ������һ���ո�
		return (INT)p;
	}
	case  NRS_GET_EXE_PATH_STR:
		// ȡ��ǰִ���ļ�����Ŀ¼����
		// ���ص�ǰִ���ļ�����Ŀ¼�ı�ָ�롣
		if (::GetModuleFileNameA(NULL, strBlackMoonFileName_Path, MAX_PATH))
		{
			char* pFind = strrchr(strBlackMoonFileName_Path, '\\');
			//	pFind++;
			*pFind = 0;
			return (INT)&strBlackMoonFileName_Path;

		}
		break;

	case  NRS_GET_EXE_NAME:
		// ȡ��ǰִ���ļ�����
		// ���ص�ǰִ���ļ������ı�ָ�롣
		if (::GetModuleFileNameA(NULL, strBlackMoonFileName_Path, MAX_PATH))
		{
			char* pFind = strrchr(strBlackMoonFileName_Path, '\\');
			//pFind++;
			*pFind = 0;
			pFind++;
			return (INT)pFind;
		}
		break;
	case  NRS_CONVERT_NUM_TO_INT:
	{
		// ת��������ֵ��ʽ��������
		// dwParam1Ϊ PMDATA_INF ָ�룬�� m_dtDataType ����Ϊ��ֵ�͡�
		// ����ת���������ֵ��
		PMDATA_INF pArgInf = (PMDATA_INF)dwParam1;
		INT nNewVal = pArgInf->m_int;
		switch (pArgInf->m_dtDataType)
		{
		case SDT_INT:
		case SDT_BYTE:
		case SDT_SHORT:
			break;
		case SDT_FLOAT:
			nNewVal = (INT)pArgInf->m_float;
			break;
		case SDT_INT64:
			nNewVal = (INT)pArgInf->m_int64;
			break;
		case SDT_DOUBLE:
			nNewVal = (INT)pArgInf->m_double;
			break;
		default:
			break;

		}
		return nNewVal;
	}
	break;
	case  NAS_GET_PATH:
		/* ���ص�ǰ���������л�����ĳһ��Ŀ¼���ļ�����Ŀ¼���ԡ�\��������
		dwParam1: ָ������Ҫ��Ŀ¼������Ϊ����ֵ��
		A�����������л����¾���Ч��Ŀ¼:
		1: ���������л���ϵͳ������Ŀ¼��
		B��������������Ч��Ŀ¼(��������������Ч):
		1001: ϵͳ���̺�֧�ֿ���������Ŀ¼��
		1002: ϵͳ��������Ŀ¼
		1003: ϵͳ������Ϣ����Ŀ¼
		1004: �������еǼǵ�ϵͳ����ģ���Ŀ¼
		1005: ֧�ֿ����ڵ�Ŀ¼
		1006: ��װ��������Ŀ¼
		C�����л�������Ч��Ŀ¼(�����л�������Ч):
		2001: �û�EXE�ļ�����Ŀ¼��
		2002: �û�EXE�ļ�����
		dwParam2: ���ջ�������ַ���ߴ����ΪMAX_PATH��
		*/
		if (::GetModuleFileNameA(NULL, strBlackMoonFileName_Path, MAX_PATH))
		{
			char* pFind = strrchr(strBlackMoonFileName_Path, '\\');
			pFind++;
			*pFind = 0;
			pFind++;
			strcpy(strBlackMoonFileName_Name, pFind);
		}
		else {
			strBlackMoonFileName_Path[0] = 0;
			strBlackMoonFileName_Name[0] = 0;
		}

		switch (dwParam1)
		{
		case 1:
			strcpy((char*)dwParam2, strBlackMoonFileName_Path);
			return (INT)strBlackMoonFileName_Path;
			break;
		case 2001:
			strcpy((char*)dwParam2, strBlackMoonFileName_Path);
			return (INT)strBlackMoonFileName_Path;
			break;
		case 2002:
			strcpy((char*)dwParam2, strBlackMoonFileName_Name);
			return (INT)strBlackMoonFileName_Name;
			break;
		default:
			return NULL;
		}
	case  NAS_GET_LANG_ID:
		// ���ص�ǰϵͳ�����л�����֧�ֵ�����ID������IDֵ���lang.h
		return 1;
	case  NAS_GET_VER:
		// ���ص�ǰϵͳ�����л����İ汾�ţ�LOWORDΪ���汾�ţ�HIWORDΪ�ΰ汾�š�
		return 0x00000004;
	case  NRS_GET_WINFORM_COUNT:
		// ���ص�ǰ����Ĵ�����Ŀ��
		return 0;
	case  NRS_GET_WINFORM_HWND:
		// ����ָ������Ĵ��ھ��������ô�����δ�����룬����NULL��
		// dwParam1Ϊ����������
		return NULL;
	case NAS_GET_APP_ICON:
		// ֪ͨϵͳ���������س����ͼ�ꡣ
		// dwParam1ΪPAPP_ICONָ�롣
	case  NAS_GET_LIB_DATA_TYPE_INFO:
		// ����ָ���ⶨ���������͵�PLIB_DATA_TYPE_INFO������Ϣָ�롣
		// dwParam1Ϊ�������������͡�
		// ���������������Ч���߲�Ϊ�ⶨ���������ͣ��򷵻�NULL�����򷵻�PLIB_DATA_TYPE_INFOָ�롣
	case  NAS_GET_HBITMAP:
		// dwParam1ΪͼƬ����ָ�룬dwParam2ΪͼƬ���ݳߴ硣
		// ����ɹ����ط�NULL��HBITMAP�����ע��ʹ����Ϻ��ͷţ������򷵻�NULL��

	case  NAS_CREATE_CWND_OBJECT_FROM_HWND:
		// ͨ��ָ��HWND�������һ��CWND���󣬷�����ָ�룬��ס��ָ�����ͨ������NRS_DELETE_CWND_OBJECT���ͷ�
		// dwParam1ΪHWND���
		// �ɹ�����CWnd*ָ�룬ʧ�ܷ���NULL
	case  NAS_DELETE_CWND_OBJECT:
		// ɾ��ͨ��NRS_CREATE_CWND_OBJECT_FROM_HWND������CWND����
		// dwParam1Ϊ��ɾ����CWnd����ָ��
	case  NAS_DETACH_CWND_OBJECT:
		// ȡ��ͨ��NRS_CREATE_CWND_OBJECT_FROM_HWND������CWND����������HWND�İ�
		// dwParam1ΪCWnd����ָ��
		// �ɹ�����HWND,ʧ�ܷ���0
	case  NAS_GET_HWND_OF_CWND_OBJECT:
		// ��ȡͨ��NRS_CREATE_CWND_OBJECT_FROM_HWND������CWND�����е�HWND
		// dwParam1ΪCWnd����ָ��
		// �ɹ�����HWND,ʧ�ܷ���0
	case  NAS_ATTACH_CWND_OBJECT:
		// ��ָ��HWND��ͨ��NRS_CREATE_CWND_OBJECT_FROM_HWND������CWND���������
		// dwParam1ΪHWND
		// dwParam2ΪCWnd����ָ��
		// �ɹ�����1,ʧ�ܷ���0
	case 	NAS_IS_EWIN:
		// ���ָ������Ϊ�����Դ��ڻ�����������������棬���򷵻ؼ١�
		// dwParam1Ϊ�����Ե�HWND.
// NRS_ ��Ϊ���ܱ������л��������֪ͨ��
	case  NRS_UNIT_DESTROIED:
		// ֪ͨϵͳָ��������Ѿ������١�
		// dwParam1ΪdwFormID
		// dwParam2ΪdwUnitID
	case  NRS_GET_UNIT_PTR:
		// ȡ�������ָ��
		// dwParam1ΪWinForm��ID
		// dwParam2ΪWinUnit��ID
		// �ɹ�������Ч���������CWnd*ָ�룬ʧ�ܷ���0��
	case  NRS_GET_AND_CHECK_UNIT_PTR:
		// ȡ�������ָ��
		// dwParam1ΪWinForm��ID
		// dwParam2ΪWinUnit��ID
		// �ɹ�������Ч���������CWnd*ָ�룬ʧ�ܱ�������ʱ�����˳�����
	case  NRS_EVENT_NOTIFY:
		// �Ե�һ�෽ʽ֪ͨϵͳ�������¼���
		// dwParam1ΪPEVENT_NOTIFYָ�롣
		//   ������� 0 ����ʾ���¼��ѱ�ϵͳ�����������ʾϵͳ�Ѿ��ɹ����ݴ��¼����û�
		// �¼������ӳ���
	case  NRS_GET_UNIT_DATA_TYPE:
		// dwParam1ΪWinForm��ID
		// dwParam2ΪWinUnit��ID
		// �ɹ�������Ч�� DATA_TYPE ��ʧ�ܷ��� 0 ��

	case  NRS_EVENT_NOTIFY2:
		// �Եڶ��෽ʽ֪ͨϵͳ�������¼���
		// dwParam1ΪPEVENT_NOTIFY2ָ�롣
		//   ������� 0 ����ʾ���¼��ѱ�ϵͳ�����������ʾϵͳ�Ѿ��ɹ����ݴ��¼����û�
		// �¼������ӳ���

	case  NRS_GET_BITMAP_DATA:
		// ����ָ��HBITMAP��ͼƬ���ݣ��ɹ����ذ���BMPͼƬ���ݵ�HGLOBAL�����ʧ�ܷ���NULL��
		// dwParam1Ϊ����ȡ��ͼƬ���ݵ�HBITMAP��
	case  NRS_FREE_COMOBJECT:
		// ֪ͨϵͳ�ͷ�ָ����DTP_COM_OBJECT����COM����
		// dwParam1Ϊ��COM����ĵ�ַָ�롣
	case  NRS_CHK_TAB_VISIBLE:
	default:
	{
		char ErrorString[255];
		wsprintfA(ErrorString, "��֧��ϵͳ���ܺ���%d. �뽫����Ϣ����������", nMsg);
		MessageBoxA(0, ErrorString, "blackmoon", MB_ICONERROR);
		break;
	}
	}
	return NULL;

}

EXTERN_C INT WINAPI BlackMoonReplac_NotifySys(INT nMsg, DWORD dwParam1, DWORD dwParam2)
{
	return NR_OK;
}

INT WINAPI NotifySys(INT nMsg, DWORD dwParam1, DWORD dwParam2)
{

	return BlackMoonFuncForeLibNotifySys(nMsg, dwParam1, dwParam2);
}

// ʹ��ָ���ı����ݽ����׳�����ʹ�õ��ı����ݡ�
/*
char* CloneTextData (char* ps)
{
	if (ps == NULL || *ps == '\0')
		return NULL;

	INT nTextLen = strlen (ps);
	char* pd = (char*)NotifySys (NRS_MALLOC, (DWORD)(nTextLen + 1), 0);
	memcpy (pd, ps, nTextLen);
	pd [nTextLen] = '\0';
	return pd;
}*/
/*
// ʹ��ָ���ı����ݽ����׳�����ʹ�õ��ı����ݡ�
//   nTextLen����ָ���ı����ֵĳ��ȣ������������㣩��
// ���Ϊ-1����ȡps��ȫ�����ȡ�
char* CloneTextData (char* ps, INT nTextLen)
{
	if (nTextLen <= 0)
		return NULL;

	char* pd = (char*)NotifySys (NRS_MALLOC, (DWORD)(nTextLen + 1), 0);
	memcpy (pd, ps, nTextLen);
	pd [nTextLen] = '\0';
	return pd;
}

// ʹ��ָ�����ݽ����׳�����ʹ�õ��ֽڼ����ݡ�
LPBYTE CloneBinData (LPBYTE pData, INT nDataSize)
{
	if (nDataSize == 0)
		return NULL;

	LPBYTE pd = (LPBYTE)NotifySys (NRS_MALLOC, (DWORD)(sizeof (INT) * 2 + nDataSize), 0);
	*(LPINT)pd = 1;
	*(LPINT)(pd + sizeof (INT)) = nDataSize;
	memcpy (pd + sizeof (INT) * 2, pData, nDataSize);
	return pd;
}

// ��������ʱ����
void GReportError (char* szErrText)
{
	NotifySys (NRS_RUNTIME_ERR, (DWORD)szErrText, 0);
}

void* MMalloc (INT nSize)
{
	return (void*)NotifySys (NRS_MALLOC, (DWORD)nSize, 0);
}

void MFree (void* p)
{
	NotifySys (NRS_MFREE, (DWORD)p, 0);
}
*/
// ������������ݲ����׵�ַ����Ա��Ŀ��
LPBYTE GetAryElementInf(void* pAryData, LPINT pnElementCount)
{
	LPINT pnData = (LPINT)pAryData;
	INT nArys = *pnData++;  // ȡ��ά����
	// �����Ա��Ŀ��
	INT nElementCount = 1;
	while (nArys > 0)
	{
		nElementCount *= *pnData++;
		nArys--;
	}

	if (pnElementCount != NULL)
		*pnElementCount = nElementCount;
	return (LPBYTE)pnData;
}
/*
#define DTT_IS_NULL_DATA_TYPE   0
#define DTT_IS_SYS_DATA_TYPE    1
#define DTT_IS_USER_DATA_TYPE   2
#define DTT_IS_LIB_DATA_TYPE    3
// ȡ���������͵����
INT GetDataTypeType (DATA_TYPE dtDataType)
{
	if (dtDataType == _SDT_NULL)
		return DTT_IS_NULL_DATA_TYPE;

	DWORD dw = dtDataType & 0xC0000000;
	return dw == DTM_SYS_DATA_TYPE_MASK ? DTT_IS_SYS_DATA_TYPE :
			dw == DTM_USER_DATA_TYPE_MASK ? DTT_IS_USER_DATA_TYPE :
			DTT_IS_LIB_DATA_TYPE;
}
*/

//������liigo���


/*
//!!! ���º�����������������֮��Աȫ��ΪINT����(��ͬ��ռ��4���ֽ�)
//index: 0..*/


void* _GetPointerByIndex(void* pBase, int index)
{
	return (void*)((INT)pBase + index * sizeof(INT));
}

INT _GetIntByIndex(void* pBase, INT index)
{
	return *((INT*)_GetPointerByIndex(pBase, index));
}

void _SetIntByIndex(void* pBase, INT index, INT value)
{
	*((INT*)_GetPointerByIndex(pBase, index)) = value;
}


INT GetIntByIndex(PMDATA_INF pArgInf, INT index)
{
	return _GetIntByIndex(pArgInf->m_pCompoundData, index);
}

void SetIntByIndex(PMDATA_INF pArgInf, INT index, INT value)
{
	_SetIntByIndex(pArgInf->m_pCompoundData, index, value);
}

void* GetPointerByIndex(PMDATA_INF pArgInf, INT index)
{
	return _GetPointerByIndex(pArgInf->m_pCompoundData, index);
}

void SetLogfont_CompileLanVer(LPLOGFONT pLogfont)
{
	::GetObject(::GetStockObject(DEFAULT_GUI_FONT), sizeof(LOGFONT), pLogfont);

}
