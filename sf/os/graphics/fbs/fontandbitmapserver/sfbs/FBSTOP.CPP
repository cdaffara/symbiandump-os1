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

#include <hal.h>
#include <gdi.h>
#include <fntstore.h>
#include <bitmap.h>
#include <ecom/ecom.h>
#include <graphics/bitmapuid.h>
#include "SERVER.H"
#include "BackGroundCompression.h"
#include "BitwiseBitmap.inl"
#include "bitmapconst.h"
#include <graphics/openfontconstants.h>
#include <graphics/openfontrasterizer.h>
#include <graphics/gdi/glyphsample.h>
#include "glyphatlas.h"
#include "FbsMessage.H"

// Local utility functions
void ListImplementationsWithRetry(TUid& aInterfaceUid, RImplInfoPtrArray &aImplementationArray, TBool aRomOnly);


CFbTop::CFbTop():
	iDefaultLanguageForMetrics(ELangNone)
	{
	}

CFbTop::~CFbTop()
	{
	if (iConIx)
		{
		iConIx->Remove(iBitmapCon);
		iConIx->Remove(iFontCon);
		}
	// there are no bitmap objects left, so the background compression queue must be empty
	delete iBackgroundCompression;
	delete iMBMCache;
	delete iFontStore;
	iFilesys.Close();
	iHeap->Check();
	__RHEAP_MARKEND(iHeap);
	delete iPile;
	iHeap->Close();
	iChunk.Close();
	iLargeBitmapChunk.Close();
	delete iConIx;
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	iDebugMutex.Close();
#endif
	iFontNameAlias.ResetAndDestroy();
	iBitmapObjectIndex.Reset();
	delete iGlyphAtlas;
	REComSession::FinalClose();
	}

CFbTop* CFbTop::NewL()
	{
	CFbTop* pT=new(ELeave) CFbTop;
	CleanupStack::PushL(pT);
	pT->ConstructL();
	CleanupStack::Pop();
	return(pT);
	}

