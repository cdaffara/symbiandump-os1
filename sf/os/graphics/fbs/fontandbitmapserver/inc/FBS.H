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

#ifndef __FBS_H__
#define __FBS_H__

/**
@file
@publishedAll
@released
 */
#include <e32std.h>
#include <gdi.h>
#include <fntstore.h>
#include <bitmap.h>
#include <graphics/fbsdefs.h>

/**
@publishedAll
@released
 */
const TUid KCFbsFontUid = { 268435518 };

/**
Indicates version of the font and bitmap server without heap locking.
@publishedAll
@released
*/
#define SYMBIAN_FBSERV_V2

/**
Indicates version of FBS that provides the CFbsFont extension 
interfaces RFbsGlyphDataIterator for using hardware glyphs, and
RFbsGlyphMetricsArray.
@publishedAll
@prototype
*/
//#define SYMBIAN_FBSERV_GLYPHDATA

/** 
Character width and adjustment information. 
@publishedAll
@released
*/
class SCharWidth
	{
public:
	/** The left adjust of the character. */
	TInt iLeftAdjust;
	/** The right adjust of the character. */
	TInt iRightAdjust;
	/** The total width of the character, defined as iLeftAdjust + iWidth + iRightAdjust. */
	TInt iMove;
	/** The width of the character. */
	TInt iWidth;
	};

class CFbsRalCache;
class CFbsSessionHelper;
class CFbsRasterizer;
class TGlyphCacheMetrics;
class TFbsOogmMessage;

/** 
A session with the font and bitmap server. 
@publishedAll 
@released
*/
class RFbsSession : protected RSessionBase
	{
	// Allow access to internals for test
	friend class RClean;
	friend class TestFbs;
	friend class CTBitmap;
	friend class CTClean;
	friend class CTFbs;
	
	friend class CFbsBitmap;
	friend class CFbsBitmapAsyncStreamer;
public:
	IMPORT_C RFbsSession();
	IMPORT_C static TInt Connect();
	IMPORT_C static TInt Connect(RFs& aFileServer);
	IMPORT_C static void Disconnect();
	IMPORT_C static RFbsSession* GetSession();
	IMPORT_C void CallBack();
	IMPORT_C void SetCallBack(TCallBack aCallBack);
	IMPORT_C void ResetCallBack();
	IMPORT_C TInt ResourceCount();
	IMPORT_C TInt SendCommand(TInt aMessage,TInt aInt0=0,TInt aInt1=0,TInt aInt2=0,TInt aInt3=0) const;
	TInt SendCommand(TInt aMessage, const TIpcArgs& aArgs) const;
	void SendCommand(TInt aMessage, const TIpcArgs& aArgs, TRequestStatus &aStatus) const;
	IMPORT_C TVersion Version();
	IMPORT_C TUint8* HeapBase() const;
	IMPORT_C TInt GetHeapSizes(TInt& aDefaultHeap, TInt& aSmallBmpHeap, TInt& aBigBmpChunk);
	TInt SessionHandle() const { return Handle(); }
	TInt AllocScanLineBuffer(TInt aSize);
	HBufC8* GetScanLineBuffer();
	void SetCallBackPtr(TInt* aBitmapHandle)const;
	HBufC8* GetDecompressionBuffer(TInt aSize);
	HBufC8* GetExtraBuffer(TInt aSize);
    TInt ServerSessionHandle() const;
    IMPORT_C TInt GetGlyphCacheMetrics(TGlyphCacheMetrics& aGlyphCacheMetrics);
    IMPORT_C TInt ConveyOogmMessage( TFbsOogmMessage& aOogmMessage );

public:
	/** WARNING: For internal use ONLY.  Compatibility is not guaranteed in future releases.	 
	Used for testing server side out of memory failures.	
	@test
	*/
	enum THeapFailType
		{
		EHeapFailTypeServerMemory,
		EHeapFailTypeHeapMemory,
		};
private:
	TBool LookupBitmapInROM(const TDesC& aFilename, TAny*& aAddr);
	RFs& FileServer() { return *((RFs*)iSpare); }
	static TInt DoAlloc(RFbsSession*& aNewSession);
	TInt DoConnect(RFs& aFileServer);
private:
	TInt iConnections;
	mutable TCallBack iCallBack;
	RChunk iSharedChunk;
	CFbsSessionHelper* iHelper;
	RChunk iLargeBitmapChunk;
	RFs iFileServer;
	CFbsRalCache* iRomFileAddrCache;
	HBufC8* iDecompressionBuffer;
	HBufC8* iScanLineBuffer;	
	TUint32* iSpare;
	};

