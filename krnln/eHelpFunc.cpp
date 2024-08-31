#include "stdafx.h"

#pragma comment(lib,"kernel32.lib") 
#pragma comment(lib,"user32.lib") 

HANDLE hBlackMoonInstanceHandle = NULL;
DWORD dwBlackMoonPESizeOfImage = 0;
DWORD dwBlackMoonPEAddrrStart;
DWORD dwBlackMoonPEAddrrEnd;

EError_CALLBACK fnEError_callback = NULL;
INT isErrorCallBack = 0;

void GetBlackMoonPESizeOfImage()
{
	PBYTE module;
	if(hBlackMoonInstanceHandle)
		module = (PBYTE)hBlackMoonInstanceHandle;
	else
		module = (PBYTE)::GetModuleHandle(NULL);
	dwBlackMoonPESizeOfImage = ((PIMAGE_NT_HEADERS)(module+((PIMAGE_DOS_HEADER)module)->e_lfanew))->OptionalHeader.SizeOfImage;
	dwBlackMoonPEAddrrStart = (DWORD)module;
	dwBlackMoonPEAddrrEnd = (DWORD)module+dwBlackMoonPESizeOfImage;

}
LPSTR sErrorListForE []=
{
(LPSTR)"�����Ա�����±곬�����巶Χ",
(LPSTR)"δ�õ�����Ҫ����ֵ������",
(LPSTR)"���������Աʱά����Ϊ1�Ҳ����ڸ�����Ŀǰ�����е�ά��",
(LPSTR)"�����Ա�����±������ڵ���1",
(LPSTR)"���ݻ��������Ͳ�ƥ��",
(LPSTR)"����DLL������ֶ�ջ����ͨ����DLL������Ŀ����ȷ",
(LPSTR)"��������δ�������ݻ��߷����˷�ϵͳ�������ͻ�����������",
(LPSTR)"���Ƚ�����ֻ��ʹ�õ��ڻ򲻵�������Ƚ�",
(LPSTR)"������ѡ�����������ֵ����С����򳬳������ṩ������Χ",
(LPSTR)"���ض������顱���������ά�������ֵ����������άʱ���ڵ�����",
(LPSTR)"���ṩ�������������Ͳ�����Ҫ��"
};
//extern BOOL bIsEnterFromBMEntrypoint;
extern INT nBMProtectESP;
extern INT nBMProtectEBP;
extern "C" {



PDESTROY DestroyAddress = NULL;

PDESTROY HFileDestroyAddress = NULL;

PDESTROY DestroyMidiPlayer = NULL;

HANDLE hBlackMoonHeap = NULL;

PDESTROY BlackMoonFreeAllUserDll = NULL;

#pragma warning(push)
#pragma warning(disable : 4731)
	void  _cdecl E_End (DWORD Param1){

		E_DestroyRes();
		//if(bIsEnterFromBMEntrypoint)
		if(nBMProtectESP==0){
			 ExitProcess(Param1);
			 return;
		}

		__asm {
			mov eax,Param1;
			mov ebp,nBMProtectEBP;
			mov esp,nBMProtectESP;
			sub esp,4;
			ret;
		}

	}
#pragma warning(pop)

	void  _cdecl E_ReportError (DWORD Param1, DWORD MethodId, DWORD position){
		char ErrorString [255];
//		int nEbx;
		//__asm mov nEbx,ebx;
		LPSTR ptxt = NULL;
		if(0 <Param1 && Param1<12)
			ptxt = sErrorListForE[Param1-1];

		BOOL bThreeParam = FALSE;
		char *pRetnAddr = (char*)*(&Param1-1); // ȡ���ص�ַ
		if (NULL != pRetnAddr)
		{
			if (0xC483 == *(unsigned short*)(pRetnAddr)) // �Ƿ�Ϊadd esp, xxx;
			{
				bThreeParam = (0x0C <= *(pRetnAddr+2)); // ƽջ����Ϊ12����ʾ����������
			}
		}
		if (bThreeParam) // ������5.8�汾�����ϲŻ��������������ɵİ汾ֻ��һ��������
		{ wsprintf(ErrorString, "program internal error number is %d. \r\n%s\r\n����λ��:%d,%d", Param1, ptxt, MethodId, position); }
		else
		{ wsprintf(ErrorString, "program internal error number is %d. \r\n%s\r\n", Param1, ptxt); }

		INT nNoErrorBox = 0;
  		if (fnEError_callback && !isErrorCallBack)
		{
			isErrorCallBack = 1;
			nNoErrorBox = fnEError_callback(Param1, ErrorString);
			isErrorCallBack = 0;
		}

		if (!nNoErrorBox)
  			MessageBox(0, ErrorString, "error", MB_ICONERROR);

		E_End(0);

	}

	void*  _cdecl E_MAlloc (DWORD Param1){
		
		if(hBlackMoonHeap==NULL)
			hBlackMoonHeap =::GetProcessHeap();
		void * pData = HeapAlloc(hBlackMoonHeap, HEAP_ZERO_MEMORY, Param1);//malloc(Param1);//
		
		if(!pData)
			//ZeroMemory(pData,Param1);
		//else
		{
			char ErrorString[255];
			char** pRetnAddr = (char**)(&Param1 - 1); // ȡ���ص�ַ
			wsprintf(ErrorString, "����%d�ֽ��ڴ�ʧ��.\r\n"
				"ջ��Ϣ:\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n", 
				Param1,
				pRetnAddr[0], pRetnAddr[1],
				pRetnAddr[2], pRetnAddr[3],
				pRetnAddr[4], pRetnAddr[5],
				pRetnAddr[6], pRetnAddr[7],
				pRetnAddr[8], pRetnAddr[9],
				pRetnAddr[10], pRetnAddr[11],
				pRetnAddr[12], pRetnAddr[13],
				pRetnAddr[14], pRetnAddr[15]);
			MessageBox(0, ErrorString, "error", MB_ICONERROR);
			E_End (0);
		}

		return pData;
	}

	void*  _cdecl E_MAlloc_Nzero (DWORD Param1){
		
		if(hBlackMoonHeap==NULL)
			hBlackMoonHeap =::GetProcessHeap();
		void * pData = HeapAlloc(hBlackMoonHeap, NULL, Param1);//malloc(Param1);//
		
		if(!pData)
		{
			char ErrorString[255];
			char** pRetnAddr = (char**)(&Param1 - 1); // ȡ���ص�ַ
			wsprintf(ErrorString, "����%d�ֽ��ڴ�ʧ��.\r\n"
				"ջ��Ϣ:\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n", 
				Param1,
				pRetnAddr[0], pRetnAddr[1],
				pRetnAddr[2], pRetnAddr[3],
				pRetnAddr[4], pRetnAddr[5],
				pRetnAddr[6], pRetnAddr[7],
				pRetnAddr[8], pRetnAddr[9],
				pRetnAddr[10], pRetnAddr[11],
				pRetnAddr[12], pRetnAddr[13],
				pRetnAddr[14], pRetnAddr[15]);
			MessageBox(0, ErrorString, "error", MB_ICONERROR);
			E_End (0);
		}
		
		return pData;
	}

	void*  _cdecl E_MRealloc (void * Param1,DWORD Param2){
		void * pData;
		if(hBlackMoonHeap==NULL)
			hBlackMoonHeap =::GetProcessHeap();
		
		if(Param1)
			pData = HeapReAlloc(hBlackMoonHeap, 0, Param1, Param2);//realloc(Param1, Param2);//
		else
			pData = HeapAlloc(hBlackMoonHeap, HEAP_ZERO_MEMORY, Param2);//malloc(Param2);//
	
		if(!pData)
		{
			char ErrorString[255];
			char** pRetnAddr = (char**)(&Param2 - 2); // ȡ���ص�ַ
			wsprintf(ErrorString, "����%d�ֽ��ڴ�ʧ��.\r\n"
				"ջ��Ϣ:\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n"
				"0x%08X\r\n0x%08X\r\n", 
				Param1,
				pRetnAddr[0], pRetnAddr[1],
				pRetnAddr[2], pRetnAddr[3],
				pRetnAddr[4], pRetnAddr[5],
				pRetnAddr[6], pRetnAddr[7],
				pRetnAddr[8], pRetnAddr[9],
				pRetnAddr[10], pRetnAddr[11],
				pRetnAddr[12], pRetnAddr[13],
				pRetnAddr[14], pRetnAddr[15]);
			MessageBox(0, "�ڴ治��", "error", MB_ICONERROR);
			E_End (0);
		}

		return pData;

	}
	

	void  _cdecl E_MFree (void * Param1){




		if((DWORD)Param1 > 0xFFFF)
		{
			if(dwBlackMoonPESizeOfImage==0)
				GetBlackMoonPESizeOfImage();


			if(dwBlackMoonPEAddrrStart <= (DWORD)Param1 && (DWORD)Param1 <= dwBlackMoonPEAddrrEnd)
			{
				//MessageBox(NULL,"ErrorString","ָ��",MB_OK);
				return;
				
			}
		
			//�ж��Ƿ�����Ч�Ķ��ڴ�
			if (HeapValidate(hBlackMoonHeap, HEAP_NO_SERIALIZE, Param1) == 0)
			{
				return;
			}


			HeapFree(hBlackMoonHeap, 0 , Param1);  //free(Param1);//


			

		}


	}

	void  _cdecl E_Destroy (PDESTROY Param1){
		//int nIdx;
		//__asm mov nIdx,eax;	
		//if(nIdx==3)	//����2��
			DestroyAddress = Param1; 

	}

	void  _cdecl E_HelpFunc12 (DWORD Param1){
		int nIdx;
		__asm mov nIdx,eax;	
	//	if(nIdx==3)	//����12�ź�������,����2��

	}
	
	void*  _cdecl E_CloneConstArray (DWORD Param1,void** Param2){
		void *result;
		result = (void *)1;
		if(dwBlackMoonPESizeOfImage==0)
			GetBlackMoonPESizeOfImage();
		void* pAryData = *Param2;
		
		
		if(dwBlackMoonPEAddrrStart <= (DWORD)pAryData && (DWORD)pAryData <= dwBlackMoonPEAddrrEnd)
		{
			LPINT pnData;
			INT nArys,nDim;
			UINT nElementCount;
			
			pnData = (LPINT)pAryData;
			
			nArys=*pnData; //ȡ��ά����
			nDim = nArys;
			pnData +=1;
			
			// �����Ա��Ŀ��
			nElementCount = 1;
			while (nArys > 0)
			{
				nElementCount *= pnData[0];
				pnData ++;
				nArys--;
			}
			
			DWORD dwSize = Param1 * nElementCount + (nDim+1)*sizeof(INT);
			result = E_MAlloc_Nzero(dwSize);
			memcpy(result,pAryData,dwSize);
			*Param2 = result;

			
		}

		return result;
	}

	void*  _cdecl E_NULLARRAY ()
	{
		LPBYTE p = (LPBYTE)E_MAlloc_Nzero (sizeof (INT) * 2);
		*(LPINT)p = 1;  // ����ά����
		*(LPINT)(p + sizeof (INT)) = 0;
		return p;
	}

	void BlackMoonCalleLibFunctionHelper(INT nArgCount,MDATA_INF ArgInf,...)
	{
		PFN_EXECUTE_CMD pfn;
		__asm mov pfn, eax;
		MDATA_INF RetData={0};
		pfn(&RetData,nArgCount,&ArgInf);
		__asm{
			mov eax, RetData.m_unit.m_dwFormID;
			mov edx, RetData.m_unit.m_dwUnitID;
			mov ecx, RetData.m_dtDataType
		}		
	}
//	void main(){};
}
extern "C" INT BlackMoonCalleLibList;
extern "C" DWORD BlackMoonFuncForeLib;

