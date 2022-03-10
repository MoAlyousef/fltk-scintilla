#![allow(dead_code)]

use fltk::{enums::*, prelude::*, *};
use fltk_scintilla::Scintilla;
use scintilla_sys::*;
use std::mem::transmute;
use std::os::raw::c_void;

// must be null terminated
const KEYWORDS: &str = "let as break continue fn crate else enum for false if impl trait loop in match mod move mut struct type unsafe use where while\0";
const TYPES: &str = "i8 u8 i16 u16 i32 u32 i64 u64 bool isize usize\0";
const SAMPLE: &str = "
// This is a simple main function
fn main() {
    let x: i32 = 5;
    let y: i32 = 6;
    let z = x + y;
}\0";

const SCLEX_RUST: usize = 111;
const SCE_RUST_DEFAULT: usize = 0;
const SCE_RUST_COMMENTBLOCK: usize = 1;
const SCE_RUST_COMMENTLINE: usize = 2;
const SCE_RUST_COMMENTBLOCKDOC: usize = 3;
const SCE_RUST_COMMENTLINEDOC: usize = 4;
const SCE_RUST_NUMBER: usize = 5;
const SCE_RUST_WORD: usize = 6;
const SCE_RUST_WORD2: usize = 7;
const SCE_RUST_WORD3: usize = 8;
const SCE_RUST_WORD4: usize = 9;
const SCE_RUST_WORD5: usize = 10;
const SCE_RUST_WORD6: usize = 11;
const SCE_RUST_WORD7: usize = 12;
const SCE_RUST_STRING: usize = 13;
const SCE_RUST_STRINGR: usize = 14;
const SCE_RUST_CHARACTER: usize = 15;
const SCE_RUST_OPERATOR: usize = 16;
const SCE_RUST_IDENTIFIER: usize = 17;
const SCE_RUST_LIFETIME: usize = 18;
const SCE_RUST_MACRO: usize = 19;
const SCE_RUST_LEXERROR: usize = 20;
const SCE_RUST_BYTESTRING: usize = 21;
const SCE_RUST_BYTESTRINGR: usize = 22;
const SCE_RUST_BYTECHARACTER: usize = 23;
const MARGIN_FOLD_INDEX: usize = 3;

unsafe fn sc_cb(notif: &SCNotification, data: *mut c_void) {
    let editor: &mut Scintilla = transmute(data);
    if notif.nmhdr.code == SCN_MARGINCLICK {
        let line_number = editor.send_editor_raw(SCI_LINEFROMPOSITION, notif.position as _, 0);
        editor.send_editor_raw(SCI_TOGGLEFOLD, line_number as _, 0);
    }
}

