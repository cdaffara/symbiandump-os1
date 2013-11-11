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

#include "elf_symbol.h"


namespace elf
{
    // All of the following functions are candidates for inlining ...

    Symbol::Symbol(const char* p) : m_data( reinterpret_cast<const Elf32_Sym*>(p) )
    {
    }

    uint32_t Symbol::get_name() const
    {
        return m_data->st_name;
    }

    uint32_t Symbol::get_value() const
    {
        return m_data->st_value;
    }

    uint32_t Symbol::get_size() const
    {
        return m_data->st_size;
    }

    bool Symbol::is_info_bind_local() const
    {
        return (m_data->st_info >> 4) == STB_LOCAL;
    }

    bool Symbol::is_info_bind_global() const
    {
        return (m_data->st_info >> 4) == STB_GLOBAL;
    }

    bool Symbol::is_info_bind_weak() const
    {
        return (m_data->st_info >> 4) == STB_WEAK;
    }
    
    bool Symbol::is_info_type_notype() const
    {
        return (m_data->st_info & 0x0f) == STT_NOTYPE;
    }

    bool Symbol::is_info_type_object() const
    {
        return (m_data->st_info & 0x0f) == STT_OBJECT;
    }

    bool Symbol::is_info_type_func() const
    {
        return (m_data->st_info & 0x0f) == STT_FUNC;
    }

    bool Symbol::is_info_type_section() const
    {
        return (m_data->st_info & 0x0f) == STT_SECTION;
    }

    bool Symbol::is_info_type_file() const
    {
        return (m_data->st_info & 0x0f) == STT_FILE;
    }

    //bool Symbol::is_info_type_common() const
    //{
    //    return (m_data->st_info == STT_COMMON);
    //}

    //bool Symbol::is_info_type_tls() const
    //{
    //    return (m_data->st_info == STT_TLS);
    //}

    uint8_t Symbol::get_other() const
    {
        return m_data->st_other;
    }

    uint16_t Symbol::get_shndx() const
    {
        return m_data->st_shndx;
    }

} // namespace elf


