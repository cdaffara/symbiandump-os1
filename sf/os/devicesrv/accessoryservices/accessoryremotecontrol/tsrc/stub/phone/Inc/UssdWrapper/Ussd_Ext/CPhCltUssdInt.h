/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface Ui information
*
*/


#ifndef CPHCLTUSSDINT_H
#define CPHCLTUSSDINT_H

//  INCLUDES
#include <e32base.h>
#include <CPhCltUssd.h>

// FORWARD DECLARATIONS
// CLASS DECLARATION


/**
*  Interface for sending messages.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltUssdInt : 
    public CPhCltUssd
    
    {
    public:  // Constructors and destructor
        
        
       // CPhCltUssdInt();
        
       /**
        * Two-phased constructor.
        *
        * @param aShowNotes If ETrue, notes are shown.
        * @return Pointer to created CPhCltUssd instance.
        */
        IMPORT_C static CPhCltUssdInt* NewL( TBool aShowNotes = ETrue );
        
               
       protected:  
        //--------------------- From app ------------------------------

        /**
        * UI informs that it is been created
        * 
        * @return Error code.
        */
        IMPORT_C virtual TInt AppStarting() = 0;

        /**
        * UI informs that it is terminating.
        *
        * @param aExitReason The reason why the application was terminated.
        * @return Error code.
        */
        IMPORT_C virtual TInt AppTerminating( 
            TPhCltUssdAppExitReason aExitReason ) = 0;

        /**
        * UI informs that it is brougth to foreground.
        *
        * @return Should the editor be emptied?
        */
        IMPORT_C virtual TBool AppToForeground() = 0;

        /**
        * UI informs that it is gone background.
        *
        * @return Error code.
        */
        IMPORT_C virtual TInt AppToBackground() = 0;
         
        /**
        * C++ constructor.
        */
        CPhCltUssdInt();
        
     protected:

        // CUssdComms can call the protected functions mentioned above.
        friend class CUssdComms;
        
    };

#endif //  CPHCLTUSSDINT_H

// End of File
