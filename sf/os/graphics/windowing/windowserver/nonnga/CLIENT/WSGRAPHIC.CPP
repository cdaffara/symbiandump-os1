// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The client-side representation of a WsGraphic artwork
// 
//

#include <w32std.h>
#include "../SERVER/w32cmd.h"
#include "w32comm.h"
#include "CLIENT.H"
#include <s32mem.h>
#include <graphics/wsgraphicdrawerinterface.h>
/** Panics the client. This will result in the client thread being destroyed. */
GLREF_C void Panic(TW32Panic aPanic);

NONSHARABLE_STRUCT(CWsGraphic::CPimpl): public CBase, public MWsClientClass
/** @internalComponent @released */
	{
	friend class CWsGraphic;
	enum
		{
		/** is registered in the graphic manager's array */
		ERegistered = 0x01,
		/** has a peer CWsGraphicDrawer on the server */
		EHasPeer	= 0x02
		};
	CPimpl(CWsGraphic& aGraphic);
	~CPimpl();
	void ConstructL();		//LeaveScan: Member of macroised structure declaration.
	CManager* iManager;
	TInt WriteCreateGraphic(TWsClCmdCreateGraphic& aCreateGraphic,const TDesC8& aData,TInt aWsHandle) const;
	CWsGraphic& iGraphic;
	TWsGraphicId iId;
	TUint iFlags;
	MWsObjectProvider* iExt;
	};

LOCAL_C void MWsGraphicMessageAllocRelease(TAny* aAny)
	{
	MWsGraphicMessageAlloc::MBuffer* buf = static_cast<MWsGraphicMessageAlloc::MBuffer*>(aAny);
	if(buf)
		{
		buf->Release();
		}
	}

NONSHARABLE_CLASS(CWsGraphic::CManager): private CActive, public RWsSession
/** Client-side manager singleton for marshalling messages for all CWsGraphics owned by a client

@publishedAll
@released
*/	{
public:
	static CManager* StaticL();			  //LeaveScan:  Member of macroised structure declaration.
	// used by CWsGraphic
	void AddL(CWsGraphic* aGraphic);	  //LeaveScan:  Member of macroised structure declaration.
	void Replace(CWsGraphic* aGraphic);
	void Remove(CWsGraphic* aGraphic);
	CWsGraphic* Find(const TWsGraphicId& aId);
	void Inc();
	void Dec();
	RWsBuffer* Buffer();
	void ScheduleFlush();
private:
	CManager();
	~CManager();
	void ConstructL();					 //LeaveScan:  Member of macroised structure declaration.
	void Queue();
	void RunL();						 //LeaveScan:  Member of macroised structure declaration.
	void DoCancel();
	static TInt GraphicCompare(const CWsGraphic& aFirst,const CWsGraphic& aSecond);
	static TInt FlushOnIdle(TAny* aArg);
private:
	TInt iRefCount;
	RPointerArray<CWsGraphic> iArray;
	CIdle* iFlusher;
	TBool iFlushScheduled;
	};

// TWsGraphicId \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C TWsGraphicId::TWsGraphicId(TUid aUid):
/** Construct a UID
    @param aUid UID of the graphic artwork.
    @publishedAll @released
*/	iFlags(EWsGraphicIdUid), iId(aUid.iUid)
	{
	}

EXPORT_C TWsGraphicId::TWsGraphicId(TInt aId):
/** Construct a transient Id
	@publishedAll @released
*/	iFlags(EWsGraphicIdTransient), iId(aId)
	{
	}

/** 
Copy constructor.
@param aCopy Graphic artwork Id.
*/
EXPORT_C TWsGraphicId::TWsGraphicId(const TWsGraphicId& aCopy):
	iFlags(aCopy.iFlags), iId(aCopy.iId)
	{
	}

EXPORT_C TUid TWsGraphicId::Uid() const
/** Returns UID.
    @return UID of graphic artwork. KNullUid if graphic artwork is transient.
    @publishedAll @released
*/	{
	if(IsUid())
		{
		return TUid::Uid(iId);
		}
	return KNullUid;
	}

EXPORT_C TBool TWsGraphicId::IsUid() const
/** Identifies whether graphic artwork is non-transient.
    @return ETrue if graphic artwork is non-transient.
    @publishedAll @released
*/	{
	return (iFlags & EWsGraphicIdUid);
	}

EXPORT_C void TWsGraphicId::Set(TUid aUid)
/** Set to be a UID
	@publishedAll @released
*/	{
	iId = aUid.iUid;
	iFlags = EWsGraphicIdUid;
	}

EXPORT_C TInt TWsGraphicId::Id() const
/** Returns the transient Id.
    @return Id of transient graphic artwork. Zero if graphic artwork is non-transient.
	@publishedAll @released
*/	{
	if(IsId())
		{
		return iId;
		}
	return 0;
	}

EXPORT_C TBool TWsGraphicId::IsId() const
/** Identifies whether graphic artwork is transient.
    @return ETrue if graphic artwork is transient.
    @publishedAll @released
*/	{
	return (iFlags & EWsGraphicIdTransient);
	}

