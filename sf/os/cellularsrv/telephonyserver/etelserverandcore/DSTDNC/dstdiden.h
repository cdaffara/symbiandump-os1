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
#ifndef __DSTDIDEN_H
#define __DSTDIDEN_H

#include <e32base.h>
#include "et_struct.h"
#include "ET_PHONE.H"
#include "../DSTD/ACQUIRE.H"
#include <et_clsvr.h>

//
// CFaxDstdIden
//
class CPhoneFactoryDummyBase;
class CFaxDstdIden : public CFaxDummyBase
/**
@internalComponent
*/
	{
public:
	static CFaxDstdIden* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxDstdIden(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	RHandleBase* GlobalKernelObjectHandle();
	~CFaxDstdIden();
	
private:
	RChunk iChunk;
	};

//
// CCallDstdIden
//
class CCallDstdIden : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallDstdIden* NewL(CPhoneFactoryDummyBase* aFac);
	CCallDstdIden(CPhoneFactoryDummyBase* aFac);
	~CCallDstdIden();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual TReqMode ReqModeL(const TInt aIpc);
	};

//
// CLineDstdIden
//
class CLineDstdIden: public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLineDstdIden* NewL(CPhoneFactoryDummyBase* aFac);
	CLineDstdIden(CPhoneFactoryDummyBase* aFac);
	~CLineDstdIden();
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

//
//	CPhoneDstdIden
//
class CPhoneDstdIden : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneDstdIden* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneDstdIden(CPhoneFactoryDummyBase* aFac);
	~CPhoneDstdIden();
public:
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual TReqMode ReqModeL(const TInt aIpc);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	virtual void Init();
	};

//
// CFaxFcIden
//
class CFaxFcIden : public CFaxDummyBase
/**
@internalComponent
*/
	{
public:
	static CFaxFcIden* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxFcIden(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	RHandleBase* GlobalKernelObjectHandle();

	~CFaxFcIden();
private:
	RChunk iChunk;
	};

//
// CCallFcIden
//
class CCallFcIden : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallFcIden* NewL(CPhoneFactoryDummyBase* aFac);
	CCallFcIden(CPhoneFactoryDummyBase* aFac);
	~CCallFcIden();
public:
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	};

//
// CLineFcIden
//
class CLineFcIden : public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLineFcIden* NewL(CPhoneFactoryDummyBase* aFac);
	CLineFcIden(CPhoneFactoryDummyBase* aFac);
	~CLineFcIden();
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
//	CPhoneFcIden
//
class CPhoneFcIden : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhoneFcIden* NewL(CPhoneFactoryDummyBase* aFac);
	CPhoneFcIden(CPhoneFactoryDummyBase* aFac);
	~CPhoneFcIden();
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
// CFaxPmIden
//
class CFaxPmIden : public CFaxDummyBase
/**
@internalComponent
*/
	{
public:
	static CFaxPmIden* NewL(CPhoneFactoryDummyBase* aFac);
	CFaxPmIden(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	RHandleBase* GlobalKernelObjectHandle();
	~CFaxPmIden();
private:
	RChunk iChunk;
	};

//
// CCallPmIden
//
class CCallPmIden : public CCallDummyBase
/**
@internalComponent
*/
	{
public:
	static CCallPmIden* NewL(TInt& aNumberOfCalls,	CPhoneFactoryDummyBase* aFac);
	CCallPmIden(TInt& aNumberOfCalls,CPhoneFactoryDummyBase* aFac);
	~CCallPmIden();
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
// CLinePmIden
//
class CLinePmIden : public CLineDummyBase
/**
@internalComponent
*/
	{
public:
	static CLinePmIden* NewL(CPhoneFactoryDummyBase* aFac);
	CLinePmIden(CPhoneFactoryDummyBase* aFac);
	void ConstructL();
	~CLinePmIden();
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
class CPhonePmIden : public CPhoneDummyBase
/**
@internalComponent
*/
	{
public:
	static CPhonePmIden* NewL(CPhoneFactoryDummyBase* aFac);
	CPhonePmIden(CPhoneFactoryDummyBase* aFac);
	~CPhonePmIden();
public:
	virtual TInt EnumerateLines(const TTsyReqHandle aTsyReqHandle,TInt*);
	virtual TInt GetLineInfo(const TTsyReqHandle aTsyReqHandle,TLineInfoIndex*);
	virtual void Init();
	virtual TInt ExtFunc(const TTsyReqHandle aTsyReqHandle,const TInt aIpc,const TDataPackage&);
	virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	virtual CTelObject* OpenNewObjectL(TDes& aNewName);
	};

//
// CPhoneFactoryDstdIden object
//
class CLibUnloader;
/**
@internalComponent
*/
class CPhoneFactoryDstdIden : public CPhoneFactoryDummyBase
	{
public:
	static CPhoneFactoryDstdIden* NewL();
	CPhoneFactoryDstdIden();
	~CPhoneFactoryDstdIden();
	virtual TInt GetPhoneInfo(const TInt aIndex, RTelServer::TPhoneInfo& aPhoneFactoryBaseInfo);
	virtual TInt EnumeratePhones();
	virtual CPhoneBase* NewPhoneL(const TDesC& aName);
	};

#endif
