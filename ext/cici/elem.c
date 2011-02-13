#include <def.h>
#include <elem.h>
#include <runtime.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// pos methods

#define ELEMENT_RESIZE(h, cx, cy) \
	(SetWindowPos((h), 0, 0, 0, (cx), (cy), SWP_NOMOVE | SWP_NOZORDER))

#define ELEMENT_SETPOS(h, x, y) \
	(SetWindowPos((h), 0, (x), (y), 0, 0, SWP_NOSIZE | SWP_NOZORDER))

DECL0(pos) {
	HWND h = VALUE2HWND(self);
	RECT r;
	if (GetWindowRect(h, &r)) {
		POINT pt = {r.left, r.top};
		ScreenToClient(GetParent(h), &pt);
		return rb_ary_new3(2, INT2NUM(pt.x), INT2NUM(pt.y));
	} else {
		return rb_ary_new();
	}
}

DECL0(absolute_pos) {
	HWND h = VALUE2HWND(self);
	RECT r;
	if (GetWindowRect(h, &r)) {
		return rb_ary_new3(2, INT2NUM(r.left), INT2NUM(r.top));
	} else {
		return rb_ary_new();
	}
}

DECL1(pos_eq, pos_ary) {
	HWND h = VALUE2HWND(self);
	RECT r;
	if (GetWindowRect(h, &r)) {
		POINT pt = {r.left, r.top};
		VALUE vx = rb_ary_entry(pos_ary, 0);
		VALUE vy = rb_ary_entry(pos_ary, 1);

		ScreenToClient(GetParent(h), &pt);
		if (vx != Qnil)
			pt.x = NUM2INT(vx);
		if (vy != Qnil)
			pt.y = NUM2INT(vy);

		ELEMENT_SETPOS(h, pt.x, pt.y);
	}
	return pos_ary;
}

DECL1(absolute_pos_eq, pos_ary) {
	HWND h = VALUE2HWND(self);
	RECT r;
	if (GetWindowRect(h, &r)) {
		POINT pt = {r.left, r.top};
		VALUE vx = rb_ary_entry(pos_ary, 0);
		VALUE vy = rb_ary_entry(pos_ary, 1);

		if(vx != Qnil)
			pt.x = NUM2INT(vx);
		if(vy != Qnil)
			pt.y = NUM2INT(vy);
		ScreenToClient(GetParent(h), &pt);

		ELEMENT_SETPOS(h, pt.x, pt.y);
	}
	return pos_ary;
}

DECL0(size) {
	HWND h = VALUE2HWND(self);
	RECT r;
	if (GetWindowRect(h, &r)) {
		return rb_ary_new3(2, INT2NUM(r.right  - r.left), INT2NUM(r.bottom - r.top));
	} else {
		return rb_ary_new();
	}
}

DECL1(size_eq, sz_ary) {
	HWND h = VALUE2HWND(self);
	RECT r;
	if (TYPE(sz_ary) != T_ARRAY) {
		rb_raise(rb_eRuntimeError, "please give an array");
		return Qnil;
	}

	if (GetWindowRect(h, &r)) {
		VALUE vcx = rb_ary_entry(sz_ary, 0);
		VALUE vcy = rb_ary_entry(sz_ary, 1);
		int cx = (RTEST(vcx)) ? NUM2INT(vcx) : (r.right - r.left);
		int cy = (RTEST(vcy)) ? NUM2INT(vcy) : (r.bottom - r.top);
		ELEMENT_RESIZE(h, cx, cy);
	}
	return sz_ary;
}

DECL0(client_size) {
	HWND h = VALUE2HWND(self);
	RECT r;
	if (GetClientRect(h, &r)) {
		return rb_ary_new3(2, INT2NUM(r.right), INT2NUM(r.bottom));
	} else {
		return Qnil;
	}
}

DECL4(displace, x, y, cx, cy) {
	SetWindowPos(VALUE2HWND(self), 0,
		NUM2INT(x), NUM2INT(y), NUM2INT(cx), NUM2INT(cy), SWP_NOZORDER);
	return self;
}

// -----------------------------------------------------------------------------
// other methods

DECL0(h) {
	return INT2NUM((int)VALUE2HWND(self));
}

DECL0(show) {
	ShowWindow(VALUE2HWND(self), SW_SHOW);
	return self;
}