EXPORT_C void TWsGraphicId::Set(TInt aId)
/** Set to be a transient Id
	@publishedAll @released
*/	{
	iId = aId;
	iFlags = EWsGraphicIdTransient;
	}

EXPORT_C TInt TWsGraphicId::Compare(const TWsGraphicId& aOther) const
/** Compares another Id with this one.
	@return 0 if the other Id is identical, else -1 if the other Id is to greater than or 1 if the other Id is less than
	@publishedAll @released
*/	{
	if(iId < aOther.iId)
		{
		return -1;
		}
	else if(iId > aOther.iId)
		{
		return 1;
		}
	// else we have to compare the iIsUid flag too; again, expect it to be a match 99.99% of these times
	else if(IsUid() == aOther.IsUid())
		{
		return 0;
		}
	// collisions of id but not iIsUid are going to be really really rare
	else if(IsUid())
		{
		return 1;
		}
	else
		{
		return -1;
		}
	}

// CWsGraphicManager \\\\\\\\\\\\\\\\\\\\\\\\

CWsGraphic::CManager* CWsGraphic::CManager::StaticL()
	{
	CManager* singleton = RWsBuffer::WsGraphicManager();
	if(!singleton)
		{
		singleton = new(ELeave) CManager;
		CleanupStack::PushL(singleton);
		singleton->ConstructL();
		CleanupStack::Pop(singleton);
		__ASSERT_DEBUG(singleton == RWsBuffer::WsGraphicManager(),Panic(EW32PanicGraphicInternal));
		}
	return singleton;
	}

CWsGraphic::CManager::~CManager()
	{
	__ASSERT_DEBUG(!ResourceCount(),Panic(EW32PanicGraphicInternal));
	__ASSERT_DEBUG(!iArray.Count(),Panic(EW32PanicGraphicOrphaned));
	Close();
	__ASSERT_DEBUG(!RWsBuffer::WsGraphicManager(),Panic(EW32PanicGraphicInternal));
	iArray.Close();
	delete iFlusher;
	}

CWsGraphic::CManager::CManager(): CActive(CActive::EPriorityStandard)
	{
	}

void CWsGraphic::CManager::ConstructL()
	{
	User::LeaveIfError(Connect());
	iBuffer->SetWsGraphicManager(this);
	CActiveScheduler::Add(this);
	iFlusher = CIdle::NewL(CActive::EPriorityIdle);
	}

void CWsGraphic::CManager::Inc()
	{
	iRefCount++;
	}

void CWsGraphic::CManager::Dec()
	{
	if(!--iRefCount)
		{
		delete this;
		}
	}

RWsBuffer* CWsGraphic::CManager::Buffer()
	{
	return iBuffer;
	}

// used by CWsGraphic

void CWsGraphic::CManager::AddL(CWsGraphic* aGraphic)
/** Leaves if the graphic couldn't be added to the list
	@internalComponent @released */
	{
	__ASSERT_ALWAYS(aGraphic && aGraphic->iPimpl,Panic(EW32PanicGraphicInternal));
	__ASSERT_ALWAYS(aGraphic->Id().IsId() || aGraphic->Id().IsUid(),Panic(EW32PanicGraphicInternal));
	__ASSERT_ALWAYS(!(aGraphic->iPimpl->iFlags & CWsGraphic::CPimpl::ERegistered),Panic(EW32PanicGraphicInternal));
	iArray.InsertInOrderL(aGraphic,GraphicCompare);
	__ASSERT_ALWAYS(0 <= iArray.FindInOrder(aGraphic,GraphicCompare),Panic(EW32PanicGraphicInternal));
	aGraphic->iPimpl->iFlags |= CWsGraphic::CPimpl::ERegistered;
	Queue();
	}

void CWsGraphic::CManager::Replace(CWsGraphic* aGraphic)
/** @internalComponent @released */
	{
	__ASSERT_ALWAYS(aGraphic && aGraphic->iPimpl,Panic(EW32PanicGraphicInternal));
	__ASSERT_ALWAYS(!(aGraphic->iPimpl->iFlags & CWsGraphic::CPimpl::ERegistered),Panic(EW32PanicGraphicInternal));
	__ASSERT_ALWAYS(aGraphic->Id().IsId() || aGraphic->Id().IsUid(),Panic(EW32PanicGraphicInternal));
	const TInt idx = iArray.FindInOrder(aGraphic,GraphicCompare);
	__ASSERT_ALWAYS(0 <= idx,Panic(EW32PanicGraphicInternal));
	__ASSERT_ALWAYS(iArray[idx]->iPimpl->iFlags & CWsGraphic::CPimpl::ERegistered,Panic(EW32PanicGraphicInternal));
	iArray[idx]->iPimpl->iFlags &= ~CWsGraphic::CPimpl::ERegistered;
	iArray[idx] = aGraphic;
	iArray[idx]->iPimpl->iFlags |= CWsGraphic::CPimpl::ERegistered;
	Queue();
	}

