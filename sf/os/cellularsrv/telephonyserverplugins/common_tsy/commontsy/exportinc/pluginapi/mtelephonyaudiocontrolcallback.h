// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef MTELEPHONYAUDIOCONTROLCALLBACK_H
#define MTELEPHONYAUDIOCONTROLCALLBACK_H

// INCLUDES
#include <etelmm.h>

// CLASS DECLARATION

/**
 *  Defines the Telephony Audio Control callback interface which is used to
 *  inform CTSY from Telephony Audio Control errors
 *
 * @publishedPartner
 * @released 
 */

class MTelephonyAudioControlCallback
	{

	public: // Pure virtual functions
	
        /**
         * Informs errors to the observer
         *
         * @param aCallName The name of the call where error happened
         * @param aError The error code
         */  
	    virtual void NotifyTelephonyAudioControlError(  
                                            const TName& aCallName, 
	                                        const TInt& aError ) = 0;
	                                        
	};


#endif  // MTELEPHONYAUDIOCONTROLCALLBACK_H

//  End of file
