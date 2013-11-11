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
 @publishedPartner
 @released
*/

#ifndef __ET_PHONE_H
#define __ET_PHONE_H

#include <etel.h>
#include <et_tsy.h>

#include <f32file.h>

//
// CReqEntry class
//
class CTelSession;
class CTelObject;
class CBuffer;
class CReqEntry : public CBase	
/**
@publishedPartner
*/
	{
friend class CPhoneBase;
public:

	static CReqEntry* NewL(TTsyReqHandle aTsyReqHandle,const RMessage2& aMessage,CTelSession* aSession,CBuffer* aBuffer,const CTelObject* aTelObject,TInt aFunction,RHeap* aHeap);
	CReqEntry(TTsyReqHandle aTsyReqHandle,const RMessage2& aMessage,CTelSession* aSession,CBuffer* aBuffer,const CTelObject* aTelObject,TInt aFunction,RHeap* aHeap);
	~CReqEntry();
	void Deque();
	void CompleteAndDeque(TInt aError);
//
	static TAny* operator new(TUint aSize) __NO_THROW;
	static TAny* operator new(TUint aSize,TLeave aLeave);
	static TAny* operator new(TUint aSize,TUint aExtraSize) __NO_THROW;
	static TAny* operator new(TUint aSize,RHeap* aHeap) __NO_THROW;	// for a priority client request, the
													// objects created must all be taken from
												// the priority heap.
    static void operator delete(TAny* aPtr);
    static void operator delete(TAny* aPtr, TLeave aLeave);
    static void operator delete(TAny* aPtr, TUint aExtraSize);
    static void operator delete(TAny* aPtr, RHeap* aHeap);

public:
	TTsyReqHandle iTsyReqHandle;
	RMessage2 iMessage;

	CTelSession* iSession;
	const CTelObject* iTelObject;
	TBool iClientInterested;
	TBool iCancelFnCalled;
	TInt iFunction;
	CBuffer* iBuffer;
	TBool iReadByClient;
	TBool iMessageNulled;
	TBool iPlacedRequest;
	MTelObjectTSY::TReqMode iReqMode;
private:
	TDblQueLink iLink;
public:
	RHeap* iHeap;	// a pointer to the priority heap, used during destruction
	};

//
// CBuffer class
//
class HEtelBufC8;
class HEtelBufC16;
class CBuffer : public CBase
/**
@publishedPartner
*/
	{
public:
	enum TWhichSlot
		{
		ESlotRead,
		ESlotWrite
		};
	static CBuffer* NewL(HEtelBufC8* aBuf8,HEtelBufC16* aBuf16,RHeap* aHeap,TInt aSizeOfData1,TInt aSizeOfData2,TInt aNoOfSlots);
	CBuffer(HEtelBufC8* aBuf8,HEtelBufC16* aBuf16,RHeap* aHeap,TInt aSizeOfData1,TInt aSizeOfData2,TInt aNoOfSlots);
	CBuffer(HEtelBufC16* aBuf,RHeap* aHeap,TInt aSizeOfData1,TInt aSizeOfData2,TInt aNoOfSlots);
	~CBuffer();
	TBool OverFlow() const;
	TBool BufferFull() const;
	TUint Size() const;
	TUint8* CurrentSlotData1(TWhichSlot aWhichSlot) const;
	TUint8* CurrentSlotData2(TWhichSlot aWhichSlot) const;
	TUint16* CurrentSlotData1u(TWhichSlot aWhichSlot) const;
	TUint16* CurrentSlotData2u(TWhichSlot aWhichSlot) const;
	void IncRead();
	void IncWrite();
	TInt CompareRWPtrs() const;
	TBool EqualSizes(TInt aSize1, TInt aSize2) const;
//
	static TAny* operator new(TUint aSize) __NO_THROW;
	static TAny* operator new(TUint aSize,TLeave aLeave);
	static TAny* operator new(TUint aSize,TUint aExtraSize) __NO_THROW;
	static TAny* operator new(TUint aSize,RHeap* aHeap) __NO_THROW;
    static void operator delete(TAny* aPtr);
    static void operator delete(TAny* aPtr, TLeave aLeave);
    static void operator delete(TAny* aPtr, TUint aExtraSize);
    static void operator delete(TAny* aPtr, RHeap* aHeap);

public:
	HEtelBufC8* iBuf8;	// ptr to buffer if narrow
	HEtelBufC16* iBuf16;	// ptr to buffer if unicode
	RHeap* iHeap;	// a pointer to the priority heap, used during destruction
private:
	TInt iRead;
	TInt iWrite;
	TInt iNoOfSlots;
	TInt iSizeOfData1;
	TInt iSizeOfData2;
	TBool iOverFlow;
	TBool iBufferFull;
	};