/** forward declaration of some private classes. */
class TGetFontTableParam;
class TReleaseGlyphOutlineParam;
class TGetGlyphOutlineParam;


/** 
Font managed by the font and bitmap server.
CFbsFont objects are used for screen and off-screen bitmap fonts. They are 
usually obtained by calls to the CBitmapDevice::GetNearestFontInPixels() and 
MGraphicsDeviceMap::GetNearestFontInTwips() functions.
The class implements a number of functions inherited from CFont.
@see CBitmapDevice::GetNearestFontInPixels()
@see MGraphicsDeviceMap::GetNearestFontInTwips() 
@publishedAll 
@released
*/
class CFbsFont: public CFont
	{
	friend class CFbsTypefaceStore;
	friend class RFbsGlyphDataIterator;
	friend class RFbsGlyphMetricsArray;

private:
	// From CFont
	/** Gets the font's UID. 
	All CFbsFont objects have the constant UID KCFbsFontUid. It is safe to cast 
	from CFont* to CFbsFont* if CFont::TypeUid() returns KCFbsFontUid.
	@return The font's UID. This should be KCFbsFontUid. 
	@see CFont::TypeUid() */
	virtual TUid DoTypeUid() const { return KCFbsFontUid; }
	IMPORT_C virtual TInt DoHeightInPixels() const;
	IMPORT_C virtual TInt DoAscentInPixels() const;
	IMPORT_C virtual TInt DoCharWidthInPixels(TChar aChar) const;
	IMPORT_C virtual TInt DoTextWidthInPixels(const TDesC& aText) const;
	IMPORT_C virtual TInt DoBaselineOffsetInPixels() const;
	IMPORT_C virtual TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels) const;
	IMPORT_C virtual TInt DoTextCount(const TDesC& aText,TInt aWidthInPixels,TInt& aExcessWidthInPixels) const;
	IMPORT_C virtual TInt DoMaxCharWidthInPixels() const;
	IMPORT_C virtual TInt DoMaxNormalCharWidthInPixels() const;
	IMPORT_C virtual TFontSpec DoFontSpecInTwips() const;
	IMPORT_C virtual CFont::TCharacterDataAvailability DoGetCharacterData(TUint aCode, TOpenFontCharMetrics& aMetrics,
		const TUint8*& aBitmap, TSize& aBitmapSize) const;
	IMPORT_C virtual TInt DoExtendedFunction(TUid aFunctionId, TAny* aParam = NULL) const;

public:
	// New functions
	IMPORT_C TBool GetFontMetrics(TOpenFontMetrics& aMetrics) const;
	IMPORT_C TInt TextWidthInPixels(const TDesC& aText) const;
	IMPORT_C void TextWidthInPixels(const TDesC& aText,SCharWidth& aCharWidth) const;
	IMPORT_C TInt Handle() const;
	IMPORT_C TCharacterMetrics CharacterMetrics(TInt aCode,const TUint8*& aBytes) const;
	IMPORT_C TInt RawTextWidthInPixels(const TDesC& aText) const;
	IMPORT_C TBool GetFaceAttrib(TOpenFontFaceAttrib& aAttrib) const;
	IMPORT_C TBool IsOpenFont() const;
	IMPORT_C TBool HasCharacter(TInt aCode) const;
	IMPORT_C TInt TextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam) const;
	IMPORT_C void TextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam,SCharWidth& aCharWidth) const;


protected:
	IMPORT_C CFbsFont();
	IMPORT_C ~CFbsFont();
	IMPORT_C CFbsFont(const CFbsFont& aFont);
	IMPORT_C CBitmapFont* Address() const;
	IMPORT_C TInt Duplicate(TInt aHandle);
	IMPORT_C void Reset();
	
private:
	TInt DoFontGetShaping(TFontShapeFunctionParameters* aParam) const;
	TInt DoFontDeleteShaping(TFontShapeDeleteFunctionParameters* aParam) const;
	TInt DoTextWidthInPixels(const TDesC& aText,const TMeasureTextInput* aParam) const;
	TInt DoGetFontTable(TGetFontTableParam * aParam) const;
	TInt DoGetGlyphOutline(TGetGlyphOutlineParam *aParam) const;
	TInt DoReleaseGlyphOutline(TReleaseGlyphOutlineParam *aParam) const;
	TInt DoReleaseFontTable(TUint32 *aParam) const;

