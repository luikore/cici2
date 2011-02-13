// main program and entry point for ext.so
#include <runtime.h>
#include <elem.h>
#include <def.h>

Runtime runtime;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)	{
	case DLL_PROCESS_ATTACH:
		runtime.module = (HINSTANCE)hModule;
		runtime.main_win = 0;
		runtime.internal = 0;
		runtime.event_lock = 0;
		runtime.mouseover = 0;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern void init_internal(VALUE);
extern void init_elem(VALUE);
extern void init_dialogs(VALUE);

#ifdef __cplusplus
extern "C"
#endif
void __declspec(dllexport) Init_ext() {
	VALUE cici = rb_define_module("Cici");

	// every item of wcex should be valued
	// some preserved values:
	//     lpszMenuName = MAKEINTRESOURCE(IDC_FRAME);
	//     hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	WNDCLASSEX wcex;

	// top level klass
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= cici_canvas_proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= runtime.module;
	// TODO other icon and cursor
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= 0;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= "canvas";
	wcex.hIconSm		= 0;
	RegisterClassEx(&wcex);

	init_internal(cici);
	init_elem(cici);
	init_dialogs(cici);
}

