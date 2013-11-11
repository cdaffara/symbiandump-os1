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

#ifndef SERVER_H
#define SERVER_H

#include <e32base.h>
#include <f32file.h>
#include <fbs.h>
#include <sgresource/sgimage.h>
#include "UTILS.H"
#include "FBSMBMC.H"
#include "FbsMessage.H"
#include "glyphatlas.h"

class CFbTop;
class CFbsBackgroundCompression;
class CFbsBackgroundCompressionQueueElement;
class TOpenFontGlyphData;


NONSHARABLE_CLASS(CFontObject): public CObject
/**
@internalComponent
*/
	{
public:
	CFontObject(CFontStore* aFontStore, CGlyphAtlas* aGlyphAtlas);
	~CFontObject();
public:
	CBitmapFont* iAddressPointer;
	CFontStore* iFontStore;
	TInt iHeightInTwips;
	CGlyphAtlas* iGlyphAtlas;
	};

NONSHARABLE_CLASS(CBitmapObject): public CObject
/**
Provides reference counting facilities for bitmaps managed by CFbTop.
@internalComponent
*/
	{
	friend class CFbsBackgroundCompression;
	friend class CFbsBackgroundCompressionQueueElement;
protected:
	CBitmapObject(CFbTop& aFbTop, CBitwiseBitmap* aBmp);
	void ConstructL(TBool aReplacement);
public:
	static CBitmapObject* NewL(CFbTop& aFbTop, CBitwiseBitmap* aBmp, TBool aReplacement);
	~CBitmapObject();
	inline CBitwiseBitmap* Address() const;
	inline TInt Handle() const;
	inline CBitmapObject* CleanBitmap() const;
	inline TBool IsInCompressionQueue() const;
	void SetCleanBitmap(CBitmapObject* aNewBmpObj);
	virtual void Close();
	static TInt Compare(const CBitmapObject& aBmpObj1, const CBitmapObject& aBmpObj2);
	static TInt Compare(const TInt* aHandle, const CBitmapObject& aBmpObj);
protected:
	CFbTop* iFbTop;
private:
	CBitwiseBitmap* iAddressPointer;
	TInt iHandle;
	CBitmapObject* iCleanBitmap;
	CFbsBackgroundCompressionQueueElement* iCompressionQueueElement;
	};

NONSHARABLE_CLASS(CSharedBitmapObject): public CBitmapObject
/**
Provides reference counting facilities for shared bitmaps managed by CFbTop.
Shared bitmaps are only loaded once.  They are uniquely identified by the
combination of the filename, bitmap ID and last modified time of the file.
@internalComponent
*/
	{
friend class CHashMap;
private:
	CSharedBitmapObject(CFbTop& aFbTop, CBitwiseBitmap* aBmp, TDesC* aKey);
	void ConstructL(TUint aHash);
public:
	static CSharedBitmapObject* NewL(CFbTop& aFbTop, CBitwiseBitmap* aBmp, TDesC* aKey, TUint aHash);
	~CSharedBitmapObject();
	static HBufC* KeyLC(const TDesC& aFileName, TInt aId, const TTime& aModTime);
private:
	TDesC* iKey;
	mutable CSharedBitmapObject* iNext;    // Next object in a linked list
	};

NONSHARABLE_CLASS(CFontBitmapServer): public CPolicyServer
/**
@internalComponent
*/
	{
public:
	CFontBitmapServer();
	~CFontBitmapServer();
	static CFontBitmapServer* NewL();
	CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
	TInt Init();
#ifdef _DEBUG
	void ProcMessage(const RMessage2& aMessage, TInt aSession, TInt& aRet);
#else	
	void ProcMessage(const RMessage2& aMessage, TInt aSession);
#endif	
	CFbTop* TopLevelStore();

private:
	void ConstructL();
	void GetHeapSizesL(const RMessage2& aMessage);
	TInt HandleMesgTypefaceSupport(const RMessage2& aMessage, TBool& aClientPanicRequired);
	TInt HandleMesgFontHeight(const RMessage2& aMessage, TBool aInTwips);

protected:
	TUint iConnectionId;
	CFbTop* iTopLevelStore;
	};

class TFontFileIndex
/**
@internalComponent
*/
	{
public:
	TUid iUid;
	TInt iAccessCount;
	};


class CFbClient;

NONSHARABLE_CLASS(TFbClientHelper): public TDblQueLink
/**
@internalComponent
*/
	{
public:
	inline TFbClientHelper(CFbClient& aClient);

public:
	CFbClient& iClient;
	RMessagePtr2 iMessage;
	TBool iDirty;
	};

