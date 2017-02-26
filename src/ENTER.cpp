#include <cstdlib>
#include <cstdio>
#include <Windows.h>

const char *g_szClassNameCodeViewer = "WNDCLASS cGTECodeViewer";
const int g_szBlockSize = 32;

char DBG[256];

void *rGTEmalloc(int sz) {
	return malloc(sz);
}
void *rGTEfree(void *ptr) {
	free(ptr);
	return 0;
}
void *rGTErealloc(void *ptr, int sz, int isz=-1) {
	void *buf=rGTEmalloc(sz);
	if(isz > 0 && sz > isz)
		memcpy(buf, ptr, isz);
	rGTEfree(ptr);
	return buf;
}

class cGTEmem {
private:
	void *_Buf;
	int _Size; // Количество занятых байт
	int _MaxSize; // Максимальное количество байт
	// 0 1 2 ... _Size-1
	// -_Size-1 -_Size
public:
	void *AddEnd(const void *ptr, int sz) {
		if(!ptr) return _Buf;
		if(sz <= 0) return _Buf;
		if(_Size + sz >= _MaxSize)
			_Buf=rGTErealloc(_Buf,(_MaxSize=(_Size + sz + g_szBlockSize)),_Size);
		memcpy((void*)(int(_Buf)+_Size), ptr, sz);
		_Size+=sz;

		return _Buf;
	}
	void *AddBegin(const void *ptr, int sz) {
		if(!ptr) return _Buf;
		if(sz <= 0) return _Buf;
		if(_Size + sz >= _MaxSize)
			_Buf=rGTErealloc(_Buf,(_MaxSize=(_Size + sz + g_szBlockSize)),_Size);
		char *pm = (char*)(int(_Buf)+sz);		
		for (char *pd = (char*)(int(_Buf)+_Size+sz); pd >= pm; --pd)
			*pd = *(pd-sz);
		memcpy(_Buf, ptr, sz);
		_Size+=sz;
		return _Buf;
	}
	void *Add(int pos, const void *ptr, int sz) {
		if(pos < 0) pos=_Size+pos+1;
		if(!ptr) return _Buf;
		if(sz <= 0) return _Buf;
		if(_Size==pos) return AddEnd(ptr, sz);
		if(pos==0) return AddBegin(ptr, sz);
		if(_Size-pos < 0) return _Buf;
		if(pos + sz >= _MaxSize)
			_Buf=rGTErealloc(_Buf,(_MaxSize=(_Size + sz + g_szBlockSize)),_Size);
		char *pm = (char*)(int(_Buf)+pos+sz);
		for (char *pd = (char*)(int(_Buf)+_Size+sz); pd >= pm; --pd)
			*pd = *(pd-sz);
		memcpy((void*)(int(_Buf)+pos), ptr, sz);
		_Size+=sz;
		return _Buf;
	}
	void *GetBuf() {return _Buf;}
	int GetSize() {return _Size;}
	int GetMaxSize() {return _MaxSize;}
	cGTEmem(const void *ptr, int sz) {cGTEmem();AddEnd(ptr,sz);}
	cGTEmem() {_Size=0;_Buf=rGTEmalloc(_MaxSize=g_szBlockSize);}
	~cGTEmem() {if(_Buf) _Buf=rGTEfree(_Buf);}
};

class cGTECodeLine : public cGTEmem {
private:
	cGTECodeLine *pPrev, *pNext;
public:
	BOOL GetBreakpoint() {
		return 1;
	}
	BOOL GetSpoiler() {
		return 1;
	}
	cGTECodeLine() {pPrev=0;pNext=0;}
	cGTECodeLine(cGTECodeLine *pP, cGTECodeLine *pN) {pPrev=pP;pNext=pN;}
	cGTECodeLine(const void *ptr, int sz) {cGTECodeLine();AddEnd(ptr, sz);}
};

class cGTECodeLines  : private cGTEmem {
public:
	INT _Lines;
	cGTECodeLine *AddNewLine(const char *ptr, int sz) {
		cGTECodeLine *o=new cGTECodeLine(ptr, sz);
		AddEnd(&o, sizeof(cGTECodeLine*));
		_Lines = GetSize()/sizeof(cGTECodeLine*);
		return o;
	}
	cGTECodeLine *GetLine(INT n) {
		if(n < 0) n = _Lines+n;
		if(n >= _Lines) return 0;
		return *(((cGTECodeLine**)(GetBuf()))+n);
	}
	~cGTECodeLines(){
		for (int i = 0; i < _Lines; ++i)
			delete GetLine(i);
	}
};

