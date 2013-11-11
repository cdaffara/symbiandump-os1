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
#ifndef __DSTD_H
#define __DSTD_H

#include <e32base.h>
#include "et_struct.h"
#include "ET_PHONE.H"
#include "../DSTD/ACQUIRE.H"
#include <et_clsvr.h>


/**
Simple extended error codes to allow testing of extended error code handling.
Valid licensee specific errors start at -21000 and go to -29999.
@internalComponent
*/
const TInt  KErrLicenseeSpecificCorrupt = -21001;
const TInt  KErrLicenseeSpecificOkay    = -21002;



// CFaxDstd

class CPhoneFactoryDummyBase;
class CFaxDstd : public CFaxDummyBase
/**
@internalComponent
*/
	{
public:
	static CFaxDstd* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxDstd(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CFaxDstd();
	RHandleBase* GlobalKernelObjectHandle();
private:
	RChunk iChunk;
	};


// CCallDstd

class CCallDstd : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallDstd* NewL(CPhoneFactoryDummyBase* aFac);
	CCallDstd(CPhoneFactoryDummyBase* aFac);
	~CCallDstd();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TReqMode ReqModeL(const TInt aIpc);
	};


// CLineDstd

class CLineDstd : public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLineDstd* NewL(CPhoneFactoryDummyBase* aFac);
	CLineDstd(CPhoneFactoryDummyBase* aFac);
	~CLineDstd();
public: // virtual functions
	virtual TInt EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount);
	virtual TInt GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
private:
	CPhoneFactoryDummyBase* iFac;
	TInt iNumberOfCalls;
	};


// CPhoneDstd
class CPhoneDstd : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneDstd* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneDstd(CPhoneFactoryDummyBase* aFac);
	~CPhoneDstd();
public:
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual void Init();
	virtual TSecurityPolicy GetRequiredPlatSecCaps(const TInt aIpc);
	TInt TestNarrowSetAndGet(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDes1, TDes8* aDes2);
	TInt TestUnicodeSetAndNarrowGet(const TTsyReqHandle aTsyReqHandle, TDes8* aDes1, const TDesC16* aDes2);
	TInt TestUnicodeSetAndGet(const TTsyReqHandle aTsyReqHandle, const TDesC16* aDes1, TDes16* aDes2);
	TInt TestNarrowSetAndUnicodeGet(const TTsyReqHandle aTsyReqHandle, const TDesC8* aDes1, TDes16* aDes2);
	TInt TestDoubleUnicodeGet(const TTsyReqHandle aTsyReqHandle, TDes16* aDes1 , TDes16* aDes2);
	};

//
// CFaxFc
//
class CFaxFc : public CFaxDummyBase
/**
@internalComponent
*/
	{
public:
	static CFaxFc* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxFc(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CFaxFc();
	RHandleBase* GlobalKernelObjectHandle();
private:
	RChunk iChunk;
	};

//
// CCallFc
//
class CCallFc : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallFc* NewL(CPhoneFactoryDummyBase* aFac);
	CCallFc(CPhoneFactoryDummyBase* aFac);
	~CCallFc();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	};

//
// CLineFc
//
class CLineFc : public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLineFc* NewL(CPhoneFactoryDummyBase* aFac);
	CLineFc(CPhoneFactoryDummyBase* aFac);
	~CLineFc();
public: // virtual functions
	virtual TInt EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount);
	virtual TInt GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
private:
	TInt iNumberOfCalls;
	};

//
//	CPhoneFc
//
class CPhoneFc : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneFc* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneFc(CPhoneFactoryDummyBase* aFac);
	~CPhoneFc();
public:
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	virtual void Init();
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
public:
	void PhoneInit();
	static TInt OpenPhoneHandler(TAny* aPtr);
private:
	TTsyTimer iOpenPhone;
	};

//
// CFaxPm
//
class CFaxPm : public CFaxDummyBase
/**
@internalComponent
*/
	{
public:
	static CFaxPm* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxPm(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CFaxPm();
	RHandleBase* GlobalKernelObjectHandle();
private:
	RChunk iChunk;
	};

//
// CCallPm
//
class CCallPm : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallPm* NewL(TInt& aNumberOfCalls,	CPhoneFactoryDummyBase* aFac);
	CCallPm(TInt& aNumberOfCalls,CPhoneFactoryDummyBase* aFac);
	~CCallPm();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	CAcquireEntry* FindByTsyReqHandle(const TTsyReqHandle aUid);
	void RemoveAcquireEntry(CAcquireEntry* aEntry);
private:
	TInt& iNumberOfCalls;
	};

//
// CLinePm
//
class CLinePm : public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLinePm* NewL(CPhoneFactoryDummyBase* aFac);
	CLinePm(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CLinePm();
public: // virtual functions
	virtual TInt EnumerateCall(const TTsyReqHandle aTsyReqHandle,TInt* aCount);
	virtual TInt GetCallInfo(const TTsyReqHandle aTsyReqHandle,TCallInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
private:
	TInt iNumberOfCalls;
	TName iCallName;
	};

//
//	CPhonePm
//
class CPhonePm : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhonePm* NewL(CPhoneFactoryDummyBase* aFac);
	CPhonePm(CPhoneFactoryDummyBase* aFac);
	~CPhonePm();
public:
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	virtual void Init();
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	};

//
// CPhoneFactoryDstd object
//
class CLibUnloader;
class CPhoneFactoryDstd: public CPhoneFactoryDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneFactoryDstd* NewL();
	CPhoneFactoryDstd();
	~CPhoneFactoryDstd();
	virtual TInt GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aPhoneFactoryBaseInfo);
	virtual TInt EnumeratePhones();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
private:
	static void CloseObj(TAny* aObj);
	};

// CPhoneDstdSlow
class CPhoneDstdSlow : public CPhoneDstd
/**
@internalComponent
*/
	{
public:
	static CPhoneDstdSlow* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneDstdSlow(CPhoneFactoryDummyBase* aFac);
	~CPhoneDstdSlow();
public:
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	};

#endif
