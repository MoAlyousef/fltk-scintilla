#ifndef __CFL_SCINTILLA_H__
#define __CFL_SCINTILLA_H__

#include <cfltk/cfl_macros.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

WIDGET_DECLARE(Fl_Scintilla)

void Fl_Scintilla_SetNotify(Fl_Scintilla *self, void (*onnotify)(void *scn, void *data), void *data);

intptr_t Fl_Scintilla_SendEditor(Fl_Scintilla *self, unsigned int iMessage, uintptr_t wParam, intptr_t lParam);

GROUP_DECLARE(Fl_Scintilla)

#ifdef __cplusplus
}
#endif
#endif