void CWsGraphic::CManager::Remove(CWsGraphic* aGraphic)
/** @internalComponent @released */
	{
	__ASSERT_ALWAYS(aGraphic && aGraphic->iPimpl,Panic(EW32PanicGraphicInternal));
	__ASSERT_ALWAYS(aGraphic->Id().IsId() || aGraphic->Id().IsUid(),Panic(EW32PanicGraphicInternal));
	__ASSERT_ALWAYS(aGraphic->iPimpl->iFlags & CWsGraphic::CPimpl::ERegistered,Panic(EW32PanicGraphicInternal));
	const TInt idx = iArray.FindInOrder(aGraphic,GraphicCompare);
	__ASSERT_ALWAYS(0 <= idx,Panic(EW32PanicGraphicInternal));
	iArray[idx]->iPimpl->iFlags &= ~CWsGraphic::CPimpl::ERegistered;
	iArray.Remove(idx);
	if(!iArray.Count())
		{
		Cancel();
		}
	}

CWsGraphic* CWsGraphic::CManager::Find(const TWsGraphicId& aId)
/** Find the active artwork identified by the Id
	@return NULL if no active artwork has the Id
	@publishedAll @released */
	{
	/*	RPointerArray can only FindInOrder other T*, which is a needless shame.  Therefore this search is
		dumb sequential */
	const TInt count = iArray.Count();
	for(TInt i=0; i<count; i++)
		{
		CWsGraphic* graphic = iArray[i];
		__ASSERT_ALWAYS(graphic && graphic->iPimpl && (graphic->iPimpl->iFlags & CWsGraphic::CPimpl::ERegistered),Panic(EW32PanicGraphicInternal));
		const TWsGraphicId& candidate = graphic->Id();
		if(0 == candidate.Compare(aId))
			{
			// found
			__ASSERT_DEBUG(i == iArray.FindInOrder(graphic,GraphicCompare),Panic(EW32PanicGraphicInternal));
			return graphic;
			}
		}
	// not found
	return NULL;
	}

void CWsGraphic::CManager::Queue()
	{
	if(!IsActive())
		{
		GraphicMessageReady(&iStatus);
		SetActive();
		}
	}

void CWsGraphic::CManager::RunL()
   	{
   	if(0 < iStatus.Int())
   		{
   		// a message to fetch!
   		const TInt msgLen = iStatus.Int();
 		TPtr8 buf(NULL,0);
 		MWsGraphicMessageAlloc* allocator = NULL;
 		MWsGraphicMessageAlloc::MBuffer* theirBuf = NULL;
 		HBufC8* ourBuf = HBufC8::New(msgLen);
 		if(ourBuf)
 			{
 			CleanupStack::PushL(ourBuf);
 			buf.Set(ourBuf->Des());
 			}
 		else // try to see if the destination CWsGraphic can allocate for us
 			{
			const TInt handle = GraphicFetchHeaderMessage();
			CWsGraphic* dest = reinterpret_cast<CWsGraphic*>(handle & ~0x03);
 			// check if it's valid
 			if(KErrNotFound != iArray.Find(dest) && dest->iPimpl && dest->iPimpl->iExt)
 				{
 				// check if the client is able to alloc memory for us
 				allocator = dest->iPimpl->iExt->ObjectInterface<MWsGraphicMessageAlloc>();
 				if(allocator)
 					{
 					// allocate memory
 					theirBuf = allocator->Alloc(msgLen);
 					if(theirBuf)
 						{
 						CleanupStack::PushL(TCleanupItem(MWsGraphicMessageAllocRelease,theirBuf));
 						buf.Set(theirBuf->Buffer());
 						}
 					}
 				}
 			}
 		if(!ourBuf && !theirBuf)
 			{
 			GraphicMessageCancel();
 			GraphicAbortMessage(KErrNoMemory);
 			}
 		else
 			{
 			GetGraphicMessage(buf);
 			// decode header and body
 			RDesReadStream in(buf);
 			in.PushL();
 			const TInt header = in.ReadInt32L();
 			__ASSERT_COMPILE(sizeof(header) == sizeof(TInt32));
 			const TInt clientHandle = (header & ~0x03);
 			const TInt msgType = (header & 0x03);
 			const TPtr8 body = buf.MidTPtr(sizeof(header));
 			// dispatch
 			CWsGraphic* dest = (CWsGraphic*)clientHandle;
 			if(KErrNotFound != iArray.Find(dest))
 				{
 				switch(msgType)
 					{
 					case EWsGraphMessageTypeUser:
 						dest->HandleMessage(body);
 						break;
 					default:
 						Panic(EW32PanicGraphicInternal);
 					}
 				}
 			// done
 			in.Pop();
 			}
 		if(ourBuf)
 			{
 			CleanupStack::PopAndDestroy(ourBuf);
 			}
 		else if(theirBuf)
 			{
 			CleanupStack::PopAndDestroy(theirBuf);
 			}
 		// done, wait for next message
 		Queue();
 		}
 	}

