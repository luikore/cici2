#pragma once
#include <ruby.h>
#include <windows.h>
#include <ruby/st.h>

typedef struct {
	VALUE v;
	HWND h;
	struct st_table* event_map;
	WNDPROC old_proc;
} Elem;

static inline HWND VALUE2HWND(VALUE v) {
	Elem* elem;
	Data_Get_Struct(v, Elem, elem);
	return elem ? elem->h : 0;
}

extern LRESULT CALLBACK cici_event_proc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK cici_top_window_proc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK cici_canvas_proc(HWND, UINT, WPARAM, LPARAM);
extern void CALLBACK cici_timer_proc(HWND, UINT, UINT, DWORD);