void BlackMoonInitAllElib(){
	LPINT pfnNotifySys = &BlackMoonCalleLibList;
	while(*pfnNotifySys){
		PFN_NOTIFY_SYS g_fnNotifySys = (PFN_NOTIFY_SYS)*pfnNotifySys;
		g_fnNotifySys(NL_SYS_NOTIFY_FUNCTION,BlackMoonFuncForeLib,0);
		pfnNotifySys++;
	}
}

void BlackMoonFreeAllElib(){
	LPINT pfnNotifySys = &BlackMoonCalleLibList;
	while(*pfnNotifySys){
		PFN_NOTIFY_SYS g_fnNotifySys = (PFN_NOTIFY_SYS)*pfnNotifySys;
		g_fnNotifySys(NL_FREE_LIB_DATA,0,0);
		pfnNotifySys++;
	}
	if(BlackMoonFreeAllUserDll)
		BlackMoonFreeAllUserDll();
}


//���� - �ô�����ʾ����
/*
    ���ø�ʽ�� ���޷���ֵ�� �ô�����ʾ���� �����ӳ���ָ�� �������д�����ʾ���ӳ���ݣ� - ϵͳ����֧�ֿ�->����
    Ӣ�����ƣ�SetErrorManger
    �������������õ�����ʱ��������˵��³�����������ش���ʱ�����Ըô��������ϸ��ʾ���ӳ����ַ�����δ���д������ã��������ش���ʱ����ʱ�������Զ���ʾ�ô������ϸ��Ϣ��ֱ���˳���������Ϊ�߼����
    ����<1>������Ϊ���������д�����ʾ���ӳ��򡱣�����Ϊ���ӳ���ָ�루SubPtr���������Ա�ʡ�ԡ�ע����ӳ���������������������һ������Ϊ�����ͣ��������մ�����룬
	�ڶ�������Ϊ�ı��ͣ�����������ϸ�����ı���ͬʱ���ӳ�����뷵��һ���߼�ֵ���������ʾ�Ѿ����д�����ϣ�ϵͳ��������ʾ�ô�����Ϣ�����ؼٱ�ʾ��ϵͳ��������ʾ�ô�����Ϣ��
	�������ñ�����ȫ��ȷ������������Ԥ�⣬���⣬����ע�����¼��㣺 
	1���ô�����ʾ�ӳ���Ҫ�������µĴ��� 
	2���ڽ���ô�����ʾ�ӳ����ϵͳ���Զ��ر��¼���Ϣ֪ͨ������ƣ�Ҳ����˵�κ��¼����޷��õ���Ӧ�� 
	3��������Σ������ӳ�������˳���ϵͳ���Զ�������Ӧ�ó���رա�  ���ʡ�Ա�������ϵͳ���ָ�������ʾ��Ĭ�ϴ���ʽ��
*/
LIBAPI(void, krnln_SetErrorManger)
{
	if (ArgInf.m_dtDataType == SDT_SUB_PTR && ArgInf.m_pdwSubCodeAdr)
		fnEError_callback = (EError_CALLBACK)ArgInf.m_pdwSubCodeAdr;
	else
		fnEError_callback = NULL;
}