void CFbTop::ConstructL()
	{
	TInt maxmem = 0;
	HAL::Get(HALData::EMemoryRAM, maxmem);
	ASSERT(maxmem > 0);
	TInt maxHeapSize = Min(maxmem, KFbServSharedHeapMaxSize);
	
	TChunkHeapCreateInfo sharedHeapCreateInfo(KFBSERVInitialHeapSize, maxHeapSize);
	sharedHeapCreateInfo.SetCreateChunk(&KFBSERVSharedChunkName);
	sharedHeapCreateInfo.SetSingleThread(EFalse);
	sharedHeapCreateInfo.SetAlignment(0);
	sharedHeapCreateInfo.SetGrowBy(KMinHeapGrowBy * KFBSERVHeapGrowByMultiplier);

	if(KFbServWritableDataPagingMode == EFbsWdpPageBitmapDataAndSharedHeapChunksOnly)
		{
		//Request that the shared heap chunk is paged.
		sharedHeapCreateInfo.SetPaging(TChunkHeapCreateInfo::EPaged);
		}
	else
		{
		//Use the creating process's paging attributes.
		sharedHeapCreateInfo.SetPaging(TChunkHeapCreateInfo::EUnspecified);
		}

	iHeap = UserHeap::ChunkHeap(sharedHeapCreateInfo);
	
	User::LeaveIfError(iChunk.OpenGlobal(KFBSERVSharedChunkName,ETrue));
	TInt virtualSize = CChunkPile::VirtualSize();
	
	TChunkCreateInfo createInfo;
	createInfo.SetDisconnected(0, 0, virtualSize);
	createInfo.SetGlobal(KFBSERVLargeChunkName);
	createInfo.SetOwner(EOwnerProcess);
	createInfo.SetClearByte(0xff); // clear to white on creation

	if(KFbServWritableDataPagingMode == EFbsWdpPageBitmapDataChunkOnly || 
		KFbServWritableDataPagingMode == EFbsWdpPageBitmapDataAndSharedHeapChunksOnly)
		{
		//Request that large bitmap chunk is paged.
		createInfo.SetPaging(TChunkCreateInfo::EPaged);
		}
	else
		{
		//Use the creating process's paging attributes.
		createInfo.SetPaging(TChunkCreateInfo::EUnspecified);
		}

	User::LeaveIfError(iLargeBitmapChunk.Create(createInfo));
	__RHEAP_MARK(iHeap);
	iConIx=CObjectConIx::NewL();
	iBitmapCon=iConIx->CreateL();
	iFontCon=iConIx->CreateL();
	User::LeaveIfError(iFilesys.Connect());
#ifdef SYMBIAN_DEBUG_FBS_LOCKHEAP
	User::LeaveIfError(iDebugMutex.CreateGlobal(KFBSERVDebugMutexName));
#endif
	iPile = CChunkPile::NewL(iLargeBitmapChunk);
	iFontStore=CFontStore::NewL(iHeap);
	//Constructing a cache to store the stream ids of the bitmaps from a mbm file.
	//The cache here will store maximum 30 bitmaps before  & maximum 30 after the 
	//current loaded bitmap.These values are chosen as to optimize the boottime performance
	//as we notice during the boottime logs,sequential loading of bitmaps never exceed 30 bitmaps.
	// The cache will also store maximum 5 mbm files. This number must be as low as possible
	// while trying to minimize flushing of the cache due to mbm file switching.
	iMBMCache=new (ELeave) CFbTopStreamIdCache(30,30,5);

	LoadOpenFontLibraries();
	iFontStore->LoadFontsAtStartupL();
	LoadShaperFactories();
	TRAP_IGNORE(iGlyphAtlas = CGlyphAtlas::NewL(KFbServGlyphAtlasCacheLimit);)
#ifdef _DEBUG
	if (!iGlyphAtlas)
	    {
        RDebug::Printf("FBSERV failed to initialize Glyph Atlas");
	    }
#endif

	// start a new thread for background compression after all the other objects have been created
	iBackgroundCompression = CFbsBackgroundCompression::NewL(*this);
	}




/*
Load all ECOM implemented rasterizer DLLs.
*/

void CFbTop::LoadOpenFontLibraries()
	{
	RImplInfoPtrArray implementationArray;
	TUid uid = {KUidOpenFontRasterizerPlunginInterface};

	// get implementation list
	ListImplementationsWithRetry(uid, implementationArray, EFalse);

	const TInt availCount = implementationArray.Count();
	for (TInt count=0; count < availCount; ++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		// Create & install a rasterizer
		// ignore Leaves, as any necessary cleanup will have already been done through the cleanup stack
		TRAP_IGNORE(SafeInstallOfRasterizerL(info->ImplementationUid()));
		}
	
	// free memory
	implementationArray.ResetAndDestroy();
	}


/*
Load all ECOM implemented shaper factory DLLs.
*/	
void CFbTop::LoadShaperFactories()
	{
	RImplInfoPtrArray implementationArray;
	TUid uid = {KUidShaperFactoryPlunginInterface};

	// get implementation list
	ListImplementationsWithRetry(uid, implementationArray, ETrue);

	const TInt availCount = implementationArray.Count();
	for (TInt count=0;count<availCount;++count)
		{
		const CImplementationInformation* info = implementationArray[count];
		// Create & install a shaper factory
		// ignore Leaves, as any necessary cleanup will have already been done through the cleanup stack
		TRAP_IGNORE(SafeInstallOfShaperFactoryL(info->ImplementationUid()));
		}

	// free memory
	implementationArray.ResetAndDestroy();
	}