protected:
	RFbsSession* iFbs;
	CBitmapFont* iAddressPointer;
	TInt iHandle;
    TInt iServerHandle;
	};

/**
An interface for initialization of extended bitmaps. By implementing this interface, 
creators of extended bitmaps can set the contents of an extended bitmap without 
allocating a temporary buffer and avoid unnecessary memory copying during calls to 
CFbsBitmap::CreateExtendedBitmap().

WARNING: Class for internal and partner use ONLY.  Compatibility is not guaranteed in future releases.

@publishedAll
@see CFbsBitmap::CreateExtendedBitmap()
*/
class MFbsExtendedBitmapInitializer
	{
public:
	/**
	Initializes the raw data of an extended bitmap.
	@param aData A pointer to the raw data to be initialized.
	@param aDataSize The size in bytes of the raw data to be initialized.
	@return KErrNone if successful, otherwise another of the system-wide error codes.
	*/
	virtual TInt InitExtendedBitmap(TAny* aData, TInt aDataSize) = 0;
	};

class CDirectFileStore;

/** 
Note that this class is not intended for user derivation.
A bitmap managed by the font and bitmap server.
The class also defines some palette functionality which is not yet 
implemented, since none of the modes currently supported by screen 
or bitmaps require variable palettes.
Note that the CFbsBitmap format is internal to Symbian, but the 
Image Converter API can be used to convert such bitmaps to standard formats. 
CFbsBitmap instances cannot be shared between different threads. They use some shared
thread local storage data. If you want to share a bitmap between threads, use Duplicate().
@publishedAll 
@released
@see CFbsBitmap::Duplicate()
*/
class CFbsBitmap : public CBase
	{
	friend class TBitmapUtil;
	friend class CBitwiseBitmap;
	friend class CFbsBitmapAsyncStreamer;
	friend class CFbsSessionHelper;
public:
	IMPORT_C CFbsBitmap();
	IMPORT_C ~CFbsBitmap();
	IMPORT_C void Reset();
	IMPORT_C static TInt ScanLineLength(TInt aLength,TDisplayMode aDispMode);
	IMPORT_C TDisplayMode DisplayMode() const;
	IMPORT_C TDisplayMode InitialDisplayMode() const;
	IMPORT_C TInt SetDisplayMode(TDisplayMode aDisplayMode);
	IMPORT_C TInt Create(const TSize& aSizeInPixels,TDisplayMode aDispMode);
	IMPORT_C TInt Duplicate(TInt aHandle);
	IMPORT_C TBool IsRomBitmap() const;
	IMPORT_C void SetRomBitmapL(CBitwiseBitmap* aRomBitmapPointer,TInt& aBitmapSizeInBytes);
	IMPORT_C TInt Load(const TDesC& aFileName,TInt32 aId=0,TBool aShareIfLoaded=ETrue);
	IMPORT_C TInt Load(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset);
	IMPORT_C TInt LoadAndCompress(const TDesC& aFileName,TInt32 aId=0,TBool aShareIfLoaded=ETrue);
	IMPORT_C TInt LoadAndCompress(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset);
	IMPORT_C TInt Load(RFile& aFile,TInt32 aId=0,TBool aShareIfLoaded=ETrue);
	IMPORT_C TInt Load(RFile& aFile,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset);		
	IMPORT_C TInt LoadAndCompress(RFile& aFile,TInt32 aId=0,TBool aShareIfLoaded=ETrue);	
	IMPORT_C TInt LoadAndCompress(RFile& aFile,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset);	
	IMPORT_C TInt Save(const TDesC& aFilename);
	IMPORT_C TInt Save(RFile& aFile);
	IMPORT_C static void StoreL(const TDesC& aFilename,TInt aNumSources,const TDesC* aSources[],TInt32 aSourceIds[]);
	IMPORT_C static void StoreL(RFile& aFile,TInt aNumSources,const TDesC* aSources[],TInt32 aSourceIds[]);
    IMPORT_C void GetScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aLength,TDisplayMode aDispMode) const;
    IMPORT_C void SetScanLine(TDes8& aBuf,TInt aY) const;
	IMPORT_C void GetVerticalScanLine(TDes8& aBuf,TInt aX,TDisplayMode aDispMode) const;
	IMPORT_C void GetVerticalScanLine(TDes8& aBuf,TInt aX,const TPoint& aDitherOffset,TDisplayMode aDispMode) const;
	IMPORT_C TInt Handle() const;
	IMPORT_C SEpocBitmapHeader Header() const;
	IMPORT_C TInt HorizontalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt HorizontalTwipsToPixels(TInt aTwips) const;
	IMPORT_C void GetPixel(TRgb& aColor,const TPoint& aPixel) const;
	IMPORT_C TInt Resize(const TSize& aSizeInPixels);
	IMPORT_C TSize SizeInPixels() const;
	IMPORT_C TSize SizeInTwips() const;
	IMPORT_C void SetSizeInTwips(const MGraphicsDeviceMap* aMap);
	IMPORT_C void SetSizeInTwips(const TSize& aSizeInTwips);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void ExternalizeRectangleL(RWriteStream& aStream,const TRect& aRect) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C TInt Compress();
	IMPORT_C TInt Compress(TBitmapfileCompressionScheme aScheme);
	IMPORT_C void CompressInBackground(TRequestStatus& aRequestStatus);
	IMPORT_C TInt CompressInBackground();
	IMPORT_C void CompressInBackground(TRequestStatus& aRequestStatus, TBitmapfileCompressionScheme aScheme);
	IMPORT_C TInt CompressInBackground(TBitmapfileCompressionScheme aScheme);
	IMPORT_C TInt VerticalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt VerticalTwipsToPixels(TInt aTwips) const;
	IMPORT_C static TBool IsFileInRom(const TDesC& aFilename,TUint32*& aWord);
	IMPORT_C static TBool IsFileInRom(RFile& aFile,TUint32*& aWord);
	IMPORT_C TBool IsMonochrome() const;
	IMPORT_C TBool IsLargeBitmap() const;
	IMPORT_C void PaletteAttributes(TBool& aModifiable,TInt& aNumEntries) const;
	IMPORT_C void SetPalette(CPalette* aPalette);
	IMPORT_C TInt GetPalette(CPalette*& aPalette) const;
	IMPORT_C TUint32* DataAddress() const;
	IMPORT_C TInt DataStride() const;
	IMPORT_C TInt CreateHardwareBitmap(const TSize& aSizeInPixels,TDisplayMode aDispMode,TUid aCreatorUid);
	IMPORT_C TInt HardwareBitmapHandle() const;
	IMPORT_C void BeginDataAccess() const;
	IMPORT_C void EndDataAccess(TBool aReadOnly=EFalse) const;
	IMPORT_C void LockHeap(TBool aAlways=EFalse) const;
	IMPORT_C void UnlockHeap(TBool aAlways=EFalse) const;
	IMPORT_C void LockHeapLC(TBool aAlways=EFalse) const;
	IMPORT_C static void UnlockHeap(TAny* aFbsBitmap);
	IMPORT_C TBool IsVolatile() const;
	IMPORT_C TInt TouchCount() const;
	IMPORT_C TInt64 SerialNumber() const;
	IMPORT_C TBool IsCompressedInRAM() const;
	IMPORT_C TInt SwapWidthAndHeight();
	IMPORT_C TInt CreateExtendedBitmap(const TSize& aSizeInPixels, TDisplayMode aDispMode, TUid aType, const TAny* aData, TInt aDataSize);
	IMPORT_C TInt CreateExtendedBitmap(const TSize& aSizeInPixels, TDisplayMode aDispMode, TUid aType, TInt aDataSize, MFbsExtendedBitmapInitializer& aInitializer);
	IMPORT_C TUid ExtendedBitmapType() const;
	IMPORT_C TInt DataSize() const;
	IMPORT_C static HBufC8* GetDecompressionBuffer(TInt aSize);
	IMPORT_C static CFbsRasterizer* Rasterizer();
	IMPORT_C TInt GetAllBitmapHandles(RArray<TInt>& aBitmapIdArray) const;
	IMPORT_C static HBufC8* GetExtraBuffer(TInt aSize);
