#include <dirent.h>
#include <string.h>

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <unordered_set>

#include "cFileFinder.hpp"

using namespace std;

//{ "cmake",       { "CMakeLists.txt;", "cmake" } },
//{ "coffeescript",{ ".coffee" } },
//{ "cpp",         { ".cpp", "cc", "cxx", "m", "hpp", "hh", "h", "hxx" } },
//{ "csharp",      { ".cs" } },
//{ "css",         { ".css" } },
//{ "dart",        { ".dart" } },
//{ "delphi",      { ".pas", "int", "dfm", "nfm", "dof", "dpk", "dproj", "groupproj", "bdsgroup", "bdsproj" } },
//{ "elisp",       { ".el" } },
//{ "elixir",      { ".ex", "exs" } },
//{ "erlang",      { ".erl", "hrl" } },
//{ "fortran",     { ".f", "f77", "f90", "f95", "f03", "for", "ftn", "fpp" } },
//{ "go",          { ".go" } },
//{ "groovy",      { ".groovy", "gtmpl", "gpp", "grunit", "gradle" } },
//{ "haskell",     { ".hs", "lhs" } },
//{ "hh",          { ".h" } },
//{ "html",        { ".htm", "html" } },
//{ "java",        { ".java", "properties" } },
//{ "js",          { ".js" } },
//{ "json",        { ".json" } },
//{ "jsp",         { ".jsp", "jspx", "jhtm", "jhtml" } },
//{ "less",        { ".less" } },
//{ "lisp",        { ".lisp", "lsp" } },
//{ "lua",         { ".lua; first line matches /^#!.*\blua(jit)?/" } },
//{ "make",        { ".mk;", "mak; makefile; Makefile; GNUmakefile" } },
//{ "matlab",      { ".m" } },
//{ "md",          { ".mkd;", "md" } },
//{ "objc",        { ".m", "h" } },
//{ "objcpp",      { ".mm", "h" } },
//{ "ocaml",       { ".ml", "mli" } },
//{ "parrot",      { ".pir", "pasm", "pmc", "ops", "pod", "pg", "tg" } },
//{ "perl",        { ".pl", "pm", "pod", "t", "psgi; first line matches /^#!.*\bperl/" } },
//{ "perltest",    { ".t" } },
//{ "php",         { ".php", "phpt", "php3", "php4", "php5", "phtml; first line matches /^#!.*\bphp/" } },
//{ "plone",       { ".pt", "cpt", "metadata", "cpy", "py" } },
//{ "pmc",         { ".pmc" } },
//{ "python",      { ".py; first line matches /^#!.*\bpython/" } },
//{ "rake",        { "Rakefile" } },
//{ "rr",          { ".R" } },
//{ "ruby",        { ".rb", "rhtml", "rjs", "rxml", "erb", "rake", "spec; Rakefile; first line matches /^#!.*\bruby/" } },
//{ "rust",        { ".rs" } },
//{ "sass",        { ".sass", "scss" } },
//{ "scala",       { ".scala" } },
//{ "scheme",      { ".scm", "ss" } },
//{ "shell",       { ".sh", "bash", "csh", "tcsh", "ksh", "zsh", "fish; first line matches /^#!.*\b(?:ba|t?c|k|z|fi)?sh\b/" } },
//{ "smalltalk",   { ".st" } },
//{ "sql",         { ".sql", "ctl" } },
//{ "tcl",         { ".tcl", "itcl", "itk" } },
//{ "tex",         { ".tex", "cls", "sty" } },
//{ "textile",     { ".textile" } },
//{ "tt",          { ".tt", "tt2", "ttml" } },
//{ "vb",          { ".bas", "cls", "frm", "ctl", "vb", "resx" } },
//{ "verilog",     { ".v", "vh", "sv" } },
//{ "vhdl",        { ".vhd", "vhdl" } },
//{ "vim",         { ".vim" } },
//{ "xml",         { ".xml", "dtd", "xsl", "xslt", "ent; first line matches /<[?]xml/" } },
//{ "yaml",        { ".yaml", "yml" } },                                                                                            