class cGTECodeViewer : public cGTECodeLines {
private:
	HWND _hWnd;
	HFONT _hF, _hFI; // Шрифты
	INT _FW, _FH; // Размеры шрифта
	INT _CX, _CY, _CW, _CH; // Размеры клиентской области
	INT _LW;
public:
	VOID Render() {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(_hWnd, &ps);		
		if(!_FW) {
			SelectObject(hdc, _hF);
			GetCharWidth32(hdc, '0', '0', &_FW);
		}
		SelectObject(hdc, _hF);
		SetTextColor(hdc, RGB(0,0,0));
		char txt[10];
		char txtfmt[16];
		sprintf(txtfmt, "%%%dd", _LW-2);
		for (int i = 0; i < _Lines; ++i) {
			cGTECodeLine* l = GetLine(i);
			if(l->GetBreakpoint())
				TextOut(hdc, 0, (i)*_FH, "+", 1);
			sprintf(txt, txtfmt, i+1);			
			if(l->GetSpoiler())
				TextOut(hdc, (_LW)*_FW-(_FW/2), (i)*_FH, ">", 1);
			TextOut(hdc, (1)*_FW, (i)*_FH, txt, _LW-2);

			TextOut(hdc, (_LW+1)*_FW, (i)*_FH, (LPCSTR)(GetLine(i)->GetBuf()), GetLine(i)->GetSize());

		}
		EndPaint(_hWnd, &ps);
	}
	INT Create(HINSTANCE hInstance) {
		_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
			g_szClassNameCodeViewer,
			"",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
			NULL, NULL, hInstance, 0);

		if(_hWnd == NULL)
		{
			MessageBox(NULL, "Window Creation Failed!", "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			return -1;
		}

		ShowWindow(_hWnd, SW_SHOW);
		UpdateWindow(_hWnd);

		_FH = 16;
		_FW = 0;
		_hF = CreateFont(_FH,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Consolas");
		_hFI = CreateFont(_FH,0,0,0,FW_DONTCARE,TRUE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Consolas");
		_Lines = 0;
		_LW = 5;

		SetWindowLongPtr(_hWnd,GWLP_USERDATA, LONG_PTR(this));
		AddNewLine("some text", 9);
		AddNewLine("some text", 9);
		AddNewLine("some text", 9);
		AddNewLine("some text", 9);
		GetLine(0)->AddBegin("First line begining ", 20);
		GetLine(1)->AddEnd(" Second line ending", 19);
		GetLine(2)->Add(4, "[4]", 3);
		GetLine(2)->Add(8, "[8]", 3);
		GetLine(3)->Add(8, "[8]", 3);

		return 0;
	}
	VOID Destroy() {
		DeleteObject(_hF);
		DeleteObject(_hFI);
		DestroyWindow(_hWnd);
	}
	VOID Msg() {
		MessageBox(0, "Text", "s", MB_OK);
	}
};

class cGTEProject {
private:
	HWND _hWnd,
	     _hWndTabs;
	cGTECodeViewer *_CV;
public:
};

/*
MSGS
https://msdn.microsoft.com/en-us/library/windows/desktop/ms644927(v=vs.85).aspx

SCROLL BaRS
https://msdn.microsoft.com/en-us/library/windows/desktop/hh298376(v=vs.85).aspx
*/

LRESULT CALLBACK WndCodeViewerProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	cGTECodeViewer *p = (cGTECodeViewer*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	switch(msg)	{
		case WM_PAINT:
			if(p)
				p->Render();
			return 0;
		break;
		case WM_CLOSE:
			if(p)
				p->Destroy();
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

INT rGTEInit(HINSTANCE hInstance) {	

	WNDCLASSEX wc;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc   = WndCodeViewerProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassNameCodeViewer;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	return 0;
};

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	AllocConsole();
	if(rGTEInit(hInstance)) return -1;

	cGTECodeViewer clCV;
	if(clCV.Create(hInstance)) return -1;

	MSG Msg;
	while(GetMessage(&Msg, NULL, 0, 0))	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}