void ListImplementationsWithRetry(TUid& aInterfaceUid, RImplInfoPtrArray &aImplementationArray, TBool aRomOnly)
	{
	// Making sure that no race situation arises between FBserv and Ecom
	// If ECom is not ready, give it another chance and try again. if it still doesn't work 
	// after the third try, then it just carries on quietly and fails... 
	for (TInt ecomnotready =0; ecomnotready <3; ecomnotready++)
		{
		TInt ecomError = KErrNone;
		if (aRomOnly)
			{
			TEComResolverParams resParams;
			TRAP(ecomError, REComSession::ListImplementationsL(aInterfaceUid, resParams, KRomOnlyResolverUid, aImplementationArray));
			}
		else
			{ // default resolver
			TRAP(ecomError, REComSession::ListImplementationsL(aInterfaceUid, aImplementationArray));
			}

		if (!ecomError)
			{
			return;
			}
		else
			{
			User::After(0);
			}
		}
	}

// utility methods to transfer ownership, or destroy object on failure
void CFbTop::SafeInstallOfRasterizerL(TUid aInterfaceImplUid)
	{
	COpenFontRasterizer* rasterizer = COpenFontRasterizer::NewL(aInterfaceImplUid);
	CleanupStack::PushL(rasterizer);
	// Install it in the font store.
	iFontStore->InstallRasterizerL(rasterizer);
	CleanupStack::Pop(rasterizer);
	}


void CFbTop::SafeInstallOfShaperFactoryL(TUid aInterfaceImplUid)
	{
	CShaperFactory* shaperFactory = CShaperFactory::NewL(aInterfaceImplUid);
	CleanupStack::PushL(shaperFactory);
	// Install it in the font store.
	iFontStore->InstallShaperFactoryL(shaperFactory);
	CleanupStack::Pop(shaperFactory);
	}

/**
Gets the nearest matching font for a given font specification. If the named font cannot be found 
font aliases are checked. 

@param aFontObjPtr On success this contains the font object that is the closest match.
@param aMessage The font request message.
@param aFontSpec The font spec to match.
@param aMaxHeight The maximum height of the font to match.
@return KErrNone if successful, KErrNotFound if the font is not found, or one of the other System Error codes.
 */
TInt CFbTop::GetNearestFont(
	CFontObject*&		aFontObjPtr,
	TFbsMessage			aMessage,
	const TFontSpec&	aFontSpec,
	TInt				aMaxHeight)
	{
#ifdef _DEBUG
	User::Heap().Check();
	iHeap->Check();
#endif
	aFontObjPtr = NULL;

	TFontSpec fontSpec(aFontSpec);

	// Check if the font typeface is empty and if so use the system default font if it is set
	if (fontSpec.iTypeface.iName.Length() == 0 && iSystemDefaultFontTypefaceName.Length() != 0)
		{
		fontSpec.iTypeface.iName = iSystemDefaultFontTypefaceName;
		}

	if (GlyphSample::EScriptDefault == fontSpec.ScriptTypeForMetrics())
		{
		fontSpec.SetScriptTypeForMetrics(iDefaultLanguageForMetrics);
		}

	// Find the requested font
	TBool familyNameExistsInTypefaceStore = iFontStore->HaveTypefaceFamilyName(fontSpec.iTypeface.iName);
	
	// If the font is not found try finding an alias font
	if (!familyNameExistsInTypefaceStore) 
		{
		TInt aliasIndex = FindFontNameAlias(fontSpec.iTypeface.iName);
		//KErrNotFound is the only error which can be returned
		if (aliasIndex != KErrNotFound)
			{
			fontSpec.iTypeface.iName = *iFontNameAlias[aliasIndex + 1];
			}
		}

	CFont* font = NULL;
	TInt ret = GetNearestNonAliasedFont(font, aMessage, fontSpec, aMaxHeight);

	if (ret != KErrNone)
		{
		return ret;
		}
	
	return GetFontObjectFromFont(aFontObjPtr, font);
	}
	
