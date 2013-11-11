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
// Classes for interpreting a memory area as an ELF or COFF object.
// <creates>
// Object_factory -----------------> Object
// Elf_object      Coff_object
// 
//

#ifndef OBJECT_H
#define OBJECT_H

#include <vector>
#include <memory>


class Object
{
public:
    virtual ~Object() = 0;
public:
    // Does the object have an unresolved reference to the given symbol.
    virtual bool is_undef(const char*) const = 0;
};

class Elf_object : public Object
{
public:
    Elf_object(const char*, const char*);
    virtual ~Elf_object();
public:
    virtual bool is_undef(const char*) const;
private:
    std::vector<const char*> m_undef_symbols;
};

class Coff_object : public Object
{
public:
    Coff_object(const char*, const char*);
    virtual ~Coff_object();
public:
    virtual bool is_undef(const char*) const;
private:
    std::vector<const char*> m_undef_symbols;
};

class Object_factory
{
public:
    enum objkind_t {ELF, COFF};
public:
    static std::auto_ptr<Object> create(objkind_t, const char*, const char*);
};


#endif

