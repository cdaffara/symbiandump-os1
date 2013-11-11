/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




/**
 @file
 @internalTechnology 
*/

#ifndef CHWRMSERVERACTIVESCHEDULER_H
#define CHWRMSERVERACTIVESCHEDULER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

/**
*  Schedules the sequence in which active object request completion events are 
*  handled by a server thread
*
*/
class CHWRMServerActiveScheduler : public CActiveScheduler
    {
    public:  // Constructors and destructor
        
        /**
        * Public constructor.
        */
        static CHWRMServerActiveScheduler* NewL();

        /**
        * Public constructor.
        */
        static CHWRMServerActiveScheduler* NewLC();

        /**
        * Destructor.
        */
        virtual ~CHWRMServerActiveScheduler();

    public: // New functions
        

    public: // Functions from base classes

        
    protected:  // New functions
        
        /**
        * Panics if occurred error serious enough
        * @param anError Error code
        * @return void
        */
        void Error( TInt anError ) const;

    protected:  // Functions from base classes
        

    private:

        /**
        * C++ constructor.
        */
        CHWRMServerActiveScheduler();

    public:     // Data

    
    protected:  // Data


    private:    // Data


    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes


    };

#endif      // CHWRMSERVERACTIVESCHEDULER_H   
            
// End of File