/*
	���ø�ʽ�� ���ı��͡� ��DLLװ��Ŀ¼ �����ı��� DLLװ��Ŀ¼�ݣ� - ϵͳ����֧�ֿ�->����
	Ӣ�����ƣ�SetDllCmdLoadPath
	�������������õ�������ִ�е�DLL����ʱװ����DLL���ļ�������װ��·������ϵͳ�����ȵ���·����ȥװ��ָ����DLL�ļ��������������ý��������DLL���������е�δָ��ȫ·����DLL���ļ�װ�ؾ���Ӱ�죬���ڳ��������ڼ�ȫ����Ч������ִ�к󷵻�ϵͳ�ڱ���������ǰ��ֵ��������Ϊ�߼����
	����<1>������Ϊ��DLLװ��Ŀ¼��������Ϊ���ı��ͣ�text���������Ա�ʡ�ԡ�������ָ��ϵͳ��DLL����������DLL���ļ�������װ��·���������ʡ�ԣ���Ĭ��ֵΪ���ı���

	����ϵͳ���� Windows
*/

#if _MSC_VER < 1916 //VS2017 VS2019
typedef BOOL (WINAPI *MySetDllDirectoryA)(LPCSTR lpPathName);
typedef DWORD (WINAPI *MyGetDllDirectoryA)(DWORD nBufferLength, LPSTR lpBuffer);
#endif

