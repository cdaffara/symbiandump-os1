/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef MPOSREQUESTOBSERVER_H
#define MPOSREQUESTOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>

// CLASS DECLARATION

/**
* Interface class to be implemented by PSY tests
*/
class MPosRequestObserver
    {
    public:  // New functions
        
        /**
        * Called when a requester is ready with a NotifyPositionUpdate
        * @param aMsg The message the should be handled.
        */
        virtual void HandlePositionCompleteL( 
			TInt aCompleteCode,
			TTimeIntervalMicroSeconds aRequestTime
		) = 0;

        /**
        * Called when a request went wrong.
        * @param aErrorCode The error code for which type of error that occured.
        */
        virtual void HandleRequestError( 
		/* IN */ TInt aErrorCode
		) = 0;

		/**
        * Add test result to log
        * @param aMsg The message the should be handled.
        */
		virtual void AddTestResultL(
        /* IN */   const TDesC& aMessage, 
        /* IN */   const TInt aResultType
        ) = 0;

    };

#endif	// MPOSREQUESTOBSERVER_H  
            
// End of File
