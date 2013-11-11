// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef IDENTIFIERBASE_H
#define IDENTIFIERBASE_H

#include <string>

namespace Parser
{

/**
The types of identifiers
*/
enum TIdentifierType
    {
    /** Constants (only integer constants supported) */
    EConstIdentifier,

    /** Any integer type (INTx/UINTx) */
    EIntegerTypeIdentifier,

    /** The enumeration type - contains enumeration value types */
    EEnumTypeIdentifier,

    /** The enumation value type that represents the values contained in an enumeration */
//    EEnumValueIdentifier,

    /** A user defined structure */
    EStructIdentifier,

    /** A user defined signature */
    ESignatureIdentifier,

    /** A user defined context */
    EContextIdentifier,

    /** A user definied message */ 
    EMessageIdentifier,

    /** Identifies padding bytes */
    EPadTypeIdentifier,

    /** The builtin struct for TMessageId */
    EMessageIdTypeIdentifier,
    };

/**
CIdentifierBase is an abstract class that provides the base functionality
for any basic or builtin types.
*/
class CIdentifierBase
    {
    public:
        /**
        D'tor
        */
        virtual ~CIdentifierBase();

        /**
        Returns the type of identifier that this identifier is
        @see TIdentifierType
        */
        inline TIdentifierType Type() const
            {
            return iType;
            }

        /**
        Returns the unique name that identifies this identifier
        */
        inline const char* Name() const
            {
            return iName;
            }

        /**
        Returns identifier flags
        */
        inline int Flags() const
            {
            return iFlags;
            }
        
        /** Returns the size of this type in bytes */
        int Size() const;

        /** Returns the size of this type in bytes including the size of any base types */
        virtual int TotalSize() const;

        /**
        Formats the data provided according to the layout of the type represented by
        the implementation and outputs it to the stream.
        @param aData A pointer to the buffer containing the data for this type.
        @param aLength The number of bytes believed to belong to this type.
               It should match the TotalSize() of the identifier.
        @param aOptions A pointer to an identifier type specific object containing
               options that the type will understand and can use to alter the output
               formatting on a per item basis. This usually comes from TMember.
        @param aDescription The stream to output to.
        */
        virtual void Describe(const unsigned char* aData, unsigned int aLength, const void* aOptions, std::ostream& aDescription) const = 0;

    protected:
        explicit CIdentifierBase(TIdentifierType aType, const char* aName)
            {
            iType = aType;
            iName = _strdup(aName);
            iSize = 0;
            iFlags = 0;
            }

    protected:
        int iSize; // The size of this type in bytes
        int iFlags;

    private:
        TIdentifierType iType;
        char* iName;
    };
    

/**
TMember represents a data member of a structure style type.
These are STRUCT, CONTEXT, and SIGNATURE.
*/
struct TMember
    {
    public:
        TMember()
            : iMemberType(NULL), iOffset(0), iArraySize(1), iIdentifierOptions(NULL)
            {
            }

        const CIdentifierBase* iMemberType;
        std::string iMemberName;
        int iOffset;
        int iArraySize;
        void* iIdentifierOptions;
    };

} // namespace Parser

#endif
// IDENTIFIERBASE_H

