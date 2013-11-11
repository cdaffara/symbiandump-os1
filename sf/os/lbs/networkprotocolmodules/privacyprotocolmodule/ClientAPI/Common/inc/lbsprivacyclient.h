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

#ifndef LBS_PRIVACYCLIENT_H
#define LBS_PRIVACYCLIENT_H

//  INCLUDES
#include <e32base.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetcommon.h>
#include "lbsprivacyservermsgenums.h"

const TUint KLbsNetworkPrivacyApiUidValue			= 0x101f7a8a;
const TUid  KLbsNetworkPrivacyApiUid				= {KLbsNetworkPrivacyApiUidValue};

// CLASS DECLARATION
NONSHARABLE_CLASS(RPrivacyServer): public RSessionBase
    {
    
    public: 
        
    	RPrivacyServer();
         
    	~RPrivacyServer();        
        
    	TInt Connect(TLbsProxyApiType aApiType);
       
    	void VerifyLocationRequest(TInt& aRequestId,TLbsExternalRequestInfo2& aRequestInfo,
        	                                TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy,
        	                                TRequestStatus& aStatus);
                          
    	TInt CancelVerifyLocationRequest(TInt aRequestId ,TInt aCancelReason);
             
    	TInt NotifyLocationRequest(TLbsExternalRequestInfo2& aRequestInfo, TInt& aRequestId,
                                            TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy);
           
        
    	TInt NotifyVerificationTimeout(TLbsExternalRequestInfo2& aRequestInfo,TInt aRequestId,
                                               TLbsNetPosRequestPrivacy& aNetPosRequestPrivacy);

    	void NewPrivacyRequest(TUint32& aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
    			const TLbsNetPosRequestPrivacy& aPrivacyRequest, TRequestStatus& aStatus);
    	
    	void RepeatPrivacyRequest(TUint32 aRequestId, const TLbsExternalRequestInfo& aRequestInfo, 
    			const TLbsNetPosRequestPrivacy& aPrivacyRequest, TRequestStatus& aStatus);
    	
    	TInt CompleteRequest(TUint32 aRequestId, TInt aReason);
    	
        /**
        * From RSessionBase
        */
    	void Close();

    private:     
        TInt StartLbsSystem();
 
    private:
    	TLbsExternalRequestInfo2 iRequestInfo;
        TPckg<TLbsExternalRequestInfo2>	iRequestInfoPkg;
        TLbsNetPosRequestPrivacy iNetPosRequestPrivacy;
		TPckg<TLbsNetPosRequestPrivacy>	iNetPosRequestPrivacyPkg;
		TInt iRequestId;
		TPckg<TInt> iRequestIdPkg;
		TUint32 iRequestIdUint32;
		TPckg<TUint32> iRequestIdUint32Pkg;
        };
#endif      // RPrivacyServer_H
