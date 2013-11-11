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

#include "elf_file_header.h"
#include <stdexcept>
#include <string>


namespace elf
{
    File_header::File_header(const char* p1, const char* p2, bool a_pedantic)
        : m_data( reinterpret_cast<const Elf32_Ehdr*>(p1) )
    {
        if ( p1 + sizeof(Elf32_Ehdr) > p2 )
        {
            throw std::runtime_error("not an ELF object; file too small");
        }

        uint8_t m0 = m_data->e_ident[0];
        uint8_t m1 = m_data->e_ident[1];
        uint8_t m2 = m_data->e_ident[2];
        uint8_t m3 = m_data->e_ident[3];

        if (m0 != 0x7f || m1 != 'E' || m2 != 'L' || m3 != 'F')
        {
            throw std::runtime_error("not an ELF object; bad magic");
        }

        if (a_pedantic) // Do some extra checks for correctness.
        {
            //if (m_data->e_ident[4] != 1)
            //{
            //    throw std::runtime_error("invalid class; this program can only handle 32-bit objects");
            //}

            //if (m_data->e_ident[5] != 1)
            //{
            //    throw std::runtime_error("invalid encoding; this program can only handle little-endian objects");
            //}

            //for (unsigned i = EI_PAD; i < sizeof(m_data->e_ident); ++i)
            //{
            //    if (m_data->e_ident[i] != 0)
            //    {
            //        throw std::runtime_error("non-zero pading");
            //    }
            //}

            //if (m_data->e_type == ET_NONE)
            //{
            //    throw std::runtime_error("invalid type");
            //}

            //if (m_data->e_phoff == 0 && m_data->e_shoff == 0)
            //{
            //    throw std::runtime_error("no program headers and no section headers");
            //}

            //if ( m_data->e_phoff + m_data->e_phentsize * m_data->e_phnum > p2 - p1 + 1 )
            //{
            //    throw std::runtime_error("file too small for header table");
            //}

            //if (m_data->e_shoff + m_data->e_shentsize * m_data->e_shnum > a_mem.get_size() )
            //{
            //    throw std::runtime_error("file too small for header table");
            //}

            //if ( m_data->e_shentsize != sizeof(Elf32_Shdr) )
            //{
            //    throw std::runtime_error("e_shentsize doesn't match the actual size of the type");
            //}

            //if (m_data->e_shoff > 0 && m_data->e_shnum == 0)
            //{
            //    throw std::runtime_error("this program can't handle the combination e_shoff > 0 and e_shnum == 0");
            //}

            //if (m_data->e_shstrndx >= 0xff00)
            //{
            //    throw std::runtime_error("this program can only handle e_shstrndx < 0xff00");
            //}

        } // if (a_pedantic)
    }

    // The following functions are obvious candidates for inlining ...

    // e_type

    bool File_header::is_type_rel() const
    {
        return (m_data->e_type == ET_REL);
    }
    bool File_header::is_type_exec() const
    {
        return (m_data->e_type == ET_EXEC);
    }
    bool File_header::is_type_dyn() const
    {
        return (m_data->e_type == ET_DYN);
    }

    // e_machine

    bool File_header::is_machine_none() const
    {
        return (m_data->e_machine == EM_NONE);
    }
    bool File_header::is_machine_arm() const
    {
        return (m_data->e_machine == EM_ARM);
    }
    bool File_header::is_machine_386() const
    {
        return (m_data->e_machine == EM_386);
    }

    uint32_t File_header::get_entry() const
    {
        return m_data->e_entry;
    }

    uint32_t File_header::get_phoff() const
    {
        return m_data->e_phoff;
    }

    uint32_t File_header::get_shoff() const
    {
        return m_data->e_shoff;
    }

    uint32_t File_header::get_flags() const
    {
        return m_data->e_flags;
    }

    uint16_t File_header::get_ehsize() const
    {
        return m_data->e_ehsize;
    }

    uint16_t File_header::get_phentsize() const
    {
        return m_data->e_phentsize;
    }

    uint16_t File_header::get_phnum() const
    {
        return m_data->e_phnum;
    }

    uint16_t File_header::get_shentsize() const
    {
        return m_data->e_shentsize;
    }

    uint16_t File_header::get_shnum() const
    {
        return m_data->e_shnum;
    }

    uint16_t File_header::get_shstrndx() const
    {
        return m_data->e_shstrndx;
    }

} // namespace elf

