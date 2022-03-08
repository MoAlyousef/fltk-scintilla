use fltk_build::fltk_out_dir;
use glob::glob;
use std::path::PathBuf;
use std::env;

fn main() {
    let manifest_dir = PathBuf::from(env::var("CARGO_MANIFEST_DIR").unwrap());
    let target_triple = env::var("TARGET").unwrap();
    let fltk_out_dir = fltk_out_dir().unwrap();
    let mut v: Vec<PathBuf> = vec![];
    let lexers = glob("Fl_Scintilla/lexers/*.cxx").unwrap();
    let lexlib = glob("Fl_Scintilla/lexlib/*.cxx").unwrap();
    let srcs = glob("Fl_Scintilla/src/*.cxx").unwrap();
    let srcs2 = glob("Fl_Scintilla/fltk/*.cxx").unwrap();
    for entry in lexers {
        v.push(entry.unwrap());
    }
    for entry in lexlib {
        v.push(entry.unwrap());
    }
    for entry in srcs {
        v.push(entry.unwrap());
    }
    for entry in srcs2 {
        v.push(entry.unwrap());
    }
    v.push(PathBuf::from("src/cfl_scintilla.cpp"));

    let mut target = cc::Build::new();
    target
        .files(v)
        .cpp(true)
        .define("SCI_LEXER", None)
        .define("SCI_NAMESPACE", None)
        .include("Fl_Scintilla/src")
        .include("Fl_Scintilla/include")
        .include("Fl_Scintilla/lexers")
        .include("Fl_Scintilla/lexlib")
        .include("Fl_Scintalla/fltk")
        .include(&fltk_out_dir.join("include"))
        .flag_if_supported("-w")
        .flag_if_supported("-fno-rtti")
        .flag_if_supported("-fpermissive");

    if target_triple.contains("windows-msvc") {
        target.include("iconv");
        println!("cargo:rustc-link-search=native={}", manifest_dir.join("iconv").display());
        println!("cargo:rustc-link-lib=static=libiconvStatic");
    }

    target.compile("fl_scintilla");
}