/**
Gets the nearest matching loaded font for a given font specification.

@param aFont On success this contains the font object that is the closest match.
@param aMessage The font request message.
@param aFontSpec The font spec to match.
@param aMaxHeight The maximum height of the font to match.
@return KErrNone if successful, KErrNotFound if the font is not found, or one of the other System Error codes.
*/
TInt CFbTop::GetNearestNonAliasedFont(CFont*& aFont, TFbsMessage aMessage, const TFontSpec&	aFontSpec, TInt	aMaxHeight)
	{
	TInt ret = KErrNotSupported;
	switch (aMessage)
		{
		case EFbsMessGetNearestFontToDesignHeightInTwips:
			{
			ret = iFontStore->GetNearestFontToDesignHeightInTwips(aFont, aFontSpec);
			break;
			}
		case EFbsMessGetNearestFontToDesignHeightInPixels:
			{
			ret = iFontStore->GetNearestFontToDesignHeightInPixels(aFont, aFontSpec);
			break;
			}
		case EFbsMessGetNearestFontToMaxHeightInTwips:
			{
			ret = iFontStore->GetNearestFontToMaxHeightInTwips(aFont, aFontSpec, aMaxHeight);
			break;
			}
		case EFbsMessGetNearestFontToMaxHeightInPixels:
			{
			ret = iFontStore->GetNearestFontToMaxHeightInPixels(aFont, aFontSpec, aMaxHeight);
			break;
			}
		}
	return ret;
	}

TInt CFbTop::GetFontObjectFromFont(CFontObject*& aFontObjPtr, CFont* aFont)
	{
	// First, check if a CFontObject exists for this CFont.
	// If so, increment its reference count and return it.
	for (TInt ii = iFontCon->Count() - 1; ii >= 0; --ii)
		{
		CFontObject* fontObjPtr = reinterpret_cast<CFontObject*>((*iFontCon)[ii]);
		if (fontObjPtr->iAddressPointer == reinterpret_cast<CBitmapFont*>(aFont))
			{
			aFontObjPtr = fontObjPtr;
			// The CFontObject instance keeps the reference count of the CBitmapFont, 
			// not the font store. There is only one CFontObject instance
			// per CBitmapFont, so to keep the reference count at 1 in the fontstore
			// call ReleaseFont(). 
			iFontStore->ReleaseFont(aFont);
			return fontObjPtr->Open();
			}
		}
	
	// Existing FontObject not found, so create new one.
	CFontObject* fontObjPtr = new CFontObject(iFontStore, iGlyphAtlas);
	if (!fontObjPtr)
		{
		iFontStore->ReleaseFont(aFont);
		return KErrNoMemory;
		}
	
	fontObjPtr->iAddressPointer = reinterpret_cast<CBitmapFont*>(aFont);
	fontObjPtr->iHeightInTwips = ((aFont->HeightInPixels() * iFontStore->iKPixelHeightInTwips) + 667) / 1000;
	TRAPD(ret, iFontCon->AddL(fontObjPtr));
	if (ret != KErrNone)
		{
		fontObjPtr->Close();
		}
	else
		{ // transfer ownership
		aFontObjPtr = fontObjPtr;
		}
	return ret;
	}


/** Create a Bitmap Font, from a UID and Algorithmic drawing Style see CFontStore::GetFontById()
@internalComponent
*/
TInt CFbTop::GetFontById(CFontObject*& aFontObjPtr,TUid aUid,const TAlgStyle& aAlgStyle)
	{
#ifdef _DEBUG
	User::Heap().Check();
	iHeap->Check();
#endif
	aFontObjPtr=NULL;
	CBitmapFont* font=NULL;
	TInt ret=iFontStore->GetFontById((CFont*&)font,aUid,(TAlgStyle&)aAlgStyle);
	if (ret != KErrNone)
		{
		return ret;
		}
	return GetFontObjectFromFont(aFontObjPtr, font);
	}


