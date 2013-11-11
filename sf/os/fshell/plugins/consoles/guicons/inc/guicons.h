// guicons.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __GUICONS_H__
#define __GUICONS_H__

#include <coecntrl.h>
#include <e32cons.h>
#include <fshell/consoleproxy.h>
#include <fshell/consoleextensions.h>

static const TInt KFontBitmapWidthChars = 8;
static const TInt KFontBitmapHeightChars = 12;
static const TInt KFontBitmapFirstCharCode = ' ';
static const TInt KNumGlyphs = KFontBitmapWidthChars * KFontBitmapHeightChars;
static const TInt KFontBitmapLastCharCode = KFontBitmapFirstCharCode + KNumGlyphs - 1;
static const TInt KDefaultCursorHeightPercentage = 20;
static const TInt KTabSize = 4;
static const TInt KNumColors = 16;

class CConsoleControl;
class CConsoleLine;
class CConsoleFont;
class MGuiConsoleReader;
class MConsoleUi;
class TBufferPosition;
class TConsCursorPosition;
class TViewPosition;
class CImageDecoder;
class RImageDecodeThreadParams;

IMPORT_C CServer2* StartGuiConsServerL(const TDesC& aServerName, MConsoleUi& aConsoleUi);

class MConsoleUi
	{
public:
	virtual void HandleNewConsoleL(CConsoleControl* aConsole) = 0;
	virtual void HandleConsoleClosed(CConsoleControl* aConsole) = 0;
	virtual void ConsoleTitleChangedL(const CConsoleControl*, const TDesC&){};
	virtual TInt GetConsoleBufferSize() = 0;
	virtual const TDesC& GetConsoleFont() = 0;
	};

/**
Position relative to the text buffer. The cursor window and view window will be somewhere within this.
*/
class TBufferPosition
	{
public:
	TBufferPosition(const CConsoleControl& aConsole, TPoint aPosition);
	TBufferPosition(const CConsoleControl& aConsole, TInt aX, TInt aY);
	TBufferPosition(const TConsCursorPosition& aCursorPosition);
	TBufferPosition(const TViewPosition& aViewPosition);
public:
	const CConsoleControl& iConsole;
	TPoint iPoint;
	};

/**
Position relative to the cursor window - i.e. the area that the cursor can move around in
*/
class TConsCursorPosition
	{
public:
	TConsCursorPosition(const CConsoleControl& aConsole, TPoint aPosition);
	TConsCursorPosition(const CConsoleControl& aConsole, TInt aX, TInt aY);
	TConsCursorPosition(const TBufferPosition& aBufferPosition);
	TConsCursorPosition(const TViewPosition& aViewPosition);
public:
	const CConsoleControl& iConsole;
	TPoint iPoint;
	};

/**
Position relative to the view window - i.e. the area of the console seen on the screen
*/
class TViewPosition
	{
public:
	TViewPosition(const CConsoleControl& aConsole, TPoint aPosition);
	TViewPosition(const CConsoleControl& aConsole, TInt aX, TInt aY);
	TViewPosition(const TConsCursorPosition& aCursorPosition);
	TViewPosition(const TBufferPosition& aBufferPosition);
public:
	const CConsoleControl& iConsole;
	TPoint iPoint;
	};

/**
Position in pixels relative to CConsoleControl's rectangle.
*/
class TScreenPosition
	{
public:
	TScreenPosition(const CConsoleControl& aConsole, TPoint aPosition);
	TScreenPosition(const CConsoleControl& aConsole, TInt aX, TInt aY);
	TScreenPosition(const TViewPosition& aViewPosition);
public:
	const CConsoleControl& iConsole;
	TPoint iPoint;
	};

class TConsoleCursor
	{
public:
	TConsoleCursor(CConsoleControl& aOwner);
	void SetFont(const CConsoleFont& aFont);
	TConsCursorPosition Position() const;
	void SetPosAbs(TConsCursorPosition aPos);
	void SetPosRel(TConsCursorPosition aPos);
	void SetHeight(TInt aPercentage);
	
	void Hide();

	void operator++(int);
	void operator--(int);
	void operator+=(TUint aHowMuch);
	void operator-=(TUint aHowMuch);
	void Down();
	void Up();
	
	void Update();
private:
	void BoundsCheck();
private:
	CConsoleControl& iOwner;
	TSize iGlyphSize;
	TPoint iPosition;
	TTextCursor iTextCursor;
	TInt iTextCursorOffset;
	};