protected:
    IMPORT_C void GetScanLine(TDes8& aBuf,const TPoint& aPixel,TInt aLength,const TPoint& aDitherOffset,TDisplayMode aDispMode) const;
	CBitwiseBitmap* Address() const;
	IMPORT_C CBitwiseBitmap* CleanAddress() const;
	inline CBitwiseBitmap* BeginDataAccessAndGetCleanAddress(TUint32*& aDataAddress) const;
	void DoSaveL(RFile& aFile);
	TInt DoCreate(const TSize& aSizeInPixels, TDisplayMode aDispMode, TUid aUid, TInt aDataSize = 0);
private:
	TInt DoLoad(RFile& aFile,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset);
	TInt DoLoad(const TDesC& aFileName,TInt32 aId,TBool aShareIfLoaded,TUint aFileOffset);
	TInt DoLoadFromRom(TUint32* aRomPointer, TInt32 aId, TUint aFileOffset, TBool& aRomPointerValid);
	TBool LoadShiftedRomBmpL(const TDesC& aFileName,TInt32 aId,TUint aFileOffset);
	static void DoStoreL(CDirectFileStore* aFileStore,CFbsBitmap* aBitmap,TInt aNumSources,const TDesC* aSources[],TInt32 aSourceIds[]);
	TInt DuplicateInRom(TInt aBitmapHandle);
	TInt DuplicateInRam(TInt aBitmapHandle);