/** Create a bitmap with the given size, display mode and UID.

@param aSize Size of the bitmap in pixels.
@param aDispMode Display mode of the bitmap.
@param aUid The UID to use for bitmap creation. This can be:
	- KUidCFbsBitmapCreation for standard bitmaps.
	- The application UID for hardware bitmaps.
	- The data type UID for extended bitmaps.
@param aReplacement If ETrue the bitmap is being created as a replacement
	for a bitmap being made dirty by a resize or compress operation.
@param aDataSize If different from zero, it indicates that the bitmap to create
	is an extended bitmap and specifies the size in bytes of the bitmap.
	If equal to zero, it indicates that the bitmap to create is a standard
	bitmap or a hardware bitmap, depending on the value of aUid, and the size
	in bytes is calculated from the size in pixels and the display mode.
@internalComponent
*/
CBitmapObject* CFbTop::CreateBitmapL(const TSize& aSize, TDisplayMode aDispMode, TUid aUid, TBool aReplacement, TInt aDataSize)
	{
#ifdef _DEBUG
	User::Heap().Check();
	iHeap->Check();
#endif
	CBitwiseBitmap* bmp=(CBitwiseBitmap*)iHeap->AllocL(sizeof(CBitwiseBitmap) + sizeof(CBitwiseBitmap::TExtra));
	new(bmp) CBitwiseBitmap(iHeap,iPile);
	CleanupDeletePushL(bmp);  // CBitwiseBitmap is not derived from CBase!

	if (aDataSize == 0)
		User::LeaveIfError(bmp->Construct(aSize, aDispMode, aUid));
	else
		User::LeaveIfError(bmp->ConstructExtended(aSize, aDispMode, aUid, aDataSize));
	// bmp popped out of the clean-up stack by NewL
	CBitmapObject* bmpObj = CBitmapObject::NewL(*this, bmp, aReplacement);
	if (!aReplacement)
		{
		bmp->Extra()->iSerialNumber = iNextAvailableSerialNumber++;
		}

	return bmpObj;
	}

CBitmapObject* CFbTop::LoadBitmapL(const TDesC& aFilename, TInt32 aId, TUint aFileOffset, RFile* aFile, TInt aSessionHandle)
	{
	CBitwiseBitmap* bmp=DoLoadBitmapLC(aFilename, aId, aFileOffset, aFile, aSessionHandle);
	// bmp popped out of the clean-up stack by NewL
	CBitmapObject* bmpObj = CBitmapObject::NewL(*this, bmp, EFalse);

	return bmpObj;
	}

_LIT(KZDrive, "z:");

CBitwiseBitmap* CFbTop::DoLoadBitmapLC(const TDesC& aFilename, TInt32 aId, TUint aFileOffset, RFile* aFile, TInt aSessionHandle)
	{
#ifdef _DEBUG
	User::Heap().Check();
	iHeap->Check();
#endif
	CBitwiseBitmap* bmp=(CBitwiseBitmap*)iHeap->AllocL(sizeof(CBitwiseBitmap) + sizeof(CBitwiseBitmap::TExtra));
	new(bmp) CBitwiseBitmap(iHeap,iPile);
	bmp->Extra()->iSerialNumber = iNextAvailableSerialNumber++;
	CleanupDeletePushL(bmp);  // CBitwiseBitmap is not derived from CBase!

	if (NULL == aFile)
		{
		// In this case file should be in z: drive
		// so load the bitmap from the mbm cache
		TStreamId streamid(0);
		streamid=iMBMCache->GetStreamIdL(iFilesys,aFilename,aId,aFileOffset,aSessionHandle);
		bmp->ConstructL(iMBMCache->MruFileStore(),streamid);
		}
	else
		{
		//only use the cache when it is Rom File which is read only because when using
		//the cache the file store is always opened as read access until it is replaced by
		//another different file, Trying to write it(RAM file) will cause access violation
		//and therefore we have to split the implementation into two parts one for ROM
		//and one for RAM
		if (aFilename.Left(2).CompareF(KZDrive))
			{
			// File is not in ROFS
			bmp->ConstructL(*aFile,aId,aFileOffset);
			}
		else
			{
			// File is in ROFS
			TStreamId streamid(0);
			streamid=iMBMCache->GetStreamIdL(*aFile,aFilename,aId,aFileOffset,aSessionHandle);

			bmp->ConstructL(iMBMCache->MruFileStore(),streamid);
			}
		}
	return bmp;
	}


