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

#ifndef PDRSTORE_H
#define PDRSTORE_H

/** 
@file 
@publishedAll
@released
*/

#include <s32mem.h>
#include <gdi.h>
#include <fbs.h>

class CPdrControl;
class CStreamStore;
class CBandedDevice;

/** 
Codes used by printer drivers. 
*/
enum TFbsEncoding
	{
	/** No code. */
	EFbsNone = 0,
	/** Run length. */
	EFbsRunLength = 1,
	/** TIFF. */
	EFbsTIFF = 2,
	/** Delta row. */
	EFbsDeltaRow = 3,
	/** Reserved. */
	EFbsReserved = 4,
	/** Adaptive. */
	EFbsAdaptive = 5
	};

/**
Determines the raster mode used by printer drivers. 
*/
enum TFbsRasterGraphicsMode
	{
	/** Logical page orientation. */
	EFbsLogicalPageOrientation = 0,
	/** Physcial page orientation. */
	EFbsPhysicalPageOrientation = 3
	};

/**
Enumerates resources used by printer drivers. 
*/
enum TPdrResourceId  // Corresponds to IdentResource in PDRREADR.H
	{
	/** Reset. */
	EPdrReset,
	/** Set page size. */
	EPdrSetPageSize,
	/** Preamble. */
	EPdrPreAmble,
	/** Postamble. */
	EPdrPostAmble,
	/** Set text colour. */
	EPdrSetTextColor,
	/** Bold text. */
	EPdrBoldOn,
	/** Normal text weighting. */
	EPdrBoldOff,
	/** Italic text. */
	EPdrItalicOn,
	/** Normal text slant. */
	EPdrItalicOff,
	/** Underline text. */
	EPdrUnderlineOn,
	/** No text underline. */
	EPdrUnderlineOff,
	/** Strike through text. */
	EPdrStrikethroughOn,
	/** No text strike through. */
	EPdrStrikethroughOff,
	/** New page. */
	EPdrNewPage,
	/** Portrait view. */
	EPdrPortrait,
	/** Landscape view. */
	EPdrLandscape,
	/** Set the x-position of the printer head. */
	EPdrSetXPos,
	/** Set the y-position of the printer head. */
	EPdrSetYPos,
	/** Increment the x-position. */
	EPdrIncrementXPos,
	/** Increment the y-position. */
	EPdrIncrementYPos,
	/** Carriage return. */
	EPdrCarriageReturn,
	/** Set the graphics colour. */
	EPdrSetGraphicsColor,
	/** Starting position of bitmap. */
	EPdrBitmapStart,
	/** End position of bitmap. */
	EPdrBitmapEnd,
	/** Scan line. */
	EPdrScanLine,
	/** End of scan line. */
	EPdrEndScanLine, 
	/** Extra resource holder. */
	EPdrResource1,
	/** Extra resource holder. */
	EPdrResource2,
	/** Extra resource holder. */
	EPdrResource3,
	/** Extra resource holder. */
	EPdrResource4,
	/** Extra resource holder. */
	EPdrResource5,
	/** Extra resource holder. */
	EPdrResource6,
	/** Extra resource holder. */
	EPdrResource7,
	/** Extra resource holder. */
	EPdrResource8
	};

/**
Maximum length of a printer command string. 
*/
const TInt KMaxCommandStringLength = 0x40;
/**
Defines a command string used by printer drivers. 
*/
typedef TBuf8<KMaxCommandStringLength> TCommandString;

// Forward declarations
class CFontInfo;
class CPdrTranslates;
class CPdrDevice;

/** WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
 */