void CWsGraphic::CManager::DoCancel()
	{
	GraphicMessageCancel();
	}

TInt CWsGraphic::CManager::GraphicCompare(const CWsGraphic& aFirst,const CWsGraphic& aSecond)
/** Compares two graphics for id equality
@internalComponent
@released
*/	{
	return aFirst.Id().Compare(aSecond.Id());
	}

void CWsGraphic::CManager::ScheduleFlush()
/** Request to schedule flush when idle
@internalComponent
@released
*/
	{
	if (iFlushScheduled)
		return;

	iFlushScheduled = ETrue;
	iFlusher->Start(TCallBack(CWsGraphic::CManager::FlushOnIdle,this));
	}

TInt CWsGraphic::CManager::FlushOnIdle(TAny* aArg)
/** Flush buffer when idle and there is outstanding data in it
@internalComponent
@released
*/
	{
	CWsGraphic::CManager* mgr = reinterpret_cast<CWsGraphic::CManager*>(aArg);
	if (mgr)
		{
		mgr->iFlushScheduled = EFalse;
		if (mgr->iBuffer && !mgr->iBuffer->IsEmpty())
			mgr->iBuffer->Flush();
		}

	return 0; // complete
	}

// CWsGraphic::CPimpl \\\\\\\\\\\\\\\\\\\\\\\\

CWsGraphic::CPimpl::CPimpl(CWsGraphic& aGraphic):
	iGraphic(aGraphic), iId(TWsGraphicId::EUninitialized)
	{
	}

CWsGraphic::CPimpl::~CPimpl()
	{
	if(iManager)
		{
		iManager->Dec();
		}
	}

void CWsGraphic::CPimpl::ConstructL()
	{
	iManager = CManager::StaticL();
	iManager->Inc();
	iBuffer = iManager->Buffer();
	}

TInt CWsGraphic::CPimpl::WriteCreateGraphic(TWsClCmdCreateGraphic& aCreateGraphic,const TDesC8& aData,TInt aWsHandle) const
/** Writes the CreateGraphic message to the server.  If the data will not fit in the buffer, uses remote-read
	@internalComponent @released */
	{
	aCreateGraphic.iDataLen = aData.Size();
	aCreateGraphic.iRemoteReadData = ((aData.Size()+sizeof(aCreateGraphic))>(TInt)(iBuffer->BufferSize()-sizeof(TWsCmdHeader)));
	if(aCreateGraphic.iRemoteReadData)
		{
		return iBuffer->WriteReplyByProvidingRemoteReadAccess(aWsHandle,EWsClOpCreateGraphic,&aCreateGraphic,sizeof(aCreateGraphic),&aData);
		}
	else if(aCreateGraphic.iDataLen)
		{
		return iBuffer->WriteReplyWs(&aCreateGraphic,sizeof(aCreateGraphic),aData.Ptr(),aData.Size(),EWsClOpCreateGraphic);
		}
	else
		{
		return iBuffer->WriteReplyWs(&aCreateGraphic,sizeof(aCreateGraphic),EWsClOpCreateGraphic);
		}
	}

// CWsGraphic \\\\\\\\\\\\\\\\\\\\\\\\

/** 
Default Constructor.
*/
EXPORT_C CWsGraphic::CWsGraphic()
	{
	}

/**
Destructor.
*/
EXPORT_C CWsGraphic::~CWsGraphic()
	{
	Destroy();
	delete iPimpl;
	}
	
/**
Completes construction of the baseclass. All the overloaded BaseConstructL() methods
should invoke this method to complete the construction of the baseclass.
*/
void CWsGraphic::BaseConstructL()
	{
	iPimpl = new(ELeave) CPimpl(*this);
	iPimpl->ConstructL();
	}

EXPORT_C void CWsGraphic::BaseConstructL(TUid aUid,TUid aType,const TDesC8& aData)
/** 
Constructs a piece of non-transient graphic artwork.
@capability ProtServ
@param aUid	Graphic artwork UID.
@param aType Graphic artwork type.
@param aData User specific data.
*/	{
	BaseConstructL();

	TWsClCmdCreateGraphic createGraphic;
	createGraphic.iFlags = EWsGraphicIdUid;
	createGraphic.iId = aUid.iUid;
	createGraphic.iType = aType;
	createGraphic.iClientHandle = (TInt)this;

	TInt ret = iPimpl->WriteCreateGraphic(createGraphic,aData,iPimpl->iManager->WsHandle());
	User::LeaveIfError(ret);

	iPimpl->iWsHandle=ret;
	iPimpl->iId = aUid;
	iPimpl->iFlags = CPimpl::EHasPeer;
	iPimpl->iManager->AddL(this);
	}

