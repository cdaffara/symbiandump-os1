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
*                
*
*/


#ifndef MREMCONOBSERVER_H
#define MREMCONOBSERVER_H

// INCLUDES
//

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
*  
*  
*
*  @lib 
*  @since Series 60 3.0
*/
class MRemConObserver
    {
    public:  // Constructors and destructor
        

    public: // New functions
        
        /**
        * Server request has been completed.
        * @since Series 60 3.0
        * @param aResult    Result code of a request processing.
        * @return void
        */
        virtual void CompleteResponse( const TInt aError ) = 0;
    
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

#endif  //MREMCONOBSERVER_H
            
// End of File
