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
//

#include "object.h"

#include "coff/coff_file_header.h"
#include "coff/coff_symbol.h"
#include "coff/coff_string_table.h"

#include <algorithm>
#include <functional>

Coff_object::Coff_object(const char* p1, const char* p2)
{
    coff::File_header fh(p1, p2);

    unsigned nr_of_syms = fh.get_symcount();
    unsigned sym_size = coff::Symbol::get_entrysize();

    // Pointer to the first entry in the symbol table.
    p1 +=  fh.get_symtab_offset();

    // The string table is locates directly after the symbol table.
    coff::String_table strtab( p1 + nr_of_syms * sym_size);

    // Iterate over the whole symbol table.
    for (unsigned i = 0; i < nr_of_syms; i++, p1 += sym_size)
    {
        coff::Symbol s(p1, p2);

        // A symbol can have auxiliary entries that follows it.
        unsigned aux_count = s.get_auxcount();

        p1 += sym_size * aux_count;
        i += aux_count;

        if ( s.get_section() == 0 ) // If symbol is undefined ...
        {
            m_undef_symbols.push_back( strtab.get_string( s.get_name() ) );
        }
    }
}

Coff_object::~Coff_object() {}

bool Coff_object::is_undef(const char a_sym[]) const
{
    using std::find_if;
    using std::not1;
    using std::bind2nd;
    using std::ptr_fun;
    using std::strcmp;

    typedef std::vector<const char*> T;

    T::const_iterator beg_p = m_undef_symbols.begin();
    T::const_iterator end_p = m_undef_symbols.end();

    T::const_iterator pos = find_if( beg_p, end_p, not1(bind2nd( ptr_fun(strcmp), a_sym)) );

    return (pos != end_p);
}

