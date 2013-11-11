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

#ifndef ELF_FILE_HEADER_H
#define ELF_FILE_HEADER_H

#include "elftools/inc/elfdefs.h"
#include <stdint.h>


namespace elf
{
    class File_header
    {
    public:
        File_header(const char*, const char*, bool = 0);
    public:
        // e_ident[]
        // No functions are defined; they shouldn't be needed.

        // e_type
        bool is_type_rel() const;
        bool is_type_exec() const;
        bool is_type_dyn() const;
        
        // e_machine
        bool is_machine_none() const;
        bool is_machine_arm() const;
        bool is_machine_386() const;

        // e_version
        // No functions necessary.

        uint32_t get_entry() const;
        uint32_t get_phoff() const;
        uint32_t get_shoff() const;
        uint32_t get_flags() const;
        uint16_t get_ehsize() const;
        uint16_t get_phentsize() const;
        uint16_t get_phnum() const;
        uint16_t get_shentsize() const;
        uint16_t get_shnum() const;
        uint16_t get_shstrndx() const;

    private:
        const Elf32_Ehdr* m_data;
    };
}




#endif