//
//	CTelObject
//
class CDestroyDummySubSession;
class CTelServer;
class CPhoneBase;
class CPhoneManager;
class CTelObject : public CObject, public MTelObjectTSY
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C CTelObject();
	IMPORT_C virtual ~CTelObject();
	IMPORT_C virtual void ReqCompleted(const TTsyReqHandle aTsyReqHandle,const TInt aError);
	IMPORT_C virtual void FlowControlSuspend();
	IMPORT_C virtual void FlowControlResume();
	IMPORT_C virtual TInt UpCallOption(const TInt aOptionNumber, TAny* aData);
	IMPORT_C virtual TInt DownCallOption(const TInt aOptionNumber, TAny* aData);	// so TSY doesn't have to implement it
	IMPORT_C virtual TInt ServerVersion() const;
	IMPORT_C virtual TSecurityPolicy GetRequiredPlatSecCaps(const TInt aIpc);

	CPhoneBase* PhoneOwner() const;
	void SetPhoneOwner(CPhoneBase* aPhoneOwner);
	void FlushReqs(CTelSession* aSession,const TInt aSubSessionHandle);
	void GeneralReq(const RMessage2& aMessage,CTelSession* aSession,CReqEntry* aNewReqEntry,TBool aFromFlowControlResume=EFalse);

	void TelObjectClose();
	TDes8* BufferDes1(const CBuffer* aBuffer,CBuffer::TWhichSlot aWhichSlot) const;
	TDes8* BufferDes2(const CBuffer* aBuffer,CBuffer::TWhichSlot aWhichSlot) const;
	TDes16* BufferDes1u(const CBuffer* aBuffer,CBuffer::TWhichSlot aWhichSlot) const;
	TDes16* BufferDes2u(const CBuffer* aBuffer,CBuffer::TWhichSlot aWhichSlot) const;
	TUint8* Ptr1(const TDes8* aDes1) const;

	void WriteBackAndCompleteBufferedReq(const RMessage2& aMessage,CReqEntry* aReqEntry,TInt aError) const;
	void WriteBackAndCompleteReq(CReqEntry* aReqEntry,const TInt aError) const;
	void UpdateAndCompleteIfNecessary(CReqEntry* aReqEntry,TInt aError) const;
	void CancelSubSession(CTelSession* aSession,const TInt aSubSessionHandle);
	void CreateDummySession(CTelSession* aSession,const TInt aSubSessionHandle, TBool aCreateDummy=EFalse);
	void CheckAndDestroyDummySubSession();
	void RemoveDummySubSessionDestroyer();
	TInt ResolveError(CTelSession* aSession, const TInt aError) const;
	void CreateDummySessionObjectL(CTelSession* aTelSession);
	
//
// Pure Virtual
//
	virtual void OpenPostProcessing(CTelSession* aSession,const TInt aSubSessionHandle)=0;
	virtual void CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle)=0;
	virtual TInt Service(const RMessage2& aMessage,CReqEntry* aReqEntry)=0;	

	
protected:
	TBool IsPriorityClientReq(TInt aReq) const;