EXPORT_C void CWsGraphic::BaseConstructL(TUid aType,const TDesC8& aData)
/** 
Constructs a piece of transient graphic artwork.
@param aType Graphic artwork type.
@param aData User specific data.
*/
	{
	BaseConstructL();

	TWsClCmdCreateGraphic createGraphic;
	createGraphic.iFlags = EWsGraphicIdTransient;
	createGraphic.iId = 0;
	createGraphic.iType = aType;
	createGraphic.iClientHandle = (TInt)this;

	TInt ret = iPimpl->WriteCreateGraphic(createGraphic,aData,iPimpl->iManager->WsHandle());
	User::LeaveIfError(ret);

	iPimpl->iWsHandle = ret;

	// fetch id from server
	TPckgBuf<TWsClCmdGdGetId> cmd;
	User::LeaveIfError(iPimpl->WriteReplyP(TWriteDescriptorType(&cmd),EWsGdOpGetGraphicId));
	if(cmd().iIsUid)
		{
		__DEBUG_ONLY(Panic(EW32PanicGraphicInternal));
		User::Leave(KErrGeneral);
		}
	iPimpl->iId = cmd().iId;

	iPimpl->iFlags = CPimpl::EHasPeer;
	iPimpl->iManager->AddL(this);
	}

EXPORT_C void CWsGraphic::BaseConstructL(const TWsGraphicId& aReplace,TUid aType,const TDesC8& aData)
/** 
Atomically replace the artwork that already exists with this artwork.
If failure to properly construct the replacement artwork occurs, the replacee artwork will remain
@param aReplace Graphic artwork which will be replaced.
@param aType New graphic artwork type.
@param aData User specific data.
*/
	{
	BaseConstructL();

	CWsGraphic* dup = iPimpl->iManager->Find(aReplace);
	if(!dup || !dup->iPimpl)
		{
		Panic(EW32PanicGraphicInternal);
		}

	const TUint flags = aReplace.IsUid()?EWsGraphicIdUid:EWsGraphicIdTransient;

	TWsClCmdCreateGraphic createGraphic;
	createGraphic.iFlags = EWsGraphicReplace|flags;
	createGraphic.iId = aReplace.IsUid()?aReplace.Uid().iUid:aReplace.Id();
	createGraphic.iType = aType;
	createGraphic.iClientHandle = (TInt)this;

	TInt ret = iPimpl->WriteCreateGraphic(createGraphic,aData,iPimpl->iManager->WsHandle());
	if(0 > ret)
		{
		User::Leave(ret);
		}

	iPimpl->iWsHandle = ret;
	iPimpl->iId = aReplace;

	iPimpl->iFlags = CPimpl::EHasPeer;
	iPimpl->iManager->Replace(this);

	// when WriteCreateGraphic succeeds, the replacee drawer has already been destroyed serverside
	// so this cleanup is not quite the same as Destroy(), as it doesn't free server side
	dup->iPimpl->iWsHandle = 0;
	dup->OnReplace();
	dup->iPimpl->iFlags &= ~CPimpl::EHasPeer;
	}

/**
Shares the graphic artwork with all the client sessions.
Sharing globally trumps explicit shares.
@return KErrNone if the graphic is globally shared, else one of the system-wide error codes.
*/
EXPORT_C TInt CWsGraphic::ShareGlobally()
	{
	if(!IsActive())
		{
		return KErrNotReady;
		}
	return iPimpl->WriteReply(EWsGdOpShareGlobally);
	}

/**
Prevents this graphic artwork from being shared with all the client sessions.
A graphic artwork that isn't shared explicitly is only available to clients it
has been explicitly shared with using Share().
@return KErrNone if the graphic is not globally shared, else one of the system-wide error codes.
*/
EXPORT_C TInt CWsGraphic::UnShareGlobally()
	{
	if(!IsActive())
		{
		return KErrNotReady;
		}
	return iPimpl->WriteReply(EWsGdOpUnShareGlobally);
	}
	
/** 
Explicitly shares this graphic artwork with client sessions with the specified Secure ID.
@param aClientId the Secure ID of the client sessions to share with.
@return KErrNone If the graphic artwork was shared, else one of the system-wide error codes.
*/
EXPORT_C TInt CWsGraphic::Share(TSecureId aClientId)
	{
	if(!IsActive())
		{
		return KErrNotReady;
		}
	return iPimpl->WriteReply(&aClientId,sizeof(TSecureId),EWsGdOpShare);
	}

/** 
Stops this graphic artwork from being shared with all client sessions with the specific Secure ID.
ShareGlobally() trumps explicit sharing.
@param aClientId the Secure ID of the client sessions to not share with
@return KErrNone if the graphic artwork is no longer shared, KErrNotFound if the graphic was not shared anyway, else one of the system-wide error codes
*/
EXPORT_C TInt CWsGraphic::UnShare(TSecureId aClientId)
	{
	if(!IsActive())
		{
		return KErrNotReady;
		}
	return iPimpl->WriteReply(&aClientId,sizeof(TSecureId),EWsGdOpUnShare);
	}