NONSHARABLE_CLASS(CFbClient): public CSession2
/**
@internalComponent
*/
	{
public:
	static CFbClient* NewL(RHeap* aHeap);
	~CFbClient();
	void Init(TUint aConnectionHandle);
	void CreateL();
	CFontBitmapServer* FontBitmapServer();
	CFbTop* TopLevelStore();
	void CopyFontInfo(CFontObject* aFontObjPtr,TInt aHandle,TFontInfo& aFontInfo);
	void ServiceL(const RMessage2& aMessage);
	void ProcFontMessage(const RMessage2& aMessage);
	void ProcBitmapMessage(const RMessage2& aMessage);
	void NotifyDirtyBitmap(CBitmapObject& aBmpObj);
	void AddFontFileIndexL(TUid aId);
	void RemoveFontFileIndex(TUid aId);
	void Disconnect(const RMessage2 &aMessage);
#ifdef _DEBUG
	void ProcMemMessage (const RMessage2& aMessage);
	void ProcAtlasMessage (const RMessage2& aMessage);
#endif	
private:
	void ConstructL();
	CFbClient(RHeap* aHeap);
	TInt HandleMesgFontDuplicate(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgGetNearestFont(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgGetFontById(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt CopyFontInfoIntoReturnMessage(const RMessage2& aMessage, TBool& aPanicRequired, CFontObject* aFontObj, TInt iWritePosition);
	TInt HandleMesgAddOrInstallFontFile(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgRemoveFontFile(const RMessage2& aMessage);
	TInt HandleMesgRasterize(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgFaceAttrib(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgHasCharacter(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgShapeText(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgShapeDelete(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgGetFontTable(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgGetGlyphOutline(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgReleaseGlyphOutline(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgReleaseFontTable(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgGetGlyphs(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgGetGlyphMetrics(const RMessage2& aMessage, TBool& aPanicRequired);
	TInt HandleMesgOogmStatus(const RMessage2& aMessage);
	void HandleMesgGlyphCacheMetrics(const RMessage2& aMessage);

protected:
	TUint iConnectionHandle;
	CObjectIx* iIx;
	TInt iResourceCount;
	CArrayFixFlat<TFontFileIndex>* iFontFileIndex;
	RHeap* iHeap;							// the heap containing iOpenFontGlyphData
	TOpenFontGlyphData* iOpenFontGlyphData;	// the metrics and bitmap, in shared memory, of the last character rasterized
	TInt iSessionHandle;					// used to identify client-specific glyph caches for the new font system
	TFbClientHelper* iHelper;				// used to notify client of any bitmap becoming dirty

private:
	RBuf16 iTextToShape;						// buffer used to hold incoming text that needs shaping
	RArray<RSgImage> iGlyphImagesInTransit;	// last glyph images retrieved from glyph atlas
#ifdef _DEBUG	
	TBool iOwnHeapCheck; //for the process heap - current state
	TBool iHeapCheck;	//for iHeap - current state
	TBool iOwnHeapCheckFlip; //for the process heap - to switch the current state
	TBool iHeapCheckFlip;	//for iHeap - used to switch the current state
	TInt iOwnHeapFailNumber; //the number for the memory fail to be set to. -1: don't set.
	TInt iSharedHeapFailNumber; //the number for the memory fail to be set to. -1 don't set
	TInt iRet;
	TInt iFontDuplicateToFail; //when this is set calls to HandleMesgFontDuplicate() fail for the class.
#endif	
	};

NONSHARABLE_CLASS(CFbActiveScheduler): public CActiveScheduler
/**
@internalComponent
*/
	{
public:
	CFbActiveScheduler();
	virtual void Error(TInt anError) const;
	};

class CFontStore;

NONSHARABLE_CLASS(CHashMap): public CBase
/**
A HashMap data structure for storing pointers to CSharedBitmapObjects.
Provides fast insert/lookup/removal based on a key.
@internalComponent
*/
	{
public:
	CHashMap();
	~CHashMap();

	void Insert(CSharedBitmapObject& aBmpObj, TUint aHash);
	CSharedBitmapObject* Lookup(const TDesC& aKey, TUint aHash) const;
	TInt Remove(const CSharedBitmapObject& aBmpObj);
	static TUint Hash(const TDesC16& aKey);

private:
	CSharedBitmapObject* iHashTable[1<<KFBSERVSharedBitmapHashTableSizeInBits]; // Array of pointers to singularly linked lists.  Linked list elements are CSharedBitmapObjects.
	TInt iTableSize;
	TInt iMask;
	};


IMPORT_C extern const TInt KFbServSharedHeapMaxSize;
IMPORT_C extern const TInt KFbServGlyphAtlasCacheLimit;


NONSHARABLE_CLASS(CFbTop): public CBase
/**
@internalComponent
*/
	{
	friend class CFontBitmapServer;
	friend class CBitmapObject;
	friend class CSharedBitmapObject;
public:
	~CFbTop();
	static CFbTop* NewL();
	TInt GetNearestFont(CFontObject*& aFontObjPtr, TFbsMessage aMessage, const TFontSpec& aFontSpec, TInt aMaxHeight = 0);
	TInt GetFontById(CFontObject*& aFontObjPtr,TUid aUid,const TAlgStyle& aAlgStyle);
	CBitmapObject* CreateBitmapL(const TSize& aSize, TDisplayMode aDispMode, TUid aUid, TBool aReplacement, TInt aDataSize = 0);
	CBitmapObject* LoadBitmapL(const TDesC& aFilename, TInt32 aId, TUint aFileOffset,RFile* aFile, TInt aSessionHandle);
	CSharedBitmapObject* ShareBitmapL(TDes& aFilename, TInt32 aId, TUint aFileOffset,RFile* aFile, TInt aSessionHandle);
	TInt GetCleanBitmap(CBitmapObject*& aBmpObjPtr);
	CBitmapObject* FindBitmap(TInt aHandle);
	TBool ValidFontHandle(TInt aHandle);
	CFontStore* FontStore() const;
	RHeap* Heap() const;
	CChunkPile* Pile() const;
	TInt HeapBase() const;
	void SetFontNameAliasL(const RMessage2& aMessage);
	void SetDefaultLanguageForMetrics(const RMessage2& aMessage);
	inline CFbsBackgroundCompression* BackgroundCompression() const;
	void CloseFileStores(TInt aSessionHandle);
	void SetSystemDefaultTypefaceName(const TDesC& aFontTypefaceName);
	TInt GetAllBitmapHandles(const RMessage2& aMessage) const;
	inline RFs& FileSession();
	void AddClientHelper(TFbClientHelper& aHelper);
	void NotifyDirtyBitmap(CBitmapObject& aBmpObj, CFbClient* aClient);
	TInt BitmapConUniqueID() const;
	TInt FontConUniqueID() const;
	CGlyphAtlas* GlyphAtlas() const;
private:
	CFbTop();
	void ConstructL();
	CBitwiseBitmap* DoLoadBitmapLC(const TDesC& aFilename, TInt32 aId, TUint aFileOffset, RFile* aFile, TInt aSessionHandle);
	void LoadOpenFontLibraries();
	void SafeInstallOfRasterizerL(TUid aInterfaceImplUid);
	TInt FindFontNameAlias(const TDesC& aAlias);
	void LoadShaperFactories();
	void SafeInstallOfShaperFactoryL(TUid aInterfaceImplUid);
	TInt GetFontObjectFromFont(CFontObject*& aFontObjPtr, CFont* aFont);
	TInt GetNearestNonAliasedFont(CFont*& aFont, TFbsMessage aMessage, const TFontSpec&	aFontSpec, TInt	aMaxHeight);
private:
	RFs iFilesys;
	CFontStore* iFontStore;
	CObjectCon* iBitmapCon;
	CObjectCon* iFontCon;
	CObjectConIx* iConIx;
	RHeap* iHeap;
	RChunk iChunk;
	RChunk iLargeBitmapChunk;
	RMutex iDebugMutex; // used only in builds with SYMBIAN_DEBUG_FBS_LOCKHEAP defined
	CChunkPile* iPile;
	RPointerArray<HBufC> iFontNameAlias;
	RPointerArray<CBitmapObject> iBitmapObjectIndex;
	CHashMap iSharedBitmapObjectHashMap;
	CFbTopStreamIdCache* iMBMCache;
	TLanguage iDefaultLanguageForMetrics;
	CFbsBackgroundCompression* iBackgroundCompression;
	TBuf<KMaxTypefaceNameLength> iSystemDefaultFontTypefaceName;
	TDblQue<TFbClientHelper> iClientHelpers;
	TInt64 iNextAvailableSerialNumber;
	CGlyphAtlas* iGlyphAtlas;
	};

inline CBitwiseBitmap* CBitmapObject::Address() const
	{
	return iAddressPointer;
	}

inline TInt CBitmapObject::Handle() const
	{
	return iHandle;
	}

inline CBitmapObject* CBitmapObject::CleanBitmap() const
	{
	return iCleanBitmap;
	}

inline TBool CBitmapObject::IsInCompressionQueue() const
	{
	return iCompressionQueueElement != NULL;
	}

inline TFbClientHelper::TFbClientHelper(CFbClient& aClient)
	: iClient(aClient), iDirty(EFalse)
	{}

inline CFbsBackgroundCompression* CFbTop::BackgroundCompression() const
	{
	__ASSERT_DEBUG(iBackgroundCompression != NULL, User::Invariant());
	return iBackgroundCompression;
	}

/**
It returns the file session handle.

@internalComponent
*/
inline RFs& CFbTop::FileSession()
	{
	return iFilesys;
	}

#endif // SERVER_H

