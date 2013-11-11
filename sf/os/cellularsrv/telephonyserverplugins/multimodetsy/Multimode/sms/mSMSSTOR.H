// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll 
*/

#ifndef __MSMSSTOR_H__
#define __MSMSSTOR_H__

#include <et_phone.h>
#include "smsbase.H"

class CATIO;
class CATInit;
class CATSmsMessagingRead;
class CATSmsMessagingWrite;
class CATSmsStorageDelete;
class CATSmsMemoryStorage;
//class CATSmsMessagingReadAll; // Raed all functionality not in use at the momemnt. Henrik 13/06/01


//
// CMobileSmsStore
//
class CMobileSmsStore : public CSubSessionExtBase
	{
public:
	static CMobileSmsStore* NewL(CATIO* aATIO, CATInit* aInit, CPhoneGlobals* aPhoneGlobals, TStorageType aStoreType);
	CMobileSmsStore(CATIO* aATIO, CATInit* aInit, CPhoneGlobals* aPhoneGlobals, TStorageType aStoreType);
	~CMobileSmsStore();
	
	virtual CTelObject* OpenNewObjectByNameL(const TDesC&);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc, const TDataPackage& aPackage);
	virtual TInt NumberOfSlotsL(const TInt aIpc);
	virtual TInt RegisterNotification(const TInt aIpc);
	virtual TInt DeregisterNotification(const TInt aIpc);
	virtual CTelObject::TReqMode ReqModeL(const TInt aIPC);
	virtual TInt CancelService(const TInt aIpc, const TTsyReqHandle aTsyReqHandle);
	virtual void Init();
private:
	void ConstructL();
	TInt GetInfo(const TTsyReqHandle aTsyReqHandle,TDes8* aInfoPckg);
	TInt GetInfoCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Read(const TTsyReqHandle aTsyReqHandleconst,TDes8* aEntryPckg);
	TInt ReadCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Write(const TTsyReqHandle aTsyReqHandleconst, TDes8* aEntryPckg);
	TInt WriteCancel(const TTsyReqHandle aTsyReqHandle);
	TInt Delete(const TTsyReqHandle aTsyReqHandleconst, TInt* aIndex);
	TInt DeleteCancel(const TTsyReqHandle aTsyReqHandle);

public:
	TStorageType iStoreName;	
private:
	CATIO* iIo;
	CATInit* iInit;
	CPhoneGlobals* iPhoneGlobals;
	
	CATSmsMessagingWrite* iATSmsStorageWrite;
	CATSmsStorageDelete* iATSmsStorageDelete;
	CATSmsMessagingRead* iATSmsStorageRead; 
	CATSmsMemoryStorage* iATSmsStorageGetInfo;
	};

#endif
