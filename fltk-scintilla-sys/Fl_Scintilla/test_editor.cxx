// Copyright 2015-2016 by cyantree <cyantree.guo@gmail.com>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "Scintilla.h"
#include "SciLexer.h"
#include "ILexer.h"

#include "fltk/Fl_Scintilla.h"

#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/x.H> // for fl_open_callback
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <FL/filename.H>
//#include <FL/Fl_ComboBox.h>

//using namespace Scintilla;

Fl_Scintilla *editor;
Fl_Button *btn1;
void cb_btn(Fl_Widget *w, void *x)
{
	//if ( btn1->visible() ) btn1->hide();
	//else btn1->show();

	Fl_Native_File_Chooser *fnfc = new Fl_Native_File_Chooser();
	fnfc->type(Fl_Native_File_Chooser::BROWSE_FILE);
	if (fnfc->show()) {
		delete fnfc;
		return;
	}

	const char *fn = fnfc->filename();
	char filename[512];
	strcpy(filename, fn);
	//fl_utf8to_mb(fn, strlen(fn), filename, 512);
	delete fnfc;

	FILE *fp = fl_fopen(filename, "rb");
	if ( fp == NULL ) return;
	unsigned char buf[1024];
	int len;
	int all=0;
	while (1) {
		len = fread(buf, 1, 1024, fp);
		if ( len <= 0 ) break;
		all += len;
	}

	fclose(fp);

	editor->SendEditor(SCI_CLEARALL);
	fp = fl_fopen(filename, "rb");
	if ( fp == NULL ) return;
	unsigned char *b = (unsigned char *)malloc(all);
	len = fread(b, 1, all, fp);
	editor->SendEditor(SCI_APPENDTEXT, len, (sptr_t)b);
	fclose(fp);
	free(b);
}

#define MARGIN_FOLD_INDEX 3

static const char *minus_xpm[] = {
	/* width height num_colors chars_per_pixel */
	"     9     9       16            1",
	/* colors */
	"` c #8c96ac",
	". c #c4d0da",
	"# c #daecf4",
	"a c #ccdeec",
	"b c #eceef4",
	"c c #e0e5eb",
	"d c #a7b7c7",
	"e c #e4ecf0",
	"f c #d0d8e2",
	"g c #b7c5d4",
	"h c #fafdfc",
	"i c #b4becc",
	"j c #d1e6f1",
	"k c #e4f2fb",
	"l c #ecf6fc",
	"m c #d4dfe7",
	/* pixels */
	"hbc.i.cbh",
	"bffeheffb",
	"mfllkllfm",
	"gjkkkkkji",
	"da`````jd",
	"ga#j##jai",
	"f.k##k#.a",
	"#..jkj..#",
	"hemgdgc#h"
};
/* XPM */
static const char *plus_xpm[] = {
	/* width height num_colors chars_per_pixel */
	"     9     9       16            1",
	/* colors */
	"` c #242e44",
	". c #8ea0b5",
	"# c #b7d5e4",
	"a c #dcf2fc",
	"b c #a2b8c8",
	"c c #ccd2dc",
	"d c #b8c6d4",
	"e c #f4f4f4",
	"f c #accadc",
	"g c #798fa4",
	"h c #a4b0c0",
	"i c #cde5f0",
	"j c #bcdeec",
	"k c #ecf1f6",
	"l c #acbccc",
	"m c #fcfefc",
	/* pixels */
	"mech.hcem",
	"eldikille",
	"dlaa`akld",
	".#ii`ii#.",
	"g#`````fg",
	".fjj`jjf.",
	"lbji`ijbd",
	"khb#idlhk",
	"mkd.ghdkm"
};

//
const size_t FUNCSIZE=2;
char* g_szFuncList[FUNCSIZE]= { //?????????
	"file-",
	"MoveWindow("
};
char* g_szFuncDesc[FUNCSIZE]= { //????????????
	"HWND CreateWindow-"
	"LPCTSTR lpClassName,"
	" LPCTSTR lpWindowName,"
	" DWORD dwStyle, "
	" PVOID lpParam"
	"="
	,
	"BOOL MoveWindow-"
	"HWND hWnd,"
	" int X,"
	" int Y,"
	" int nWidth,"
	" int nHeight,"
	" BOOL bRepaint"
	"="
};