DECL0(hide) {
	ShowWindow(VALUE2HWND(self), SW_HIDE);
	return self;
}

DECL0(restore) {
	ShowWindow(VALUE2HWND(self), SW_RESTORE);
	return self;
}

DECL0(focus) {
	SetFocus(VALUE2HWND(self));
	return self;
}

DECL0(has_focus) {
	return (VALUE2HWND(self) == GetFocus()) ? Qtrue : Qfalse;
}

DECL3(send_msg, code, wp, lp) {
	return LONG2NUM(SendMessage(VALUE2HWND(self), NUM2UINT(code), NUM2LONG(wp), NUM2LONG(lp)));
}

DECL0(update) {
	UpdateWindow(VALUE2HWND(self));
	return self;
}

DECL0(text) {
	HWND h = VALUE2HWND(self);
	int len = GetWindowTextLength(h);
	if (len <= 0) {
		return rb_str_new2("");
	} else {
		// TODO test it
		VALUE vstr = rb_str_new(NULL, len);
		char* ptr = RSTRING_PTR(vstr);
		GetWindowText(h, ptr, len);
		VALUE exenc = rb_funcall(rb_cEncoding, rb_intern("default_external"), 0);
		rb_funcall(vstr, rb_intern("encode!"), 2, rb_str_new2("utf-8"), exenc);
		return vstr;
	}
}

DECL1(text_eq, txt) {
	VALUE txt2 = rb_funcall(txt, rb_intern("toString"), 0);
	SetWindowText(VALUE2HWND(self), StringValueCStr(txt2));
	return txt;
}


// return a Timer number
DECL1(set_timer, milisec) {
	static long timer_id = 1;
	Elem* elem;
	Data_Get_Struct(self, Elem, elem);
	if (!elem || !elem->h)
		return Qnil; // TODO raise
	rb_need_block();
	// add ref to timer
	rb_funcall(runtime.internal, rb_intern("register_timer"), 2, INT2NUM(timer_id), rb_block_proc());
	SetTimer(elem->h, timer_id++, NUM2INT(milisec), cici_timer_proc);
	return INT2NUM(timer_id);
}

// remove a timer of id
DECL1(kill_timer, timer_id) {
	long id = NUM2INT(timer_id);
	Elem* elem;
	Data_Get_Struct(self, Elem, elem);
	KillTimer(elem->h, id);
	return rb_funcall(runtime.internal, rb_intern("unregister_timer"), 1, timer_id);
}

// -----------------------------------------------------------------------------
// memory

static void elem_free(void* ptr) {
	Elem* elem = (Elem*) ptr;
	SetWindowLong(elem->h, GWL_USERDATA, 0);
	free(ptr);
}

// allocate elem (before creating window)
static VALUE elem_alloc(VALUE klass) {
	Elem* elem = malloc(sizeof(Elem));
	volatile VALUE events = rb_hash_new();
	elem->h = 0;
	elem->event_map = RHASH_TBL(events);
	elem->old_proc = 0;
	runtime.event_lock = 1;
    elem->v = Data_Wrap_Struct(klass, 0, elem_free, elem);
	rb_iv_set(elem->v, "@events", events);
	return elem->v;
}

//
void init_elem(VALUE cici) {
    VALUE elem = rb_define_class_under(cici, "Elem", rb_cObject);
    rb_define_alloc_func(elem, elem_alloc);
	DEF(elem, "h", h, 0);
	DEF(elem, "pos", pos, 0);
	DEF(elem, "pos=", pos_eq, 1);
	DEF(elem, "absolute_pos", absolute_pos, 0);
	DEF(elem, "absolute_pos=", absolute_pos_eq, 1);
	DEF(elem, "size", size, 0);
	DEF(elem, "size=", size_eq, 1);
	DEF(elem, "client_size", client_size, 0);
	DEF(elem, "displace", displace, 4);
	DEF(elem, "show", show, 0);
	DEF(elem, "hide", hide, 0);
	DEF(elem, "restore", restore, 0);
	DEF(elem, "text", text, 0);
	DEF(elem, "text=", text_eq, 1);
	DEF(elem, "focus", focus, 0);
	DEF(elem, "has_focus", has_focus, 0);
	DEF(elem, "send_msg", send_msg, 3);
	DEF(elem, "update", update, 0);
	DEF(elem, "set_timer", set_timer, 1);
	DEF(elem, "kill_timer", kill_timer, 1);
}