private:
	void CompleteAndDestroyReq(CReqEntry* aReqEntry,const TInt aStatus) const;
	void DestroyReq(CReqEntry* aReqEntry) const;

	CReqEntry* ReqAnalyserL(const RMessage2& aMessage,CTelSession* aSession,TReqMode& aReqMode);
	CReqEntry* CheckForSimilarRequestAndCreateL(const RMessage2& aMessage,CTelSession* aSession) const;

	void CompleteOrBufferRead(const RMessage2& aMessage,CReqEntry* aReqEntry);

	TBool IsActiveReq(CTelSession* aSession,const TInt aSubSessionHandle);
	void CancelActiveReq(CTelSession* aSession,const TInt aSubSessionHandle);
	void CheckAndResize(TInt& aSizeOfData,const RMessage2& aMessage) const;


	HEtelBufC8* CreateNarrowBufferLC(RHeap* aHeap,const RMessage2& aMessage,TUint aIndexOfClientPtr1,TUint aIndexOfClientPtr2,TInt& aSize1,TInt& aSize2,TInt aNoOfSlots) const;
	HEtelBufC16* CreateUnicodeBufferLC(RHeap* aHeap,const RMessage2& aMessage,TUint aIndexOfClientPtr1,TUint aIndexOfClientPtr2,TInt& aSize1,TInt& aSize2,TInt aNoOfSlots) const;
	TBool IsSameMessageType(const RMessage2& aMessage, CReqEntry* aReqEntry) const;
	void GetMessageDescriptorSizes(const RMessage2& aMessage,TInt &aSize1, TInt &aSize2) const;
private:// data
	CPhoneBase* iPhoneOwner;
	TInt iActiveReqCount;
	CTelServer* iTelServer;
	CDestroyDummySubSession* iDestroyDummySubSession;
	TBool iCreateDummy;
	IMPORT_C virtual void CTelObject_Reserved1();
	TAny* iCTelObject_Reserved;
	};

//
// CSubSessionExtBase
//
class CSubSessionExtBase : public CTelObject, public MSubSessionExtBaseTSY
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C CSubSessionExtBase();
	IMPORT_C virtual ~CSubSessionExtBase();

	TInt ServiceExtFunc(const RMessage2& aMessage,CReqEntry* aReqEntry);

protected:
	IMPORT_C virtual TInt Service(const RMessage2& aMessage,CReqEntry* aReqEntry);

	IMPORT_C virtual void OpenPostProcessing(CTelSession*,const TInt);
	IMPORT_C virtual void CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle);
	IMPORT_C void NullMethod1();	
	IMPORT_C virtual RHandleBase* GlobalKernelObjectHandle();
private:
	inline void Inc();
	};

//
// CFaxBase Object
//
class CFaxBase : public CTelObject, public MFaxBaseTSY
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C CFaxBase();
	IMPORT_C virtual ~CFaxBase();
	IMPORT_C virtual TReqMode ReqModeL(const TInt aIpc);
	IMPORT_C virtual TInt NumberOfSlotsL(const TInt aIpc);
	IMPORT_C virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
//
// Pure Virtual
//
private:
	IMPORT_C virtual CTelObject* OpenNewObjectByNameL(const TDesC& aName);
	IMPORT_C virtual CTelObject* OpenNewObjectL(TDes& aName);
	IMPORT_C virtual TInt Service(const RMessage2& aMessage,CReqEntry* aReqEntry);

	IMPORT_C virtual void OpenPostProcessing(CTelSession*,const TInt);
	IMPORT_C virtual void CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle);
	IMPORT_C virtual void Init();
	IMPORT_C void NullMethod2();	 
	};

