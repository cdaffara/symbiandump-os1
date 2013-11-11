/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used as an reference implementation
*
*/


#ifndef CASYSTUBCMDOBSERVER_H
#define CASYSTUBCMDOBSERVER_H

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CASYStubCmdHandlerBase;

// CLASS DECLARATION

/**
*  Timer based Active object for reference implementation.
*  Demonstrates asynchronous event handling in ASY.
*  It is optional.
*
*  @lib ?library
*  @since S60 3.1
*/
class CAsyStubCmdObserver : public CTimer
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aASYReference Pointer to the main service object.
        * @return The created object.        
        */
        static CAsyStubCmdObserver* NewL( CASYStubCmdHandlerBase* aASYReference );
        
        /**
        * Destructor.
        */
        virtual ~CAsyStubCmdObserver();

    public: // New functions
    
       /**
        * Sets completion time for timer to complete
        * @since S60 3.1
        * @param 
        * @return void
        */    
        void StartTimer();
        
    public: // Functions from base classes

    protected: // New functions
        
    protected: // Functions from base classes
        
        /**
        * From CActive Handles an active object's request completion event.
        */
        void RunL();

    private:

        /**
        * C++ default constructor.
        */
        CAsyStubCmdObserver( CASYStubCmdHandlerBase* aASYReference );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        

    public:     // Data
    
    protected:  // Data

    private:    // Data
    
        // Pointer to main service object.
        CASYStubCmdHandlerBase* iASYStubCmdHandler; // Not owned

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif // CASYSTUBCMDOBSERVER_H
            
// End of File
