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
* Description:  It defines dial interface for phone client.
*
*/


#ifndef CPHCLTEXTPHONEBASE_H
#define CPHCLTEXTPHONEBASE_H

//  INCLUDES
#include <e32base.h>
#include "PhCltTypes.h"

// FORWARD DECLARATIONS
class MPhCltExtPhoneObserver;
class CPhCltExtPhoneDialData;

// CLASS DECLARATION

/**
*  It defines dial interface for phone client.
*
*  Usage:
*  
*  To dial, you must have a string of characters from set 
*  { 0, 1, 2, .. 9, #, *, +, p, P, w, W }. It is the only 
*  mandatory parameter. String may also contain other characters,
*  but then only a note is displayed. So you should proceed as follows:
*       1. Check that string is acceptable. 
*          (application specific grammar)
*       2. If not, stop. Otherwise continue.
*       3. Convert to string acceptable by phoneclient.
*          Note PhCltUtils class may help.
*       4. Dial using the class.
*
*  Note that dialling does not always mean creating a call. 
*  (E.g. *#43# makes phone to interrogate status of call waiting.)
*  
*  In order to dial, you need instance of CPhCltExtPhoneBase. You 
*  can create one as follows:
*       iExtCall = CPhCltExtPhoneBase::NewL( this );
*  (the parameter is pointer to observer)
*
*  Then you must fill dial parameters to instance of TPhCltExtPhoneDialData.
*  (you can read more details from its header)
*
*       TPhCltExtPhoneDialData data;
*       data.SetTelephoneNumber( .. );
*       ..
*
*  And finally, you can dial. Note that the method is asynchronous and
*  observer will be informed when phone starts dialing.
*
*       iExtCall->DialL( data );
*
*  Finally, you can delete the instance. If dialing is ongoing, this should
*  be done after observer has been informed. You can also delete before that,
*  but then the request might also be canceled.
*
*  @lib phoneclient.lib
*  @since 1.0
*/
class CPhCltExtPhoneBase : public CActive
    {
    public:  // Constructors and destructor
 
        /**
        * Create an instance that implements the interface.
        *
        * @param aObserver It is the observer for the instance.
        * @return Returns the created instance.
        */
        IMPORT_C static CPhCltExtPhoneBase* NewL( 
            MPhCltExtPhoneObserver* aObserver );


    public: // New fuctions    
        

       /**
       * Makes a new call. This is the preferred way of dialling!
       *
       * @param aData The object containing the parameters for 
       *              the dial request.
       */
       IMPORT_C virtual void DialL( CPhCltExtPhoneDialData& aData ) = 0;


    protected:

        /**
        * C++ constructor.
        */
        CPhCltExtPhoneBase( MPhCltExtPhoneObserver* aObserver );


    protected: // Data
        
        // Observer to handle events.
        MPhCltExtPhoneObserver* iObserver;

    };

#endif      // CPHCLTEXTPHONEBASE_H
            
// End of File