/**
Returns graphic artwork Id.
@return Graphic artwork Id. KNullWsGraphicId if graphic artwork is not active.
*/
EXPORT_C const TWsGraphicId& CWsGraphic::Id() const
	{
	if(IsActive())
		{
		return iPimpl->iId;
		}
	else
		{
		// fallback
		static const TInt KNullWsGraphicId[4] = //binary compatible with TWsGraphicId
			{
			0, 0, 0, 0
			};
		__ASSERT_COMPILE(sizeof(KNullWsGraphicId) == sizeof(TWsGraphicId));
		return reinterpret_cast<const TWsGraphicId&>(KNullWsGraphicId);
		}
	}
	
/** 
Checks whether a peer of this graphic artwork has been fully constructed on the server.
@return ETrue if this graphic artwork has a peer CWsGraphic on the server.
*/
EXPORT_C TBool CWsGraphic::IsActive() const
	{
	return (iPimpl && iPimpl->iWsHandle && (iPimpl->iFlags & CPimpl::EHasPeer));
	}
	
/**
Derived class can override this method to provide custom operations when the client is closed.
*/
EXPORT_C void CWsGraphic::OnClientClose()
	{
	}
	
/** 
Sends message to this graphic artwork peer on the server.
@param aData User specific data.
*/
EXPORT_C void CWsGraphic::SendMessage(const TDesC8& aData) const
	{
	TWsClCmdGdSendMessage cmd;
	cmd.iDataLen = aData.Size();
	__ASSERT_DEBUG(cmd.iDataLen, Panic(EW32PanicGraphicNullData));
	cmd.iRemoteReadData = ((aData.Size()+sizeof(cmd))>(TInt)(iPimpl->iBuffer->BufferSize()-sizeof(TWsCmdHeader)));
	if(cmd.iRemoteReadData)
		{
		iPimpl->WriteReplyByProvidingRemoteReadAccess(&cmd,sizeof(cmd),&aData,EWsGdOpSendMsg);
		//ignore return value!
		}
	else
		{
		iPimpl->Write(&cmd,sizeof(cmd),aData.Ptr(),aData.Size(),EWsGdOpSendMsg);
		}
	}

EXPORT_C TInt CWsGraphic::SendSynchronMessage(const TDesC8& aData) const
	 {
	 TWsClCmdGdSendMessage cmd;
	 cmd.iDataLen = aData.Size();
	 __ASSERT_DEBUG(cmd.iDataLen, Panic(EW32PanicGraphicNullData));
	 cmd.iRemoteReadData = ((aData.Size()+sizeof(cmd))>(TInt)(iPimpl->iBuffer->BufferSize()-sizeof(TWsCmdHeader)));
	 if(cmd.iRemoteReadData)
		{
		return iPimpl->WriteReplyByProvidingRemoteReadAccess(&cmd,sizeof(cmd),&aData,EWsGdOpSendSynchronMsg);
		}
	 else
		{
		return iPimpl->WriteReply(&cmd,sizeof(cmd),aData.Ptr(),aData.Size(),EWsGdOpSendSynchronMsg);
		}
	 }

/** 
Flushes window server command buffer
@return One of system-wide error codes.
*/
EXPORT_C TInt CWsGraphic::Flush() const
	{
	return iPimpl->iBuffer->Flush();
	}

EXPORT_C void CWsGraphic::Destroy()
/** Destroys the corresponding CWsGraphicDrawer instance on the server
@released
@publishedAll
*/	{
	if(iPimpl && (iPimpl->iFlags & CPimpl::ERegistered))
		{
		iPimpl->iManager->Remove(this);
		}
	if(IsActive()) // if iPimpl==NULL, IsActive() returns false
		{
		iPimpl->Write(EWsGdOpFree);
		iPimpl->iWsHandle = 0;
		iPimpl->iFlags &= ~CPimpl::EHasPeer;
		iPimpl->iManager->ScheduleFlush();
		}
	}

EXPORT_C void CWsGraphic::SetGraphicExtension(MWsObjectProvider* aExt)
	{
	iPimpl->iExt = aExt;
	}

EXPORT_C RWsSession&  CWsGraphic::Session()
	{
	return *iPimpl->iManager;
	}

EXPORT_C TInt CWsGraphic::CWsGraphic_Reserved1()
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CWsGraphic::CWsGraphic_Reserved2()
	{
	return KErrNotSupported;
	}

EXPORT_C TInt CWsGraphic::CWsGraphic_Reserved3()
	{
	return KErrNotSupported;
	}

// TWsGraphicMsgFixedBase \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C TWsGraphicMsgFixedBase::TWsGraphicMsgFixedBase(TUid aTypeId,TInt aSizeOfDerived):
/** Protected constructor for subclasses to call
@param aTypeId The UID representing this type of data
@param aSizeOf The size of the derived class

Example:
@code
TMyDerivedFixedMsg::TMyDerivedFixedMsg(): TWsGraphicMsgFixedBase(KUidMyDerivedType,sizeof(TMyDerivedFixedMsg)), ...
@endcode
*/	iTypeId(aTypeId), iSize(aSizeOfDerived-sizeof(TWsGraphicMsgFixedBase))
	{
	__ASSERT_COMPILE(sizeof(*this) == (sizeof(TInt32)*2));
	}

