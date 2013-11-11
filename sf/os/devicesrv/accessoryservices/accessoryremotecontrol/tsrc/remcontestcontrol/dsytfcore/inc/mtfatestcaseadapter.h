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
* Description:  Declaration of MTFATestCaseAdapter class
*
*/


#ifndef __MTFATESTCASEADAPTER_H__
#define __MTFATESTCASEADAPTER_H__

#include "mtfatestadapter.h"

/**
* Test case adapter is implemented by the adapter libraries.
*/
class MTFATestCaseAdapter : public MTFATestAdapter
    {
    protected:
        /**
        * Constructor
        */
        IMPORT_C MTFATestCaseAdapter( void );

    public:
        /**
        * Destructor
        */
        IMPORT_C virtual ~MTFATestCaseAdapter( void );
    
    private:
        /**
        * Copy constructor is hidden
        */
        MTFATestCaseAdapter( const MTFATestCaseAdapter& aAdapter );

        /**
        * Assignment operator is hidden
        */
        MTFATestCaseAdapter& operator=( const MTFATestCaseAdapter& aAdapter );
    };

#endif