class MGuiConsoleReader
	{
public:
	virtual void ReadComplete(TInt aStatus) = 0;
	};
	
class CGuiConsole	: public CConsoleBase
					, public MGuiConsoleReader
					, public MProxiedConsole
	{
public:
	IMPORT_C CGuiConsole(CConsoleControl& aControl);
	
	IMPORT_C CConsoleControl& Control();
	
	// from CConsoleBase
	IMPORT_C virtual TInt Create(const TDesC &aTitle,TSize aSize);
	IMPORT_C virtual void Read(TRequestStatus& aStatus);
	IMPORT_C virtual void ReadCancel();
	IMPORT_C virtual void Write(const TDesC &aDes);
	IMPORT_C virtual TPoint CursorPos() const;
	IMPORT_C virtual void SetCursorPosAbs(const TPoint &aPoint);
	IMPORT_C virtual void SetCursorPosRel(const TPoint &aPoint);
	IMPORT_C virtual void SetCursorHeight(TInt aPercentage);
	IMPORT_C virtual void SetTitle(const TDesC &aTitle);
	IMPORT_C virtual void ClearScreen();
	IMPORT_C virtual void ClearToEndOfLine();
	IMPORT_C virtual TSize ScreenSize() const;
	IMPORT_C virtual TKeyCode KeyCode() const;
	IMPORT_C virtual TUint KeyModifiers() const;
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny*& a0, TAny* a1);
	
	// from MGuiConsoleReader
	virtual void ReadComplete(TInt aStatus);
	// from MProxiedConsole:
	virtual void Open();
	virtual void Close();
	virtual CConsoleBase* Console();
	virtual void Read(CConsoleProxySession& aSession);
private:
	~CGuiConsole();
private:
	CConsoleControl& iControl;
	TInt iRefCount;
	TRequestStatus* iReadStatus;
	CConsoleProxySession* iReader;
	};

class CConsoleControl : public CCoeControl
	{
public:
	IMPORT_C static CConsoleControl* NewL(TInt aBufferSize, MConsoleUi* aUi = NULL);
	IMPORT_C static CConsoleControl* NewL(TInt aBufferSize, const TDesC& aFontFile, MConsoleUi* aUi = NULL);
	IMPORT_C virtual ~CConsoleControl();
	IMPORT_C void Closed();
	
	IMPORT_C void InjectKeysL(const TDesC& aKeys);
	IMPORT_C TBool FivewayNavIsDisplaying() const;
	IMPORT_C void SetDisplayFivewayNav(TBool aShow);
	
	// from CCoeControl:
	IMPORT_C virtual void Draw(const TRect& aRect) const;
	IMPORT_C virtual void SizeChanged();
	IMPORT_C virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
	IMPORT_C virtual TCoeInputCapabilities InputCapabilities() const;
	IMPORT_C virtual void FocusChanged(TDrawNow aDrawNow);
	IMPORT_C virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);
	
	// CConsoleBase API
	IMPORT_C void Read(MGuiConsoleReader &aStatus);
	IMPORT_C void ReadCancel();
	IMPORT_C void Write(const TDesC &aDes);
	IMPORT_C TPoint CursorPos() const;
	IMPORT_C void SetCursorPosAbs(const TPoint &aPoint);
	IMPORT_C void SetCursorPosRel(const TPoint &aPoint);
	IMPORT_C void SetCursorHeight(TInt aPercentage);
	IMPORT_C void SetTitle(const TDesC &aTitle);
	IMPORT_C void ClearScreen();
	IMPORT_C void ClearToEndOfLine();
	IMPORT_C TSize ScreenSize() const;
	IMPORT_C TKeyCode KeyCode() const;
	IMPORT_C TUint KeyModifiers() const;
	
	IMPORT_C void ViewScrollUp();
	IMPORT_C void ViewScrollDown();
	IMPORT_C void ViewPageUp();
	IMPORT_C void ViewPageDown();
	IMPORT_C void ViewHome();
	IMPORT_C void ViewEnd();
	IMPORT_C TInt SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor);
	
	// calls from CConsoleCursor:
	void CursorWindowScrollDown();
	
	TPoint ViewPosition() const;
	TPoint CursorWindowPosition() const;
	TSize GlyphSize() const;
	void ActivateL();
	void StartBlinking();
	
