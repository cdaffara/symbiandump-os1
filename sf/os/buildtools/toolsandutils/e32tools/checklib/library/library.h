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

#ifndef LIBRARY_H
#define LIBRARY_H

#include <vector>
#include <utility>


class Library
{
public:
    Library(const char a_file_name[]);
    ~Library();
public:
    bool contains_symbol(const char[]) const;

    const std::vector< std::pair<const char*, const char*> >* get_objects() const;
private:
    const char* _eat_ar_header(const char*, const char*) const;
    const char* _eat_sym_table(const char*, const char*) const;
    const char* _eat_junk_objs(const char*, const char*) const;

    const char* _eat_obj_header(const char*, const char*, unsigned long*, const char* = 0) const;

private:
    const char* m_mem_p;

    const char* m_first_p;
    const char* m_last_p;

    mutable std::vector<const char*> m_symbols;
    mutable std::vector< std::pair<const char*, const char*> > m_objects;
};


#endif

