// event callbacks
#include <elem.h>
#include <runtime.h>

#define WM_TABSELECT	(WM_USER+110)
#define WM_TABUNSELECT	(WM_USER+111)

// map notify code to method
static ID notify_to_method(UINT code) {
	switch (code) {
#		include "event_maps/notify.inc"
	}
	return 0;
}

// map wm_xxx to method
static ID wm_to_method(UINT msg) {
	switch (msg) {
#		include "event_maps/wm.inc"
	}
	return 0;
}

// for rb_protect invoke
static VALUE rb_funcall_protect(VALUE params) {
	VALUE* arr = (VALUE*)params;
	rb_funcall(arr[0], (ID)arr[1], 2, arr[1], arr[2]);
	return 0;
}

// event callback for all
LRESULT CALLBACK cici_event_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_NCHITTEST) {
		// generate mouseover and mouseleave msg
		if (runtime.mouseover != hwnd) {
			if (runtime.mouseover)
				SendMessage(runtime.mouseover, WM_MOUSELEAVE, 0, 0);
			SendMessage(hwnd, WM_MOUSEHOVER, 0, 0);
			runtime.mouseover = hwnd;
			return 0;
		}
	} else if (msg == WM_TIMER) {
		// fire timer
		// NOTE there is a rescue in fire_timer
		rb_funcall(runtime.internal, rb_intern("fire_timer"), 1, INT2NUM(wparam));
	} else {
		if (runtime.event_lock) {
			// stop events when locked
			goto def_proc;
		} else {
			// figure out receiver and method
			Elem* elem = 0;
			ID method;
			if (msg == WM_NOTIFY) {
				LPNMHDR nmh = (LPNMHDR) lparam;
				// call sender's event processor
				elem = (Elem*)GetWindowLong(nmh->hwndFrom, GWL_USERDATA);
				method = notify_to_method(nmh->code);
			} else if (msg == WM_COMMAND) {
				if (!lparam)
					goto def_proc;
				elem = (Elem*)GetWindowLong((HWND)lparam, GWL_USERDATA);
				method = wm_to_method(msg);
			}else {
				elem = (Elem*)GetWindowLong(hwnd, GWL_USERDATA);
				method = wm_to_method(msg);
			}
			if (!elem)
				goto def_proc;

			// invoke ruby
			if (method && rb_respond_to(elem->v, method)) {
				int error;
				VALUE rb_params[4] = { elem->v, (VALUE)method, INT2NUM(wparam), INT2NUM(lparam) };
				rb_protect(rb_funcall_protect, (VALUE)rb_params, &error);
				if (error) {
					VALUE lasterr = rb_gv_get("$!");
					if(RTEST(lasterr)) {
						char* str;
						VALUE msg = rb_funcall(lasterr, rb_intern("to_s"), 0);
						VALUE trace = rb_funcall(lasterr, rb_intern("backtrace"), 0);
						VALUE line_break = rb_str_new2("\n");
						VALUE trace_str = rb_funcall(trace, rb_intern("join"), 1, line_break);
						rb_funcall(msg, rb_intern("<<"), 1, line_break);
						rb_funcall(msg, rb_intern("<<"), 1, trace_str);
						// transform error to filesystem style
						rb_funcall(msg, rb_intern("encode!"), 1, rb_enc_from_encoding(rb_filesystem_encoding()));
						str = StringValueCStr(msg);
						MessageBox(0, str, "!", MB_OK);
					}
				}
			}
			
			// invoke old proc
			if (elem->old_proc) {
				return CallWindowProc(elem->old_proc, hwnd, msg, wparam, lparam);
			}
		}
	}

def_proc:
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// default canvas proc, for registering class
LRESULT CALLBACK cici_canvas_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// timer callback
void CALLBACK cici_timer_proc(HWND hwnd, UINT msg, UINT timer_id, DWORD time) {
	rb_funcall(runtime.internal, rb_intern("fire_timer"), 1, INT2NUM(timer_id));
}
