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

#ifndef TRACEEVENT_H
#define TRACEEVENT_H

class MTraceEvent
    {
    public:
        // Flags from the frame header
        virtual unsigned int Flags() const = 0;

        // Primary filter (Category) of the frame
        virtual unsigned int PrimaryFilter() const = 0;

        // Secondary filter of the frame
        virtual unsigned int SecondaryFilter() const = 0;

        // Subcategory of the frame (meaning dependant on the Category)
        virtual unsigned int SubCategory() const = 0;

        // Flag accessors
        virtual bool Header2Present() const = 0;
        virtual bool Timestamp1Present() const = 0;
        virtual bool Timestamp2Present() const = 0;
        virtual bool ContextIdPresent() const = 0;
        virtual bool PcPresent() const = 0;
        virtual bool ExtraPresent() const = 0;

        // These return values from the header if they are present
        virtual unsigned int Header2() const = 0;
        virtual unsigned int Timestamp1() const = 0;
        virtual unsigned int Timestamp2() const = 0;
        virtual unsigned int ContextType() const = 0;
        virtual unsigned int ContextId() const = 0;
        virtual unsigned int ProgramCounter() const = 0;
        virtual unsigned int Extra() const = 0;


        virtual unsigned int Arg1() const = 0;
        virtual unsigned char* Data() const = 0;
        virtual unsigned int DataLength() const = 0;

        virtual unsigned int EventEntryNumber() const = 0;

    protected:
        unsigned int BytesToInt(const unsigned char *aBytes) const;
        unsigned int BytesToInt(const char *aBytes) const;
    };


#endif
// TRACEEVENT_H

