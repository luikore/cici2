#include <def.h>
#include <elem.h>
#include <runtime.h>
#include <commdlg.h>


// -----------------------------------------------------------------------------
// pop dialogs

#define DEF_MSGBOX(method_name, msgbox_style) \
	DECL1(method_name, obj) {\
		VALUE str = rb_funcall(obj, rb_intern("toString"), 0);\
		const char* title;\
		switch(msgbox_style) {\
		case MB_OK:	title = "Alert"; break;\
		case MB_YESNO:\
		case MB_YESNOCANCEL: title = "Confirm"; break;\
		default: title = "Oops";\
		}\
		int res = MessageBox(runtime.main_win, StringValueCStr(str), title, msgbox_style | MB_ICONINFORMATION);\
		switch(res) {\
		case IDOK:\
		case IDYES:	return Qtrue;\
		case IDNO:\
		case IDABORT: return Qfalse;\
		}\
		return Qnil;\
	}

DEF_MSGBOX(alert, MB_OK);
DEF_MSGBOX(yesno, MB_YESNO);
DEF_MSGBOX(confirm, MB_YESNOCANCEL);


// -----------------------------------------------------------------------------
// file dialogs, not thread safe

static OPENFILENAME ofn;
static char file_path[MAX_PATH*4];
static char file_title[MAX_PATH*4];

// note: vfilter is split by '\0', ended with '\0\0'
DECL1(_choose_file, vfilter) {
	ofn.hwndOwner = runtime.main_win;
	ofn.lpstrFilter = StringValuePtr(vfilter);
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT;
	if(GetOpenFileName(&ofn)) {
		return rb_str_new_cstr(file_path);
	}
	return Qnil;
}

DECL1(_save_file, vfilter) {
	ofn.hwndOwner = runtime.main_win;
	ofn.lpstrFilter = StringValuePtr(vfilter);
	ofn.Flags = OFN_OVERWRITEPROMPT;
	if(GetSaveFileName(&ofn)) {
		return rb_str_new_cstr(file_path);
	}
	return Qnil;
}


// -----------------------------------------------------------------------------
// font dialog, not thread safe

static CHOOSEFONT cf;
static LOGFONT logfont;

// note: color is not used
DECL0(choose_font) {
	cf.hwndOwner = runtime.main_win;
	if(ChooseFont(&cf)) {
		VALUE arr = rb_ary_new2(4);
		int sz;
		char style[4];
		int slen = 0;
		// face
		rb_ary_push(arr, rb_str_new2(cf.lpLogFont->lfFaceName));
		// size
		sz = cf.lpLogFont->lfHeight;
		rb_ary_push(arr, INT2NUM(sz > 0 ? sz : -sz));
		// style
		if(cf.lpLogFont->lfWeight)
			style[slen++] = 'b';
		if(cf.lpLogFont->lfItalic)
			style[slen++] = 'i';
		if(cf.lpLogFont->lfStrikeOut)
			style[slen++] = 's';
		if(cf.lpLogFont->lfUnderline)
			style[slen++] = 'u';
		rb_ary_push(arr, rb_str_new(style, slen));
		// charset
		rb_ary_push(arr, INT2NUM(cf.lpLogFont->lfCharSet));
		return arr;
	}
	return Qnil;
}


// -----------------------------------------------------------------------------
// color dialog, not thread safe

static CHOOSECOLOR cc;
static COLORREF cust_colors[16];

// return [result, custom_colors[16]]
DECL0(choose_color) {
	cc.hwndOwner = runtime.main_win;
	if(ChooseColor(&cc)) {
		VALUE vcolors[16];
		int i;
		for(i = 0; i < 16; ++i) {
			vcolors[i] = INT2NUM(cc.lpCustColors[i]);
		}
		return rb_ary_new3(2, INT2NUM(cc.rgbResult), rb_ary_new4(16, vcolors));
	}
	return Qnil;
}


// -----------------------------------------------------------------------------
// init

void init_dialogs(VALUE c) {
	MODULE_DEF(c, "alert", alert, 1);
	MODULE_DEF(c, "yesno", yesno, 1);
	MODULE_DEF(c, "confirm", confirm, 1);
	MODULE_DEF(c, "_choose_file", _choose_file, 1);
	MODULE_DEF(c, "_save_file", _save_file, 1);
	MODULE_DEF(c, "choose_font", choose_font, 0);
	MODULE_DEF(c, "choose_color", choose_color, 0);

	/* static TCHAR szFilter[] = TEXT ("Text Files (*.TXT)\0*.txt\0")  \
							   TEXT ("ASCII Files (*.ASC)\0*.asc\0") \
							   TEXT ("All Files (*.*)\0*.*\0\0") ;
	*/
	// init open / save file
	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	//ofn.hwndOwner         = hwnd ;
	ofn.hInstance         = runtime.module;
	ofn.lpstrFilter       = 0; //szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 0;
	ofn.lpstrFile         = file_path;
	ofn.nMaxFile          = MAX_PATH*4;
	ofn.lpstrFileTitle    = file_title;
	ofn.nMaxFileTitle     = MAX_PATH*4;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0;             // Set in Open and Close functions
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = TEXT("txt") ;
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
	memset(file_path, 0, MAX_PATH*4);
	memset(file_title, 0, MAX_PATH*4);

	// init choose font
	cf.lStructSize    = sizeof (CHOOSEFONT);
	cf.hInstance	   = runtime.module;
	cf.hDC            = NULL;
	cf.lpLogFont      = &logfont;
	cf.iPointSize     = 0;
	cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
	cf.rgbColors      = 0;
	cf.lCustData      = 0;
	cf.lpfnHook       = NULL;
	cf.lpTemplateName = NULL;
	cf.hInstance      = NULL;
	cf.lpszStyle      = NULL;
	cf.nFontType      = 0;               // Returned from ChooseFont
	cf.nSizeMin       = 0;
	cf.nSizeMax       = 0;

	// init choose color
	cc.lStructSize    = sizeof (CHOOSECOLOR);
	// cc.hwndOwner      = NULL;
	cc.hInstance      = NULL;
	cc.rgbResult      = RGB (0x80, 0x80, 0x80);
	cc.lpCustColors   = cust_colors;
	cc.Flags          = CC_RGBINIT | CC_FULLOPEN;
	cc.lCustData      = 0;
	cc.lpfnHook       = NULL;
	cc.lpTemplateName = NULL;
}
