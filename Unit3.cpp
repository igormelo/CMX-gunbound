//---------------------------------------------------------------------------

#define TOTALHOOKS 5
#define TIROS_BIG_FOOT 6
#define DLL_EXPORT __declspec(dllexport)

#include "mobiles.h"
#include "variaveis.h"
#include "drawUtils.h"
#include <tlhelp32.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"imagehlp.lib")

static HINSTANCE hApiHookDLL = 0;

struct FuncList Functions[] =
 {
   {"kernel32.dll","LoadLibraryA",LoadLibraryAHooked},
   {"wsock32.dll","recv",RecvHooked},
   {"wsock32.dll","send",SendHooked},
   {"ws2_32.dll","recv",RecvHooked},
   {"ws2_32.dll","send",SendHooked}
};

void RemoverHK(int index);
TAimBot aimbot;
HMODULE cmlib = 0;
DWORD debugFlag;
bool renderizar = false;
bool finishedLib = false;
bool libEncontrada = false;
HMODULE npsch = 0;
HMODULE npggnt = 0;
HMODULE handleBot = 0;

//#define DEBUGAR

//-------------------------------------------------------------------------
//    Envia mensagem de debug
//-------------------------------------------------------------------------
void debugar(int indice, int valor){
	SendMessage(HWND_BROADCAST, debugFlag, indice, valor );
}

void debugar2(int indice, int valor){
/*
if(BotInfos){
	BotInfos->teste.param1++;
	BotInfos->teste.param2 = indice;
	BotInfos->teste.param3 = valor;
}
*/
}

extern "C" DLL_EXPORT int WSAAPI O_recv( UINT s, const char * buf, int len, int flags ) {
	_asm {
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		ret
	}
}

int WINAPI H_recv( UINT s, const char * buf, int len, int flags ) {
	//MessageBoxA(NULL, buf, "Erro", NULL);
	//debugar(10, 20);
    return O_recv(s, buf, len, flags);
}

extern "C" DLL_EXPORT int WINAPI O_WSARecv(
  __in     SOCKET s,
  __inout  LPWSABUF lpBuffers,
  __in     DWORD dwBufferCount,
  __out    LPDWORD lpNumberOfBytesRecvd,
  __inout  LPDWORD lpFlags,
  __in     LPWSAOVERLAPPED lpOverlapped,
  __in     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
){
    _asm {
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		ret
	}
}

int WINAPI H_WSARecv(
  __in     SOCKET s,
  __inout  LPWSABUF lpBuffers,
  __in     DWORD dwBufferCount,
  __out    LPDWORD lpNumberOfBytesRecvd,
  __inout  LPDWORD lpFlags,
  __in     LPWSAOVERLAPPED lpOverlapped,
  __in     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
){
	debugar(20, 30);
	return O_WSARecv(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpOverlapped,lpCompletionRoutine);
}


int WINAPI O_WSARecvFrom(
   __in     SOCKET s,
  __inout  LPWSABUF lpBuffers,
  __in     DWORD dwBufferCount,
  __out    LPDWORD lpNumberOfBytesRecvd,
  __inout  LPDWORD lpFlags,
  __out    struct sockaddr *lpFrom,
  __inout  LPINT lpFromlen,
  __in     LPWSAOVERLAPPED lpOverlapped,
  __in     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
){
	_asm {
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		ret
	}
}

extern "C" DLL_EXPORT int WINAPI H_WSARecvFrom(
   __in     SOCKET s,
  __inout  LPWSABUF lpBuffers,
  __in     DWORD dwBufferCount,
  __out    LPDWORD lpNumberOfBytesRecvd,
  __inout  LPDWORD lpFlags,
  __out    struct sockaddr *lpFrom,
  __inout  LPINT lpFromlen,
  __in     LPWSAOVERLAPPED lpOverlapped,
  __in     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine

){
	debugar(30, 40);
	int resultado = O_WSARecvFrom(s,lpBuffers,dwBufferCount,lpNumberOfBytesRecvd,lpFlags,lpFrom,lpFromlen,lpOverlapped,lpCompletionRoutine);
    debugar(31, 41);
    return resultado;
}

int O_recvfrom(
  __in         SOCKET s,
  __out        char *buf,
  __in         int len,
  __in         int flags,
  __out        struct sockaddr *from,
  __inout_opt  int *fromlen
){
	_asm {
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		ret
	}
}

extern "C" DLL_EXPORT int WSAAPI H_recvfrom(
  __in         SOCKET s,
  __out        char *buf,
  __in         int len,
  __in         int flags,
  __out        struct sockaddr *from,
  __inout_opt  int *fromlen
){
   debugar(40, 50);
   int resultado = O_recvfrom(s,buf,len,flags,from,fromlen);
   debugar(41, 51);
   return resultado;
}





//-------------------------------------------------------------------------
//  Gera uma mensagem de erro
//-------------------------------------------------------------------------
void PerformError(char *msg, char *titulo = NULL) {
	if(!titulo)
        MessageBoxA(NULL, msg, "Erro", NULL);
    else
        MessageBoxA(NULL, msg, titulo, NULL);
    HANDLE proc = OpenProcess(PROCESS_TERMINATE, false, GetCurrentProcessId());
    TerminateProcess(proc, 0);
    CloseHandle(proc);
}

