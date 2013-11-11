// Copyright (c) 1995-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <fntstore.h>
#include <bitmap.h>
#include "FBSVER.H"
#include "SERVER.H"
#include "BackGroundCompression.h"
#include "linkedfonts.h"
#include "BitwiseBitmap.inl"

const TInt KFbsPriority = 0;
const TInt KFbsGoomMonitorSid = 0x10207218;
// Setup security policies
const TInt KRanges[] = 
	{
	0,
	EFbsMessShutdown,
	EFbsMessClose,
	EFbsSetSystemDefaultTypefaceName,
	EFbsGetAllBitmapHandles,
	EFbsMessUnused1,
	EFbsMessUnused2,
	EFbsMessRegisterLinkedTypeface,
	EFbsMessFetchLinkedTypeface,
	EFbsMessUpdateLinkedTypeface,
	EFbsMessGetFontTable,
    EFbsMessOogmNotification,
    EFbsMessGetGlyphCacheMetrics,
	};

const TUint KRangeCount = sizeof(KRanges)/sizeof(TInt);
const TUint8 KElementsIndex[KRangeCount] = 
	{
	0,
	3, // For EFbsMessShutdown
	0,
	1, // For EFbsSetSystemDefaultTypefaceName
	2, // For EFbsGetAllBitmapHandles
	0, // ECapability_None for EFbsMessUnused1
	0, // ECapability_None for EFbsMessUnused2 and beyond
	1, // ECapabilityWriteDeviceData for EFbsMessRegisterLinkedTypeface
	2, // ECapabilityReadDeviceData for EFbsMessFetchLinkedTypeface
	1, // ECapabilityWriteDeviceData for EFbsMessUpdateLinkedTypeface
	0, // ECapability_None for EFbsMessGetFontTable and beyond
    4, // SID for EFbsMessOogmNotification.
    0, // For EFbsMessGetGlyphCacheMetrics and on.
	};

const CPolicyServer::TPolicyElement KElements[] = 
	{
	{_INIT_SECURITY_POLICY_C1(ECapability_None), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityWriteDeviceData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityReadDeviceData), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_C1(ECapabilityPowerMgmt), CPolicyServer::EFailClient},
	{_INIT_SECURITY_POLICY_S0(KFbsGoomMonitorSid), CPolicyServer::EFailClient}
 	};

const CPolicyServer::TPolicy KFbsPolicy =
	{
	CPolicyServer::EAlwaysPass, //specifies all connect attempts should pass
	KRangeCount,
	KRanges,
	KElementsIndex,
	KElements,
	};

CFontBitmapServer::CFontBitmapServer():
	CPolicyServer(KFbsPriority, KFbsPolicy)
	{
	}

CFontBitmapServer::~CFontBitmapServer()
	{
	delete iTopLevelStore;
	iTopLevelStore = NULL;	// so that CFBClient::~CFbClient can avoid using this pointer;
							// it may be called inside CFontBitmapServer::~CFontBitmapServer
	}

CFontBitmapServer* CFontBitmapServer::NewL()
	{
	CFontBitmapServer* self = new(ELeave) CFontBitmapServer;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); //self
	return self;
	}

/**
Two-phase constructor.
*/
void CFontBitmapServer::ConstructL()
	{
	iTopLevelStore=CFbTop::NewL();
	
	// If fbserv data paging is configured as unpaged, automatically pin client descriptors 
	if(!RProcess().DefaultDataPaged())
		{
		SetPinClientDescriptors(ETrue);
		}
	StartL(KFBSERVGlobalThreadName);
	}

CFbTop* CFontBitmapServer::TopLevelStore()
	{
	return(iTopLevelStore);
	}

CSession2* CFontBitmapServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/ ) const
	{
	TVersion v(KFbsMajorVersionNumber,KFbsMinorVersionNumber,KFbsBuildVersionNumber);
	if(!User::QueryVersionSupported(v,aVersion))
		User::Leave(KErrNotSupported);

	CSession2* pSession2 = CFbClient::NewL(iTopLevelStore->Heap());
	return pSession2;
	}

TInt CFontBitmapServer::Init()
	{
	return(iConnectionId++);
	}


