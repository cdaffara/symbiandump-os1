/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Active scheduler for SSY threads. 
*                This is required, since it cannot be guaranteed SSY 
*                implementations handle RunL errors properly. 
*
*/


#ifndef CSENSRVSSYACTIVESCHEDULER_H
#define CSENSRVSSYACTIVESCHEDULER_H

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
*  handled by a SSY thread
*
*  @since Series 60 4.0
*/
class CSensrvSsyActiveScheduler : public CActiveScheduler
    {
    public:  // Constructors and destructor
        
        /**
        * Public constructor.
        */
        static CSensrvSsyActiveScheduler* NewL();

        /**
        * Public constructor.
        */
        static CSensrvSsyActiveScheduler* NewLC();

        /**
        * Destructor.
        */
        virtual ~CSensrvSsyActiveScheduler();

    protected:  // Inherited from CActiveScheduler
        
        /**
        * Panics if occurred error serious enough.
        *
        * @since Series 60 4.0
        * @param aError Error code
        * @return void
        */
        void Error( TInt aError ) const;

    private:

        /**
        * C++ constructor.
        */
        CSensrvSsyActiveScheduler();

    };

#endif      // CSENSRVSSYACTIVESCHEDULER_H   
            
// End of File
