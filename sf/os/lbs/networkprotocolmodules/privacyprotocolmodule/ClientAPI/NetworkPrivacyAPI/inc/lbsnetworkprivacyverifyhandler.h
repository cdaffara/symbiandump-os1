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
 @released
*/


#ifndef LBS_NETWORKPRIVACYVERIFYHANDLER_H
#define LBS_NETWORKPRIVACYVERIFYHANDLER_H

// System  
#include <e32base.h>

//Component
#include "lbsnetworkprivacyimpl.h" 
#include <lbs/epos_mposverificationobserver.h>

/**
* This Class Handles verification requests.
*/
NONSHARABLE_CLASS(CNetworkPrivacyVerifyHandler) : public CActive
    {
    public:  

        static CNetworkPrivacyVerifyHandler* NewL(RPrivacyServer& aPrivacyServer,
        		                             MPosVerificationObserver& aClientObserver,
        		                             MPosVerificationObserver& aBufferObserver);
              
        virtual ~CNetworkPrivacyVerifyHandler();

    public: 
        
        void VerifyLocationRequest(
               TInt& aRequestId,TLbsExternalRequestInfo2& aRequestInfo,
               TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy);
        
        void CancelRequest(TInt aCancelReason);
       
        TInt RequestId()const;
    
    private: // From CActive

       void RunL();
       void DoCancel();
       TInt RunError(TInt aError);
              
    private:
          
        CNetworkPrivacyVerifyHandler(RPrivacyServer& aPrivacyServer,
        		                     MPosVerificationObserver& aClientObserver,
        		                     MPosVerificationObserver& aBufferObserver);
        
                                    
    private:    // Data
        RPrivacyServer& iPrivacyServer;
        MPosVerificationObserver& iClientObserver;
        MPosVerificationObserver& iBufferObserver;
        TInt iRequestId;
        TInt iCancelReason;
        
    };

#endif      // lbsnetworkprivacyverifyhandler.h