TInt CFontBitmapServer::HandleMesgTypefaceSupport(const RMessage2& aMessage, TBool& aClientPanicRequired)
	{
	TTypefaceSupport tfi;
	TPckgBuf<TTypefaceSupport> ttipckg(tfi);
	TPckgBuf<TSize> pixelSize;
	TInt index=aMessage.Int0();
	TInt limit=iTopLevelStore->FontStore()->NumTypefaces();
	if(index < 0 || index > limit)
		{
		return KErrArgument;
		}
	TInt ret = aMessage.Read(2, pixelSize);
	if (ret != KErrNone)
		{
		return ret;
		}
	iTopLevelStore->FontStore()->iKPixelHeightInTwips = pixelSize().iHeight;
	iTopLevelStore->FontStore()->iKPixelWidthInTwips = pixelSize().iWidth;
	iTopLevelStore->FontStore()->TypefaceSupport(ttipckg(),index);
	ret = aMessage.Write(1,ttipckg);
	if(ret!=KErrNone)
		{
		aClientPanicRequired = ETrue;
		}
	return ret;
	}


TInt CFontBitmapServer::HandleMesgFontHeight(const RMessage2& aMessage, TBool aInTwips)
	{
	TInt typefaceindex = aMessage.Int0();
	TInt fontsize = aMessage.Int1();

	if(typefaceindex < 0)
		{
		aMessage.Panic(KFBSERVPanicCategory,KErrArgument);
		return KErrArgument;
		}
	
	// pixel size (used to be set in a separate call)
	TPckgBuf<TSize> size;
	TInt ret = aMessage.Read(2, size);
	if (ret != KErrNone)
		{
		return ret;
		}
		
	if(size().iHeight <=  0 || size().iWidth < 0)
		{
		aMessage.Panic(KFBSERVPanicCategory,KErrArgument);
		return KErrArgument;
		}
			
	iTopLevelStore->FontStore()->iKPixelHeightInTwips = size().iHeight;
	iTopLevelStore->FontStore()->iKPixelWidthInTwips = size().iWidth;
	if (aInTwips)
		{
		return iTopLevelStore->FontStore()->FontHeightInTwips(typefaceindex,fontsize);
		}
	else
		{
		return iTopLevelStore->FontStore()->FontHeightInPixels(typefaceindex,fontsize);
		}
	}


