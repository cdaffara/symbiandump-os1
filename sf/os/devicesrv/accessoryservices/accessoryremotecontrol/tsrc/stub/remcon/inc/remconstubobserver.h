/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the header file of the 
*                MCommonAccessoryServerControlObserver class.
*
*/


#ifndef MCOMMONACCESSORYSERVERCONTROLOBSERVER_H
#define MCOMMONACCESSORYSERVERCONTROLOBSERVER_H

#include <accremconpstypes.h>

// INCLUDES

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
//

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None

// CLASS DECLARATION

/**
*  Pure virtual API class for CCommonAccessoryServerControl class. Other server 
*  classes may use this class to send messages to CWalletDataStorage instance.
*
*  @lib 
*  @since Series 60 3.0
*/
class MRemConStubObserver
    {
    public:  // Constructors and destructor
        

    public: // New functions
        
        /**
        * Server request has been completed.
        * @since Series 60 3.0
        * @param aResult    Result code of a request processing.
        * @return void
        */
        virtual void TestCaseCommandReceived(TInt aTFControlKey, TRemConPckgStubBuf& aKeyEvents) = 0;
    
    public: // Functions from base classes
        
    protected:  // New functions

    protected:  // Functions from base classes

    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data
         
    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#endif  //MCOMMONACCESSORYSERVERCONTROLOBSERVER_H   
            
// End of File