class CInfoFont : public CFont
	{
	friend class CPdrTypefaceStore;
public:
	// New functions
	IMPORT_C HBufC8* TranslateStringL(const TDesC& aString) const;
	IMPORT_C TPtrC8 CommandString() const;
	IMPORT_C TBool RepertoireContains(TChar aChar) const;
	IMPORT_C TBool AllCharsInFontRepertoire(const TDesC& aString, TInt& aFirstCharNotInRepertoire, TInt& aLength) const;
	inline CFontInfo* FontInfo() const {return iFontInfo;}
	CFont* RealFont() const;
private:
	CInfoFont(TInt aBaselineOffsetInPixels, const TFontSpec& aFontSpecInTwips, TInt aFontInfoHeightInTwips, TInt aHeightInPixels, CPdrTranslates* aTranslates, const TDesC8& aCommandString, CPdrDevice* aPdrDevice);
	~CInfoFont();
	TInt Width(TInt aNum) const;
	TInt Height(TInt aNum) const;
	void CreateBandedFontIfRequired();
	// From CFont
	IMPORT_C virtual TUid DoTypeUid() const;
	IMPORT_C virtual TInt DoHeightInPixels() const;
	IMPORT_C virtual TInt DoAscentInPixels() const;
	IMPORT_C virtual TInt DoCharWidthInPixels(TChar aChar) const;
	IMPORT_C virtual TInt DoTextWidthInPixels(const TDesC &aText) const;
	IMPORT_C virtual TInt DoBaselineOffsetInPixels() const;
	IMPORT_C virtual TInt DoTextCount(const TDesC &aText, TInt aWidthInPixels) const;
	IMPORT_C virtual TInt DoTextCount(const TDesC &aText, TInt aWidthInPixels, TInt &aExcessWidthInPixels) const;
	IMPORT_C virtual TInt DoMaxCharWidthInPixels() const;
	IMPORT_C virtual TInt DoMaxNormalCharWidthInPixels() const;
	IMPORT_C virtual TFontSpec DoFontSpecInTwips() const;
private:
    TCommandString iCommandString;
	TInt iBaselineOffsetInPixels;
	TFontSpec iFontSpecInTwips;
	TInt iFontInfoHeightInTwips;
	TInt iHeightInPixels;
	CFontInfo* iFontInfo;
	CPdrTranslates* iTranslates;
	CPdrDevice* iPdrDevice;
	CFont* iRealFont;
	};


class CPageBuffer : public CActive
	{
public:
	IMPORT_C static CPageBuffer* NewL(CPrinterPort* aPrinterPort);
	IMPORT_C static CPageBuffer* NewL(CPrinterPort* aPrinterPort, TInt aGranularity);
	IMPORT_C void StartFlush(TRequestStatus& aRequestStatus);
	IMPORT_C void AddBytesL(const TDesC8& aDes);
	IMPORT_C RWriteStream& CreateWriteStreamL();
	IMPORT_C void CloseWriteStream();
	IMPORT_C ~CPageBuffer();
	void DoCancel();
	void RunL();
private:
	void ConstructL();
	CPageBuffer(CPrinterPort* aPrinterPort, TInt aGranularity);
	void Queue();
private:
	
	TRequestStatus* iRequestStatus;
	
	TInt iWritePos;
	
	TInt iGranularity;
	
	CBufSeg* iBuffer;
	
	TPtr8 iPtr;
	
	CPrinterPort* iPrinterPort;
	
	RBufWriteStream iWriteStream;
	
	CBufSeg* iWriteStreamBuffer;
	};


/** WARNING: Class for internal use ONLY.  Compatibility is not guaranteed in future releases.
 */
class TTextFormat	
	{
public:
	IMPORT_C TTextFormat();
	IMPORT_C TTextFormat(const TFontUnderline anUnderlineStyle, const TFontStrikethrough aStrikethroughStyle, const TRgb& aColor, const TDesC8& aFontString, const TFontStyle& aFontStyle);
	IMPORT_C TBool operator == (const TTextFormat& aFormat) const;
public:
	
	TFontUnderline iUnderlineStyle;
	
	TFontStrikethrough iStrikethroughStyle;
	
	TRgb iColor;
	
	TCommandString iFontString;
	
	TFontStyle iFontStyle;
	};


// Forward declarations
class CPdrModelInfo;
class CPdrTypefaceStore;

