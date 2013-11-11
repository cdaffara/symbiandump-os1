// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains definitions for classes that implement network orientated functionality.
// 
//

/**
 @file
*/

#ifndef CSIMCALLBarring_H__
#define CSIMCALLBarring_H__

#include <etelmm.h>
#include <et_phone.h>
#include <mmretrieve.h>
#include "CSimSmsStore.h"//ugly but needed for listReadAllAttempt


class CSimPhone;
class CTestConfigSection;
class CRetrieveMobilePhoneCBList;
class CListReadAllAttempt;

class CSimCallBarring : public CBase
/**
 * Implements the RMobilePhone based functions that constitute the callBarring orientated
 * functionality provided by the SIM TSY.
 */
	{
public:
	static CSimCallBarring* NewL(CSimPhone* aPhone);
	CSimCallBarring(CSimPhone* aPhone);
	~CSimCallBarring();
	
	TInt ExtFunc(const TTsyReqHandle aReqHandle,const TInt aIpc, const TDataPackage& aPckg);
	TInt CancelService(const TInt aIpc, const TTsyReqHandle aReqHandle);
	TInt SetCallBarringStatus(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCBCondition* aCB,  RMobilePhone::TMobilePhoneCBChangeV1* aInfo);
	TInt SetCallBarringStatusCancel(const TTsyReqHandle aReqHandle);
	TInt SetCallBarringPassword(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhonePasswordChangeV1* aInfo);
	TInt SetCallBarringPasswordCancel(const TTsyReqHandle aReqHandle);
	TInt NotifyCallBarringStatusChange(const TTsyReqHandle aReqHandle, RMobilePhone::TMobilePhoneCBCondition* aCB);
	TInt NotifyCallBarringStatusChangeCancel(const TTsyReqHandle aReqHandle);
	
	TInt GetCallBarringStatusPhase1(const TTsyReqHandle aTsyReqHandle, CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData,TInt* aBufSize);
	TInt ProcessGetCallBarringStatusPhase1L(const TTsyReqHandle aTsyReqHandle,CRetrieveMobilePhoneCBList::TGetCallBarringRequest* aReqData,TInt* aBufSize);
	TInt GetCallBarringStatusPhase2(const TTsyReqHandle aTsyReqHandle, RMobilePhone::TClientId* aClient, TDes8* aBuf);
	TInt GetCallBarringStatusCancel(const TTsyReqHandle aTsyReqHandle);

	void UpdateCBListL(RMobilePhone::TMobilePhoneCBCondition* aCB, RMobilePhone::TMobilePhoneCBChangeV1* aCBInfo );
	void CBActivateAllOutgingL(TDesC* codeString);
	void CBDeactivateAllOutgingL(TDesC* codeString);
	void FindAndCreateCBListL();
	
	const CTestConfigSection* CfgFile();

private:
	void ConstructL();
	

private:
	CSimPhone* iPhone;			// < Pointer to the parent phone class.
	
	struct TCBNotification
		{
		TBool iCBChangeInfoNotificationPending;
		TTsyReqHandle iCBChangeInfoReqHandle;
		RMobilePhone::TMobilePhoneCBCondition* iCurrentCBCondition;
		};
		
	TCBNotification iCBNotification;
	
	RMobilePhone::TMobilePassword iPassword;
	
	CArrayPtrFlat<CListReadAllAttempt>* iGetCBStatus;
	CMobilePhoneCBList* iCBList; //The list for call barring.
	
	};

#endif