EXPORT_C TPtrC8 TWsGraphicMsgFixedBase::Pckg() const
/** @return this fixed message as a descriptor so that it can be passed as draw data in the CWindowGc::DrawWsGraphic command directly if only one such message is to be sent
*/	{
	return TPtrC8(reinterpret_cast<const TUint8*>(this),sizeof(*this) + iSize);
	}

EXPORT_C TUid TWsGraphicMsgFixedBase::TypeId() const
/** @return the UID identifying the type of the data that follows */
	{
	return iTypeId;
	}

EXPORT_C TInt TWsGraphicMsgFixedBase::Size() const
/** @return the size of the derived class (not including this fixed base class size) */
	{
	return iSize;
	}

// RWsGraphicMsgBuf \\\\\\\\\\\\\\\\\\\\\\\\

EXPORT_C RWsGraphicMsgBuf::RWsGraphicMsgBuf()
/** Default Constructor */
	{
	}

TInt RWsGraphicMsgBuf::IntAt(TInt aOfs) const
/** @internalComponent @released */
	{
	if((aOfs < 0) || ((aOfs+sizeof(TInt)) > Length()))
		{
		Panic(EW32PanicGraphicBadBuffer);
		}
	TInt ret;
	memcpy(&ret,Ptr()+aOfs,sizeof(TInt));
	return ret;
	}

EXPORT_C TInt RWsGraphicMsgBuf::Append(TUid aTypeId,const TDesC8& aData)
/** Append a descriptor as data
	@param aTypeId the type of the message to append
	@param aData arbitrary length data consisting of the whole message
	@return KErrNone if successful, else a system-wide error code
*/	{
	TInt err = ExpandForAppend(aData.Length());
	if (err)
		{
		return err;
		}
	WriteHeader(aTypeId,aData.Length());
	// append data
	Insert(Length(),aData);
	return KErrNone;
	}

EXPORT_C TInt RWsGraphicMsgBuf::Append(TUid aTypeId,const TDesC16& aData)
/** Append a descriptor as data
	@param aTypeId the type of the message to append
	@param aData arbitrary length data consisting of the whole message
	@return KErrNone if successful, else a system-wide error code
*/	{
	TPtr8 data(NULL,0);
	TInt err = Append(aTypeId,aData.Size(),data);
	if (err)
		{
		return err;
		}
	// append data
	data.Copy(reinterpret_cast<const TUint8*>(aData.Ptr()),aData.Size());
	return KErrNone;
	}

TInt RWsGraphicMsgBuf::ExpandForAppend(TInt aDataLen)
/** @internalComponent @released */
	{
	__ASSERT_COMPILE(sizeof(TInt) == sizeof(TInt32));
	const TInt required = (sizeof(TUid) + sizeof(TInt) + aDataLen);
	if(MaxLength() < (Length() + required))
		{
		TInt err = ReAlloc(Length()+required);
		if(KErrNone != err)
			{
			return err;
			}
		}
	return KErrNone;
	}

void RWsGraphicMsgBuf::WriteHeader(TUid aTypeId,TInt aLen)
/** @internalComponent @released */
	{
	__ASSERT_COMPILE(sizeof(TInt) == sizeof(TInt32));
	// append header
	TPckgBuf<TInt32> i(aTypeId.iUid);
	Insert(Length(),i);
	i() = aLen;
	Insert(Length(),i);
	}

EXPORT_C TInt RWsGraphicMsgBuf::Append(TUid aTypeId,TInt aLen,TPtr8& aPtr)
/** Append a message of the specified length and type, and return a pointer to
	allow client code to modify the message.
	aPtr is only set if the append succeeds.
	aPtr is only valid until the next message is appended to the buffer.
	@param aTypeId the type of the message to append
	@param aLen the length of the message to be reserved
	@param aPtr a modifiable descriptor to be used by the client code to write into the message body
	@return KErrNone if successful, else a system-wide error code
*/	{
	TInt err = ExpandForAppend(aLen);
	if (err)
		{
		return err;
		}
	WriteHeader(aTypeId,aLen);
	// set aPtr
	TInt usedLen = Length();
	SetLength(usedLen+aLen);
	aPtr.Set(MidTPtr(usedLen,aLen));
	aPtr.Fill('_'); // prettier when debugging, but want consistant behaviour in release builds
	aPtr.Zero();
	return KErrNone;
	}

EXPORT_C void RWsGraphicMsgBuf::Remove(TInt aIndex)
/** Remove a message from the buffer
	@panic if the index is out of bounds
	@param aIndex the ordinal position of message to be removed
*/	{
	if((aIndex < 0) || (aIndex >= Count()))
		{
		Panic(EW32PanicGraphicBadBuffer);
		}
	TPtrC8 ptr = Data(aIndex);
	const TInt ofs = (ptr.Ptr()-(sizeof(TInt)*2)-Ptr());
	Delete(ofs,ptr.Length() + (sizeof(TInt)*2));
	}