/* Similar to LoadBitmap.
This function only performs a load the first time it is called for a
particular bitmap.  Subsequent calls increment a reference counting object.

Upon return, aBmpObjPtr points to an object containing a pointer to the loaded bitmap.
*/
CSharedBitmapObject* CFbTop::ShareBitmapL(TDes& aFilename, TInt32 aId, TUint aFileOffset, RFile* aFile, TInt aSessionHandle)
	{
	TTime modtime(0);
	if (aFilename.Left(2).CompareF(KZDrive))// instead of Compare, CompareF is used to perform folding prior to Compare which is safe with Unicode.
		{
		// file is not in z: drive so it should not be null.
		__ASSERT_DEBUG(aFile != NULL, User::Panic(KFBSERVPanicCategory, KErrBadHandle));
		// File is not in ROM so ModTime is needed to identify it
		User::LeaveIfError(aFile->Modified(modtime));
		}

	// Creation of the key is performed here so that it can potentially be
	// reused in both object lookup and object creation
	HBufC* key = CSharedBitmapObject::KeyLC(aFilename, aId, modtime);

	// Calculation of the hash value is performed here so that it can 
	// potentially be reused in both object lookup and object insert.
	const TUint hash = iSharedBitmapObjectHashMap.Hash(*key);    

	CSharedBitmapObject* bmpObj = iSharedBitmapObjectHashMap.Lookup(*key, hash);
	
	if (bmpObj)
		{
		// Bitmap already in memory
		CleanupStack::PopAndDestroy(key);      // key will not be needed
		User::LeaveIfError(bmpObj->Open());    // increase reference count
		}
	else
		{
		// Bitmap not in memory
		CBitwiseBitmap* bmp = DoLoadBitmapLC(aFilename, aId, aFileOffset, aFile, aSessionHandle);
		// bmp and key popped out of the clean-up stack by NewL
		bmpObj = CSharedBitmapObject::NewL(*this, bmp, key, hash);
		}

	return bmpObj;
	}

TInt CFbTop::GetCleanBitmap(CBitmapObject*& aBmpObjPtr)
	{
	while (aBmpObjPtr->CleanBitmap() != NULL)
		{
		aBmpObjPtr = aBmpObjPtr->CleanBitmap();
		}
	if (aBmpObjPtr->IsInCompressionQueue())
		return KErrInUse;
	return KErrNone;
	}

CBitmapObject* CFbTop::FindBitmap(TInt aHandle)
	{
	TInt index = iBitmapObjectIndex.FindInOrder(aHandle, CBitmapObject::Compare);
	if (index != KErrNotFound)
		return iBitmapObjectIndex[index];
	return NULL;
	}

TBool CFbTop::ValidFontHandle(TInt aHandle)
	{
	TInt limit=iFontCon->Count();
	for(TInt count=0;count<limit;count++)
		if(aHandle==(TInt)((*iFontCon)[count]))
			return(ETrue);
	return(EFalse);
	}
	
CFontStore* CFbTop::FontStore() const
	{
	return(iFontStore);
	}

RHeap* CFbTop::Heap() const
	{
	return(iHeap);
	}

CChunkPile* CFbTop::Pile() const
	{
	return(iPile);
	}

TInt CFbTop::HeapBase() const
	{
	return(TInt(iChunk.Base()));
	}