protected:
	/** WARNING: For internal use ONLY.  Compatibility is not guaranteed in future releases.
	 */
	enum // Flags
		{
		EIsRomBitmap = 0x0001,
		EIsExtendedBitmap = 0x0002,
		EIsReadOnlyBitmapMask = EIsRomBitmap | EIsExtendedBitmap
		};
protected:		
	RFbsSession* iFbs;	/**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */	
	CBitwiseBitmap* iAddressPointer; /**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */	
	TUint16 iFlags; /**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TInt16 iUseCount; /**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TInt iHandle; /**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	TInt iServerHandle; /**< WARNING: Member variable for internal use ONLY. Compatibility is not guaranteed in future releases. Please access using the provided get/set APIs only. */
	};


/** 
Provides fast read/write access to the pixel data of a bitmap. 

Functionality includes getting and setting the individual pixels of a bitmap. 
Common to this is the concept of the current pixel position. This is held 
as member data and is changed by using various efficient functions. For example, 
incrementing the position's x- or y co-ordinate is much quicker than setting 
a completely new position. This efficiency issue is important as these functions 
are used as part of the frequently used blitting DDA used for creating 
stretched or compressed versions of bitmaps.

Calls to Begin() and End() must surround any calls to the other member functions.
Note that you should always ensure each Begin() is matched by an End() call 
when using this class.

Note also that all display modes including EColor16M are supported, but the 
TDisplayMode values ERgb and ENone are not considered to be display modes. 
@publishedAll 
@released
*/
class TBitmapUtil
	{
public:
	IMPORT_C void Begin(const TPoint& aPosition);
	IMPORT_C void Begin(const TPoint& aPosition,const TBitmapUtil& aUtil);
	IMPORT_C void End();
	IMPORT_C TBitmapUtil(CFbsBitmap* aBitmap);
	IMPORT_C TUint32 GetPixel() const;
	IMPORT_C void SetPixel(TUint32 aValue);
	IMPORT_C void SetPixel(const TBitmapUtil& aSource);
	IMPORT_C void SetPos(const TPoint& aPosition);
	IMPORT_C void DecXPos();
	IMPORT_C void DecYPos();
	IMPORT_C void IncXPos();
	IMPORT_C void IncYPos();
private:
	CFbsBitmap* iFbsBitmap;
	TUint32* iWordPos;
	TUint32* iMinWordPos;
	TUint32* iMaxWordPos;
	TInt iBpp;
	TInt iPixelShift;
	TInt iBitShift;
	TUint32 iMask;
	TInt iScanlineWordLength;
	TBool iWritten;
	};

class TFontInfo;

