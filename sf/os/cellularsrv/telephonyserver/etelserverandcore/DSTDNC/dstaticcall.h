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

/**@file
   @internalAll */
#ifndef __DSTATICCALL_H
#define __DSTATICCALL_H

#include <e32base.h>
#include "ET_PHONE.H"
#include "../DSTD/ACQUIRE.H"

//
// CCallStaticCall
//
class CCallStaticCall : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallStaticCall* NewL(CPhoneFactoryDummyBase* aFac);
	CCallStaticCall(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CCallStaticCall();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes&);
	};

//
// CLineStaticCall
//
class CLineStaticCall : public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLineStaticCall* NewL(const TDesC& aName,CPhoneFactoryDummyBase* aFac);
	CLineStaticCall(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CLineStaticCall();
// virtual functions
	TInt EnumerateCall(const TTsyReqHandle aUid,TInt* aCount);
	virtual TInt GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes&);
public:
	CCallStaticCall* iCall;
	};

//
//	CPhoneStaticCall
//
class CPhoneStaticCall : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneStaticCall* NewL(CPhoneFactoryDummyBase* aFac);
private:
	CPhoneStaticCall(CPhoneFactoryDummyBase* aFac);
	~CPhoneStaticCall();
	void ConstructL();
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes&);
	virtual void Init();
private:
	TTsyReqHandle iNotifyPhoneDisappearedUid;
	};

//
// CPhoneFactoryDummyBase object
//
class CLibUnloader;
class CPhoneFactoryStaticCall: public CPhoneFactoryDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneFactoryStaticCall* NewL();
	CPhoneFactoryStaticCall();
	~CPhoneFactoryStaticCall();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	virtual TInt GetPhoneInfo(const TInt aIndex,RTelServer::TPhoneInfo& aPhoneInfo);
	virtual TInt EnumeratePhones();
private:
	RSemaphore iSemaphore;
	};

#endif