//
// CCallBase
//
class CCallBase : public CSubSessionExtBase, public MCallBaseTSY
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C CCallBase();
	IMPORT_C virtual ~CCallBase();


	IMPORT_C TCallOwnership CheckOwnership(const TTsyReqHandle aTsyReqHandle) const;
	IMPORT_C TInt SetUnowned();
	IMPORT_C TInt SetOwnership(const TTsyReqHandle aTsyReqHandle);
	IMPORT_C TBool CheckPriorityClient(const TTsyReqHandle aTsyReqHandle) const;
	IMPORT_C CCallBase* ResolveSubSessionHandle(const TTsyReqHandle aTsyReqHandle,const TInt aSubSessionHandle);
	IMPORT_C void RelinquishOwnershipCompleted(const TInt aError);
	IMPORT_C void RecoverDataPortAndRelinquishOwnershipCompleted(const TInt aError);
	IMPORT_C RFax::TProgress* CreateFaxProgressChunk();
	IMPORT_C void DeleteFaxProgressChunk();
	IMPORT_C virtual TReqMode ReqModeL(const TInt aIpc);
	IMPORT_C virtual TInt NumberOfSlotsL(const TInt aIpc);
	IMPORT_C virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
	IMPORT_C virtual RHandleBase* GlobalKernelObjectHandle();
	IMPORT_C virtual TInt SetFaxSharedHeaderFile(const TTsyReqHandle aTsyReqHandle, CFaxSharedFileHandles* aFaxSharedFileHandles);

private:
	IMPORT_C RCall::TOwnershipStatus CheckOwnershipBySession(CTelSession* aSession,const TInt aSubSession) const;
	IMPORT_C virtual TInt Service(const RMessage2& aMessage,CReqEntry* aReqEntry);

	IMPORT_C virtual void Init();
	IMPORT_C virtual void OpenPostProcessing(CTelSession*,const TInt);
	IMPORT_C virtual void CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle);
	
	TCallOwnership iOwnershipStatus;
	CTelSession* iOwnerSession;
	TInt iOwnerSubSessionHandle;
	TBool iLoanDataPort;
	RChunk iChunk;
	};

//
// CLineBase object
//
class CCallBase;
class CLineBase : public CSubSessionExtBase, public MLineBaseTSY
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C CLineBase();
	IMPORT_C virtual ~CLineBase();
	IMPORT_C virtual TReqMode ReqModeL(const TInt aIpc);
	IMPORT_C virtual TInt NumberOfSlotsL(const TInt aIpc);
	IMPORT_C virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
private:
	IMPORT_C virtual TInt Service(const RMessage2& aMessage,CReqEntry* aReqEntry);

	IMPORT_C virtual void Init();
	IMPORT_C virtual void OpenPostProcessing(CTelSession*,const TInt);
	IMPORT_C virtual void CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle);
	
	};

//
// CPhoneBase object
//
class CPhoneBase : public CSubSessionExtBase, public MPhoneBaseTSY
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C CPhoneBase();
	IMPORT_C virtual ~CPhoneBase();
	IMPORT_C virtual TReqMode ReqModeL(const TInt aIpc);
	IMPORT_C virtual TInt NumberOfSlotsL(const TInt aIpc);
	IMPORT_C virtual TInt CancelService(const TInt aIpc,const TTsyReqHandle aTsyReqHandle);
	IMPORT_C TBool CheckEmergencyClient(const TTsyReqHandle aTsyReqHandle) const;
	
	void FlowControlInc();
	void FlowControlDec();
	TInt FlowControl() const;
	CReqEntry* ActivateNextWaitingReq();
	CReqEntry* FindByTsyHandle(const TTsyReqHandle aTsyReqHandle);
	CReqEntry* FindByTsyHandleAndPlacedRequest(const TTsyReqHandle aTsyReqHandle);
	void UpdateBuffer(CReqEntry* aUpdatedReqEntry,CReqEntry* aReqEntry);
	CReqEntry* FindClientInWaiting(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc);
	CReqEntry* FindClientInActive(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc);
	CReqEntry* FindByIpcAndTelObject(const TInt aIpc, const CTelObject* aTelObject, const TUint aBufSize);
	CReqEntry* FindByIpcAndTelObjectInWaiting(const TInt aIpc, const CTelObject* aTelObject, const TUint aBufSize);
	void AddReqToActive(CReqEntry* aReqEntry);
	void AddReqToWaiting(CReqEntry* aReqEntry);

	CReqEntry* NewReqL(const RMessage2& aMessage,CTelSession* aSession,CBuffer* aBuffer,const CTelObject* aTelObject,TInt aFunction);
	CReqEntry* FindSameClientEntry(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc);
	CReqEntry* FindClientReqInWaitList(CTelSession* aSession,const TInt aSubSessionHandle);
	CReqEntry* FindThisReqByAnotherClient(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc,const TUint aBufSize,const CTelObject* aThisTelObject);
	CReqEntry* FindNonCancelledClientReq(CTelSession* aSession,const TInt aSubSessionHandle,const TInt aIpc);
	void CheckAndCompleteAllActive(CReqEntry* aUpdatedReqEntry,const TReqMode aReqMode,const TInt aIpc,const TInt aError);
	TDblQue<CReqEntry>& ReqActiveList();
