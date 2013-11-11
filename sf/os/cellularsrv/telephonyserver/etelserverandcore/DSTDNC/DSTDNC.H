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
#ifndef __DSTDNC_H
#define __DSTDNC_H

#include <e32base.h>
#include "et_struct.h"
#include "ET_PHONE.H"
#include "../DSTD/ACQUIRE.H"

//
// CFaxDstdNc
//
class CPhoneFactoryDstdNc;
class CFaxDstdNc : public CFaxDummyBase
/**
@internalComponent
*/
	{
public:
	static CFaxDstdNc* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxDstdNc(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	RHandleBase* GlobalKernelObjectHandle();
	~CFaxDstdNc();
private:
	RChunk iChunk;
	};

//
// CCallDstdNc
//
class CCallDstdNc : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallDstdNc* NewL(CPhoneFactoryDummyBase* aFac);
	CCallDstdNc(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CCallDstdNc();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes&);
private:
	TBool iLoanCommPort;
	};

//
// CLineDstdNc
//
class CLineDstdNc : public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLineDstdNc* NewL(const TDesC& aName,CPhoneFactoryDummyBase* aFac);
	CLineDstdNc(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CLineDstdNc();
public: // virtual functions
	TInt EnumerateCall(const TTsyReqHandle aUid,TInt* aCount);
	virtual TInt GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes&);
	};

//
//	CPhoneDstdNc
//
class CPhoneDstdNc : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneDstdNc* NewL(CPhoneFactoryDummyBase* aFac);
private:
	CPhoneDstdNc(CPhoneFactoryDummyBase* aFac);
	~CPhoneDstdNc();
	void ConstructL();
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes&);
	virtual void Init();
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual TSecurityPolicy GetRequiredPlatSecCaps(const TInt aIpc);
private:
	TTsyReqHandle iNotifyPhoneDisappearedUid;
	};

//
// CPhoneFactoryDummyBase object
//
class CLibUnloader;
class CPhoneFactoryDstdNc: public CPhoneFactoryDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneFactoryDstdNc* NewL();
	CPhoneFactoryDstdNc();
	~CPhoneFactoryDstdNc();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	virtual TInt GetPhoneInfo(const TInt aIndex,RTelServer::TPhoneInfo& aPhoneInfo);
	virtual TInt EnumeratePhones();
private:
	RSemaphore iSemaphore;
	};

#endif