/**
@see CFbsDrvDevice
*/
class CPdrDevice : public CPrinterDevice
	{
protected:
	IMPORT_C CPdrDevice();
public:
	IMPORT_C virtual ~CPdrDevice();
	IMPORT_C void SelectPageSpecInTwips(const TPageSpec& aPageSpec);
	IMPORT_C TDisplayMode DisplayMode() const;
	IMPORT_C TSize SizeInPixels() const;
	IMPORT_C TSize SizeInTwips() const;
	IMPORT_C TRect PrintablePageInPixels() const;
	IMPORT_C TInt HorizontalTwipsToPixels(TInt aTwips) const;
	IMPORT_C TInt VerticalTwipsToPixels(TInt aTwips) const;
	IMPORT_C TInt HorizontalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt VerticalPixelsToTwips(TInt aPixels) const;
	IMPORT_C TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToDesignHeightInTwips(CFont *&aFont, const TFontSpec &aFontSpec);
	IMPORT_C TInt GetNearestFontToMaxHeightInTwips(CFont *&aFont, const TFontSpec &aFontSpec, TInt aMaxHeight);
	IMPORT_C TInt NumTypefaces() const;
	IMPORT_C void TypefaceSupport(TTypefaceSupport& aTypefaceSupport, TInt aTypefaceIndex) const;
	IMPORT_C TInt FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const;
	IMPORT_C void PaletteAttributes(TBool& aModifiable, TInt& aNumEntries) const;
	IMPORT_C void SetPalette(CPalette* aPalette);
	IMPORT_C TInt GetPalette(CPalette*& aPalette) const;
	IMPORT_C TPrinterModelEntry Model() const;
	IMPORT_C TInt Flags() const;
	IMPORT_C TInt SetModel(const TPrinterModelHeader& aModel, CStreamStore& aStore);
	IMPORT_C void ReleaseFont(CFont* aFont);
	IMPORT_C TPoint OffsetInPixels();

protected:
	IMPORT_C virtual TSize KPixelSizeInTwips() const;
private:
	void DoSetModelL();
protected:
	
	CStreamStore* iStore;
	
	TPrinterModelHeader iModel;
	
	CPdrModelInfo* iModelInfo;
	
	CPdrTypefaceStore* iTypefaceStore;
	}; 


//Forward declarations
class CPdrResources;
class CPageText;

class CPdrControl : public CPrinterControl
	{
public:
	IMPORT_C ~CPdrControl();
	IMPORT_C TInt CreateContext(CGraphicsContext*& aGC);
	IMPORT_C TInt BandsPerPage();
	IMPORT_C TMoreOnPage QueueGetBand(TRequestStatus& aStatus, TBandAttributes& aBand); // returns affirmative if more to print on current page
	IMPORT_C void QueueEndPrint(TRequestStatus& aStatus); // Called at finish to flush buffer, tidy up printer etc.
	IMPORT_C void AbortPrint(); // tidy up synchronously in a short time
	IMPORT_C virtual void DrawTextL(const TPoint& aPoint, const TFontUnderline aUnderlineStyle, const TFontStrikethrough aStrikethroughStyle, const TRgb& aColor, const CInfoFont* aFont, const TDesC& aString); 
	IMPORT_C TBool IsGraphicsBand() const;
	inline CBandedDevice* BandedDevice() const {return iBandedDevice;}
protected:
	IMPORT_C CPdrControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort);
	IMPORT_C virtual void DoQueueGetBandL();
	IMPORT_C virtual void DoQueueEndPrintL();
	IMPORT_C void ConstructL(CStreamStore& aStore, TStreamId aStreamId);

	virtual void OutputBandL() = 0;

	IMPORT_C virtual void MoveToL(const TPoint& aPoint); 
	IMPORT_C virtual void MoveByL(const TPoint& aVector); 
	IMPORT_C virtual void OutputTextL(const TPoint& aPoint, TInt aWidthInPixels, const TTextFormat& aTextFormat, const TDesC8& aString); 

	IMPORT_C virtual void SetPageSizeL(); 
	IMPORT_C virtual void SetPageOrientationL();
	IMPORT_C virtual void SetFontPostureL(const TFontPosture aPosture);
	IMPORT_C virtual void SetFontStrokeWeightL(const TFontStrokeWeight aStrokeWeight);
	IMPORT_C virtual void SetTextColorL(const TRgb& aColor);
	IMPORT_C virtual void CommandL(const TInt anId);

	IMPORT_C TBool TransformBuffer(); // These two functions are not implemented anywhere
	IMPORT_C TBool CompressBuffer();  // for this class. Nor do I believe they are defined
									  // for a reason. But since they are part of the 
									  // existing interface of a DLL in a ROM I need to
									  // patch. They get to stay there for the moment. CJC