#ifdef _DEBUG
//aRet is used in debug builds to allow the server to know if the call was successful.
//On success there should be no panics from out of memory testing, since memory
//may have been genuinely allocated.
void CFontBitmapServer::ProcMessage(const RMessage2& aMessage, TInt aSession, TInt& aRet)
#else
void CFontBitmapServer::ProcMessage(const RMessage2& aMessage, TInt aSession)
#endif
	{
    TInt ret=KErrNone;
	TBool clientPanicRequired = EFalse;
#ifdef _DEBUG
	TInt num=0;
#endif
	switch (aMessage.Function())
		{
	case EFbsMessShutdown:
		CActiveScheduler::Stop();
		break;
	case EFbsMessNumTypefaces:
		ret=iTopLevelStore->FontStore()->NumTypefaces();
		break;
	case EFbsMessTypefaceSupport:
		ret = HandleMesgTypefaceSupport(aMessage, clientPanicRequired);
		break;

	case EFbsMessFontHeightInTwips:
		ret = HandleMesgFontHeight(aMessage, ETrue);
		break;

	case EFbsMessFontHeightInPixels:
		ret = HandleMesgFontHeight(aMessage, EFalse);
		break;

	case EFbsMessSetPixelHeight:
		iTopLevelStore->FontStore()->iKPixelWidthInTwips=aMessage.Int0();
		iTopLevelStore->FontStore()->iKPixelHeightInTwips=aMessage.Int1();
		break;
#ifdef _DEBUG
	case EFbsMessDefaultAllocFail:
		num=aMessage.Int0();
		if(num) __UHEAP_FAILNEXT(num);
		else __UHEAP_RESET;
		break;
	case EFbsMessDefaultMark:
		__UHEAP_MARK;
		break;
	case EFbsMessDefaultMarkEnd:
		num=aMessage.Int0();
		if(num) __UHEAP_MARKENDC(num);
		else __UHEAP_MARKEND;
		break;
	case EFbsMessUserAllocFail:
		num=aMessage.Int0();
		if(num) __RHEAP_FAILNEXT(iTopLevelStore->Heap(),num);
		else __RHEAP_RESET(iTopLevelStore->Heap());
		break;
	case EFbsMessUserMark:
		__RHEAP_MARK(iTopLevelStore->Heap());
		break;
	case EFbsMessUserMarkEnd:
		num=aMessage.Int0();
		if(num) __RHEAP_MARKENDC(iTopLevelStore->Heap(),num);
		else __RHEAP_MARKEND(iTopLevelStore->Heap());
		break;
#endif
	case EFbsMessHeapCheck:
		iTopLevelStore->Heap()->Check();
		User::Heap().Check();
		break;
	case EFbsMessSetDefaultGlyphBitmapType:
		iTopLevelStore->FontStore()->SetDefaultBitmapType((TGlyphBitmapType)aMessage.Int0());
		break;
	case EFbsMessGetDefaultGlyphBitmapType:
		ret = iTopLevelStore->FontStore()->DefaultBitmapType();
		break;
	case EFbsMessFontNameAlias:
		TRAP(ret, iTopLevelStore->SetFontNameAliasL(aMessage));
		break;
	case EFbsMessGetHeapSizes:
		TRAP(ret, GetHeapSizesL(aMessage));
		break;
	case EFbsMessDefaultLanguageForMetrics:
		iTopLevelStore->SetDefaultLanguageForMetrics(aMessage);
		break;
	case EFbsCompress:
		iTopLevelStore->BackgroundCompression()->CompressAll();
		break;
	case EFbsMessRegisterLinkedTypeface:
		{
		TPckgBuf<TLinkedTypefaceSpecificationArgs> buf;
		ret=aMessage.Read(0,buf);
		if (ret==KErrNone)
			{
			//Id used by original font linking; now unused.
			TInt id;
			ret=iTopLevelStore->FontStore()->CreateLinkedTypeface(buf(),aSession,id);
			}
		}
		break;
	case EFbsMessFetchLinkedTypeface:
			{
			TBuf<KMaxTypefaceNameLength> name;
			ret = aMessage.GetDesLength(0);
				
			if (ret < 0) 
				break;
				
			if (ret > KMaxTypefaceNameLength)
				{
				clientPanicRequired = ETrue;
				ret = KErrTooBig;
				break;
				}
				
			ret = aMessage.Read(0,name);
	
			TLinkedTypefaceSpecificationArgs spec;
				
			spec.iName = name;
				
			if (ret==KErrNone)
				{
				TRAP(ret, iTopLevelStore->FontStore()->GetLinkedTypefaceL(spec));
	
				if (ret == KErrNone)
					{
					TPckgBuf<TLinkedTypefaceSpecificationArgs> specArgs = spec;					
					ret = aMessage.Write(2,specArgs);
					}
				}
			}
		break;
		case EFbsMessUpdateLinkedTypeface:
			{
			TPckgBuf<TLinkedTypefaceSpecificationArgs> buf;
			ret=aMessage.Read(0,buf);
			if (ret==KErrNone)
				{
				TRAP(ret, iTopLevelStore->FontStore()->UpdateLinkedTypefaceL(buf()));
				}
			}
		break;
	}

	if (clientPanicRequired)
		{
		aMessage.Panic(KFBSERVPanicCategory,ret);
		}
	else
		{
		if(!aMessage.IsNull())
			{
			aMessage.Complete(ret);
			}		
		}

#ifdef _DEBUG
	aRet=ret;
#endif	

	}

/**
Returns the current sizes of the FBServ default heap, the heap for large bitmaps, 
and the heap for small bitmaps.

Not supported in release builds.

@internalComponent
@test
@param aMessage Encapsulates references to integers supplied by the caller, which will on return contain the sizes of the default heap, heap for small bitmaps, and heap for large bitmaps.
@leave KErrNotSupported when used in release mode.
*/
#ifdef _DEBUG
void CFontBitmapServer::GetHeapSizesL(const RMessage2& aMessage)
	{
	TPckgBuf<THeapSizes> data;
	TInt defaultHeapSize = 0;
	TInt smallBmpHeapSize = 0;		
	TInt bigBmpHeapSize = 0;		
	
	User::Heap().AllocSize(defaultHeapSize);
	bigBmpHeapSize = iTopLevelStore->iLargeBitmapChunk.Size();
	iTopLevelStore->Heap()->AllocSize(smallBmpHeapSize);
	
	data().iDefault = defaultHeapSize;
	data().iBig = bigBmpHeapSize;
	data().iSmall = smallBmpHeapSize;
	
	aMessage.WriteL(0, data);
	}
#else
void CFontBitmapServer::GetHeapSizesL(const RMessage2&)
	{
	User::Leave(KErrNotSupported);	
	}	
#endif


CFontObject::CFontObject(CFontStore* aFontStore, CGlyphAtlas* aGlyphAtlas):
	CObject(),
	iFontStore(aFontStore),
	iGlyphAtlas(aGlyphAtlas)
	{
	}