private:

	IMPORT_C virtual TInt Service(const RMessage2& aMessage,CReqEntry* aReqEntry);
	IMPORT_C virtual void OpenPostProcessing(CTelSession* aSession,const TInt aSubSessionHandle);
	IMPORT_C virtual void CloseSubSessionPreProcessing(CTelSession* aSession,const TInt aSubSessionHandle);
	
	TTsyReqHandle TsyReqHandle();
	TInt iTsyReqHandleCnt;
	TInt iFlowControlCnt;
	TDblQue<CReqEntry> iReqWaitList;
	TDblQue<CReqEntry> iReqActiveList;
	};


class CLibUnloader;

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <et_phone_util.h>
#endif

//
// CPhoneBase factory object
//
class CPhoneFactoryBase : public CObject, public MPhoneFactoryBaseTSY
/**
@publishedPartner
@released
*/
	{
public:
	IMPORT_C CPhoneFactoryBase();
	IMPORT_C virtual ~CPhoneFactoryBase();

	IMPORT_C virtual TBool QueryVersionSupported(TVersion const& aVersion) const;
	void ConstructL(RLibrary& aLib);
	inline void Inc();
	inline TVersion TsyVersionNumber() const;
protected:
	TVersion iVersion;
private:
	CLibUnloader* iLibUnloader;
	IMPORT_C virtual void CPhoneFactoryBase_Reserved1();
	TAny* iCPhoneFactoryBase_Reserved;
	};

//
// HEtelBufC8 class - this is our version of HBufC8 which will allocate to the 
// priority client heap if aHeap is not NULL (i.e. this is a priority client 
// request)
//

class HEtelBufC8
/**
@publishedPartner
*/
	{
public:
	static HEtelBufC8* NewMaxLC(TInt aMaxLength,RHeap* aHeap);
	TPtr8 Des();
	const TUint8* Ptr() const;
	static void operator delete(TAny* aPtr);
protected:
	RHeap* iHeap;	// pointer to the priority heap. Used during destruction
	TInt iLength;
	};

//
// and the unicode version...
//							 

class HEtelBufC16
/**
@publishedPartner
*/
	{
public:
	static HEtelBufC16* NewMaxLC(TInt aMaxLength,RHeap* aHeap);
	TPtr16 Des();
	const TUint16* Ptr() const;
	static void operator delete(TAny* aPtr);
protected:
	RHeap* iHeap;
	TInt iLength;
	};

/**
Utility class that provides a container for holding an adopted file handle passed between a client and server.

As part of the classes instantiation it takes in an RMessage2 reference which it uses to call RFile::AdoptFromClient(). 
Once the file been successfully adopted it is the responsibility of the adoptee to close the file when finished.  Therefore the 
destructor of the class calls RFile::Close() to close the adopted file handle on deletion of this class.

This class is only intended to be instantiated from within Etel which is why the factory constructor is not exported.  Etel passes a pointer to TSYs as part of transmitting a fax.

@publishedPartner
@released Released in v9.0 to aid passing of adopted file handle between Etel server, TSY and Fax server.
*/
class CFaxSharedFileHandles : CBase
	{
public:
	static CFaxSharedFileHandles* NewL(const RMessage2& aMsg);
	IMPORT_C  ~CFaxSharedFileHandles();
	IMPORT_C RFile& File();
protected:
	CFaxSharedFileHandles();
	void ConstructL(const RMessage2& aMsg);
private:
	RFile iFile;
	};
#include <et_phone.inl>

#endif