protected:
	
	CPdrDevice* iPdrDevice;
	
	CPageBuffer* iPageBuffer;
	
	CPdrResources* iResources;
	
	CBandedDevice* iBandedDevice;
	
	CPageText* iPageText;
	
	TInt iBandIndex;
	
	TInt iEntryIndex;
	
	TPoint iPosition;
	
	TTextFormat iTextFormat;
	};


class CFbsDrvDevice : public CPdrDevice
	{
public:
	IMPORT_C CFbsDrvDevice();
	IMPORT_C ~CFbsDrvDevice();

	IMPORT_C TInt GetNearestFontInTwips(CFont*& aFont, const TFontSpec& aFontSpec);
	IMPORT_C TInt GetNearestFontToDesignHeightInTwips(CFont *&aFont, const TFontSpec &aFontSpec);
	IMPORT_C void ReleaseFont(CFont* aFont);
	IMPORT_C TInt NumTypefaces() const;
	IMPORT_C void TypefaceSupport(TTypefaceSupport& aTypefaceSupport, TInt aTypefaceIndex) const;
	IMPORT_C TInt FontHeightInTwips(TInt aTypefaceIndex, TInt aHeightIndex) const;

	IMPORT_C TInt CreateContext(CGraphicsContext*& aGc);
	IMPORT_C void CreateControlL(CPrinterPort* aPrinterPort);
	IMPORT_C TInt SetModel(const TPrinterModelHeader& aModel, CStreamStore& aStore);
	IMPORT_C void LoadTypeFaceList();
	IMPORT_C TSize KPixelSizeInTwips() const;
	IMPORT_C void LoadTypeFaceListL();
	IMPORT_C TInt SetModelL(const TPrinterModelHeader& aModel, CStreamStore& aStore);
private:
// reserved virtual function space
	IMPORT_C virtual void Reserved_1();

protected:
	
	CFbsTypefaceStore* iFbsTypefaceStore;
	
	CArrayFixFlat<TInt>* iGenTypefaceFontsList;
	
	CArrayFixFlat<TInt>* iGenTypefaceFontsType;
	
	TInt iFbsTypefaceCount;
	
	TInt iPdrTypefaceCount;

private:
	
	TAny* iReserved;
	};


class CFbsDrvControl : public CPdrControl
	{
public:
	IMPORT_C static CFbsDrvControl* NewL(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort, CStreamStore& aStore, TStreamId aResourcesStreamId);
	IMPORT_C ~CFbsDrvControl();

protected:
	IMPORT_C virtual void OutputBandL();

	IMPORT_C CFbsDrvControl(CPdrDevice* aPdrDevice, CPrinterPort* aPrinterPort);
	IMPORT_C void ConstructL(CStreamStore& aStore, TStreamId aResourcesStreamId);
	IMPORT_C void SetPageSizeL();

	IMPORT_C TBool TransformBuffer();
	IMPORT_C TBool CompressBuffer();

private:
// reserved virtual function space
	IMPORT_C virtual void Reserved_1();
		
protected:
	
	HBufC8* iScanLine;
	
	HBufC8* iCompressedScanLine;

private:
	
	TAny* iReserved;
	};

#endif