//-------------------------------------------------------------------------
//  Verifica se o driver est� carregado
//-------------------------------------------------------------------------
SC_HANDLE CM_SCM;
SC_HANDLE CM_SVC;
BOOL IsDriverLoaded() {
	__try{
		BOOL result = false;
		DWORD bytesNeeded = 0;
		LPDWORD pBytesNeeded = &bytesNeeded;
		LPBYTE pBuffer = NULL;
		SERVICE_STATUS_PROCESS *dave = NULL;

		char *buf = (char *)malloc(MAX_PATH);
		CM_StrCat(DLLPath, "Common\\Configs.ini", buf);
    
		TCMIniFile *ini = new TCMIniFile(buf);

		__try {
			char *dbg = ini->ReadString( "configs", "debug", "");
			__try {
				debugmode = ( strcmp(dbg, "967") == 0 );
			}
			__finally {
				free(dbg);
			}
        
			char *valor = HexToString(ini->ReadString( "configs", "Cod2", ""));
			//MessageBoxA(0, valor, "", 0);
			__try {
				if(CM_StrLen(valor) != 0) {
					CM_SCM = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE|GENERIC_READ);
					if(CM_SCM) {
						CM_SVC = OpenService(CM_SCM, valor, SERVICE_ALL_ACCESS);
                    
						BOOL result2 = QueryServiceStatusEx(CM_SVC, SC_STATUS_PROCESS_INFO, NULL, 0,	pBytesNeeded);
                    
						pBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *pBytesNeeded);
						result2 = QueryServiceStatusEx(CM_SVC, SC_STATUS_PROCESS_INFO, pBuffer, *pBytesNeeded, pBytesNeeded);
						dave = (SERVICE_STATUS_PROCESS *)pBuffer;
						result = (dave->dwCurrentState == SERVICE_RUNNING);
                    
						HeapFree(GetProcessHeap(), 0, pBuffer);
                    
						if(CM_SVC)
							CloseServiceHandle(CM_SVC);
						if(CM_SCM)
							CloseServiceHandle(CM_SCM);
					}
				}
			}
			__finally {
				free(valor);
			}
		}
		__finally {
			free(ini);
			free(buf);
		}
		return result;
	}__except(1){
    }
}

//-------------------------------------------------------------------------
//   Copia a mem�ria Bit a Bit
//-------------------------------------------------------------------------
void CMCopyMemory(void *dest, void *source, int size) {
    for(int i = 0; i < size; i++) {
      //  ((AnsiChar *)(dest))[i] = ((AnsiChar *)(source))[i];
    }
}


//-------------------------------------------------------------------------
//   Pega posi��o da c�mera
//-------------------------------------------------------------------------
TPoint GetCameraPos() {
	__try{
		TPoint camera(0,0);
		if(BotInfos){
			__try {
				camera.x = *((PWORD)(BotInfos->enderecos[1]));
				camera.y = *((PWORD)(BotInfos->enderecos[2]));

				camera.x -= 400;
				camera.y -= 240;
				if(camera.x > 60000)
					camera.x = (camera.x - 65535);

			}__except(1) {
				return camera;
			}
		}
		return camera;
	}__except(1){
	}
}