EXPORT_C TInt RWsGraphicMsgBuf::Count() const
/** Returns the number of messages in the buffer
	@return the number of messages in the buffer
*/	{
	const TInt length = Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		count++;
		ofs += IntAt(ofs+sizeof(TInt)) + (sizeof(TInt)*2);
		}
	if(ofs != length)
		{
		Panic(EW32PanicGraphicBadBuffer);
		}
	return count;
	}

EXPORT_C TUid RWsGraphicMsgBuf::TypeId(TInt aIndex) const
/** Returns the Type ID of a message in the buffer
	@param aIndex the ordinal position of the message
	@return the Type ID of the message
	@panic if the index is out of bounds
*/	{
	const TInt length = Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if(count == aIndex)
			{
			return TUid::Uid(IntAt(ofs));
			}
		count++;
		ofs += IntAt(ofs+sizeof(TInt)) + (sizeof(TInt)*2);
		}
	Panic(EW32PanicGraphicBadBuffer);
	return KNullUid; //dumb compiler
	}

EXPORT_C TPtrC8 RWsGraphicMsgBuf::Data(TInt aIndex) const
/** Returns a non-modifiable descriptor of a message body in the buffer
	@param aIndex the ordinal position of the message
	@return the message body
	@panic if the index is out of bounds
*/	{
	const TInt length = Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if(count == aIndex)
			{
			return Mid(ofs+(sizeof(TInt)*2),IntAt(ofs+sizeof(TInt)));
			}
		count++;
		ofs += IntAt(ofs+sizeof(TInt)) + (sizeof(TInt)*2);
		}
	Panic(EW32PanicGraphicBadBuffer);
	return TPtrC8(KNullDesC8()); //dumb compiler
	}

EXPORT_C TPtr8 RWsGraphicMsgBuf::Data(TInt aIndex)
/** Returns a modifiable descriptor of a message body in the buffer
	The returned TPtr8 is only valid until the next message is appended to the buffer.
	@param aIndex the ordinal position of the message
	@return the message body
	@panic if the index is out of bounds
*/	{
	const TInt length = Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if(count == aIndex)
			{
			return MidTPtr(ofs+(sizeof(TInt)*2),IntAt(ofs+sizeof(TInt)));
			}
		count++;
		ofs += IntAt(ofs+sizeof(TInt)) + (sizeof(TInt)*2);
		}
	Panic(EW32PanicGraphicBadBuffer);
	return TPtr8(NULL,0); //dumb compiler
	}

EXPORT_C const TDesC8& RWsGraphicMsgBuf::Pckg() const
/** Returns the message buffer as a descriptor.  Example:
	@code
	RWsGraphicMsgBuf msgBuf;
	msgBuf.Append(...);
	...
	TWsGraphicId id(...);
	SystemGc().DrawWsGraphic(id,Rect(),msgBuf.Pckg());
	msgBuf.Close();
	@endcode
	@see CWindowGc::DrawWsGraphic
	@see CCoeControl::Draw
	@return the message buffer to be attached a command to draw a CWsGraphic
*/	{
	return *this;
	}

EXPORT_C TInt RWsGraphicMsgBuf::Append(const TWsGraphicMsgFixedBase& aMsg)
/** Append a fixed-size message
	@param aMsg the fixed-size message to append
	@return KErrNone if successful, else a system-wide error code
*/	{
	__ASSERT_COMPILE(sizeof(TWsGraphicMsgFixedBase) == (sizeof(TInt)*2));
	TInt err = ExpandForAppend(aMsg.Size());
	if (err)
		{
		return err;
		}
	// append data
	RBuf8::Append(reinterpret_cast<const TUint8*>(&aMsg),sizeof(TWsGraphicMsgFixedBase) + aMsg.Size());
	Count();
	return KErrNone;
	}

EXPORT_C void RWsGraphicMsgBuf::GetFixedMsg(TWsGraphicMsgFixedBase& aMsg,TInt aIndex) const
/** Returns a copy of a fixed-size message in the buffer
	@param a copy of the message
	@param aIndex the ordinal position of the message
	@panic if the index is out of bounds
	@panic the message specified is not of the correct type
*/	{
	__ASSERT_COMPILE(sizeof(TWsGraphicMsgFixedBase) == (sizeof(TInt32)*2));
	const TInt KHeaderSize = sizeof(TWsGraphicMsgFixedBase);
	const TInt length = Length();
	TInt ofs = 0, count = 0;
	while(ofs < length)
		{
		if(count == aIndex)
			{
			if((TUid::Uid(IntAt(ofs)) != aMsg.TypeId()) ||
				(IntAt(ofs+sizeof(TInt)) != aMsg.Size()) ||
				((ofs + KHeaderSize + aMsg.Size()) > length))
				{
				Panic(EW32PanicGraphicBadBuffer);
				}
			memcpy(&aMsg,(Ptr()+ofs),KHeaderSize + aMsg.Size());
			return;
			}
		count++;
		ofs += IntAt(ofs+sizeof(TInt)) + (sizeof(TInt)*2);
		}
	Panic(EW32PanicGraphicBadBuffer);
	}

