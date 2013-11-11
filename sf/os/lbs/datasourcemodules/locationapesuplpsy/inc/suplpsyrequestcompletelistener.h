/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SUPL PSY Request Complete Listener Interface
*
*/



#ifndef SUPLPSYREQUESTCOMPLETELISTENER_H
#define SUPLPSYREQUESTCOMPLETELISTENER_H

//  INCLUDES
#include <lbs.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  This class is request complete listener interface.
*  
*  This class provides the interface for listener to get request 
*  complete events.
*
*  @lib nlaapesuplpsy.dll
*  @since S60 3.1
*/
class MSuplPsyRequestCompleteListener 
    {
    public:  
        
        /**
        * Request complete callback function
        * @param aErr Error code. If it is not KErrNone, aPosInfo shall 
        *        not be used.
        * @param aPos Contains position information
        * @return ETrue if the request is completed. Otherwise
        *         return EFalse
        */
        virtual TBool RequestComplete( 
            TInt aErr,
            const HPositionGenericInfo& aPos ) = 0;
        
    };

#endif      // SUPLPSYREQUESTCOMPLETELISTENER_H   
            
// End of File