/** 
Font and bitmap server typeface store. 
@publishedAll 
@released
*/
class CFbsTypefaceStore : public CTypefaceStore
	{
	friend class CWsScreenDevice;
public:
	IMPORT_C static CFbsTypefaceStore* NewL(CGraphicsDevice* aDevice);
	IMPORT_C ~CFbsTypefaceStore();
	IMPORT_C TInt AddFile(const TDesC& aName,TInt& aId);
	IMPORT_C TInt InstallFile(const TDesC& aName,TInt& aId);
	IMPORT_C void RemoveFile(TInt aId=0);
	IMPORT_C virtual TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontInPixels(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToDesignHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToDesignHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C virtual TInt GetNearestFontToMaxHeightInTwips(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	IMPORT_C TInt GetNearestFontToMaxHeightInPixels(CFont*& aFont, const TFontSpec& aFontSpec, TInt aMaxHeight);
	IMPORT_C TInt GetFontById(CFont *&aFont,TUid aUid,const TAlgStyle& aAlgStyle);
	IMPORT_C virtual TInt NumTypefaces() const;
	IMPORT_C virtual void TypefaceSupport(TTypefaceSupport& aTypefaceSupport,TInt aTypefaceIndex) const;
	IMPORT_C virtual TInt FontHeightInTwips(TInt aTypefaceIndex,TInt aHeightIndex) const;
	IMPORT_C TInt FontHeightInPixels(TInt aTypefaceIndex,TInt aHeightIndex) const;
	IMPORT_C TGlyphBitmapType DefaultBitmapType() const;
	IMPORT_C void SetDefaultBitmapType(TGlyphBitmapType aType) const;
	IMPORT_C void SetFontNameAliasL(const TDesC& aFontAlias,const TDesC& aFontName) const;
	IMPORT_C void SetDefaultLanguageForMetrics(TLanguage aLanguage) const;
	IMPORT_C static void RemoveFontFileLocksL();
	IMPORT_C static void RemoveFontFileLocksL(const TDesC& aDrive, TBool aAllFonts);
	IMPORT_C static void RemoveFontFileLocksL(const TDesC& aFileName);
	IMPORT_C void ReleaseTwipsCache();
	IMPORT_C void SetSystemDefaultTypefaceNameL(const TDesC& aFontTypefaceName);
	IMPORT_C TInt RegisterLinkedTypeface(const CLinkedTypefaceSpecification& aLinkedTypefaceSpec, TInt& aId);
	IMPORT_C void GetLinkedTypefaceL(CLinkedTypefaceSpecification& aLinkedTypefaceSpec);
	IMPORT_C TInt RegisterLinkedTypeface(const CLinkedTypefaceSpecification& aLinkedTypefaceSpec);
	IMPORT_C TInt UpdateLinkedTypeface(const CLinkedTypefaceSpecification& aLinkedTypefaceSpec);
private:
	CFbsTypefaceStore(CGraphicsDevice* aDevice);
	void ConstructL();
	TInt CreateFont(CFont*& aFont,const TFontInfo& aFontInfo);
	TBool IsFontLoaded(CFont*& aFont, const TFontInfo& aFontInfo) const;
	TInt FontHeight(TInt aTypefaceIndex,TInt aHeightIndex,TInt aMessage) const;
	TInt GetNearestFontInTwipsAndCreateFont(CFont*& aFont, TInt aFbsMessage, const TFontSpec& aFontSpec, TInt aMaxHeight = 0);
	TInt SendGetNearestFontCommandNCreateFont(
		CFont*& aFont, TInt aFbsMessage, const TFontSpec& aFontSpec, TInt aMaxHeight = 0);
	void GetPixelSizeInTwips(TSize& aSize) const;

private:
	RFbsSession* iFbs;
	CGraphicsDevice* iDevice;
	CFontCache* iTwipsCache;
	};

class CDitherColor256;

/** 
Copies a bitmap in any display mode of at least 256 colours into a 256 
colour bitmap, optionally using a user-defined palette. 
@publishedAll 
@released
*/
class CFbsColor256BitmapUtil : public CBase
	{
public:
	/** Indicates whether or not to use dithering. */
	enum TDither
		{
		/** No dithering. */
		ENoDither,
		/** Use the Floyd-Steinberg dithering algorithm. */
		EFloydSteinberg
		};
public:

	IMPORT_C static CFbsColor256BitmapUtil* NewL(const CPalette* aPalette);

	IMPORT_C TInt CopyBitmap(CFbsBitmap* aColor256Destination,CFbsBitmap* aSource,TDither aDither=ENoDither);

	~CFbsColor256BitmapUtil();
private:
	CFbsColor256BitmapUtil();
private:
	TColor256Util* iColor256Util;
	CDitherColor256* iDither;
	};


/**
 A class encapsulating the current state of the glyph cache.

 @publishedAll
 @released
*/
class TGlyphCacheMetrics
    {
public:
    TInt iMaxCacheSizeInBytes;
    TInt iMaxCacheSizeHigh;
    TInt iMaxCacheSizeLow;
    TInt iCacheSizeInBytes;
    TBool iGpuCacheSizeLimitIsMax;
    };

#endif // __FBS_H__