static void cb_editor(Scintilla::SCNotification *scn, void *data)
{
	Scintilla::SCNotification *notify = scn;

	if(notify->nmhdr.code == SCN_MARGINCLICK ) {
		if ( notify->margin == 3) {
			// ???????????????????????????
			const int line_number = editor->SendEditor(SCI_LINEFROMPOSITION,notify->position);
			editor->SendEditor(SCI_TOGGLEFOLD, line_number);
		}
	}

	if(notify->nmhdr.code == SCN_CHARADDED) {
		// ??????????????????
		static const char* pCallTipNextWord = NULL;//?????????????????????
		static const char* pCallTipCurDesc = NULL;//???????????????????????????
		if(notify->ch == '-') { //?????????????????????????????????????????????
			char word[1000]; //??????????????????????????????(?????????)
			Scintilla::TextRange tr;    //??????SCI_GETTEXTRANGE??????
			int pos = editor->SendEditor(SCI_GETCURRENTPOS); //???????????????????????????????????????
			int startpos = editor->SendEditor(SCI_WORDSTARTPOSITION,pos-1);//????????????????????????
			int endpos = editor->SendEditor(SCI_WORDENDPOSITION,pos-1);//????????????????????????
			tr.chrg.cpMin = startpos;  //?????????????????????????????????
			tr.chrg.cpMax = endpos;
			tr.lpstrText = word;
			editor->SendEditor(SCI_GETTEXTRANGE,0, sptr_t(&tr));

			for(size_t i=0; i<FUNCSIZE; i++) { //???????????????????????????????????????
				if(memcmp(g_szFuncList[i],word,strlen(g_szFuncList[i])) == 0) {
					printf("show all\n");
					//?????????????????????????????????
					pCallTipCurDesc = g_szFuncDesc[i]; //???????????????????????????
					editor->SendEditor(SCI_CALLTIPSHOW,pos,sptr_t(pCallTipCurDesc));//??????????????????
					const char *pStart = strchr(pCallTipCurDesc,'-')+1; //?????????????????????
					const char *pEnd = strchr(pStart,',');//???????????????????????????
					if(pEnd == NULL) pEnd = strchr(pStart,'=');//?????????????????????????????????????????????
					editor->SendEditor(SCI_CALLTIPSETHLT, pStart-pCallTipCurDesc, pEnd-pCallTipCurDesc);
					pCallTipNextWord = pEnd+1;//????????????????????????
					break;
				}
			}
		} else if(notify->ch == '=') { //?????????????????????????????????????????????
			printf("close\n");
			editor->SendEditor(SCI_CALLTIPCANCEL);
			pCallTipCurDesc = NULL;
			pCallTipNextWord = NULL;
		} else if(notify->ch == ',' && editor->SendEditor(SCI_CALLTIPACTIVE) && pCallTipCurDesc) {
			printf("show param\n");
			//??????????????????????????????????????????
			const char *pStart = pCallTipNextWord;
			const char *pEnd = strchr(pStart,',');
			if(pEnd == NULL) pEnd = strchr(pStart,'=');
			if(pEnd == NULL) //???????????????????????????????????????
				editor->SendEditor(SCI_CALLTIPCANCEL);
			else {
				printf("show param, %d %d\n", pStart-pCallTipCurDesc, pEnd-pCallTipCurDesc);
				editor->SendEditor(SCI_CALLTIPSETHLT,pStart-pCallTipCurDesc, pEnd-pCallTipCurDesc);
				pCallTipNextWord = pEnd+1;
			}
		}

		if(notify->ch == '.') {
			char word[1000]; //??????????????????????????????
			Scintilla::TextRange tr;    //??????SCI_GETTEXTRANGE??????
			int pos = editor->SendEditor(SCI_GETCURRENTPOS); //??????????????????
			int startpos = editor->SendEditor(SCI_WORDSTARTPOSITION,pos-1);//????????????????????????
			int endpos = editor->SendEditor(SCI_WORDENDPOSITION,pos-1);//????????????????????????
			tr.chrg.cpMin = startpos;  //?????????????????????????????????
			tr.chrg.cpMax = endpos;
			tr.lpstrText = word;
			editor->SendEditor(SCI_GETTEXTRANGE,0, sptr_t(&tr));
			if(strcmp(word,"file.") == 0) { //??????file.?????????file?????????????????????
				editor->SendEditor(SCI_REGISTERIMAGE, 2, sptr_t(minus_xpm));
				editor->SendEditor(SCI_REGISTERIMAGE, 5, sptr_t(plus_xpm));
				editor->SendEditor(SCI_AUTOCSHOW,0,
				                   sptr_t(
				                           "close?2 "
				                           "eof?4 "
				                           "goodjhfg "
				                           "open?5 "
				                           "rdbuf1?????? "
				                           "size "
										   "t1 "
										   "t2 "
										   "t3 "
										   "t4 "
										   "t5?5"
				                   ));
			}
		}
	}

	
}
int main(int argc, char **argv)
{
	HDC hdc = GetDC(NULL);
	if (hdc)
	{
		// Initialize the DPIManager member variable
		// This will correspond to the DPI setting
		// With all Windows OS's to date the X and Y DPI will be identical					
		int x = GetDeviceCaps(hdc, LOGPIXELSX);
		int y = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(NULL, hdc);

		printf("dpi x, y:%d %d\n", x, y);
	}

	Fl::get_system_colors();

	Fl_Window *win = new Fl_Double_Window(800, 400, 800, 500, "scintilla for fltk");

	win->color(fl_rgb_color(0, 128, 128));

	win->begin();
	editor = new Fl_Scintilla(60, 5, 700, 400);
	editor->SetNotify(cb_editor, 0);
	{
		//editor->box(FL_FLAT_BOX);
		//*
		editor->SendEditor(SCI_SETMARGINTYPEN,2, SC_MARGIN_NUMBER);
		editor->SendEditor(SCI_SETMARGINWIDTHN,2, 20);

		char *sss = "file\n";
		editor->SendEditor(SCI_APPENDTEXT, strlen(sss), (sptr_t)sss);

		const char* szKeywords1=
		        "asm auto break case catch class const "
		        "const_cast continue default delete do double "
		        "dynamic_cast else enum explicit extern false "
		        "for friend goto if inline mutable "
		        "namespace new operator private protected public "
		        "register reinterpret_cast return signed "
		        "sizeof static static_cast struct switch template "
		        "this throw true try typedef typeid typename "
		        "union unsigned using virtual volatile while";
		const char* szKeywords2=
		        "bool char float int long short void wchar_t";

		// ??????????????????
		//editor->SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT,(sptr_t)"Fixedsys");//"Courier New");
		editor->SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT,(sptr_t)"Courier New");
		editor->SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT,10);
		editor->SendEditor(SCI_STYLECLEARALL);

		editor->SendEditor(SCI_CALLTIPUSESTYLE, 0);
		editor->SendEditor(SCI_STYLESETFONT, STYLE_CALLTIP,(sptr_t)"Courier New");
		editor->SendEditor(SCI_STYLESETSIZE, STYLE_CALLTIP,9);

		//C++????????????
		editor->SendEditor(SCI_SETLEXER, SCLEX_CPP);
		editor->SendEditor(SCI_SETKEYWORDS, 0, (sptr_t)szKeywords1);//???????????????
		editor->SendEditor(SCI_SETKEYWORDS, 1, (sptr_t)szKeywords2);//???????????????

		// ????????????????????????????????????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_WORD, 0x00FF0000);   //?????????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_WORD2, 0x00800080);   //?????????
		editor->SendEditor(SCI_STYLESETBOLD, SCE_C_WORD2, 1);   //?????????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_STRING, 0x001515A3); //?????????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_CHARACTER, 0x001515A3); //??????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_PREPROCESSOR, 0x00808080);//???????????????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_COMMENT, 0x00008000);//?????????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTLINE, 0x00008000);//?????????
		editor->SendEditor(SCI_STYLESETFORE, SCE_C_COMMENTDOC, 0x00008000);//???????????????/**?????????

		editor->SendEditor(SCI_SETCARETLINEVISIBLE, 1);
		editor->SendEditor(SCI_SETCARETLINEBACK, 0xb0ffff);
		editor->SendEditor(SCI_SETMARGINTYPEN,0,SC_MARGIN_SYMBOL);
		editor->SendEditor(SCI_SETMARGINWIDTHN,0, 9);
		editor->SendEditor(SCI_SETMARGINMASKN,0, 0x01);

		// 1?????????????????????9?????????1,2?????????(0..0110B)
		editor->SendEditor(SCI_SETMARGINTYPEN,1, SC_MARGIN_SYMBOL);
		editor->SendEditor(SCI_SETMARGINWIDTHN,1, 9);
		editor->SendEditor(SCI_SETMARGINMASKN,1, 0x06);

		// 2?????????????????????20???????????????
		editor->SendEditor(SCI_SETMARGINTYPEN,2, SC_MARGIN_NUMBER);
		editor->SendEditor(SCI_SETMARGINWIDTHN,2, 20);

		// ????????????????????????
		editor->SendEditor(SCI_MARKERSETFORE,0,0x0000ff);//0-??????
		editor->SendEditor(SCI_MARKERSETFORE,1,0x00ff00);//1-??????
		editor->SendEditor(SCI_MARKERSETFORE,2,0xff0000);//2-??????

		editor->SendEditor(SCI_SETPROPERTY,(sptr_t)"fold",(sptr_t)"1");
		editor->SendEditor(SCI_SETMARGINTYPEN, MARGIN_FOLD_INDEX, SC_MARGIN_SYMBOL);//????????????
		editor->SendEditor(SCI_SETMARGINMASKN, MARGIN_FOLD_INDEX, SC_MASK_FOLDERS); //????????????
		editor->SendEditor(SCI_SETMARGINWIDTHN, MARGIN_FOLD_INDEX, 11); //????????????
		editor->SendEditor(SCI_SETMARGINSENSITIVEN, MARGIN_FOLD_INDEX, true); //??????????????????

		// ??????????????????
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_CIRCLEPLUS);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_ARROW);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND,  SC_MARK_CIRCLEPLUSCONNECTED);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_CIRCLEMINUSCONNECTED);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);
		/*
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDER, SC_MARK_PIXMAP);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPEN, SC_MARK_PIXMAP);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEREND,  SC_MARK_PIXMAP);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDEROPENMID, SC_MARK_PIXMAP);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERMIDTAIL, SC_MARK_TCORNERCURVE);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERSUB, SC_MARK_VLINE);
		editor->SendEditor(SCI_MARKERDEFINE, SC_MARKNUM_FOLDERTAIL, SC_MARK_LCORNERCURVE);
		*/

		/*
		editor->SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDER, (sptr_t)plus_xpm);
		editor->SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEROPEN, (sptr_t)plus_xpm);
		editor->SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEREND, (sptr_t)plus_xpm);
		editor->SendEditor(SCI_MARKERDEFINEPIXMAP, SC_MARKNUM_FOLDEROPENMID, (sptr_t)minus_xpm);
		*/

		// ??????????????????
		editor->SendEditor(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB, 0xa0a0a0);
		editor->SendEditor(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL, 0xa0a0a0);
		editor->SendEditor(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL, 0xa0a0a0);
		editor->SendEditor(SCI_SETFOLDFLAGS, 16|4, 0); //??????????????????????????????????????????????????????
		//*/

		editor->SendEditor(SCI_SETTABWIDTH, 4);

		//editor->SendEditor(SCI_SETCODEPAGE, SC_CP_UTF8);
		editor->SendEditor(SCI_SETCODEPAGE, 936);
		//editor->SendEditor(SCI_STYLESETCHARACTERSET, SC_CHARSET_GB2312);

		//editor->SendEditor(SCI_SETHSCROLLBAR, false);
	}

	Fl_Button *btn = new Fl_Button(100, 410, 80, 30, "Open");
	btn->callback(cb_btn);

	btn1 = new Fl_Button(0, 410, 80, 30, "1");

	win->end();
	win->resizable(win);
	win->show(argc, argv);

	editor->take_focus();

	/*
	editor->SendEditor(SCI_STYLESETFONT, STYLE_DEFAULT, LPARAM("Courier New"));
	editor->SendEditor(SCI_STYLESETSIZE, STYLE_DEFAULT, 10);

	editor->SendEditor(SCI_SETMARGINTYPEN,2, SC_MARGIN_NUMBER);
	editor->SendEditor(SCI_SETMARGINWIDTHN,2, 20);

	editor->SendEditor(SCI_APPENDTEXT, 12, (sptr_t)"hello world\n");
	editor->SendEditor(SCI_APPENDTEXT, 12, (sptr_t)"hello world\n");
	editor->SendEditor(SCI_APPENDTEXT, 12, (sptr_t)"hello world\n");
	*/



	return Fl::run();
}