#define ACTIONSCRIPT_TYPES  ".as", ".mxml",
#define ADA_TYPES           ".ada", ".adb", ".ads",
#define ASM_TYPES           ".asm", ".s",
#define ASP_TYPES           ".asp",
#define ASPX_TYPES          ".master", ".ascx", ".asmx", ".aspx", ".svc",
#define BATCH_TYPES         ".bat", ".cmd",
#define CC_TYPES            ".c", ".h", ".xs",
#define CFMX_TYPES          ".cfc", ".cfm", ".cfml",
#define CLOJURE_TYPES       ".clj",
#define CMAKE_TYPES         "CMakeLists.txt", ".cmake",
#define COFFEE_TYPES        ".coffee",
#define CPP__TYPES          ".cpp", ".cc", ".cxx", ".m", ".hpp", ".hh", ".h", ".hxx",
#define CS_TYPES            ".cs",
#define CSS_TYPES           ".css",
#define DART_TYPES          ".dart",
#define PAS_TYPES           ".pas", ".int", ".dfm", ".nfm", ".dof", ".dpk", ".dproj", ".groupproj", ".bdsgroup", ".bdsproj",
#define EL_TYPES            ".el",
#define EX_TYPES            ".ex", ".exs",
#define ERL_TYPES           ".erl", ".hrl",
#define F_TYPES             ".f", ".f77", ".f90", ".f95", ".f03", ".for", ".ftn", ".fpp",
#define GO_TYPES            ".go",
#define GROOVY_TYPES        ".groovy", ".gtmpl", ".gpp", ".grunit", ".gradle",
#define HS_TYPES            ".hs", ".lhs",
#define H_TYPES             ".h",
#define HTM_TYPES           ".htm", ".html",
#define JAVA_TYPES          ".java", ".properties",
#define JS_TYPES            ".js",
#define JSON_TYPES          ".json",
#define JSP_TYPES           ".jsp", ".jspx", ".jhtm", ".jhtml",
#define LESS_TYPES          ".less",
#define LISP_TYPES          ".lisp", ".lsp",
#define LUA_TYPES           ".lua", 
#define MAKE_TYPES            ".mk", "mak", "makefile", "Makefile", "GNUmakefile",
#define MATLAB_TYPES             ".m",
#define MKD_TYPES           ".mkd", ".md",
#define M_TYPES             ".m", ".h",
#define MM_TYPES            ".mm", ".h",
#define ML_TYPES            ".ml", ".mli",
#define PIR_TYPES           ".pir", ".pasm", ".pmc", ".ops", ".pod", ".pg", ".tg",
#define PL_TYPES            ".pl", ".pm", ".pod", ".t", ".psgi",
#define T_TYPES             ".t",
#define PHP_TYPES           ".php", ".phpt", ".php3", ".php4", ".php5", ".phtml",
#define PT_TYPES            ".pt", ".cpt", ".metadata", ".cpy", ".py",
#define PMC_TYPES           ".pmc",
#define PY_TYPES            ".py", 
#define RAKE_TYPES          "Rakefile",
#define R_TYPES             ".R",
#define RB_TYPES            ".rb", ".rhtml", ".rjs", ".rxml", ".erb", ".rake",
#define RS_TYPES            ".rs",
#define SASS_TYPES          ".sass", "scss",
#define SCALA_TYPES         ".scala",
#define SCM_TYPES           ".scm", ".ss",
#define SH_TYPES            ".sh", ".bash", ".csh", ".tcsh", ".ksh", ".zsh", ".fish",
#define ST_TYPES            ".st",
#define SQL_TYPES           ".sql", ".ctl",
#define TCL_TYPES           ".tcl", ".itcl", ".itk",
#define TEX_TYPES           ".tex", ".cls", ".sty",
#define TEXTILE_TYPES       ".textile",
#define TT_TYPES            ".tt", ".tt2", ".ttml",
#define BAS_TYPES           ".bas", ".cls", ".frm", ".ctl", ".vb", ".resx",
#define V_TYPES             ".v", ".vh", ".sv",
#define VHD_TYPES           ".vhd", ".vhdl",
#define VIM_TYPES           ".vim",
#define XML_TYPES           ".xml", ".dtd", ".xsl", ".xslt", ".ent", 
#define YAML_TYPES          ".yaml", "yml",                                                                                            

