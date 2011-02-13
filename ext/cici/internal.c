// Cici::Internal
#include <def.h>
#include <runtime.h>
#include <elem.h>

// generate an id
static HMENU new_id() {
	static int id = 1;
	return (HMENU)(id ++);
}

// set global lock
// when true, events are stopped
// when false, events can be fired
DECL1(set_event_lock, v_pred) {
	runtime.event_lock = RTEST(v_pred) ? 1 : 0;
	return v_pred;
}

// quit program
DECL1(quit, n) {
	PostQuitMessage(NUM2INT(n));
	return Qnil;
}

// main window is created? true : false
DECL0(inited) {
	return runtime.main_win ? Qtrue : Qfalse;
}

// create top level window
DECL6(new_top_window, v_elem, v_title, v_xstyle, v_style, v_cx, v_cy) {
	// prepare
	VALUE v_window_class;
	char* window_class;
	char* title = StringValueCStr(v_title);
	int xstyle = FIX2INT(v_xstyle);
	int style = FIX2INT(v_style) | WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	int cx = FIX2INT(v_cx);
	int cy = FIX2INT(v_cy);
	Elem* elem;
	Data_Get_Struct(v_elem, Elem, elem);
	// TODO validate elem
	v_window_class = rb_funcall(v_elem, rb_intern("window_class"), 0);
	window_class = StringValueCStr(v_window_class);
	// create
	elem->h = CreateWindowExA(xstyle, window_class, title, style,
			100, 100, cx, cy,
			0, 0, runtime.module, 0);
	if (!elem->h)
		rb_raise(rb_eRuntimeError, "failed to create window");
	// change runtime
	runtime.main_win = elem->h;
	SetWindowLong(elem->h, GWL_USERDATA, (long)elem);
	elem->old_proc = (WNDPROC)SetWindowLong(elem->h, GWL_WNDPROC, (long)cici_event_proc);
	return Qnil;
}

// create child window
DECL6(new_child_window, v_elem, v_parent, v_xstyle, v_style, v_cx, v_cy) {
	VALUE v_window_class;
	char* window_class;
	int xstyle = FIX2INT(v_xstyle);
	int style = FIX2INT(v_style) | WS_CHILD | WS_VISIBLE;
	int cx = FIX2INT(v_cx);
	int cy = FIX2INT(v_cy);
	HWND parent = (HWND)(NUM2INT(v_parent));
	Elem* elem;
	Data_Get_Struct(v_elem, Elem, elem);
	// TODO validate elem
	v_window_class = rb_funcall(v_elem, rb_intern("window_class"), 0);
	window_class = StringValueCStr(v_window_class);
	elem->h = CreateWindowExA(xstyle, window_class, 0, style,
			CW_USEDEFAULT, 0, cx, cy,
			parent, new_id(), runtime.module, 0);
	SetWindowLong(elem->h, GWL_USERDATA, (long)elem);
	elem->old_proc = (WNDPROC)SetWindowLong(elem->h, GWL_WNDPROC, (long)cici_event_proc);
	return Qnil;
}

// message loop
DECL0(msg_loop) {
	MSG msg;
	int key_status;
	int is_accel;
	while (GetMessage(&msg, NULL, 0, 0)) {
		is_accel = 0;
		if ((msg.message == WM_KEYDOWN) || (msg.message == WM_SYSKEYDOWN)) {
			key_status = ((GetKeyState(VK_SHIFT) < 0) ? 1 : 0);
			if (GetKeyState(VK_CONTROL) < 0)
				key_status |= 2;
			if (GetKeyState(VK_MENU) < 0)
				key_status |= 4;
			// is_accel = parse_accel(key_status, msg.wParam);
		}
		if (!is_accel) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return Qnil;
}

// define the module
void init_internal(VALUE cici) {
	VALUE c = rb_define_module_under(cici, "Internal");
	runtime.internal = c;
	MODULE_DEF(c, "set_event_lock", set_event_lock, 1);
	MODULE_DEF(c, "new_top_window", new_top_window, 6);
	MODULE_DEF(c, "new_child_window", new_child_window, 6);
	MODULE_DEF(c, "msg_loop", msg_loop, 0);
	MODULE_DEF(c, "inited?", inited, 0);
	MODULE_DEF(c, "quit", quit, 1);
}