private:
	CConsoleControl(MConsoleUi* aUi, TInt aBufferSize);
	void ConstructL(CConsoleFont* aFont);
	void SizeChangedL();
	
	void DrawLine(TViewPosition aLine, CBitmapContext& aDrawTo) const;
	TRect LineRect(TViewPosition aLine) const;
	
	TBool IsSpecialChar(TChar aChar) const;
	void HandleSpecialChar(TChar aChar);
	
	void SendKey();
	
	CConsoleLine* GetLine(TBufferPosition aLine) const;
	void ViewMoved();
	void Invalidate5Way();
	enum TButton { EUp, EDown, ELeft, ERight, ECenter, ENumButtons };
	void SimulateKeyL(TButton aButton);
	void WriteL(const TDesC &aDes);

	static TInt BlinkCallback(TAny* aPtr);
	
private:
	MConsoleUi* iUi;
	CConsoleFont* iFont;
	RPointerArray<CConsoleLine> iBuffer;
	CPeriodic* iBlinkTimer;
	TSize iSizeChars;
	TConsoleCursor iCursor;
	TPoint iCursorWindow;
	TPoint iViewWindow;

	MGuiConsoleReader* iReader;
	RArray<TKeyEvent> iKeyQueue;
	TKeyEvent iCurrentKey; 
	
	TPoint iDragStart;
	TBool iIgnoringDrags;

	TRect iButtonRects[ENumButtons];
	TBool iDrawNavigator;
	ConsoleAttributes::TAttributes iCurrentAttributes;
	
	TInt iBufferSize;
	TBool iBlinkOn;
	};
	
class CConsoleFont : public CBase
	{
public:
	IMPORT_C static CConsoleFont* NewL(const TDesC& aFontFile);
	IMPORT_C static CConsoleFont* NewL(const TDesC8& aEncodedFontImage);
	IMPORT_C ~CConsoleFont();
	IMPORT_C TSize GlyphSize() const;
	IMPORT_C void DrawChar(TChar aChar, CBitmapContext& aDrawTo, TPoint aPosPixels, TRgb aColor);
	void PrepareForegroundColorL(TRgb aColor);
private:
	CConsoleFont();
	void ConstructL(const TDesC& aFontFile);
	void ConstructL(const TDesC8& aEncodedFontImage);
	void ConstructL(RImageDecodeThreadParams& aThreadParams);
	CFbsBitmap* ForegroundBitmap(TRgb aColor);
private:
	TFixedArray<CFbsBitmap*, KNumGlyphs> iChars;
	TFixedArray<CFbsBitmap*, KNumColors> iColors;
	TRect iGlyphRect;
	};

class RAttributeMap
	{
public:
	RAttributeMap();
	void Close();
	void Reset();
	void AddL(TInt aPosition, TInt aLineWidth, const ConsoleAttributes::TAttributes& aAttributes);
	void RemoveFrom(TInt aPosition);
	TInt NumberOfBlocks() const;
	void GetBlock(TInt aIndex, TInt aLineWidth, TInt& aStartPosition, TInt& aEndPosition, ConsoleAttributes::TAttributes& aAttributes) const;
private:
	class TAttributes
		{
	public:
		TAttributes(TInt aPosition, const ConsoleAttributes::TAttributes& aAttributes);
		TBool operator==(const TAttributes& aAttributes) const;
	public:
		TInt iPosition;
		ConsoleAttributes::TAttributes iAttributes;
		};
private:
	RArray<TAttributes> iAttributes;
	};
	
NONSHARABLE_CLASS(CConsoleLine) : public CBase
	{
public:
	static CConsoleLine* NewL(CConsoleControl& aControl, const CConsoleFont& aFont, TInt aWidth);
	~CConsoleLine();
	
	void SetWidthL(TInt aNewWidth);
	
	void SetL(TInt aIndex, TUint8 aChar, const ConsoleAttributes::TAttributes& aAttributes);
	void Clear();
	void ClearFrom(TBufferPosition aPosition);
	
	void Draw(CBitmapContext& aDrawTo, TViewPosition aViewPosition) const;
	TBool NeedToBlink(TBool aBlinkOn);
private:
	CConsoleLine(CConsoleControl& aControl, const CConsoleFont& aFont);
	void ConstructL(TInt aWidth);
	TUint16 At(TInt aPos) const;
private:
	CConsoleControl& iControl;
	const CConsoleFont& iFont;
	RBuf8 iText;
	RAttributeMap iAttributeMap;
	TInt iWidth;
	TBool iBlinkOn;
	};


#endif //__GUICONS_H__