CFontObject::~CFontObject()
	{
	if (AccessCount()==1)
		{
		Dec();
		}
	iFontStore->ReleaseFont(iAddressPointer);
	if (iGlyphAtlas)
		{
		iGlyphAtlas->FontReleased(*iAddressPointer);
		}
	}

// CBitmapObject constructor - takes ownership of aBmp
CBitmapObject::CBitmapObject(CFbTop& aFbTop, CBitwiseBitmap* aBmp):
	CObject(),
	iFbTop(&aFbTop),
	iAddressPointer(aBmp)
	{
	__ASSERT_DEBUG(iAddressPointer, User::Invariant());
	}

/** Second-phase constructor of reference counting objects for bitmaps.
Adds the CBitmapObject to the bitmap object container and, if it is not a
replacement for a dirty bitmap, assigns a server-wide handle to it and adds
it to the server's bitmap index. When a replacement bitmap is attached to
the corresponding dirty bitmap with a call to SetCleanBitmap(), then it is
assigned the same server-wide handle as the bitmap to be replaced.

@param aReplacement If ETrue the bitmap is being created as a replacement
	for a bitmap being made dirty by a resize or compress operation.
@internalComponent
*/
void CBitmapObject::ConstructL(TBool aReplacement)
	{
	iFbTop->iBitmapCon->AddL(this);
	if (!aReplacement)
		{
		iHandle = reinterpret_cast<TInt>(this);
		while (iFbTop->iBitmapObjectIndex.FindInOrder(this, Compare) != KErrNotFound)
			{
			++iHandle;
			}
		User::LeaveIfError(iFbTop->iBitmapObjectIndex.InsertInOrder(this, Compare));
		}
	}

void CleanupBitmapObject(TAny* aPtr)
	{
	CBitmapObject* ptr = static_cast<CBitmapObject*>(aPtr);	
	ptr->Close();
	}

/** Create a reference counting object for a new bitmap.

@param aFbTop Reference to the CFbTop singleton.
@param aBmp Bitmap to be attached to the new reference counting object.
	The bitmap must have been pushed on the clean-up stack and it will be
	popped out of the stack by this function. Ownership will be transferred
	to the reference counting object.
@param aReplacement If ETrue the bitmap is being created as a replacement
	for a bitmap being made dirty by a resize or compress operation.
@internalComponent
*/
CBitmapObject* CBitmapObject::NewL(CFbTop& aFbTop, CBitwiseBitmap* aBmp, TBool aReplacement)
	{
	CBitmapObject* bmpObj = new(ELeave) CBitmapObject(aFbTop, aBmp);
	CleanupStack::Pop(aBmp); // aBmp is owned now by bmpObj
	TCleanupItem bitmapObjectCleanupItem(CleanupBitmapObject, bmpObj);
	CleanupStack::PushL(bitmapObjectCleanupItem);
	bmpObj->ConstructL(aReplacement);
	CleanupStack::Pop(bmpObj);
	return bmpObj;
	}

CBitmapObject::~CBitmapObject()
	{
	iFbTop->BackgroundCompression()->RemoveFromCompressionQueue(this);
	// the associated clean bitmap object cannot possibly be now in the background compression queue
	if (iHandle)
		{
		if (Owner() == NULL)
			{
			TInt index = iFbTop->iBitmapObjectIndex.FindInOrder(this, Compare);
			if (index != KErrNotFound)
				{
				if (iCleanBitmap)
					{
					iFbTop->iBitmapObjectIndex[index] = iCleanBitmap;
					}
				else
					{
					iFbTop->iBitmapObjectIndex.Remove(index);
					}
				}
			}
		if (iCleanBitmap != NULL)
			{
			iCleanBitmap->SetOwner(NULL);
			iCleanBitmap->Close();
			}
		}
	delete iAddressPointer;
	}

void CBitmapObject::SetCleanBitmap(CBitmapObject* aNewBmpObj)
	{
	__ASSERT_DEBUG(iHandle, User::Invariant());
	__ASSERT_DEBUG(iCleanBitmap == NULL, User::Invariant());
	__ASSERT_DEBUG(aNewBmpObj->Owner() == NULL, User::Invariant());
	__ASSERT_DEBUG(aNewBmpObj->iHandle == 0, User::Invariant());
	__ASSERT_DEBUG(aNewBmpObj->iCleanBitmap == NULL, User::Invariant());
	iCleanBitmap = aNewBmpObj;
	aNewBmpObj->SetOwner(this);
	aNewBmpObj->iHandle = iHandle;
	iAddressPointer->iSettings.SetDirtyBitmap();
	aNewBmpObj->iAddressPointer->Extra()->iSerialNumber = iAddressPointer->Extra()->iSerialNumber;
	aNewBmpObj->iAddressPointer->Extra()->iTouchCount = iAddressPointer->Extra()->iTouchCount + 1;
	}