void __fastcall FillPoints(HDC dc){
	__try{
		/*char teste[100];
		int n = sprintf(teste, "Handles: %x - %x - x", npsch, npggnt, handleBot);
		TextOut(dc, 10, 50, teste, n);        */

		if(!BotInfos->modoDebug && renderizar){
			aimbot.camera = GetCameraPos();
			if(dc == 0)
				aimbot.fillPoints();
			else
				aimbot.fillPoints(dc);
		}
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//    Transforma uma cor para gdiplus:color
//-------------------------------------------------------------------------
Gdiplus::Color ToGdiColor(DWORD valor) {
	__try{
		TCM_RGB result = *(PCM_RGB)(&valor);
		result.x = 0xFF;
		return Gdiplus::Color(result.x , result.r, result.g, result.b);
    }__except(1){
	}
}

//-------------------------------------------------------------------------
//    Length da string unicode
//-------------------------------------------------------------------------
int CM_WStrLen(wchar_t *valor) {
	__try{
		wchar_t j = 0;
		for(int i = 0; i < MAX_PATH; i++) {
			if(valor[i] == j )
				return i;
		}
		return 0;
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//    Nem lembro pra que isso
//-------------------------------------------------------------------------
DWORD TransformInt(int index, DWORD valor) {
    DWORD res = ((index << 0x1c) & 0xFFFFFFFF) +  (valor & 0xFFFFFF);
    return res;
}

//-------------------------------------------------------------------------
//    Envia mensagem de debug
//-------------------------------------------------------------------------
void SendDebugInfo() {
	__try{
		if(!debugmode) return;
		SendMessage(HWND_BROADCAST, debugFlag, 15, TransformInt(1, (DWORD)BotInfos) );
		if(BotInfos) {
			SendMessage(HWND_BROADCAST, debugFlag, 15, TransformInt(3, (DWORD)BotInfos->forcaBot) );
			//SendMessage(HWND_BROADCAST, debugFlag, 15, TransformInt(4, (DWORD)BotInfos->enderecoCameraX.Ponteiro) );
			SendMessage(HWND_BROADCAST, debugFlag, 15, TransformInt(5, (DWORD)BotInfos->espessuraLinha) );
			SendMessage(HWND_BROADCAST, debugFlag, 15, TransformInt(6, (DWORD)BotInfos->janela) );
		}
    }__except(1){
	}
}

//-------------------------------------------------------------------------
//    Verifica se a tecla informada est� pressionada
//-------------------------------------------------------------------------
BOOL __fastcall pressionado(int valor){
   return (valor < -1 || valor > 1);
}

double __fastcall getCos(int angulo){
   return cos(intToRadian(angulo));
}

double __fastcall getSin(int angulo){
   return sin(intToRadian(angulo));
}

//-------------------------------------------------------------------------
//    SendName - Envia string para o exe de debug
//-------------------------------------------------------------------------
void SendName(char * nome2) {
	__try{
		char *nome = (char *)malloc(256);
		memcpy(nome, nome2, 256);
		//nome = (char *)(LPCTSTR)valor;
		int j = 0;
		DWORD valor = 0;
		for(int i = 0; i < 256; i++) {
			if(nome[i] == 0) {
				SendMessage(HWND_BROADCAST, SPY_MESSAGE, 8, valor );
				break;
			}
			j++;
			valor = (valor * 0x100) + nome[i];
			if(j == 4) {
				SendMessage(HWND_BROADCAST, SPY_MESSAGE, 8, valor );
				valor = 0;
			}
		}
    }__except(1){
	}
}

//-------------------------------------------------------------------------
//    Desenha status
//-------------------------------------------------------------------------
bool bitmapCarregado = false;

void DrawStatus(HDC hdc) {
	__try
	{
		bool semDC = (!hdc || hdc == 0);
		if(semDC){
			hdc = CreateDC("DISPLAY", "", "", NULL);
		}

		if(!hdc)
			return;

		__try
		{
			if(!BotInfos->mostrarStatus)
				return;

			// Carrega apenas uma vez
			if ( hBmp == NULL )
				 hBmp = Load32bppTga(ImageNameA, true);

			if(!bitmapCarregado){
				GetObject(hBmp, sizeof(BITMAP), &bmp);
				bitmapCarregado = true;
			}

			AlphaDraw(hdc, 10, 10, bmp.bmWidth, bmp.bmHeight, hBmp, 75);
    
			int bk = GetBkMode(hdc);
			SetBkMode(hdc, TRANSPARENT);

			HFONT Font;
			HFONT oldFont;
			Font = CreateFont(12, 7, 0, 0, 4, 0, FALSE, FALSE, 0, 0, 0, DEFAULT_QUALITY, 0, "Arial");
			oldFont = (HFONT)SelectObject(hdc, Font);
    
			DWORD cor;
			cor = GetTextColor(hdc);

			__try{
				__try{
					const sx = 10;
    
					SetTextColor(hdc, 0xFFFFFF);
					TextOutA(hdc, sx+10, 30, "Mobile: ", 8);
					TextOutA(hdc, sx+10, 45, "Alvo: ", 6);
					TextOutA(hdc, sx+10, 60, "BackShot: ", 10);
					TextOutA(hdc, sx+10, 75, "Modo: ", 6);
					TextOutA(hdc, sx+10, 90, "Efeito: ", 6);
					TextOutA(hdc, sx+10, 105, "Status: ", 8);
    
					SetTextColor(hdc, 0x00FF00);
					TextOutA(hdc, sx+60, 30, BotInfos->nomeMobile, CM_StrLen(BotInfos->nomeMobile));
					TextOutA(hdc, sx+50, 45, BotInfos->nomeAlvo, CM_StrLen(BotInfos->nomeAlvo));
					TextOutA(hdc, sx+76, 60, (BotInfos->backShot ? "Sim" : "Nao"), 3);
					switch((int)BotInfos->modoMira) {
						case 0: TextOutA(hdc, sx+50, 75, "Mais Perto", 10);
						break;
						case 1: TextOutA(hdc, sx+50, 75, "Mais Longe", 10);
						break;
						case 2: TextOutA(hdc, sx+50, 75, "Slice", 5);
						break;
						case 3: TextOutA(hdc, sx+50, 75, "Mouse", 5);
						break;
						case 4: TextOutA(hdc, sx+50, 75, "Random", 6);
						break;
						default: TextOutA(hdc, sx+50, 75, "Personalizado", 13);
						break;
					}
					TextOutA(hdc, sx+60, 90, (BotInfos->modoEspelho ? "Espelho" : "Tornado"), 7);
					if(BotInfos->ligado)
						TextOutA(hdc, sx+60, 105, "Ligado", 6);
					else
						TextOutA(hdc, sx+60, 105, "Desligado", 9);
				}__except(1){
				}
			}__finally{
				SelectObject(hdc, oldFont);
				SetTextColor(hdc, cor);
    
				DeleteObject(oldFont);
				DeleteObject(Font);
    
				SetBkMode(hdc, bk);
			}
		}__finally{
			 if(semDC)
				DeleteDC(hdc);
		}
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//     Callback do Blt
//-------------------------------------------------------------------------
HRESULT __stdcall H_Blt(LPVOID *param1 , LPRECT lpDestRect, LPVOID lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPVOID lpDDBltFx) {
	HRESULT retval = 0;

	__try{
    	// Pega o ponteiro do m�todo original, armazenado no recipiente
		DWORD* pvtbl = (DWORD*) *param1;
		DWORD* pMetodoOriginal = (DWORD*)( DWORD(pvtbl) + 28 );
		DWORD*	metodoOriginal = NULL;
		metodoOriginal = (DWORD*)*pMetodoOriginal;
		MyBlt_Type myBlt = (MyBlt_Type)metodoOriginal;

		//SendMessage(HWND_BROADCAST, debugFlag, (DWORD)710, (DWORD)metodoOriginal );
		//SendMessage(HWND_BROADCAST, debugFlag, (DWORD)lpDDSrcSurface, (DWORD)param1 );

		if(lpDDSrcSurface != NULL){
			HDC dc = 0;
			__try
			{
				HRESULT rdc = 0;
                ((LPDIRECTDRAWSURFACE7)(lpDDSrcSurface))->Restore();
				rdc = ((LPDIRECTDRAWSURFACE7)(lpDDSrcSurface))->GetDC(&dc);

				/*
				if(BotInfos){
					char teste[100];
					int n = sprintf(teste, "%x | %f | %f", BotInfos->enderecos[0],  BotInfos->maya.raio,  BotInfos->maya.velocidadeAngular);
					TextOut(dc, 10, 90, teste, n);
				}
				*/

				if(rdc == DD_OK){
					DrawStatus(dc);
					if(!BotInfos->linhaGraficoAntigo)
						FillPoints(dc);
				}
				((LPDIRECTDRAWSURFACE7)(lpDDSrcSurface))->ReleaseDC(dc);
			}
			__except(1)
			{
			}
		}else{
			if(BotInfos) {
				if(BotInfos->mostrarStatus) {
					DrawStatus(0);
					/*
					HDC dc = 0;
					((LPDIRECTDRAWSURFACE7)(param1))->GetDC(&dc);
					DrawStatus(dc);
					if(!BotInfos->linhaGraficoAntigo)
						FillPoints(dc);
					((LPDIRECTDRAWSURFACE7)(param1))->ReleaseDC(dc);
					*/
				}
			}
		}

		retval = myBlt(param1, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx);

		if(BotInfos != NULL && BotInfos->ligado && BotInfos->linhaGraficoAntigo){
			CanDrawNow = true;
		}
	}
	__except(1)
	{
		//SendMessage(HWND_BROADCAST, debugFlag, (DWORD)1000, (DWORD)10 );
		return DD_FALSE;
	}
	return retval;
}

//-------------------------------------------------------------------------
//  CreateSurface Hook
//-------------------------------------------------------------------------
HRESULT __stdcall PASCAL H_CreateSurface( LPVOID *param1 ,  LPVOID lpDDSurfaceDesc, LPVOID * lplpDDSurface, DWORD * pUnkOuter) {

	// Chama o CreateSurface original
	__try{
		HRESULT retval = OldCreateSurface(param1 , lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);
#ifdef DEBUGAR
		if(npsch != 0){
			if(!finishedLib)
			{
				 finishedLib = true;
				 FreeLibrary(npsch);
			}
        }else{
			npsch = GetModuleHandleA("npsc.des");
			npggnt = GetModuleHandleA("npggNT.des");
			handleBot = GetModuleHandleA("msvcrt36.dll");
        }
#endif

		__try
		{
			BOOL DrvLoad = IsDriverLoaded();
			if(retval == DD_OK) {
				DWORD* ppvtbl = (DWORD*) *lplpDDSurface;
				DWORD* pvtbl = (DWORD*) *ppvtbl;

				//Variaveis para desproteger a memoria
				DWORD flOldProtect, flDontCare;
        
				if(DrvLoad)
					CMVirtualProtect(GetCurrentProcess(), (PVOID)pvtbl, 60, PAGE_EXECUTE_READWRITE, &flOldProtect);
				else
					VirtualProtect((PVOID)pvtbl, 60, PAGE_EXECUTE_READWRITE, &flOldProtect);

				/* 		Vamos hookar o Blt e uma outra que n�o � utilizada pelo jogo.
				* Uma vez que o metodo X n�o � usado pelo jogo, utilizaremos como
				* recipiente para nosso Blt original;
				*  		V�rias surfaces s�o criadas e, assim, cada uma tem seu endere�o
				* de fun��es como o Blt. Desta forma precisar�amos manipular um array a
				* cada callback para achar o metodo correspondente a chamada.
				*/
				DWORD* pMetodoAlvo = (DWORD*)( DWORD(pvtbl) + 20 );
				DWORD* pRecipienteOriginal = (DWORD*)( DWORD(pvtbl) + 28 ); //Vamos usar o flip  pBltFastX

				DWORD* metodoAlvo = NULL;
				DWORD* recipienteOriginal = NULL;

				metodoAlvo = (DWORD*)*pMetodoAlvo;
				recipienteOriginal = (DWORD*)*pRecipienteOriginal;

				DWORD* pCallBackAlvo = (DWORD*)&H_Blt;
				DWORD* pCallBackRecipiente = (DWORD*)&metodoAlvo;

				//SendMessage(HWND_BROADCAST, debugFlag, (DWORD)910, (DWORD)metodoAlvo );
				//SendMessage(HWND_BROADCAST, debugFlag, (DWORD)920, (DWORD)recipienteOriginal );

				if((DWORD)metodoAlvo != (DWORD)&H_Blt){
					//*pCallBackRecipiente = (DWORD)metodoAlvo;
					*pRecipienteOriginal = (DWORD)metodoAlvo;
					*pMetodoAlvo = (DWORD)pCallBackAlvo;
				}

				if(DrvLoad)
					CMVirtualProtect(GetCurrentProcess(), (PVOID)pvtbl, 60, flOldProtect, &flDontCare);
				else
					VirtualProtect((PVOID)pvtbl, 60, flOldProtect, &flDontCare);
			}
		}__except(1){}

		return retval;
	}__except(1){
    	return DD_FALSE;
	}
}

//-------------------------------------------------------------------------
//   Trampolim do DirectDrawCreateEx
//-------------------------------------------------------------------------
extern "C" DLL_EXPORT DWORD WINAPI O_DirectDrawCreateEx( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid, IUnknown FAR *pUnkOuter ) {
    _asm {
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        nop
        ret
    }
}

//-------------------------------------------------------------------------
//   CallBack do DirectDrawCreateEx
//-------------------------------------------------------------------------
HRESULT WINAPI H_DirectDrawCreateEx( GUID FAR * lpGuid, LPVOID  *lplpDD, REFIID  iid, IUnknown FAR *pUnkOuter ) {

	__try{
		// Chama o DirectDrawCreateEx original
		HRESULT retval = O_DirectDrawCreateEx(lpGuid, lplpDD, iid, pUnkOuter);

		(void*)glpdd = (void*)*lplpDD;

		// Precisamos do driver para cuidar da prote��o da mem�ria
		BOOL DrvLoad = IsDriverLoaded();
    
		__try {
			//Pega o pontero da interface
			DWORD* ppvtbl = (DWORD*)*lplpDD;
			DWORD* pvtbl = (DWORD*) *ppvtbl;
        
			//Variaveis para desproteger a memoria
			DWORD flOldProtect, flNewProtect, flDontCare;
			MEMORY_BASIC_INFORMATION mbi;
        
			//Se o driver n�o estiver carregado n�o hooka, evitando crash do sistema
			if(DrvLoad)
				CMVirtualProtect(GetCurrentProcess(), (PVOID)pvtbl, 30, PAGE_EXECUTE_READWRITE, &flOldProtect);
			else
				VirtualProtect((PVOID)pvtbl, 38, PAGE_EXECUTE_READWRITE, &flOldProtect);
        
			DWORD*	g_pCreateSurface = NULL;
			DWORD*  pCreateSurface;

			pCreateSurface = (DWORD*)( DWORD(pvtbl) + 24 );
			g_pCreateSurface = (DWORD*)*pCreateSurface;

			OldCreateSurface = (MyCreateSurface_Type)g_pCreateSurface;
        
			DWORD* pMyCreateSurface = (DWORD*)&H_CreateSurface;
        
			//replace CreateDevice entry with my own function
			*pCreateSurface = (DWORD)pMyCreateSurface;


        
			if(DrvLoad)
				CMVirtualProtect(GetCurrentProcess(), (PVOID)pvtbl, 30, flOldProtect, &flDontCare);
			else
				VirtualProtect((PVOID)pvtbl, 30, flOldProtect, &flDontCare);
		}
		__except(1) {
			PerformError("Erro n� 33");
		}
		//SendMessage(HWND_BROADCAST, SPY_MESSAGE, 5, 1 );
		RemoverHK(0);
		return retval;
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//   Zera os dados de um item no array de hook
//-------------------------------------------------------------------------
void ZeroValue(int index) {
	__try{
		for (int i = 0; i < 5; i++) {
			hooks[index].newdata[i] = 0;
			hooks[index].olddata[i] = 0;
			hooks[index].tramp[i] = 0;
		}
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//   Insere hook de CodeOverwrite
//-------------------------------------------------------------------------
void InsertIt(int index, PVOID func, PVOID callback, PVOID tranpolin) {
    __try {
		ZeroValue(index);
		unsigned char* FuncHookPtr = (unsigned char*)func;
        unsigned char* HookTramp = (unsigned char*)tranpolin;
		BOOL DrvLoad = IsDriverLoaded();
		hooks[index].original = (unsigned char*)FuncHookPtr;
        hooks[index].callback = (unsigned char*)callback;
        hooks[index].tranpolin = (unsigned char*)tranpolin;
		DWORD OldProtect;
        int i;

        hooks[index].newdata[0] = 0xE9;
        *(PDWORD)&(hooks[index].newdata[1]) = (DWORD)( (DWORD)callback - ((DWORD)FuncHookPtr + 5));
        
        hooks[index].tramp[0] = 0x68; //Push
        *(PDWORD)&(hooks[index].tramp[1]) = ((DWORD)FuncHookPtr + 5);
        
		if(DrvLoad)
            CMVirtualProtect(GetCurrentProcess(), FuncHookPtr, 10, PAGE_EXECUTE_READWRITE, &OldProtect); //Unprotect the target memory. 10 bytes for good measure.
        else
            VirtualProtect(FuncHookPtr, 10, PAGE_EXECUTE_READWRITE, &OldProtect); //Unprotect the target memory. 10 bytes for good measure.
        
		//Copia os bytes originais
        for (i = 0; i < 5; i++) {
            hooks[index].olddata[i] = FuncHookPtr[i];
            FuncHookPtr[i] = hooks[index].newdata[i];
        }
        
        if(DrvLoad)
            CMVirtualProtect(GetCurrentProcess(), FuncHookPtr, 10, OldProtect, NULL); //Reprotect the memory.
        else
            VirtualProtect(FuncHookPtr, 10, OldProtect, NULL); //Reprotect the memory.
        
        if(DrvLoad)
            CMVirtualProtect(GetCurrentProcess(), HookTramp, 25, PAGE_EXECUTE_READWRITE, &OldProtect);
        else
            VirtualProtect(HookTramp, 25, PAGE_EXECUTE_READWRITE, &OldProtect); //Reprotect the memory.
        
        for (i = 0; i < 5; i++) {
            HookTramp[i] = hooks[index].olddata[i];
        }
        
        for (i = 0; i < 50; i++) {
            if (HookTramp[i] == 0x90  &&
                    HookTramp[i+1] == 0x90 &&
                    HookTramp[i+2] == 0x90 &&
                    HookTramp[i+3] == 0x90 &&
                    HookTramp[i+4] == 0x90) {
                HookTramp[i]   = hooks[index].tramp[0];
                HookTramp[i+1] = hooks[index].tramp[1];
                HookTramp[i+2] = hooks[index].tramp[2];
                HookTramp[i+3] = hooks[index].tramp[3];
                HookTramp[i+4] = hooks[index].tramp[4];
                break;
            }
        }
        
        if(DrvLoad)
            CMVirtualProtect(GetCurrentProcess(), HookTramp, 25, OldProtect, NULL);
        else
            VirtualProtect(HookTramp, 25, OldProtect, NULL);
	}__except(1) {
        PerformError("Erro n� 24");
    }
	//SendMessage(HWND_BROADCAST, SPY_MESSAGE, 4, 0x111111 );
}

//-------------------------------------------------------------------------
//  Remove hook de CodeOverwrite
//-------------------------------------------------------------------------
void RemoverHK(int index) {
	__try{
		unsigned char* FuncHookPtr = (unsigned char*)hooks[index].original;
		DWORD OldProtect;
		BOOL DrvLoad = IsDriverLoaded();

		// O Driver precisa estar carregado, para utilizar o Virtual Protect
		if(!IsDriverLoaded())
			return;
    
		if(DrvLoad)
			CMVirtualProtect(GetCurrentProcess(), FuncHookPtr, 10, PAGE_EXECUTE_READWRITE, &OldProtect);
		else
			VirtualProtect(FuncHookPtr, 10, PAGE_EXECUTE_READWRITE, &OldProtect);

		// Restaura os 5 primeiros bytes
		for (int i = 0; i < 5; i++) {
			FuncHookPtr[i] = hooks[index].olddata[i];
		}
    
		if(DrvLoad)
			CMVirtualProtect(GetCurrentProcess(), FuncHookPtr, 10, OldProtect, NULL);
		else
			VirtualProtect(FuncHookPtr, 10, OldProtect, NULL);
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//  Controle de Drawing
//-------------------------------------------------------------------------
DWORD WINAPI ThreadDesenha( LPVOID lpParam ) {
	bool desenhando = false;
	while(true) {
		Sleep(2);

		if(!desenhando)
			desenhando = true;
		else
            continue;

		__try{
			__try{
				if(CanDrawNow) {
					CanDrawNow = false;
					FillPoints(0);
				}
			}__except(1){
			}
		}__finally{
            desenhando = false;
        }
    }
}

DWORD WINAPI ThreadEnderecos( LPVOID lpParam ) {
	bool procurando = false;
	while(true) {
		Sleep(10);

		if(!procurando)
			procurando = true;
		else
            continue;

		__try{
			__try{
				if(BotInfos){
					__try{
						// Indice do Jogador
						BotInfos->indiceBot = *((PBYTE)(BotInfos->enderecos[4]));

						// For�a do Vento
						BotInfos->aceleracaoVento = *((PBYTE)(BotInfos->enderecos[12]));

						BotInfos->anguloVento = *((PWORD)(BotInfos->enderecos[13]));

						if(BotInfos->autoMobile)
						   BotInfos->mobile = *((PBYTE)(BotInfos->enderecos[0]));

						int j = MobileToIndex(BotInfos->mobile);

						BotInfos->inclinacaoBot    = *((PWORD)(BotInfos->enderecos[5]+(0x18*BotInfos->indiceBot)+8));

						for(int i = 0; i < 8; i++){
							BotInfos->semiPosicaoBot[i].x = *((PWORD)(BotInfos->enderecos[5]+(0x18*i)  ));
							BotInfos->semiPosicaoBot[i].y = *((PWORD)(BotInfos->enderecos[5]+(0x18*i)+4));
						}

						Byte indexInGame = *((PBYTE)(BotInfos->enderecos[6]));
						renderizar = (indexInGame != 0);//((indexInGame == 3) || (indexInGame == 11));

						BotInfos->inGame = renderizar;

						BotInfos->semiDireita = *((PBYTE)(BotInfos->enderecos[16]+(BotInfos->indiceBot*0x18)));

						BotInfos->semiForcaBot = *((PDWORD)(BotInfos->enderecos[9]));

						BotInfos->camera = GetCameraPos();

						memcpy(&BotInfos->times[0], (PBYTE)(BotInfos->enderecos[15]), 8);
						memcpy(&BotInfos->flags[0], (PBYTE)(BotInfos->enderecos[11]), 8);
					}__except(1){
					}
				}
			}__except(1){
			}
		}__finally{
            procurando = false;
        }
    }
}

//-------------------------------------------------------------------------
//   Mapeia a mem�ria para comunica��o com o CMX
//-------------------------------------------------------------------------
void MapearMemoria() {
	__try{
		int i = 0;
		BOOL DrvLoad = IsDriverLoaded();

		// Aguarda 1 segundo caso o Driver n�o esteja carregado
		while(!DrvLoad) {
			if(i >= 10)
				break;
			DrvLoad = IsDriverLoaded();
			i++;
			Sleep(100);
		}

		// Driver n�o carregado
		if(!DrvLoad){
			PerformError("Erro n� 27");
		}
    
		__try {
			BotInfos = new TBotInfos;
			BotInfos = 0;
			hMemFile = cmOpenFileMappingA(FILE_MAP_WRITE, False, MapName);
			BotInfos = (PBotInfos)cmMapViewOfFile(hMemFile, FILE_MAP_WRITE, 0, 0, 0);
			//debug(20,1);
			if( !BotInfos ) {
				hMemFile = CreateFileMappingA((PVOID)MAXDWORD, NULL, PAGE_READWRITE, 0, sizeof (*BotInfos), (PCHAR)MapName);
				BotInfos = (PBotInfos)cmMapViewOfFile(hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				if(!BotInfos){
					//debug(99,3);
				   PerformError("Erro n� 31");
				   return;
				}
				//debug(20,(DWORD)BotInfos);
				return;
			}
		}
		__except(1) {
			//debug(99,4);
			PerformError("Erro n� 29");
		}
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//   Pega endere�o de uma fun��o na dll informada
//-------------------------------------------------------------------------
PVOID GetAddress(char *lib, char *func) {
	__try{
		HANDLE libH = LoadLibrary(lib);
		PVOID address = (PVOID)GetProcAddress((HMODULE)libH, func);
		return address;
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//   L� o arquivo de configura��es e pega o prefixo do nome das fun��es
//   da dll de exports de nomes mutantes
//-------------------------------------------------------------------------
char *CompletaPrefixo(char *index) {
	__try{
		char *buf = (char *)malloc(MAX_PATH);
		CM_StrCat(DLLPath, "Common\\Configs.ini", buf);
		TCMIniFile *ini = new TCMIniFile(buf);
		char *prefixo = ini->ReadString("configs", "Prefix", "F");
		CM_StrCat(prefixo, index, buf);
		free(ini);
		return buf;
    }__except(1){
	}
}

//-------------------------------------------------------------------------
//   Visto que os metodos de import da dll tem o nome dinamico,
//   localiza-o pelo indice
//-------------------------------------------------------------------------
PDWORD GetMutantAddress(HMODULE module, int valor) {
	__try{
		typedef DWORD(__stdcall *TF1)(int index);
		TF1 F1 = TF1(GetProcAddress(module, CompletaPrefixo("1")));
		PVOID result = (PDWORD)( F1(valor) -456789);
    }__except(1){
	}
}

//-------------------------------------------------------------------------
//   Carrega Fun��es do Driver
//-------------------------------------------------------------------------
void LoadFunctions(void) {
	__try{
		BOOL DrvLoad = IsDriverLoaded();
		int j = 0;

		// Espera 1 segundo at� o driver ser carregado
		while(!DrvLoad) {
			if(j >= 10)
				break;
			j++;
			DrvLoad = IsDriverLoaded();
			Sleep(100);
		}

		// Falha ao carregar o driver
		if(!DrvLoad)
			PerformError("Erro n� 15");

		__try {
			DriverLib = cmlib;
			CMOpenProcess = f_CMOpenProcess( GetMutantAddress(DriverLib, 4 ));
			CMVirtualProtect = TcmVirtualProtect( GetMutantAddress(DriverLib, 7 ));
			cmOpenFileMappingA = TcmOpenFileMappingA( GetMutantAddress(DriverLib, 8 ));
			cmMapViewOfFile = TcmMapViewOfFile( GetMutantAddress(DriverLib, 9 ));
			CMGetDC = f_CMGetDC( GetMutantAddress( DriverLib, 5 ));
			CMGetWindowDC = f_CMGetDC( GetMutantAddress( DriverLib, 10 ));
			if(!cmOpenFileMappingA || !cmMapViewOfFile || !CMGetWindowDC) {
				PerformError("Erro n� 17");
			}
			if(!CMOpenProcess ||!CMVirtualProtect || !CMGetWindowDC || !cmOpenFileMappingA ||!cmMapViewOfFile || !CMGetWindowDC)
				PerformError("Erro n� 19");
		}
		__except(1) {
			PerformError("Erro n� 21");
		}
	}__except(1){
	}
}

//-------------------------------------------------------------------------
//   HOOK POR IAT PATCH
//-------------------------------------------------------------------------

struct FuncList Functions[] =
 {

   {"kernel32.dll","LoadLibraryA",LoadLibraryAHooked},
   {"wsock32.dll","recv",RecvHooked},
   {"wsock32.dll","send",SendHooked},
   {"ws2_32.dll","recv",RecvHooked},
   {"ws2_32.dll","send",SendHooked}


};

EXPORT int HookFunctions()
{

 DWORD dwPID, dwHooked, dwNumOfFunctions, dwMatches, i;
 HANDLE hSnapShot;
 MODULEENTRY32 mModuleEntry;
 PFUNCLIST pFuncList;


    dwPID     = GetCurrentProcessId();
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    if(hSnapShot == INVALID_HANDLE_VALUE)
      return -1;

    mModuleEntry.dwSize = sizeof(MODULEENTRY32);

    if(!Module32First(hSnapShot, &mModuleEntry))
      return -1;

    dwNumOfFunctions = sizeof(Functions) / sizeof(struct FuncList);

    dwHooked = dwMatches = 0;

    do
     {

      if(mModuleEntry.hModule != hApiHookDLL)
       {

         for(pFuncList = Functions, i = 0; i < dwNumOfFunctions;  pFuncList++, i++)
          {

           if(ReplaceIATEntries(pFuncList, mModuleEntry.hModule, &dwMatches) == REPLACE_SUCCESS)
             dwHooked++;
          }

       }

     }while(Module32Next(hSnapShot,&mModuleEntry));


    CloseHandle(hSnapShot);


    if(!dwMatches)
      return -1;
    if(dwHooked == dwMatches)
      return 1;
    else
      return 0;

}

/////////////////////////////////////////////////////////////////////////////////////

EXPORT int ReplaceIATEntries(PFUNCLIST pFuncList, HMODULE hModuleBase, LPDWORD lpMatches)
{

 LPCSTR  lpszModuleName;
 DWORD   dwSize, dwOld;
 HANDLE  hProcess;

 PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
 PIMAGE_THUNK_DATA pThunk;
 PROC * pfnFunc, pfnTarget;

     hProcess = GetCurrentProcess();

     pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
                    ImageDirectoryEntryToData(hModuleBase,
                                             TRUE,
                                             IMAGE_DIRECTORY_ENTRY_IMPORT,
                                             &dwSize);
     if(!pImportDesc)
       return NO_IMPORT_DESCRIPTOR;

     while(pImportDesc->Name)
      {

        lpszModuleName = (LPSTR)((LPBYTE)hModuleBase + pImportDesc->Name);

        if(!_strcmpi(lpszModuleName, pFuncList->lpszModuleName))
          break;

        pImportDesc++;

      }

      if(!pImportDesc->Name)
       return MODULE_NOT_FOUND;


     pThunk = (PIMAGE_THUNK_DATA)((LPBYTE) hModuleBase + pImportDesc->FirstThunk);

     while(pThunk->u1.Function)
      {
        pfnFunc   = (PROC*)&pThunk->u1.Function;

        pfnTarget = GetProcAddress(GetModuleHandle(pFuncList->lpszModuleName),
                                pFuncList->lpszFunctionName);

        if(pfnFunc && pfnTarget && *pfnFunc == pfnTarget)
         {

          if(lpMatches)
            (*lpMatches)++;

          if(VirtualProtect(pfnFunc, sizeof(pfnFunc), PAGE_EXECUTE_READWRITE, &dwOld) &&
            (WriteProcessMemory(hProcess, pfnFunc, &pFuncList->lpNewAddress, sizeof(LPVOID), NULL)))
            {
              return REPLACE_SUCCESS;
            }
         }

        pThunk++;

      }

  return FUNCTION_NOT_FOUND;
}

/////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------
//   Carrega DLL
//-------------------------------------------------------------------------
//IMAGE_DOS_HEADER __ImageBase;
#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved) {
    
    switch(reason) {
		case DLL_PROCESS_ATTACH: {

			// Registra mensagem para debug
			debugFlag = RegisterWindowMessage("spy_message");

            // Inicializa GDI+.
            __try {
                Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
                
                // Diretorio do processo injetado - GB
                char *DLLPath1 = (char *)malloc(MAX_PATH);
                DLLPath = (char *)malloc(MAX_PATH);
                GetModuleFileNameA(hinst, &DLLPath1[0], MAX_PATH);
                MyExtractFilePath(DLLPath1, DLLPath);
                free(DLLPath1);
                
                // Dll de intermedia��o com o driver
                char *buf = (char *)malloc(MAX_PATH);
                CM_StrCat(DLLPath, "LangLib.dll", buf);
                cmlib = LoadLibraryA(buf);

                if(!cmlib)
                    PerformError("Erro n� 9");
                
                wchar_t x;
                int totlen = (CM_StrLen(DLLPath)+9)*(sizeof x);
                ImageName = new wchar_t[totlen];
                memset(ImageName, 0x00, totlen);
                mbstowcs(ImageName, DLLPath, CM_StrLen(DLLPath));
                
                //Nome do arquivo de imagem TGA do menu (WCHAR)
                wcscat(ImageName, L"stat.dat");
                
                //Nome do arquivo de imagem TGA do menu (PCHAR)
                ImageNameA = new char[CM_StrLen(DLLPath)+9];
                CM_StrCat(DLLPath, "stat.dat", ImageNameA);

                // Carrega ponteiros das funcoes do driver
				LoadFunctions();

                Sleep(800);
			}
            __except(1) {
				MessageBoxA(0, "Erro n� 138.", "Erro", MB_ICONERROR );
                HANDLE proc = OpenProcess(PROCESS_TERMINATE, false, GetCurrentProcessId());
                TerminateProcess(proc, 0);
                CloseHandle(proc);
			}

			__try{
				// Mapeia a memoria para troca de informa��es com o CMX
				MapearMemoria();
            
				// Hooka o LoadLibrary para evitar load do npggnt.des
				// InsertIt(0,GetAddress("kernel32.dll", "LoadLibraryA"), (PVOID)&MyLoadLib, (PVOID)&OldLoadLib);
                npsch = GetModuleHandleA("npsc.des");
				// Hooka o directDraw
				InsertIt(0, GetAddress("ddraw.dll", "DirectDrawCreateEx"), (PVOID)&H_DirectDrawCreateEx, (PVOID)&O_DirectDrawCreateEx);
				/*InsertIt(1, GetAddress("Ws2_32.dll", "recv"), (PVOID)&H_recv, (PVOID)&O_recv);
				InsertIt(2, GetAddress("Ws2_32.dll", "recvfrom"), (PVOID)&H_recvfrom, (PVOID)&O_recvfrom);
				InsertIt(3, GetAddress("Ws2_32.dll", "WSARecv"), (PVOID)&H_WSARecv, (PVOID)&O_WSARecv);
				InsertIt(4, GetAddress("Ws2_32.dll", "WSARecvFrom"), (PVOID)&H_WSARecvFrom, (PVOID)&O_WSARecvFrom);
				*/
				Sleep(300);

				// Cria thread para desenho na tela
				HandleThread1 = CreateThread( NULL, 0, ThreadDesenha, &ParamsThread1, 0, NULL);
				HandleThread2 = CreateThread( NULL, 0, ThreadEnderecos, &ParamsThread2, 0, NULL);
            }__except(1){
			}
        }
        break;
        case DLL_PROCESS_DETACH: {
            Gdiplus::GdiplusShutdown(gdiplusToken);
        }
        break;
        
        default:  break;
    }
    return true;
    
}
//---------------------------------------------------------------------------
