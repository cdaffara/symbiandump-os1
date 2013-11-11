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

#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include "elftools/inc/elfdefs.h"
#include <stdint.h>

namespace elf
{
    class Section_header
    {
    public:
        Section_header(const char*, const char*);

        uint32_t get_name();

        bool is_type_null() const;
        bool is_type_progbits() const;
        bool is_type_symtab() const;
        bool is_type_dynsym() const;
        bool is_type_strtab() const;
        bool is_type_rela() const;
        bool is_type_nobits() const;
        bool is_type_rel() const;

        bool is_flags_write() const; 
        bool is_flags_alloc() const;
        bool is_flags_execinstr() const;

        uint32_t get_addr();
        uint32_t get_offset() const;
        uint32_t get_size() const;
        uint32_t get_link();
        uint32_t get_info();
        uint32_t get_addralign();
        uint32_t get_entsize() const;

    private:
        const Elf32_Shdr* m_data;
    };

} // namespace elf


#endif