LIBAPI(char*, _krnln_SetDllCmdLoadPath)
{
	char szPath[MAX_PATH];
#if _MSC_VER >= 1916 //VS2017 VS2019
	DWORD dwLen = GetDllDirectoryA(MAX_PATH, szPath);
	if (ArgInf.m_pText) {
		SetDllDirectoryA(ArgInf.m_pText);
	}
#else
	HMODULE hKernel32 = GetModuleHandle("Kernel32.dll");
	if (!hKernel32)
		return NULL;

	MyGetDllDirectoryA MyGetDllDirectory = (MyGetDllDirectoryA)GetProcAddress(hKernel32, "GetDllDirectoryA");
	MySetDllDirectoryA MySetDllDirectory = (MySetDllDirectoryA)GetProcAddress(hKernel32, "SetDllDirectoryA");

	if(!MyGetDllDirectory || !MySetDllDirectory)
		return NULL;

	DWORD dwLen = MyGetDllDirectory(MAX_PATH, szPath);
	if (ArgInf.m_pText) {
		MySetDllDirectory(ArgInf.m_pText);
	}
#endif
	char* pText = NULL;
	if (dwLen > 0 && dwLen < MAX_PATH) {
		pText = (char*)E_MAlloc_Nzero(dwLen);
		memcpy(pText, szPath, dwLen);
		pText[dwLen] = '\0';
	}
	return pText;
}