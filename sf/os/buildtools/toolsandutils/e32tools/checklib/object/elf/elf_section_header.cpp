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

#include "elf_section_header.h"


namespace elf
{
    Section_header::Section_header(const char* p1, const char* p2)
        : m_data( reinterpret_cast<const Elf32_Shdr*>(p1) )
    {
        // This could be useful for future error checking.
        (void) p2;

        //--if (m_data->sh_type != SHT_NOBITS)
        //--{
        //--    if (m_data->sh_offset + m_data->sh_size > a_mem.get_size() )
        //--    {
        //--        throw std::runtime_error("file is too small");
        //--    }
        //--}

        //--if (m_data->sh_addralign > 1)
        //--{
        //--    if (m_data->sh_addr % m_data->sh_addralign != 0)
        //--    {
        //--        throw std::runtime_error("incorrect alignment");
        //--    }
        //--}
    }

    uint32_t Section_header::get_name()
    {
        return m_data->sh_name;
    }

    bool Section_header::is_type_null() const
    {
        return (m_data->sh_type == SHT_NULL);
    }

    bool Section_header::is_type_progbits() const
    {
        return (m_data->sh_type == SHT_PROGBITS);
    }
    
    bool Section_header::is_type_symtab() const
    {
        return (m_data->sh_type == SHT_SYMTAB);
    }
    
    bool Section_header::is_type_dynsym() const
    {
        return (m_data->sh_type == SHT_DYNSYM);
    }

    bool Section_header::is_type_strtab() const
    {
        return (m_data->sh_type == SHT_STRTAB);
    }

    bool Section_header::is_type_rela() const
    {
        return (m_data->sh_type == SHT_RELA);
    }

    bool Section_header::is_type_nobits() const
    {
        return (m_data->sh_type == SHT_NOBITS);
    }

    bool Section_header::is_type_rel() const
    {
        return (m_data->sh_type == SHT_REL);
    }

    bool Section_header::is_flags_write() const
    {
        return (m_data->sh_flags & SHF_WRITE) != 0;
    }
    bool Section_header::is_flags_alloc() const
    {
        return (m_data->sh_flags & SHF_ALLOC) != 0;
    }
    bool Section_header::is_flags_execinstr() const
    {
        return (m_data->sh_flags & SHF_EXECINSTR) != 0;
    }

    uint32_t Section_header::get_addr()
    {
        return m_data->sh_addr;
    }

    uint32_t Section_header::get_offset() const
    {
        return m_data->sh_offset;
    }

    uint32_t Section_header::get_size() const
    {
        return m_data->sh_size;
    }

    uint32_t Section_header::get_link()
    {
        return m_data->sh_link;
    }

    uint32_t Section_header::get_info()
    {
        return m_data->sh_info;
    }

    uint32_t Section_header::get_addralign()
    {
        return m_data->sh_addralign;
    }

    uint32_t Section_header::get_entsize() const
    {
        return m_data->sh_entsize;
    }

} // namespace elf

