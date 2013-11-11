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

#ifndef ELF_SYMBOL_H
#define ELF_SYMBOL_H

#include "elftools/inc/elfdefs.h"
#include <stdint.h>


namespace elf
{
    class Symbol
    {
    public:
        Symbol(const char*);

        uint32_t get_name() const ;
        uint32_t get_value() const ;
        uint32_t get_size() const ;

        bool is_info_bind_local() const;
        bool is_info_bind_global() const;
        bool is_info_bind_weak() const;

        bool is_info_type_notype() const;
        bool is_info_type_object() const;
        bool is_info_type_func() const;
        bool is_info_type_section() const;
        bool is_info_type_file() const;
        //bool is_info_type_common() const;
        //bool is_info_type_tls() const;

        uint8_t  get_info_other() const ;
        uint8_t  get_other() const ;
        uint16_t get_shndx() const ;
    private:
        const Elf32_Sym* m_data;
    };

}


#endif

