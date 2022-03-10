#include "cfl_scintilla.h"
#include "../Fl_Scintilla/fltk/Fl_Scintilla.h"

#include <cfltk/cfl_lock.h>
#include <cfltk/cfl_widget.hpp>

#include <FL/Fl_Group.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

using scn_fn = void (*)(Scintilla::SCNotification *scn, void *data);

WIDGET_CLASS(Fl_Scintilla)

WIDGET_DEFINE(Fl_Scintilla)

void Fl_Scintilla_SetNotify(Fl_Scintilla *self, void (*onnotify)(void *scn, void *data), void *data) {
    LOCK(self->SetNotify((scn_fn)onnotify, data));
}

intptr_t Fl_Scintilla_SendEditor(Fl_Scintilla *self, unsigned int iMessage, uintptr_t wParam, intptr_t lParam) {
    LOCK(auto ret = self->SendEditor(iMessage, wParam, lParam)) return ret;
}

GROUP_DEFINE(Fl_Scintilla)