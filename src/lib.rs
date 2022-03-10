use fltk_scintilla_sys::sys::*;

use fltk::prelude::*;
use fltk::utils::FlString;
use fltk::widget::WidgetTrackerPtr;
use std::ffi::{CStr, CString};
use scintilla_sys::SCNotification;
use std::os::raw::*;

/// Creates a flow widget
#[derive(Debug)]
pub struct Scintilla {
    inner: *mut Fl_Scintilla,
    tracker: WidgetTrackerPtr,
    is_derived: bool,
}

fltk::macros::widget::impl_widget_ext!(Scintilla, Fl_Scintilla);
fltk::macros::widget::impl_widget_base!(Scintilla, Fl_Scintilla);
fltk::macros::group::impl_group_ext!(Scintilla, Fl_Scintilla);

impl Scintilla {
    pub unsafe fn set_notify_raw(&mut self, f: unsafe fn (notif: &SCNotification, data: *mut c_void), data: *mut c_void) {
        Fl_Scintilla_SetNotify(self.inner, std::mem::transmute(Some(f)), data)
    }
    pub unsafe fn send_editor_raw(&mut self, i_message: u32, w_param: usize, l_param: isize) -> isize {
        Fl_Scintilla_SendEditor(self.inner, i_message, w_param, l_param)
    }
}