std::unordered_set< std::string > f_allKnownTypes =
{
    ACTIONSCRIPT_TYPES
    ADA_TYPES
    ASM_TYPES
    ASP_TYPES
    ASPX_TYPES
    BATCH_TYPES
    CC_TYPES
    CLOJURE_TYPES
    CMAKE_TYPES         
    COFFEE_TYPES        
    CPP__TYPES          
    CS_TYPES            
    CSS_TYPES           
    DART_TYPES          
    PAS_TYPES           
    EL_TYPES            
    EX_TYPES            
    ERL_TYPES           
    F_TYPES             
    GO_TYPES            
    GROOVY_TYPES        
    HS_TYPES            
    H_TYPES             
    HTM_TYPES           
    JAVA_TYPES          
    JS_TYPES            
    JSON_TYPES          
    JSP_TYPES           
    LESS_TYPES          
    LISP_TYPES          
    LUA_TYPES           
    MAKE_TYPES            
    MATLAB_TYPES             
    MKD_TYPES           
    M_TYPES             
    MM_TYPES            
    ML_TYPES            
    PIR_TYPES           
    PL_TYPES            
    T_TYPES             
    PHP_TYPES           
    PT_TYPES            
    PMC_TYPES           
    PY_TYPES            
    RAKE_TYPES          
    R_TYPES             
    RB_TYPES            
    RS_TYPES            
    SASS_TYPES          
    SCALA_TYPES         
    SCM_TYPES           
    SH_TYPES            
    ST_TYPES            
    SQL_TYPES           
    TCL_TYPES           
    TEX_TYPES           
    TEXTILE_TYPES       
    TT_TYPES            
    BAS_TYPES           
    V_TYPES             
    VHD_TYPES           
    VIM_TYPES           
    XML_TYPES           
    YAML_TYPES          
};

std::map< std::string, std::unordered_set< std::string > > f_fileFilter =
{
    { "actionscript",   { ACTIONSCRIPT_TYPES } },
    { "ada",            { ADA_TYPES } },
    { "asm",            { ASM_TYPES } },
    { "asp",            { ASP_TYPES } },
    { "aspx",           { ASPX_TYPES } },
    { "batch",          { BATCH_TYPES } },
    { "cc",             { CC_TYPES } },
    { "cfmx",           { CFMX_TYPES } },
    { "clojure",        { CLOJURE_TYPES } },
    { "cmake",          { CMAKE_TYPES       } },
    { "coffeescript",   { COFFEE_TYPES      } },    
    { "cpp",            { CPP__TYPES        } },    
    { "csharp",         { CS_TYPES          } },    
    { "css",            { CSS_TYPES         } },    
    { "dart",           { DART_TYPES        } },    
    { "delphi",         { PAS_TYPES         } },    
    { "elisp",          { EL_TYPES          } },    
    { "elixir",         { EX_TYPES          } },    
    { "erlang",         { ERL_TYPES         } },    
    { "fortran",        { F_TYPES           } },    
    { "go",             { GO_TYPES          } },    
    { "groovy",         { GROOVY_TYPES      } },    
    { "haskell",        { HS_TYPES          } },    
    { "hh",             { H_TYPES           } },    
    { "html",           { HTM_TYPES         } },    
    { "java",           { JAVA_TYPES        } },    
    { "js",             { JS_TYPES          } },    
    { "json",           { JSON_TYPES        } },    
    { "jsp",            { JSP_TYPES         } },    
    { "less",           { LESS_TYPES        } },    
    { "lisp",           { LISP_TYPES        } },    
    { "lua",            { LUA_TYPES         } },    
    { "make",           { MAKE_TYPES        } },    
    { "matlab",         { MATLAB_TYPES      } },    
    { "md",             { MKD_TYPES         } },    
    { "objc",           { M_TYPES           } },    
    { "objcpp",         { MM_TYPES          } },    
    { "ocaml",          { ML_TYPES          } },    
    { "parrot",         { PIR_TYPES         } },    
    { "perl",           { PL_TYPES          } },    
    { "perltest",       { T_TYPES           } },    
    { "php",            { PHP_TYPES         } },    
    { "plone",          { PT_TYPES          } },    
    { "pmc",            { PMC_TYPES         } },    
    { "python",         { PY_TYPES          } },    
    { "rake",           { RAKE_TYPES        } },    
    { "rr",             { R_TYPES           } },    
    { "ruby",           { RB_TYPES          } },    
    { "rust",           { RS_TYPES          } },    
    { "sass",           { SASS_TYPES        } },    
    { "scala",          { SCALA_TYPES       } },    
    { "scheme",         { SCM_TYPES         } },    
    { "shell",          { SH_TYPES          } },    
    { "smalltalk",      { ST_TYPES          } },    
    { "sql",            { SQL_TYPES         } },    
    { "tcl",            { TCL_TYPES         } },    
    { "tex",            { TEX_TYPES         } },    
    { "textile",        { TEXTILE_TYPES     } },    
    { "tt",             { TT_TYPES          } },    
    { "vb",             { BAS_TYPES         } },    
    { "verilog",        { V_TYPES           } },    
    { "vhdl",           { VHD_TYPES         } },    
    { "vim",            { VIM_TYPES         } },    
    { "xml",            { XML_TYPES         } },    
    { "yaml",           { YAML_TYPES        } },    
};

