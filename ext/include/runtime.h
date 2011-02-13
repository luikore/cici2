#pragma once
#include <windows.h>
#include <ruby.h>

typedef struct {
	HINSTANCE module;
	HWND main_win;
	VALUE internal; // Cici::Internal for C/Ruby comunication
	HWND mouseover;
	int event_lock; // global lock when binding
} Runtime;

extern Runtime runtime;