fn main() {
    let a = app::App::default().with_scheme(app::Scheme::Gtk);

    let mut win = window::Window::default().with_size(640, 480);
    let mut sc = Scintilla::new(5, 5, 630, 470, None);
    win.end();
    win.show();
    win.wait_for_expose();

    sc.handle(|sc, ev| match ev {
        Event::KeyDown => {
            if let Some(ch) = app::event_text().chars().next() {
                if app::compose().is_some() {
                    unsafe {
                        let len = fltk::utils::char_len(ch);
                        let s = std::ffi::CString::new(ch.to_string()).unwrap();
                        sc.send_editor_raw(
                            SCI_ADDTEXT,
                            if cfg!(target_env = "msvc") {
                                len - 1
                            } else {
                                len
                            },
                            transmute(s.into_raw()),
                        );
                    }
                    true
                } else {
                    false
                }
            } else {
                false
            }
        }
        _ => false,
    });

    unsafe {
        sc.set_notify_raw(sc_cb, transmute(&mut sc.clone()));
        sc.send_editor_raw(SCI_CLEARALL, 0, 0);
        sc.send_editor_raw(SCI_APPENDTEXT, SAMPLE.len() -1, transmute(SAMPLE.as_ptr()));

        sc.send_editor_raw(SCI_SETMARGINTYPEN, 2, SC_MARGIN_NUMBER as _);
        sc.send_editor_raw(SCI_SETMARGINWIDTHN, 2, 20);

        sc.send_editor_raw(
            SCI_STYLESETFONT,
            STYLE_DEFAULT as _,
            transmute("Courier New\0".as_ptr()),
        );
        sc.send_editor_raw(SCI_STYLESETSIZE, STYLE_DEFAULT as _, 10);
        sc.send_editor_raw(SCI_STYLECLEARALL, 0, 0);

        sc.send_editor_raw(SCI_CALLTIPUSESTYLE, 0, 0);
        sc.send_editor_raw(
            SCI_STYLESETFONT,
            STYLE_CALLTIP as _,
            transmute("Courier New\0".as_ptr()),
        );
        sc.send_editor_raw(SCI_STYLESETSIZE, STYLE_CALLTIP as _, 9);

        sc.send_editor_raw(SCI_SETLEXER, SCLEX_RUST, 0);
        sc.send_editor_raw(SCI_SETKEYWORDS, 0, transmute(KEYWORDS.as_ptr()));
        sc.send_editor_raw(SCI_SETKEYWORDS, 1, transmute(TYPES.as_ptr()));

        sc.send_editor_raw(SCI_STYLESETFORE, SCE_RUST_WORD, 0x00FF0000);
        sc.send_editor_raw(SCI_STYLESETFORE, SCE_RUST_WORD2, 0x00800080);
        sc.send_editor_raw(SCI_STYLESETBOLD, SCE_RUST_WORD2, 1);
        sc.send_editor_raw(SCI_STYLESETFORE, SCE_RUST_STRING, 0x001515A3);
        sc.send_editor_raw(SCI_STYLESETFORE, SCE_RUST_CHARACTER, 0x001515A3);
        sc.send_editor_raw(SCI_STYLESETFORE, SCE_RUST_IDENTIFIER, 0x00808080);
        sc.send_editor_raw(SCI_STYLESETFORE, SCE_RUST_COMMENTLINE, 0x00008000);

        sc.send_editor_raw(SCI_SETCARETLINEVISIBLE, 1, 0);
        sc.send_editor_raw(SCI_SETCARETLINEBACK, 0xb0ffff, 0);
        sc.send_editor_raw(SCI_SETMARGINTYPEN, 0, SC_MARGIN_SYMBOL as _);
        sc.send_editor_raw(SCI_SETMARGINWIDTHN, 0, 9);
        sc.send_editor_raw(SCI_SETMARGINMASKN, 0, 0x01);

        sc.send_editor_raw(SCI_SETMARGINTYPEN, 1, SC_MARGIN_SYMBOL as _);
        sc.send_editor_raw(SCI_SETMARGINWIDTHN, 1, 9);
        sc.send_editor_raw(SCI_SETMARGINMASKN, 1, 0x06);

        sc.send_editor_raw(SCI_SETMARGINTYPEN, 2, SC_MARGIN_NUMBER as _);
        sc.send_editor_raw(SCI_SETMARGINWIDTHN, 2, 20);

        sc.send_editor_raw(SCI_MARKERSETFORE, 0, 0x0000ff);
        sc.send_editor_raw(SCI_MARKERSETFORE, 1, 0x00ff00);
        sc.send_editor_raw(SCI_MARKERSETFORE, 2, 0xff0000);

        sc.send_editor_raw(
            SCI_SETPROPERTY,
            transmute("fold\0".as_ptr()),
            transmute("1\0".as_ptr()),
        );
        sc.send_editor_raw(
            SCI_SETMARGINTYPEN,
            MARGIN_FOLD_INDEX as _,
            SC_MARGIN_SYMBOL as _,
        );
        sc.send_editor_raw(
            SCI_SETMARGINMASKN,
            MARGIN_FOLD_INDEX as _,
            SC_MASK_FOLDERS as _,
        );
        sc.send_editor_raw(SCI_SETMARGINWIDTHN, MARGIN_FOLD_INDEX as _, 11);
        sc.send_editor_raw(SCI_SETMARGINSENSITIVEN, MARGIN_FOLD_INDEX as _, true as _);

        sc.send_editor_raw(
            SCI_MARKERDEFINE,
            SC_MARKNUM_FOLDER as _,
            SC_MARK_CIRCLEPLUS as _,
        );
        sc.send_editor_raw(
            SCI_MARKERDEFINE,
            SC_MARKNUM_FOLDEROPEN as _,
            SC_MARK_ARROW as _,
        );
        sc.send_editor_raw(
            SCI_MARKERDEFINE,
            SC_MARKNUM_FOLDEREND as _,
            SC_MARK_CIRCLEPLUSCONNECTED as _,
        );
        sc.send_editor_raw(
            SCI_MARKERDEFINE,
            SC_MARKNUM_FOLDEROPENMID as _,
            SC_MARK_CIRCLEMINUSCONNECTED as _,
        );
        sc.send_editor_raw(
            SCI_MARKERDEFINE,
            SC_MARKNUM_FOLDERMIDTAIL as _,
            SC_MARK_TCORNERCURVE as _,
        );
        sc.send_editor_raw(
            SCI_MARKERDEFINE,
            SC_MARKNUM_FOLDERSUB as _,
            SC_MARK_VLINE as _,
        );
        sc.send_editor_raw(
            SCI_MARKERDEFINE,
            SC_MARKNUM_FOLDERTAIL as _,
            SC_MARK_LCORNERCURVE as _,
        );

        sc.send_editor_raw(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERSUB as _, 0xa0a0a0);
        sc.send_editor_raw(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERMIDTAIL as _, 0xa0a0a0);
        sc.send_editor_raw(SCI_MARKERSETBACK, SC_MARKNUM_FOLDERTAIL as _, 0xa0a0a0);
        sc.send_editor_raw(SCI_SETFOLDFLAGS, 16 | 4, 0);
    }

    a.run().unwrap();
}