void CBitmapObject::Close()
	{
	if ((iCleanBitmap != NULL) && (Owner() != NULL) && (AccessCount() == 2))
		{
		static_cast<CBitmapObject*>(Owner())->iCleanBitmap = iCleanBitmap;
		iCleanBitmap->SetOwner(Owner());
		iHandle = 0;
		Dec();
		}
	CObject::Close();
	}

TInt CBitmapObject::Compare(const CBitmapObject& aBmpObj1, const CBitmapObject& aBmpObj2)
	{
	return aBmpObj1.iHandle - aBmpObj2.iHandle;
	}

TInt CBitmapObject::Compare(const TInt* aHandle, const CBitmapObject& aBmpObj)
	{
	return *aHandle - aBmpObj.iHandle;
	}

// CSharedBitmapObject constructor - takes ownership of aBmp and aKey
CSharedBitmapObject::CSharedBitmapObject(CFbTop& aFbTop, CBitwiseBitmap* aBmp, TDesC* aKey):
	CBitmapObject(aFbTop, aBmp),
	iKey(aKey)
	{
	__ASSERT_DEBUG(iKey, User::Invariant());	
	}

/** Second-phase constructor of reference counting objects for bitmaps
loaded to share from a file. Adds the CSharedBitmapObject to the bitmap
object container, assigns a server-wide handle to it and adds it to the
server's bitmap index and to the server's shared bitmap hash map.

@param aHash Hash value of the key associated with the bitmap.
@internalComponent
*/
void CSharedBitmapObject::ConstructL(TUint aHash)
	{
	CBitmapObject::ConstructL(EFalse);
	iFbTop->iSharedBitmapObjectHashMap.Insert(*this, aHash);
	}

/** Create a reference counting object for a bitmap loaded to share from a file.

@param aFbTop Reference to the CFbTop singleton.
@param aBmp Bitmap to be attached to the new reference counting object.
	The bitmap must have been pushed on the clean-up stack and it will be
	popped out of the stack by this function. Ownership will be transferred
	to the reference counting object.
@param aKey Key associated with the bitmap. The key must have been pushed on
	the clean-up stack and it will be popped out of the stack by this function.
	Ownership will be transferred to the reference counting object.
@param aHash Hash value of the key associated with the bitmap.
@internalComponent
*/
CSharedBitmapObject* CSharedBitmapObject::NewL(CFbTop& aFbTop, CBitwiseBitmap* aBmp, TDesC* aKey, TUint aHash)
	{
	CSharedBitmapObject* bmpObj = new(ELeave) CSharedBitmapObject(aFbTop, aBmp, aKey);
	CleanupStack::Pop(2, aKey); // aBmp and aKey are owned now by bmpObj
	TCleanupItem bitmapObjectCleanupItem(CleanupBitmapObject, bmpObj);
	CleanupStack::PushL(bitmapObjectCleanupItem);
	bmpObj->ConstructL(aHash);
	CleanupStack::Pop(bmpObj);
	return bmpObj;
	}

CSharedBitmapObject::~CSharedBitmapObject()
	{
	iFbTop->iSharedBitmapObjectHashMap.Remove(*this);
	delete iKey;
	}

// Constructs a single key object from constituent parts that uniquely identify a bitmap
HBufC* CSharedBitmapObject::KeyLC(const TDesC& aFileName, TInt aId, const TTime& aModTime)
	{
	static const TInt K16BitSizeOfTInt  = sizeof(TInt) /sizeof(TUint16);
	static const TInt K16BitSizeOfTTime = sizeof(TTime)/sizeof(TUint16);
	
	HBufC* key = HBufC::NewLC(K16BitSizeOfTInt + K16BitSizeOfTTime + aFileName.Length());
	
	TPtr keyPtr = key->Des();
	
	keyPtr.Append(reinterpret_cast<const TUint16*>(&aId),      K16BitSizeOfTInt);
	keyPtr.Append(reinterpret_cast<const TUint16*>(&aModTime), K16BitSizeOfTTime);
	keyPtr.Append(aFileName);

	return key;
	}
