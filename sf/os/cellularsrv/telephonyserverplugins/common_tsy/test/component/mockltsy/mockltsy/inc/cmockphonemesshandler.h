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

#ifndef CMockPhoneMessHandler_H
#define CMockPhoneMessHandler_H

#include <mmlist.h>
#include "mmockmesshandlerbase.h"

class CMmMessageRouter;
class CMmDataPackage;
class CMobilePhoneCFList;
class CMobilePhoneCWList;

class CMockPhoneMessHandler : public CBase, public MMockMessHandlerBase
    {
public: 
	static CMockPhoneMessHandler* NewL(CMmMessageRouter* aMessageRouter);
	virtual ~CMockPhoneMessHandler();

public: 
  	virtual TInt ExtFuncL(TInt aIpc,const CMmDataPackage* aDataPackage);
	virtual void CompleteL(TInt aIpc, const TDesC8& aData, TInt aResult);
     
private: 
 	CMockPhoneMessHandler(CMmMessageRouter* aMessageRouter);
	void ConstructL();
        
private:
    CMmMessageRouter* iMessageRouter;
    RPointerArray<RMobilePhone::CImsAuthorizationInfoV5> iAuthorizationInfos;
    RPointerArray<CMobilePhoneCBList> iCBLists;
    RPointerArray<CMobilePhoneCFList> iCFLists;
    RPointerArray<CMobilePhoneCWList> iCWLists;
    RPointerArray<RMobilePhone::TMobilePhoneVoicemailIdsV3> iVoicemailIds;
    RPointerArray<RMobilePhone::TMobilePhoneNetworkInfoV1> iNetworkInfoV1List;
    RPointerArray<RMobilePhone::TMobilePhoneNetworkInfoV2> iNetworkInfoV2List;
    RPointerArray<RMobilePhone::TMobilePhoneNetworkInfoV5> iNetworkInfoV5List;
    };

#endif // CMockPhoneMessHandler_H

