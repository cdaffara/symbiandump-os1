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

#include "elf/elf_file_header.h"
#include "elf/elf_section_header.h"
#include "elf/elf_symbol.h"
#include "elf/elf_string_table.h"

#include <algorithm>
#include <functional>

Elf_object::Elf_object(const char* p1, const char* p2)
{
    const char* first_p = p1;
    const char* last_p = p2;

    elf::File_header fh(first_p, last_p);

    const char* sh_table_p = first_p + fh.get_shoff();

    const char* p = sh_table_p;

    unsigned D = fh.get_shentsize();
    unsigned N = fh.get_shnum();

    // Iterate over the section header table.
    for (unsigned i = 0; i < N; i++, p += D )
    {
        elf::Section_header sh(p, last_p);

        if ( sh.is_type_symtab() ) // We've found the symbol table's section header.
        {
            // Locate the string table.
            elf::Section_header strtab_sh( sh_table_p + D*sh.get_link(), last_p );
            elf::String_table strtab(first_p + strtab_sh.get_offset(), strtab_sh.get_size() );

            unsigned D = sh.get_entsize();  // The difference between two symbol pointers.
            unsigned N = sh.get_size() / D; // The number of symbols.

            const char* p = first_p + sh.get_offset();

            // Iterate over all symbols.
            for (unsigned i = 0; i < N; i++, p += D)
            {
                const elf::Symbol s(p);

                if (s.get_shndx() == 0)
                {
                    m_undef_symbols.push_back( strtab.get_string( s.get_name() ) );
                }
            }

            break; // We're only interested in the symbol table ...
        }
    }
}

Elf_object::~Elf_object() {}

bool Elf_object::is_undef(const char a_sym[]) const
{
    using std::find_if;
    using std::not1;
    using std::bind2nd;
    using std::ptr_fun;
    using std::strcmp;

    typedef std::vector<const char*> T;

    T::const_iterator beg_p = m_undef_symbols.begin();
    T::const_iterator end_p = m_undef_symbols.end();

    // "STL considered harmful."
    T::const_iterator pos = find_if( beg_p, end_p, not1(bind2nd( ptr_fun(strcmp), a_sym)) );

    return (pos != end_p);
}

