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

/**
 @file
 @internalTechnology
 @prototype
*/


#ifndef LBS_PRIVACYREQUESTRESPONSEHANDLER_H
#define LBS_PRIVACYREQUESTRESPONSEHANDLER_H

// System  
#include <e32base.h>

//Component
#include "lbsprivacyrequest.h" 
#include "lbsprivacyrequestimpl.h"

/**
* This Class Handles verification requests.
*/
NONSHARABLE_CLASS(CPrivacyRequestResponseHandler) : public CActive
    {
    public:  

        static CPrivacyRequestResponseHandler* NewLC(CLbsPrivacyRequestImpl& aParent, RPrivacyServer& aPrivacyServer,
        					     MLbsPrivacyRequestObserver& aObserver);
        virtual ~CPrivacyRequestResponseHandler();
    
    	void NewPrivacyRequest(TUint32& aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
    			const TLbsNetPosRequestPrivacy& aPrivacyRequest);
    	
    	void RepeatPrivacyRequest(TUint32 aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
    			const TLbsNetPosRequestPrivacy& aPrivacyRequest);

    private: // From CActive

       void RunL();
       void DoCancel();
       TInt RunError(TInt aError);
              
    private:
          
        CPrivacyRequestResponseHandler(CLbsPrivacyRequestImpl& aParent, RPrivacyServer& aPrivacyServer,
        						  MLbsPrivacyRequestObserver& aObserver);
        
                                    
    private:  // Data
    	CLbsPrivacyRequestImpl& iParent;
        RPrivacyServer& iPrivacyServer;
        MLbsPrivacyRequestObserver& iObserver;
        TUint32 iRequestId;
    };

#endif      // LBS_PRIVACYREQUESTRESPONSEHANDLER_H
