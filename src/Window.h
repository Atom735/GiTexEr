#ifndef HD_WINDOW
#define HD_WINDOW

#include <cstdio>
#include <Windows.h>
#include <WindowsX.h>

#include "../res/res.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


class ciWindowMain {
private:
	HWND hWnd;
	HINSTANCE hInstance;
	HACCEL hAccel;
public:
	INT Run(char *argv) {
		MSG msg;
		BOOL bRet;
		while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
			if (bRet == -1) {
			} else {
				if (!TranslateAccelerator(hWnd, hAccel, &msg)) {
					TranslateMessage(&msg); 
					DispatchMessage(&msg); 
				}
			}
		}
		return msg.wParam;
	}
	INT pciF_Null() {
		MessageBox(0, "pciF_Null", "pciF_Null", MB_OK);
		return 0;
	}
	INT pciF_NewFile() {
		MessageBox(0, "pciF_NewFile", "pciF_NewFile", MB_OK);
		return 0;
	}
	INT pciF_OpenFile() {
		MessageBox(0, "pciF_OpenFile", "pciF_OpenFile", MB_OK);
		return 0;
	}
	INT Command(UINT);
	ciWindowMain(HINSTANCE hI) {
		hInstance = hI;

		hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCEL)); 

		hWnd = CreateWindow("CLASS MainWindow","Empty Project", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,CW_USEDEFAULT,800,600,NULL,NULL,hInstance,LPVOID(this));
		SetWindowLongPtr(hWnd, GWLP_USERDATA, LONG_PTR(this));
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);
	}
};

#define HD_WINDOW_IDM
#include "../res/idm.h"

INT ciWindowMain::Command(UINT comId) {
	return (this->*pciCommands[comId])();
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if(msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	ciWindowMain *p = (ciWindowMain*)(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if(!p) return DefWindowProc(hwnd,msg,wparam,lparam);
	if(msg == WM_COMMAND) return p->Command(wparam);
	return DefWindowProc(hwnd,msg,wparam,lparam);
}

#endif