void CFbTop::SetFontNameAliasL(const RMessage2& aMessage)
	{
	const TInt aliasNameLength = aMessage.Int1();
	if (aliasNameLength <= 0)
		return; // No alias name to set
	
	if(aliasNameLength * sizeof(TText) * 2 >= KMaxTInt)
		{
		aMessage.Panic(KFBSERVPanicCategory,KErrArgument);
		return;
		}
		
	HBufC* aliasName = HBufC::NewMaxLC(aliasNameLength);
	TPtr aliasNamePtr(aliasName->Des());
	aMessage.ReadL(0,aliasNamePtr);

	const TInt aliasIndex = FindFontNameAlias(*aliasName);

	const TInt fontNameLength = aMessage.Int3();

	if (fontNameLength > 0)
		{ // Set or change an alias
		HBufC* fontName = HBufC::NewMaxLC(fontNameLength);
		TPtr fontNamePtr(fontName->Des());
		aMessage.ReadL(2,fontNamePtr);

		if (aliasIndex != KErrNotFound)
			{ // Change an existing alias
			delete iFontNameAlias[aliasIndex + 1];
			iFontNameAlias[aliasIndex + 1] = fontName;

			CleanupStack::Pop(); // fontName
			CleanupStack::PopAndDestroy(); // aliasName
			}
		else
			{ // Set a new alias
			User::LeaveIfError(iFontNameAlias.Append(aliasName));
			TInt ret = iFontNameAlias.Append(fontName);
			if (ret != KErrNone)
				{
				iFontNameAlias.Remove(iFontNameAlias.Count() - 1);
				User::Leave(ret);
				}

			CleanupStack::Pop(); // fontName
			CleanupStack::Pop(); // aliasName
			}
		}
	else
		{ // No fontName so delete the alias
		CleanupStack::PopAndDestroy(); // aliasName
		if (aliasIndex != KErrNotFound)
			{
			delete iFontNameAlias[aliasIndex];
			iFontNameAlias.Remove(aliasIndex);
			delete iFontNameAlias[aliasIndex];
			iFontNameAlias.Remove(aliasIndex);
			}
		}
	}

TInt CFbTop::FindFontNameAlias(const TDesC& aAlias)
	{
	const TInt fontNameAliasCount = iFontNameAlias.Count();

	for (TInt index = 0; index < fontNameAliasCount; index += 2)
		{
		if ((*iFontNameAlias[index]).CompareF(aAlias)==0)
			{
			return index;
			}
		}

	return KErrNotFound;
	}

void CFbTop::SetDefaultLanguageForMetrics(const RMessage2& aMessage)
	{
	iDefaultLanguageForMetrics = static_cast<TLanguage>(aMessage.Int0());
	}

void CFbTop::CloseFileStores(TInt aSessionHandle)
	{
	iMBMCache->CloseFileStores(aSessionHandle);
	}

void CFbTop::SetSystemDefaultTypefaceName(const TDesC& aFontTypefaceName)
	{
	iSystemDefaultFontTypefaceName = aFontTypefaceName;
	}

TInt CFbTop::GetAllBitmapHandles(const RMessage2& aMessage) const
	{
	TPckgBuf<TInt> handleBuffer; // Use this buffer to store the bitmap handles to write to the message buffer
	const TInt numBitmaps = iBitmapObjectIndex.Count();
	TInt ret = KErrNone;
	for (TInt count=0; count<numBitmaps; ++count)
		{
		handleBuffer() = iBitmapObjectIndex[count]->Handle();
		ret = aMessage.Write(0, handleBuffer, KNumBytesPerBitmapHandle * count);
		if (ret!=KErrNone)
			break;
		}	
	return ret;
	}

void CFbTop::AddClientHelper(TFbClientHelper& aHelper)
	{
	iClientHelpers.AddLast(aHelper);
	}

void CFbTop::NotifyDirtyBitmap(CBitmapObject& aBmpObj, CFbClient* aClient)
	{
	TDblQueIter<TFbClientHelper> iterator(iClientHelpers);
	TFbClientHelper* helper;
	while ((helper = iterator++) != NULL)
		{
		if (aClient != &helper->iClient)
			helper->iClient.NotifyDirtyBitmap(aBmpObj);
		}
	}

TInt CFbTop::BitmapConUniqueID() const
	{
	return iBitmapCon->UniqueID();
	}

TInt CFbTop::FontConUniqueID() const
	{
	return iFontCon->UniqueID();
	}

CGlyphAtlas* CFbTop::GlyphAtlas() const
	{
	return iGlyphAtlas;
	}
