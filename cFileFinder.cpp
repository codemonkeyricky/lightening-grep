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

std::unordered_set< std::string > f_allKnownTypes =
{
#include "cFileFinderTypeList.hpp"
};

std::map< std::string, std::unordered_set< std::string > > f_fileFilter =
{
#include "cFileFinderFilterList.hpp"
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