void cFileFinder::exploreDirectory(
    int                     workerThreads,
    std::string             root,
    std::string             filter_string,
    iQueue< sSearchEntry > *list
    )
{
    auto filter = ( filter_string != "" ) ?
        f_fileFilter[ filter_string ] :
        f_allKnownTypes;

    std::queue< std::string >   toExplore;

    toExplore.push( root );

    int count = 0;

    while ( toExplore.size() > 0 )
    {
        auto to_explore = toExplore.front();
        toExplore.pop();

        auto dirp = opendir( to_explore.c_str() );

        struct dirent * entry;
        while ( ( entry = readdir( dirp ) ) != nullptr )
        {
            auto name = entry->d_name;
            auto name_len = entry->d_reclen;

            if ( entry->d_type == DT_DIR )
            {
                if ( strcmp( name, "." ) == 0
                    || strcmp( name, ".." ) == 0 )
                {
                    continue;
                }

                if ( strncmp( name, ".", 1 ) == 0 )
                {
                    // Ignore all hidden directories.

                    continue;
                }

                string to_add;
                if ( to_explore != "." )
                {
                    to_add += to_explore + "/";
                }

                to_add += string( name );

                toExplore.push( to_add );

//                cout << "### path : " << to_add << endl;
            }
            else
            {
                if ( strncmp( name, ".", 1 ) == 0 )
                {
                    // Ignore all hidden files.

                    continue;
                }

                int allow = 0;

                const char *curr = name;
                const char *ext = nullptr;
                while ( curr = strstr( curr, "." ) )
                {
                    ext = curr;
                    curr ++;
                }

                if ( ext == nullptr )
                {
                    continue;
                }

                if (   filter.find( ext ) != filter.end()
                    || filter.find( name ) != filter.end ()
                )
                {
                    allow = 1;
                }

                if ( !allow )
                {
//                    cout << "to not add  ## " << name << endl;

                    continue;
                }

                string to_add;
                if ( to_explore != "." )
                {
                    to_add += to_explore + "/";
                }

                to_add += string( name );

                sSearchEntry se( sSearchEntry::Msg::Search, to_add );

                list->push( se );

//                cout << to_add << endl;

                count ++;
            }
        }

        closedir( dirp );
    }

    cout << "######  files to process " << count << endl;

    for ( auto i = 0; i < workerThreads; i ++ )
    {
        string empty;
        sSearchEntry se( sSearchEntry::Msg::Done, empty );

        list->push( se );
    }
}

