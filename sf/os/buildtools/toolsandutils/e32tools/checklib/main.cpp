// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Entry point for the checklib utility.
// 
//

#include "library/library.h" // For handling libraries of Unix "ar" format.
#include "object/object.h"   // For handling (parts of) ELF and COFF object files.

#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>


const char SELF_NAME[] = "checklib";

// Describes what kind of library it is.
enum libkind_t
{
    STDCPP, // Library should be compatible with Symbian C++.
    SYMCPP  // Library should be compatible with standard C++ (Open Environment).
};

// Describes what kind of objects are in the library: ELF or COFF.
typedef Object_factory::objkind_t objkind_t;

void _usage_and_exit(int exit_code=EXIT_FAILURE);
void _error_and_exit(const std::string&, int exit_code=EXIT_FAILURE);

// These are the ::operator new symbols we are worried about.
const char* NEW_NAMES[2][4] = {
    {"_Znwj", "_Znaj", "_ZnwjRKSt9nothrow_t", "_ZnajRKSt9nothrow_t"}, // ELF
    {"??2@YAPAXI@Z", "??_U@YAPAXI@Z", "??2@YAPAXIABUnothrow_t@std@@@Z", "??_U@YAPAXIABUnothrow_t@std@@@Z"} // COFF
};

// Checks whether the object files in the given library references any of the
// ::operator new functions.
bool _lib_ref_new(const Library&, objkind_t);


int main(int argc, const char* argv[])
{
    // Do we have any standard module for handling the command-line interface? If
    // not, see if we can start using getopt or Boost. 

    if (argc < 4)
    {
        _usage_and_exit();
    }

    // Command-line options.
    libkind_t opt_libkind;
    objkind_t opt_objkind;

    if ( std::strcmp(argv[1], "stdc++") == 0 )
    {
        opt_libkind = STDCPP;
    }
    else if ( std::strcmp(argv[1], "symc++") == 0 )
    {
        opt_libkind = SYMCPP;
    }
    else
    {
        _usage_and_exit();
    }

    if ( std::strcmp(argv[2], "--elf") == 0 )
    {
        opt_objkind = Object_factory::ELF;
    }
    else if ( std::strcmp(argv[2], "--coff") == 0 )
    {
        opt_objkind = Object_factory::COFF;
    }
    else
    {
        _usage_and_exit();
    }

    try
    {
        // Check each library that was given on the command-line.
        for (int i = 3; i < argc; i++)
        {
            Library lib( argv[i] ); // May throw std::runtime_error.

            // If the library has the tag, we know that it was built to be OE
            // compatible, and vice versa.
            bool lib_is_tagged = lib.contains_symbol("____symbian_stdcpp_mmviii");

            // Handle the two only possible error cases:

            if ( opt_libkind == STDCPP && !lib_is_tagged && _lib_ref_new(lib, opt_objkind) )
            {
                std::ostringstream err_msg;
                err_msg << "library " << argv[i] <<  " is incompatible with standard C++";

                _error_and_exit(err_msg.str());
            }
            else if ( opt_libkind == SYMCPP && lib_is_tagged && _lib_ref_new(lib, opt_objkind) )
            {
                std::ostringstream err_msg;
                err_msg << "library " << argv[i] <<  " is incompatible with Symbian C++";

                _error_and_exit(err_msg.str());
            }
        }
    }
    catch (std::runtime_error& e)
    {
        _error_and_exit( e.what() );
    }

    return 0;
}

void _error_and_exit(const std::string& a_msg, int a_exit_code)
{
    std::cerr << SELF_NAME << ": error: " << a_msg << "." << std::endl;
    std::exit(a_exit_code);
}

void _usage_and_exit(int a_exit_code)
{
    using std::cout;
    using std::endl;

    cout << "usage: " << SELF_NAME << " stdc++ --elf|--coff <lib_list>\n"
         << "       " << SELF_NAME << " symc++ --elf|--coff <lib_list>" << endl;

    std::exit(a_exit_code);
}

bool _lib_ref_new(const Library& a_lib, objkind_t a_objkind)
{
    typedef std::vector< std::pair<const char*, const char*> > T;

    const T* objects_p = a_lib.get_objects();

    T::const_iterator p = objects_p->begin();
    T::const_iterator end_p = objects_p->end();

    // Iterate over all the objects ...
    for (; p != end_p; ++p)
    {
        std::auto_ptr<Object> obj_p = Object_factory::create(a_objkind, p->first, p->second);

        // ... And check for references to any ::operator new function.
        for (unsigned i = 0; i < sizeof(NEW_NAMES[a_objkind]) / sizeof(NEW_NAMES[a_objkind][0]); i++)
        {
            if ( obj_p->is_undef(NEW_NAMES[a_objkind][i]) )
            {
                return 1;
            }
        }
    }

    return 0;
}

