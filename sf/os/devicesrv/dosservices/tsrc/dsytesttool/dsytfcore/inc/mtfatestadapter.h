/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Declaration of MTFATestAdapter class
*
*/


#ifndef __MTFATESTADAPTER_H__
#define __MTFATESTADAPTER_H__

#include "tftypes.h"

/**
* Root interface for test adapter implementations
*/
class MTFATestAdapter 
    {
    protected:
        /**
        * Default constructor
        */
        IMPORT_C MTFATestAdapter( void );
    
    public:
        /**
        * Destructor
        */
        IMPORT_C virtual ~MTFATestAdapter( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFATestAdapter( const MTFATestAdapter& aAdapter );

        /**
        * Assignment operator is hidden
        */
        MTFATestAdapter& operator=( const MTFATestAdapter& aAdapter );

    public:
        /**
        * Returns the type of the test case wrapped into this adapter
        */
        virtual TTFATestType Type( void ) const = 0;
    };

#endif
