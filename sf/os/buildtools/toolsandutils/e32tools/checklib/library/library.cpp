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
// Class for reading libraries in Unix "ar" format.
// 
//

#include "library.h"

#include "misc/endian.h" // For misc::reverse_bytes()

#include <fstream>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdlib>

namespace
{
    // An ar library begins with this string.
    const char MAGIC[] = "!<arch>\n";

    // The size of an object header.
    const int HEADER_LENGTH = 60;

    // Each object *header* has this trailer.
    const char TRAILER[] = "`\n";
}


Library::Library(const char a_file_name[])
{
    std::ifstream file(a_file_name, std::ios::in | std::ios::binary | std::ios::ate);

    if ( ! file.is_open() )
    {
        throw std::runtime_error("couldn't open file");
    }

    unsigned size  = file.tellg();
    m_mem_p = new char[size];

    file.seekg(0, std::ios::beg);
    file.read( const_cast<char*>(m_mem_p), size );

    file.close();

    const char* p1 = m_mem_p;
    const char* p2 = p1 + size - 1;

    // Eat all meta objects.

    p1 = _eat_ar_header(p1, p2);
    p1 = _eat_sym_table(p1, p2);
    p1 = _eat_junk_objs(p1, p2);

    m_first_p = p1; // Pointer to the start of the first "real" object.
    m_last_p = p2;  // Pointer to end of the last real object.
}

Library::~Library()
{
    delete[] m_mem_p;
}

bool Library::contains_symbol(const char a_sym_name[]) const
{
    std::vector<const char*>::const_iterator p = m_symbols.begin();
    std::vector<const char*>::const_iterator end_p = m_symbols.end();

    for ( ; p != end_p; ++p)
    {
        if ( std::strcmp(a_sym_name, *p) == 0 ) return 1;
    }

    return 0;
}

const std::vector< std::pair<const char*, const char*> >* Library::get_objects() const
{
    if ( m_objects.empty() )
    {
        const char* p = m_first_p;

        unsigned long size;

        while (p < m_last_p)
        {
            p = _eat_obj_header(p, m_last_p, &size);

            m_objects.push_back( std::pair<const char*, const char*>(p, p + size) );

            p += size;
        }
    }

    return &m_objects;
}

const char* Library::_eat_ar_header(const char* p1, const char* p2) const
{
    int magic_length = std::strlen(MAGIC);

    if (p2 - p1 + 1 < magic_length)
    {
        throw std::runtime_error("library too small for magic word");
    }

    if ( std::strncmp(p1, MAGIC, magic_length) != 0 )
    {
        throw std::runtime_error("bad magic; this is not a valid library");
    }

    return (p1 + magic_length);
}

const char* Library::_eat_sym_table(const char* a_first_p, const char* a_last_p) const
{
    unsigned long obj_size;

    const char* p = _eat_obj_header(a_first_p, a_last_p, &obj_size, "/               "); // Read the header of the symbol table.

    if (p == a_first_p)
    {
        throw std::runtime_error("no library symbol table found");
    }

    const char* obj_end_p = p + obj_size;

    // Check that we're 4-byte aligned.
    assert( (reinterpret_cast<int>(p) & 0x3) == 0 );

    uint32_t nr_of_syms = *reinterpret_cast<const int*>(p);
    nr_of_syms = misc::reverse_bytes(nr_of_syms);

    p += sizeof(nr_of_syms);            // Go past the integer we just read.
    p += nr_of_syms * sizeof(uint32_t); // Go past all the offsets.

    unsigned n = 0;

    while (n < nr_of_syms && p < a_last_p)
    {
        m_symbols.push_back(p);

        p += std::strlen(p) + 1; 

        n++;
    }

    if (n != nr_of_syms)
    {
        throw std::runtime_error("inconsistent symbol table");
    }

    if (p > obj_end_p)
    {
        throw std::runtime_error("over-running symbol table");
    }

    return obj_end_p;
}

const char* Library::_eat_junk_objs(const char* p1, const char* p2) const
{
    unsigned long obj_size;
    const char* p;

    p = _eat_obj_header(p1, p2, &obj_size, "//              ");

    if (p > p1)
    {
        p1 = p + obj_size;
    }

    p = _eat_obj_header(p1, p2, &obj_size, "/               ");

    if (p > p1)
    {
        p1 = p + obj_size;
    }

    p = _eat_obj_header(p1, p2, &obj_size, "//              ");

    if (p > p1)
    {
        p1 = p + obj_size;
    }

    return p1;
}

const char* Library::_eat_obj_header(const char* a_first_p, const char* a_last_p, unsigned long* a_size_p, const char* a_name) const
{
    const char* p = a_first_p;

    // The header is 2-byte aligned, so ignore the previous object's trailing
    // padding byte.
    if ( reinterpret_cast<int>(p) & 1)
    {
        p++;
    }

    if (a_last_p - p + 1 < HEADER_LENGTH)
    {
        throw std::runtime_error("no space for library object header");
    }

    // At the moment We can only handle short names. This is enough for identifying
    // the meta objects "/" (symbol table) and "//" (object table).

    if ( a_name && std::strncmp(p, a_name, std::strlen(a_name)) != 0 )
    {
        return a_first_p;
    }

    p += 16; // Ignore the name field.
    p += 12; // Ignore the modification time.
    p +=  6; // Ignore the group ID.
    p +=  6; // Ignore the user ID.
    p +=  8; // Ignore the file mode.

    // Read the object size.

    if (a_size_p)
    {
        char* tail_p;
        *a_size_p = std::strtoul(p, &tail_p, 0);

        if (tail_p == p || tail_p > a_last_p + 1)
        {
            throw std::runtime_error("could not read library object size");
        }
    }

    p += 10; // Jump over the object size field.

    // Verify that the header trailer is correct.

    int trailer_length = std::strlen(TRAILER);

    if ( std::strncmp(p, TRAILER, trailer_length) != 0  )
    {
        throw std::runtime_error("incorrect library object header trailer");
    }

    return (p + trailer_length);
}


