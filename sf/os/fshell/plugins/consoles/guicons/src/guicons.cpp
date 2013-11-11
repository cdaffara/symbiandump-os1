// guicons.cpp
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

#include <fshell/common.mmh>
#include "guicons.h"
#include <ImageConversion.h>
#include <e32Math.h>

#include "defaultfontdata.inl"

const TInt KAttributeMapGranularity = 3;

const ConsoleAttributes::TColor KDefaultForegroundColor = ConsoleAttributes::EBlack;
const ConsoleAttributes::TColor KDefaultBackgroundColor = ConsoleAttributes::EWhite;


ConsoleAttributes::TAttributes DefaultAttributes()
	{
	return ConsoleAttributes::TAttributes(ConsoleAttributes::ENone, KDefaultForegroundColor, KDefaultBackgroundColor);
	}

TRgb MapColor(TUint aAttributes, ConsoleAttributes::TColor aColor)
	{
	TRgb rgb;

	switch (aColor)
		{
		case ConsoleAttributes::EBlack:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbGray;
				}
			else
				{
				rgb = KRgbBlack;
				}
			break;
		case ConsoleAttributes::ERed:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbRed;
				}
			else
				{
				rgb = KRgbDarkRed;
				}
			break;
		case ConsoleAttributes::EGreen:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbGreen;
				}
			else
				{
				rgb = KRgbDarkGreen;
				}
			break;
		case ConsoleAttributes::EYellow:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbYellow;
				}
			else
				{
				rgb = KRgbDarkYellow;
				}
			break;
		case ConsoleAttributes::EBlue:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbBlue;
				}
			else
				{
				rgb = KRgbDarkBlue;
				}
			break;
		case ConsoleAttributes::EMagenta:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbMagenta;
				}
			else
				{
				rgb = KRgbDarkMagenta;
				}
			break;
		case ConsoleAttributes::ECyan:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbCyan;
				}
			else
				{
				rgb = KRgbDarkCyan;
				}
			break;
		case ConsoleAttributes::EWhite:
			if (aAttributes & ConsoleAttributes::EBold)
				{
				rgb = KRgbWhite;
				}
			else
				{
				rgb = KRgbWhite;//KRgbGray;
				}
			break;
		case ConsoleAttributes::EReset:
		case ConsoleAttributes::EUnchanged:
		default:
			ASSERT(EFalse);
		}

	if (aAttributes & ConsoleAttributes::EInverse)
		{
		rgb = ~rgb;
		}

	return rgb;
	}

void SignalSemaphore(TAny* aSemaphore)
	{
	((RSemaphore*)aSemaphore)->Signal();
	}

void CleanupSignalSemaphorePushL(RSemaphore& aSemaphore)
	{
	CleanupStack::PushL(TCleanupItem(SignalSemaphore, &aSemaphore));
	}
	
void WaitForRequest(TAny* aStatus)
	{
	User::WaitForRequest(*(TRequestStatus*)aStatus);
	}
	
void CleanupWaitForRequestPushL(TRequestStatus& aStatus)
	{
	CleanupStack::PushL(TCleanupItem(WaitForRequest, &aStatus));
	}
	
//______________________________________________________________________________
//						CConsoleControl
EXPORT_C CConsoleControl* CConsoleControl::NewL(TInt aBufferSize, MConsoleUi* aUi)
	{
	CConsoleControl* self = new(ELeave)CConsoleControl(aUi, aBufferSize);
	CleanupStack::PushL(self);
	self->ConstructL(CConsoleFont::NewL(KDefaultFontImageData));
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CConsoleControl* CConsoleControl::NewL(TInt aBufferSize, const TDesC& aFontFile, MConsoleUi* aUi)
	{
	CConsoleControl* self = new(ELeave)CConsoleControl(aUi, aBufferSize);
	CleanupStack::PushL(self);
	self->ConstructL(CConsoleFont::NewL(aFontFile));
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CConsoleControl::~CConsoleControl()
	{
	delete iBlinkTimer;
	delete iFont;
	iCursor.Hide();
	iBuffer.ResetAndDestroy();
	iKeyQueue.Close();
	}
	
EXPORT_C void CConsoleControl::Closed()
	{
	if (iUi)
		{
		iUi->HandleConsoleClosed(this);
		}
	}

EXPORT_C void CConsoleControl::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.SetBrushStyle(CGraphicsContext::ENullBrush);
	gc.SetPenStyle(CGraphicsContext::ENullPen);
	// Need to do a clear here, to ensure we actually draw to all the pixels in aRect that we've been asked to draw. If we don't do that, wserv generally gets upset.
//	gc.SetBrushColor(KDefaultBackgroundColor);
//	gc.Clear(aRect);
	
	for (TInt y=0; y<iSizeChars.iHeight; ++y)
		{
		TViewPosition line(*this, 0, y);
		if (aRect.Intersects(LineRect(line)))
			{
			DrawLine(line, gc);
			}
		}

	if (iDrawNavigator)
		{
		// And draw the 5-way UI
#ifdef FSHELL_WSERV2_SUPPORT
		// XOR draw mode doesn't seem to be supported in recent wserv, so use a colour with alpha channel instead
		const TRgb col(15, 15, 15, 128);
		gc.SetDrawMode(CGraphicsContext::EDrawModePEN);
#else
		const TRgb col(15,15,15);
		gc.SetDrawMode(CGraphicsContext::EDrawModeXOR);
#endif
		gc.SetBrushColor(col);
		gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc.SetPenColor(col);
		for (TInt i = 0; i < ENumButtons; i++)
			{
			if (aRect.Intersects(iButtonRects[i]))
				{
				if (i == ECenter)
					{
					gc.DrawEllipse(iButtonRects[i]);
					}
				else
					{
					gc.DrawRect(iButtonRects[i]);
					}
				}
			}
		}
	}
	
EXPORT_C TCoeInputCapabilities CConsoleControl::InputCapabilities() const
	{
	return TCoeInputCapabilities(
				TCoeInputCapabilities::ENonPredictive 
			|	TCoeInputCapabilities::EAllText
			|	TCoeInputCapabilities::ENavigation
			);
	}
	
EXPORT_C void CConsoleControl::FocusChanged(TDrawNow)
	{
	iCursor.Update();
	if (IsFocused())
		{
		StartBlinking();
		}
	else
		{
		iBlinkTimer->Cancel();
		}
	}
	
EXPORT_C void CConsoleControl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	if (aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
		for (TInt i = 0; i < ENumButtons; i++)
			{
			if (iButtonRects[i].Contains(aPointerEvent.iPosition))
				{
				SimulateKeyL((TButton)i);
				iIgnoringDrags = ETrue;
				return;
				}
			}
		iIgnoringDrags = EFalse;
		}

	if (iIgnoringDrags) return;
	if (aPointerEvent.iType == TPointerEvent::EButton1Down)
		{
		iDragStart = aPointerEvent.iPosition;
		}
	if (aPointerEvent.iType == TPointerEvent::EDrag)
		{
		TPoint delta = iDragStart - aPointerEvent.iPosition;
		TInt lines = delta.iY / iFont->GlyphSize().iHeight;
		if (lines)
			{
			iViewWindow.iY += lines;
			ViewMoved();
			
			iDragStart.iY -= (lines * iFont->GlyphSize().iHeight);
			}
		}
	}
	
EXPORT_C TKeyResponse CConsoleControl::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
	{
	if ((!IsFocused()) || IsNonFocusing()) return EKeyWasNotConsumed;
	
	if (aType == EEventKey)
		{
		iKeyQueue.AppendL(aKeyEvent);
		SendKey();
		
#ifdef LOCAL_ECHO
		TUint code = aKeyEvent.iCode;
		if (code == '\r') code = '\n';
		
		TBuf<0x20> ip;
		ip.AppendFormat(_L("key %d"), code);
		User::InfoPrint(ip);

		TBuf<1> buf;
		buf.Append(TChar(code));
		Write(buf);
#endif
		return EKeyWasConsumed;
		}
	return EKeyWasNotConsumed;	
	}
	
EXPORT_C void CConsoleControl::InjectKeysL(const TDesC& aKeys)
	{
	for (TInt i=0; i<aKeys.Length(); ++i)
		{
		TKeyEvent key;
		key.iCode = aKeys[i];
		key.iModifiers = 0;
		iKeyQueue.AppendL(key);		
		}
	SendKey();
	}
	
void CConsoleControl::DrawLine(TViewPosition aLine, CBitmapContext& aDrawTo) const
	{
	CConsoleLine* line = GetLine(aLine);
	if (line)
		{
		line->Draw(aDrawTo, aLine);
		}
	}

TRect CConsoleControl::LineRect(TViewPosition aLine) const
	{
	return TRect(TScreenPosition(aLine).iPoint, TSize(iFont->GlyphSize().iWidth * iSizeChars.iWidth, iFont->GlyphSize().iHeight));
	}


EXPORT_C void CConsoleControl::SizeChanged()
	{
	TSize sizePixels = Size();
	TSize newSizeChars(sizePixels.iWidth / iFont->GlyphSize().iWidth, sizePixels.iHeight / iFont->GlyphSize().iHeight);
	if (newSizeChars != iSizeChars)
		{
		iSizeChars = newSizeChars;
		TRAP_IGNORE(SizeChangedL());
		}
	}

CConsoleControl::CConsoleControl(MConsoleUi* aUi, TInt aBufferSize)
	: iUi(aUi), iCursor(*this), iCurrentAttributes(ConsoleAttributes::ENone, KDefaultForegroundColor, KDefaultBackgroundColor), iBufferSize(aBufferSize)
	{
	}

void CConsoleControl::ConstructL(CConsoleFont* aFont)
	{
	iBlinkTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	iFont = aFont;
	iCursor.SetFont(*iFont);
	SetCursorHeight(KDefaultCursorHeightPercentage);
	}

void CConsoleControl::SizeChangedL()
	{
	TInt bufferSize = Max(iBufferSize, iSizeChars.iHeight);
	for (TInt i=0; i<Min(iBuffer.Count(), bufferSize); ++i)
		{
		iBuffer[i]->SetWidthL(iSizeChars.iWidth);
		}
	while (iBuffer.Count() < bufferSize)
		{
		CConsoleLine* line = CConsoleLine::NewL(*this, *iFont, iSizeChars.iWidth);
		CleanupStack::PushL(line);
		iBuffer.AppendL(line);
		CleanupStack::Pop();
		}
	while (iBuffer.Count() > bufferSize)
		{
		delete iBuffer[iBuffer.Count()-1];
		iBuffer.Remove(iBuffer.Count()-1);
		}
	iCursor.Update();

	if (FivewayNavIsDisplaying()) SetDisplayFivewayNav(ETrue); // re-layout the onscreen nav if applicable
	}
	
CConsoleLine* CConsoleControl::GetLine(TBufferPosition aLine) const
	{
	return iBuffer[aLine.iPoint.iY];
	}
	
TPoint CConsoleControl::ViewPosition() const
	{
	return iViewWindow;
	}
	
TPoint CConsoleControl::CursorWindowPosition() const
	{
	return iCursorWindow;
	}

TSize CConsoleControl::GlyphSize() const
	{
	return iFont->GlyphSize();
	}
	
TBool CConsoleControl::IsSpecialChar(TChar aChar) const
	{
	switch(aChar)
		{
		case 0x00:	// Null.
		case 0x07:	// Bell.
		case 0x08:	// Backspace.
		case 0x7f:	// Delete.
		case 0x09:	// Tab.
		case 0x0a:	// Line feed.
		case 0x0b:	// Vertical tab.
		case 0x0c:	// Form feed.
		case 0x0d:	// Carriage return.
			return ETrue;
		default:
			return EFalse;
		}
	}
	
void CConsoleControl::HandleSpecialChar(TChar aChar)
	{
	switch(aChar)
		{
		case 0x00:	// Null.
			break;
		case 0x07:	// Bell.
			break;
		case 0x08:	// Backspace.
		case 0x7f:	// Delete.
			iCursor--;
			break;
		case 0x09:	// horizontal tab
			iCursor += KTabSize - (iCursor.Position().iPoint.iX % KTabSize);
			break;
		case 0x0a:	// line feed
			iCursor -= iCursor.Position().iPoint.iX;
			iCursor.Down();
			break;
		case 0x0b:	// Vertical tab.
			break;
		case 0x0c:  // Form feed
			ClearScreen();
			break;
		case 0x0d:	// carriage return
			iCursor -= iCursor.Position().iPoint.iX;
			break;
		default:
			User::Invariant();
		}
	}
	
void CConsoleControl::SendKey()
	{
	if (iReader && iKeyQueue.Count())
		{
		iCurrentKey = iKeyQueue[0];
		iKeyQueue.Remove(0);
		iReader->ReadComplete(KErrNone);
		iReader = NULL;
		}
	}
	
EXPORT_C void CConsoleControl::Read(MGuiConsoleReader& aReader)
	{
	if (iReader)
		{
		aReader.ReadComplete(KErrInUse);
		}
	else
		{
		iReader = &aReader;
		SendKey();
		}
	}
	
EXPORT_C void CConsoleControl::ReadCancel()
	{
	iReader = NULL;
	}
	
EXPORT_C void CConsoleControl::Write(const TDesC &aDes)
	{
	TRAP_IGNORE(WriteL(aDes));
	}

void CConsoleControl::WriteL(const TDesC &aDes)
	{
	TRect invalidRect;
	for (TInt i=0; i<aDes.Length(); ++i)
		{
		if (IsSpecialChar(aDes[i]))
			{
			if (!invalidRect.IsEmpty())
				{
				// Draw to the invalid rectangle before handling this special character because it may cause the
				// window to scroll, thereby invalidating the coordinates of invalidRect.
				DrawNow(invalidRect);
				invalidRect = TRect();
				}
			HandleSpecialChar(aDes[i]);
			}
		else
			{
			CConsoleLine* line = GetLine(iCursor.Position());
			line->SetL(iCursor.Position().iPoint.iX, aDes[i], iCurrentAttributes);
			TRect lineRect(LineRect(iCursor.Position()));
			Window().Invalidate(lineRect);
			if (invalidRect.IsEmpty())
				{
				invalidRect = lineRect;
				}
			else
				{
				invalidRect.BoundingRect(lineRect);
				}
			iCursor++;
			}
		}

	if (!invalidRect.IsEmpty())
		{
		DrawNow(invalidRect);
		}
	
	iCursor.Update();
	}

void CConsoleControl::StartBlinking()
	{
	if (!iBlinkTimer->IsActive())
		{
		iBlinkTimer->Start(500000, 500000, TCallBack(BlinkCallback, this));
		}
	}

TInt CConsoleControl::BlinkCallback(TAny* aPtr)
	{
	CConsoleControl* self = static_cast<CConsoleControl*>(aPtr);

	TBool neededToBlink(EFalse);
	for (TInt y = 0; y < self->iSizeChars.iHeight; ++y)
		{
		TViewPosition line(*self, 0, y);
		if (self->GetLine(line)->NeedToBlink(self->iBlinkOn))
			{
			neededToBlink = ETrue;
			self->Window().Invalidate(self->LineRect(line));
			}
		}
	if (neededToBlink)
		{
		self->iBlinkOn = !self->iBlinkOn;
		}
	else
		{
		self->iBlinkTimer->Cancel();
		}

	return KErrNone;
	}
	
EXPORT_C void CConsoleControl::ViewScrollUp()
	{
	iViewWindow.iY--;
	ViewMoved();
	}
	
EXPORT_C void CConsoleControl::ViewScrollDown()
	{
	iViewWindow.iY++;
	ViewMoved();
	}
	
EXPORT_C void CConsoleControl::ViewPageUp()
	{
	iViewWindow.iY -= iSizeChars.iHeight-1;
	ViewMoved();
	}
	
EXPORT_C void CConsoleControl::ViewPageDown()
	{
	iViewWindow.iY += iSizeChars.iHeight-1;
	ViewMoved();
	}
	
EXPORT_C void CConsoleControl::ViewHome()
	{
	iViewWindow.iY = 0;
	ViewMoved();
	}
	
EXPORT_C void CConsoleControl::ViewEnd()
	{
	iViewWindow = iCursorWindow;
	ViewMoved();
	}

void CConsoleControl::Invalidate5Way()
	{
	if (iDrawNavigator)
		{
		for (TInt i = 0; i < ENumButtons; i++)
			{
			Window().Invalidate(iButtonRects[i]);
			}
		}
	}

EXPORT_C TInt CConsoleControl::SetAttributes(TUint aAttributes, ConsoleAttributes::TColor aForegroundColor, ConsoleAttributes::TColor aBackgroundColor)
	{
	ConsoleAttributes::TAttributes currentAttributes = iCurrentAttributes;

	if (aAttributes == ConsoleAttributes::ENone)
		{
		currentAttributes = DefaultAttributes();
		}
	else
		{
		currentAttributes.iAttributes = aAttributes;
		}

	switch (aForegroundColor)
		{
		case ConsoleAttributes::EReset:
			currentAttributes.iForegroundColor = KDefaultForegroundColor;
			break;
		case ConsoleAttributes::EUnchanged:
			// Do nothing.
			break;
		default:
			currentAttributes.iForegroundColor = aForegroundColor;
			break;
		}

	switch (aBackgroundColor)
		{
		case ConsoleAttributes::EReset:
			currentAttributes.iBackgroundColor = KDefaultBackgroundColor;
			break;
		case ConsoleAttributes::EUnchanged:
			// Do nothing.
			break;
		default:
			currentAttributes.iBackgroundColor = aBackgroundColor;
			break;
		}

	TRAPD(err, iFont->PrepareForegroundColorL(MapColor(currentAttributes.iAttributes, currentAttributes.iForegroundColor)));
	if (err == KErrNone)
		{
		iCurrentAttributes = currentAttributes;
		}

	return err;
	}
	
void CConsoleControl::ViewMoved()
	{
	if (iViewWindow.iY < 0) iViewWindow.iY = 0;
	if (iViewWindow.iY + iSizeChars.iHeight > iBuffer.Count()) iViewWindow.iY = iBuffer.Count() - iSizeChars.iHeight;
	Window().Invalidate();
	}

void CConsoleControl::CursorWindowScrollDown()
	{
	TBool viewWindowMatchesCursorWindow = (iViewWindow.iY == iCursorWindow.iY);
	iCursorWindow.iY++;
	if (iCursorWindow.iY + iSizeChars.iHeight > iBuffer.Count())
		{
		iCursorWindow.iY--;
		CConsoleLine* topLine = iBuffer[0];
		iBuffer.Remove(0);
		topLine->Clear();
		iBuffer.Append(topLine);
		
		// JB - Disabled the code below because it forces a full redraw of the window which seems
		// unnecessary, particularly now that this function trys to efficiently scroll using
		// RDrawableWindow::Scroll. Note though that I can't claim to fully understand this code,
		// so this may be a dumb thing to do. Original comment follows...
		// move this up so that the text in the view window moves up with the scrolling text.
		// ViewScrollUp();
		}
	
	if (viewWindowMatchesCursorWindow) // keep them in sync if they were when we started
		{
		iViewWindow.iY = iCursorWindow.iY;
		}

#ifndef FSHELL_WSERV2_SUPPORT
	// On wserv1 we can't use Scroll if we're showing the 5-way nav - it looks bad
	if (iDrawNavigator)
		{
		DrawDeferred();
		}
	else
#endif
		{
		// on wserv2, or wserv1 when the 5-way nav isn't displayed, we can use RWindow::Scroll for better performance (although, performance only seems to be a problem on wserv1 with a wide screen. On wserv2 probably just a DrawDeferred() would be sufficient)
		Invalidate5Way();
		Window().Scroll(Rect(), TPoint(0, -(iFont->GlyphSize().iHeight)));
		Invalidate5Way();
		TRect invalidRect(LineRect(TViewPosition(*this, 0, iSizeChars.iHeight - 1)));
		Window().Invalidate(invalidRect);
		}
	}
	
EXPORT_C TPoint CConsoleControl::CursorPos() const
	{
	return iCursor.Position().iPoint;
	}
	
EXPORT_C void CConsoleControl::SetCursorPosAbs(const TPoint &aPoint)
	{
	iCursor.SetPosAbs(TConsCursorPosition(*this, aPoint));
	iCursor.Update();
	}
	
EXPORT_C void CConsoleControl::SetCursorPosRel(const TPoint &aPoint)
	{
	iCursor.SetPosRel(TConsCursorPosition(*this, aPoint));
	iCursor.Update();
	}
	
EXPORT_C void CConsoleControl::SetCursorHeight(TInt aPercentage)
	{
	iCursor.SetHeight(aPercentage);
	iCursor.Update();
	}
	
EXPORT_C void CConsoleControl::SetTitle(const TDesC &aTitle)
	{
	if (iUi)
		{
		TRAP_IGNORE(iUi->ConsoleTitleChangedL(this, aTitle));
		}
	}
	
EXPORT_C void CConsoleControl::ClearScreen()
	{
	Window().Invalidate();
	for (TInt y=0; y<iBuffer.Count(); ++y)
		{
		iBuffer[y]->Clear();
		}
	iCursorWindow.iY = 0;
	iViewWindow.iY = 0;
	iCursor.SetPosAbs(TConsCursorPosition(*this, 0,0));
	iCurrentAttributes = DefaultAttributes();
	}
	
EXPORT_C void CConsoleControl::ClearToEndOfLine()
	{
	GetLine(iCursor.Position())->ClearFrom(iCursor.Position());
	Window().Invalidate(LineRect(iCursor.Position()));
	}
	
EXPORT_C TSize CConsoleControl::ScreenSize() const
	{
	return iSizeChars;
	}
	
EXPORT_C TKeyCode CConsoleControl::KeyCode() const
	{
	return (TKeyCode)iCurrentKey.iCode;
	}
	
EXPORT_C TUint CConsoleControl::KeyModifiers() const
	{
	return iCurrentKey.iModifiers;
	}

void CConsoleControl::SimulateKeyL(TButton aButton)
	{
	TKeyEvent key;
	key.iRepeats = 0;
	key.iModifiers = 0;
	switch (aButton)
		{
		case EUp:
			key.iCode = EKeyUpArrow;
			key.iScanCode = EStdKeyUpArrow;
			break;
		case EDown:
			key.iCode = EKeyDownArrow;
			key.iScanCode = EStdKeyDownArrow;
			break;
		case ELeft:
			key.iCode = EKeyLeftArrow;
			key.iScanCode = EStdKeyLeftArrow;
			break;
		case ERight:
			key.iCode = EKeyRightArrow;
			key.iScanCode = EStdKeyRightArrow;
			break;
		case ECenter:
			key.iCode = EKeyEnter;
			key.iScanCode = EStdKeyEnter;
			break;
		default:
			return;
		}
	iCoeEnv->SimulateKeyEventL(key, EEventKey);
	}

EXPORT_C TBool CConsoleControl::FivewayNavIsDisplaying() const
	{
	return iButtonRects[ECenter].Width();
	}

EXPORT_C void CConsoleControl::SetDisplayFivewayNav(TBool aShow)
	{
	if (aShow)
		{
		TRect rect(Rect());
		TInt shortLen = Min(rect.Width(), rect.Height()); // Look nicer in landscape
		TSize buttonSize(shortLen / 4, shortLen / 4);
		TRect center(TPoint(rect.Width()/2 - buttonSize.iWidth/2, rect.Height() - 2*buttonSize.iHeight), buttonSize);
		iButtonRects[ECenter] = center;
		iButtonRects[ECenter].Shrink(5, 5);
		iButtonRects[ELeft] = center; iButtonRects[ELeft].Move(-buttonSize.iWidth, 0);
		iButtonRects[ERight] = center; iButtonRects[ERight].Move(buttonSize.iWidth, 0);
		iButtonRects[EUp] = center; iButtonRects[EUp].Move(0, -buttonSize.iHeight);
		iButtonRects[EDown] = center; iButtonRects[EDown].Move(0, buttonSize.iHeight);
		iDrawNavigator = ETrue;
		}
	else
		{
		Mem::FillZ(iButtonRects, sizeof(iButtonRects));
		}
	DrawDeferred();
	}

void CConsoleControl::ActivateL()
	{
	Window().SetBackgroundColor(KRgbWhite);
	CCoeControl::ActivateL();
	}

//______________________________________________________________________________
//						Image loader thread
// We use ICl to decode the font image. Despite ICL having the ability to create
// a thread of it own to do the decoding, when we decode from a descriptor
// (instead of a file), it still requires a AO RunL's in out thread to operate.
// So, we have to create a new thread manually so that we can always load images
// synchronously.

class RImageDecodeThreadParams
	{
public:
	RImageDecodeThreadParams(const TDesC& aFileName)
		: iMode(EDecodeFile), iFileName(aFileName), iData(KNullDesC8), iBitmapHandle(KNullHandle) {}
	RImageDecodeThreadParams(const TDesC8& aImageData)
		: iMode(EDecodeData), iFileName(KNullDesC), iData(aImageData), iBitmapHandle(KNullHandle) {}
	TInt Open()
		{
		TInt err = iBitmapReadySemaphore.CreateLocal(0);
		if (err==KErrNone) err = iThreadExitSemaphore.CreateLocal(0);
		if (err!=KErrNone) Close();
		return err;
		}
	void Close()
		{
		iBitmapReadySemaphore.Close();
		iThreadExitSemaphore.Close();
		}
public:
	enum
		{
		EDecodeFile,
		EDecodeData,
		} iMode;
	const TPtrC iFileName;
	const TPtrC8 iData;
	TFrameInfo iFrameInfo;
	TInt iBitmapHandle;
	RSemaphore iBitmapReadySemaphore;
	RSemaphore iThreadExitSemaphore;
	TInt iThreadError;
	};
	
class CImageDecodeWaiter : public CActive
	{
public:
	CImageDecodeWaiter(TInt aPriority)
		: CActive(aPriority)
		{
		CActiveScheduler::Add(this);
		}
	void Start(CImageDecoder* aDecoder, CFbsBitmap* aBitmap)
		{
		TRequestStatus* stat = &iStatus;
		aDecoder->Convert(stat, *aBitmap);
		SetActive();
		CActiveScheduler::Start();
		}
	virtual void RunL()
		{
		iCompletionErr = iStatus.Int();
		CActiveScheduler::Stop();
		}
	virtual void DoCancel() {}
public:
	TInt iCompletionErr;
	};

void ImageDecodeThreadL(RImageDecodeThreadParams& aParams)
	{
	User::LeaveIfError(RFbsSession::Connect());
	
	CActiveScheduler* as = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(as);
	CleanupStack::PushL(as);
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.ShareAuto());

	CImageDecoder* dec = NULL;
	switch (aParams.iMode)
		{
	case RImageDecodeThreadParams::EDecodeFile:
		dec = CImageDecoder::FileNewL(fs, aParams.iFileName);
		break;
	case RImageDecodeThreadParams::EDecodeData:
		dec = CImageDecoder::DataNewL(fs, aParams.iData);
		break;
		}
	CleanupStack::PushL(dec);
	
	aParams.iFrameInfo = dec->FrameInfo();
	CFbsBitmap* bmp = new(ELeave)CFbsBitmap;
	CleanupStack::PushL(bmp);
	User::LeaveIfError(bmp->Create(aParams.iFrameInfo.iOverallSizeInPixels, aParams.iFrameInfo.iFrameDisplayMode));
	
	
	CImageDecodeWaiter* waiter = new(ELeave)CImageDecodeWaiter(CActive::EPriorityStandard);
	CleanupStack::PushL(waiter);
	
	waiter->Start(dec, bmp);
	User::LeaveIfError(waiter->iCompletionErr);
	
	aParams.iBitmapHandle = bmp->Handle();
	aParams.iThreadError = KErrNone;
	aParams.iBitmapReadySemaphore.Signal();
	aParams.iThreadExitSemaphore.Wait();
	
	CleanupStack::PopAndDestroy(5, as); // as, fs, dec, bmp, waiter
	}

TInt ImageDecodeThead(TAny* aParams)
	{
	User::SetCritical(User::EProcessCritical);
	__UHEAP_MARK;
	RImageDecodeThreadParams* params = (RImageDecodeThreadParams*)aParams;

	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	if (!cleanup) return KErrNoMemory;
	TRAPD(err, ImageDecodeThreadL(*params));
	params->iThreadError = err;
	if (err!=KErrNone)
		{
		params->iBitmapReadySemaphore.Signal();
		}

	delete cleanup;
	REComSession::FinalClose();
	RFbsSession::Disconnect();
	__UHEAP_MARKEND;
	User::SetCritical(User::ENotCritical);
	return err;	
	}

//______________________________________________________________________________
//						CConsoleFont
EXPORT_C CConsoleFont* CConsoleFont::NewL(const TDesC& aFontFile)
	{
	CConsoleFont* self = new(ELeave)CConsoleFont;
	CleanupStack::PushL(self);
	self->ConstructL(aFontFile);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CConsoleFont* CConsoleFont::NewL(const TDesC8& aEncodedFontImage)
	{
	CConsoleFont* self = new(ELeave)CConsoleFont;
	CleanupStack::PushL(self);
	self->ConstructL(aEncodedFontImage);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CConsoleFont::~CConsoleFont()
	{
	for (TInt i = 0; i < KNumGlyphs; ++i)
		{
		delete iChars[i];
		}
	for (TInt i = 0; i < KNumColors; ++i)
		{
		delete iColors[i];
		}
	}

EXPORT_C TSize CConsoleFont::GlyphSize() const
	{
	return iGlyphRect.Size();
	}


EXPORT_C void CConsoleFont::DrawChar(TChar aChar, CBitmapContext& aDrawTo, TPoint aPosPixels, TRgb aColor)
	{
	// Note, this method assumes that the glyph's background has already been set to the appropriate color.

	if ((aChar < KFontBitmapFirstCharCode) || (aChar > KFontBitmapLastCharCode))
		{
		aChar = KFontBitmapLastCharCode;
		}
	
	aDrawTo.BitBltMasked(aPosPixels, ForegroundBitmap(aColor), iGlyphRect, iChars[(TInt)aChar - KFontBitmapFirstCharCode], ETrue);
	}

CConsoleFont::CConsoleFont()
	{
	}

void CConsoleFont::ConstructL(const TDesC& aFontFile)
	{
	RImageDecodeThreadParams decodeParams(aFontFile);
	ConstructL(decodeParams);
	}

void CConsoleFont::ConstructL(const TDesC8& aEncodedFontImage)
	{
	RImageDecodeThreadParams decodeParams(aEncodedFontImage);
	ConstructL(decodeParams);
	}

_LIT(KImageDecodeThreadNameFmt, "ImageDecodeThread%08x");

void CConsoleFont::ConstructL(RImageDecodeThreadParams& aDecodeParams)
	{
	User::LeaveIfError(aDecodeParams.Open());
	CleanupClosePushL(aDecodeParams);
	RThread decodeThread;
	CleanupClosePushL(decodeThread);
	// kick off the sub thread doing the decoding
	TInt err;
	TInt seq = 0;
	do
		{
		TBuf<0x20> threadName;
		threadName.AppendFormat(KImageDecodeThreadNameFmt, seq);
		err = decodeThread.Create(threadName, ImageDecodeThead, KDefaultStackSize, 0x1000, 0x200000, &aDecodeParams);
		seq++;
		} while (err==KErrAlreadyExists);
	User::LeaveIfError(err);
	decodeThread.Resume();

	TRequestStatus threadExit;
	decodeThread.Logon(threadExit);

	CleanupWaitForRequestPushL(threadExit); // things will probably go wrong if we leave on this line
	CleanupSignalSemaphorePushL(aDecodeParams.iThreadExitSemaphore);
	
	/*
	Now, on the cleanup stack we have:
	1. iThreadExitSemaphore.Signal()
	2. WaitForRequest(threadExit)
	3. decodeThread
	4. aDecodeParams
	
	So, if we leave now:
	1. We will signal the sub thread to exit
	2. We will then wait for the sub thread to exit
	3. close the sub thread handle
	4. close aDecodeParams, which will destroy the semaphores
	
	Note, it's important that we wait for the thread to exit before the cleanup stack
	if fully unwound, as if we destroy the semaphores before the sub thread has exitted,
	then it will panic with KERN-EXEC 0.
	*/
	
	// wait for it to decode the bitmap (or die)
	aDecodeParams.iBitmapReadySemaphore.Wait();
	User::LeaveIfError(aDecodeParams.iThreadError); // we will leave here if there was an error in the subthread
	
	// process decoded image
	TSize bmpSize = aDecodeParams.iFrameInfo.iOverallSizeInPixels;
	if (bmpSize.iWidth % KFontBitmapWidthChars) User::Leave(KErrCorrupt);// better error code / panic?
	if (bmpSize.iHeight % KFontBitmapHeightChars) User::Leave(KErrCorrupt);// better error code / panic?
	iGlyphRect = TRect(TPoint(0, 0), TSize(bmpSize.iWidth / KFontBitmapWidthChars, bmpSize.iHeight / KFontBitmapHeightChars));
	
	CFbsBitmap* allChars = new(ELeave)CFbsBitmap();
	CleanupStack::PushL(allChars);
	User::LeaveIfError(allChars->Duplicate(aDecodeParams.iBitmapHandle));

	for (TInt i = 0; i < KNumGlyphs; ++i)
		{
		iChars[i] = new(ELeave)CFbsBitmap;
		User::LeaveIfError(iChars[i]->Create(iGlyphRect.Size(), EGray2));

		CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(iChars[i]);
		CleanupStack::PushL(bitmapDevice);
	
		CBitmapContext* bitmapContext;
		User::LeaveIfError(bitmapDevice->CreateBitmapContext(bitmapContext));
		CleanupStack::PushL(bitmapContext);

		TPoint charPos(i % KFontBitmapWidthChars, i / KFontBitmapWidthChars);
		TPoint pixelPos(charPos.iX * iGlyphRect.Size().iWidth, charPos.iY * iGlyphRect.Size().iHeight);
		TRect charRect(pixelPos, iGlyphRect.Size());
		bitmapContext->BitBlt(TPoint(0, 0), allChars, charRect);	

		CleanupStack::PopAndDestroy(2, bitmapDevice);
		}

	// this will clean everything up - see above comment.
	CleanupStack::PopAndDestroy(5, &aDecodeParams); // decodeThread, aDecodeParams

	PrepareForegroundColorL(KDefaultForegroundColor);
	}

void CConsoleFont::PrepareForegroundColorL(TRgb aColor)
	{
	CFbsBitmap* bitmap = ForegroundBitmap(aColor);
	if (bitmap == NULL)
		{
		bitmap = new(ELeave)CFbsBitmap;
		CleanupStack::PushL(bitmap);
		User::LeaveIfError(bitmap->Create(iGlyphRect.Size(), EColor16));
		CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
		CleanupStack::PushL(bitmapDevice);
		CBitmapContext* bitmapContext;
		User::LeaveIfError(bitmapDevice->CreateBitmapContext(bitmapContext));
		CleanupStack::PushL(bitmapContext);
		bitmapContext->SetBrushColor(aColor);
		bitmapContext->SetPenStyle(CGraphicsContext::ENullPen);
		bitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
		bitmapContext->DrawRect(iGlyphRect);
		CleanupStack::PopAndDestroy(2, bitmapDevice);
		CleanupStack::Pop(bitmap);
		iColors[aColor.Color16()] = bitmap;
		}
	}

CFbsBitmap* CConsoleFont::ForegroundBitmap(TRgb aColor)
	{
	return iColors[aColor.Color16()];
	}

//______________________________________________________________________________
//						CConsoleLine
CConsoleLine* CConsoleLine::NewL(CConsoleControl& aControl, const CConsoleFont& aFont, TInt aWidth)
	{
	CConsoleLine* self = new(ELeave)CConsoleLine(aControl, aFont);
	CleanupStack::PushL(self);
	self->ConstructL(aWidth);
	CleanupStack::Pop(self);
	return self;
	}

CConsoleLine::~CConsoleLine()
	{
	iText.Close();
	iAttributeMap.Close();
	}

void CConsoleLine::SetWidthL(TInt aNewWidth)
	{
	if (aNewWidth != iWidth)
		{
		RBuf8 newText;
		newText.CreateL(aNewWidth);
		newText.Copy(iText.Left(aNewWidth));
		newText.Swap(iText);
		newText.Close();
		iWidth = aNewWidth;
		iAttributeMap.RemoveFrom(aNewWidth);
		}
	}

void CConsoleLine::SetL(TInt aIndex, TUint8 aChar, const ConsoleAttributes::TAttributes& aAttributes)
	{
	__ASSERT_ALWAYS((aIndex < iWidth)&&(aIndex>=0), User::Invariant());
	iAttributeMap.AddL(aIndex, iWidth, aAttributes);
	if (aIndex > iText.Length())
		{
		TInt oldLen = iText.Length();
		iText.SetLength(aIndex);
		iText.MidTPtr(oldLen).Fill(' ');
		iText.Append(aChar);
		}
	else if (aIndex == iText.Length())
		{
		iText.Append(aChar);
		}
	else
		{
		iText[aIndex] = aChar;
		}
	}
	
void CConsoleLine::Clear()
	{
	iAttributeMap.Reset();
	iText.Zero();
	}
	
void CConsoleLine::ClearFrom(TBufferPosition aPos)
	{
	TInt pos = Min(aPos.iPoint.iX, iText.Length());
	iText.SetLength(pos);
	iAttributeMap.RemoveFrom(pos);
	}

void CConsoleLine::Draw(CBitmapContext& aDrawTo, TViewPosition aViewPosition) const
	{
	const TSize glyphSize(iFont.GlyphSize());

	TScreenPosition screenPos(aViewPosition);
	TRect rectToClear(screenPos.iPoint, TScreenPosition(aViewPosition.iConsole, 0, (aViewPosition.iPoint.iY + 1) * glyphSize.iHeight).iPoint);

	const TInt numAttributeBlocks = iAttributeMap.NumberOfBlocks();
	for (TInt i = 0; i < numAttributeBlocks; ++i)
		{
		TInt startXPos = 0;
		TInt endXPos = 0;
		ConsoleAttributes::TAttributes attributes(ConsoleAttributes::ENone);
		iAttributeMap.GetBlock(i, iWidth, startXPos, endXPos, attributes);

		// Draw the background in the appropriate color in one hit.
		rectToClear.iTl.iX = screenPos.iPoint.iX + (startXPos * glyphSize.iWidth);
		rectToClear.iBr.iX = screenPos.iPoint.iX + (endXPos * glyphSize.iWidth);
		TRgb backgroundColor = MapColor(attributes.iAttributes, attributes.iBackgroundColor);
		aDrawTo.SetBrushColor(backgroundColor);
		aDrawTo.Clear(rectToClear);

		// Check if we're blinking.
		TRgb foregroundColor = MapColor(attributes.iAttributes, attributes.iForegroundColor);
		if ((attributes.iAttributes & ConsoleAttributes::EBlink) && iBlinkOn)
			{
			foregroundColor = backgroundColor;
			}

		// Draw any non-white-space characters in the appropriate foreground color.
		for (TInt i = startXPos; i < endXPos; ++i)
			{
			TUint16 c = At(i);
			if (c != ' ')
				{
				const_cast<CConsoleFont&>(iFont).DrawChar(c, aDrawTo, TPoint(screenPos.iPoint.iX + (i * glyphSize.iWidth), screenPos.iPoint.iY), foregroundColor);
				}
			if (attributes.iAttributes & ConsoleAttributes::EUnderscore)
				{
				const_cast<CConsoleFont&>(iFont).DrawChar('_', aDrawTo, TPoint(screenPos.iPoint.iX + (i * glyphSize.iWidth), screenPos.iPoint.iY), foregroundColor);
				}
			}

		if (attributes.iAttributes & ConsoleAttributes::EBlink)
			{
			iControl.StartBlinking();
			}
		}
	}

TBool CConsoleLine::NeedToBlink(TBool aBlinkOn)
	{
	TBool needToBlink(EFalse);
	const TInt numAttributeBlocks = iAttributeMap.NumberOfBlocks();
	for (TInt i = 0; i < numAttributeBlocks; ++i)
		{
		TInt startXPos = 0;
		TInt endXPos = 0;
		ConsoleAttributes::TAttributes attributes(ConsoleAttributes::ENone);
		iAttributeMap.GetBlock(i, iWidth, startXPos, endXPos, attributes);
		if (attributes.iAttributes & ConsoleAttributes::EBlink)
			{
			needToBlink = ETrue;
			iBlinkOn = aBlinkOn;
			break;
			}
		}
	return needToBlink;
	}
	
TUint16 CConsoleLine::At(TInt aPos) const
	{
	__ASSERT_ALWAYS((aPos < iWidth)&&(aPos>=0), User::Invariant());
	if (aPos < iText.Length())
		{
		return iText[aPos];
		}
	else
		{
		return ' ';
		}
	}
	
CConsoleLine::CConsoleLine(CConsoleControl& aControl, const CConsoleFont& aFont)
	: iControl(aControl), iFont(aFont)
	{
	}
	
void CConsoleLine::ConstructL(TInt aWidth)
	{
	iText.CreateL(aWidth);
	iWidth = aWidth;
	}

//______________________________________________________________________________
//						TConsoleCursor
TConsoleCursor::TConsoleCursor(CConsoleControl& aOwner)
	: iOwner(aOwner)
	{
	iTextCursor.iType = TTextCursor::ETypeRectangle;
	iTextCursor.iAscent = 0;
	iTextCursor.iColor = KRgbWhite;
	}

void TConsoleCursor::SetFont(const CConsoleFont& aFont)
	{
	iGlyphSize = aFont.GlyphSize();
	iTextCursor.iWidth = iGlyphSize.iWidth;
	}

TConsCursorPosition TConsoleCursor::Position() const
	{
	return TConsCursorPosition(iOwner, iPosition);
	}

void TConsoleCursor::SetPosAbs(TConsCursorPosition aPos)
	{
	iPosition = aPos.iPoint;
	BoundsCheck();
	}

void TConsoleCursor::SetPosRel(TConsCursorPosition aPos)
	{
	iPosition += aPos.iPoint;
	BoundsCheck();
	}

void TConsoleCursor::SetHeight(TInt aPercentage)
	{
	aPercentage = Max(0, Min(100, aPercentage));
	TReal height = ((TReal)iGlyphSize.iHeight * (TReal)aPercentage) / 100.0;
	TReal roundedHeight;
	Math::Round(roundedHeight, height, 0);
	iTextCursor.iHeight = (TInt)roundedHeight;
	// make sure the cursor is always visible if the percentage is non-zero.
	if ((iTextCursor.iHeight==0) && (aPercentage!=0)) iTextCursor.iHeight = 1;
	iTextCursor.iAscent = 0;
	iTextCursorOffset = iGlyphSize.iHeight - iTextCursor.iHeight;
	}

void TConsoleCursor::Update()
	{
	if (iOwner.IsFocused() && !iOwner.IsNonFocusing())
		{
		TViewPosition viewPos = Position();
		TPoint cursorPosPixels;
		cursorPosPixels.iX = viewPos.iPoint.iX * iGlyphSize.iWidth;
		cursorPosPixels.iY = ((viewPos.iPoint.iY) * iGlyphSize.iHeight) + iTextCursorOffset;
		/*if (iOwner.Rect().Contains(cursorPosPixels))
			{*/
			if (iOwner.DrawableWindow())
				{
				CCoeEnv::Static()->RootWin().SetTextCursor(*iOwner.DrawableWindow(), cursorPosPixels, iTextCursor);
				}
		/*	}
		else
			{
			Hide();
			}*/
		}
	}
	
void TConsoleCursor::Hide()
	{
	CCoeEnv::Static()->RootWin().CancelTextCursor();
	}

void TConsoleCursor::BoundsCheck()
	{
	iPosition.iX = Max(0, iPosition.iX);
	iPosition.iY = Max(0, iPosition.iY);
	iPosition.iX = Min(iOwner.ScreenSize().iWidth-1, iPosition.iX);
	iPosition.iY = Min(iOwner.ScreenSize().iHeight-1, iPosition.iY);
	}

void TConsoleCursor::operator++(int)
	{
	(*this)+=1;
	}
	
void TConsoleCursor::operator--(int)
	{
	(*this)-=1;
	}
	
void TConsoleCursor::operator+=(TUint aHowMuch)
	{
	TSize consSize(iOwner.ScreenSize());
	while (aHowMuch)
		{
		TInt len = Min(consSize.iWidth - iPosition.iX, aHowMuch);
		iPosition.iX += len;
		aHowMuch -= len;

		if (iPosition.iX == consSize.iWidth)
			{
			Down();
			iPosition.iX = 0;
			}
		}
	}
	
void TConsoleCursor::operator-=(TUint aHowMuch)
	{
	TSize consSize(iOwner.ScreenSize());
	while (aHowMuch)
		{
		if (iPosition.iX == 0)
			{
			Up();
			iPosition.iX = consSize.iWidth;
			}

		TInt len = Min(iPosition.iX, aHowMuch);
		iPosition.iX -= len;
		aHowMuch -= len;
		}
	
	}
	
void TConsoleCursor::Down()
	{
	iPosition.iY++;
	if (iPosition.iY >= iOwner.ScreenSize().iHeight)
		{
		iPosition.iY--;
		iOwner.CursorWindowScrollDown();
		}
	}

void TConsoleCursor::Up()
	{
	iPosition.iY--;
	BoundsCheck();
	}

//______________________________________________________________________________
//						CGuiConsole
EXPORT_C CGuiConsole::CGuiConsole(CConsoleControl& aControl)
	: iControl(aControl), iRefCount(1)
	{
	}
	
EXPORT_C CConsoleControl& CGuiConsole::Control()
	{
	return iControl;
	}

EXPORT_C TInt CGuiConsole::Create(const TDesC &aTitle, TSize)
	{
	iControl.SetTitle(aTitle);
	return KErrNone;
	}

EXPORT_C void CGuiConsole::Read(TRequestStatus &aStatus)
	{
	if (iReadStatus || iReader)
		{
		TRequestStatus* stat = &aStatus;
		User::RequestComplete(stat, KErrInUse);
		}
	else
		{
		iReadStatus = &aStatus;
		iControl.Read(*this);
		}
	}

EXPORT_C void CGuiConsole::ReadCancel()
	{
	if (iReadStatus)
		{
		User::RequestComplete(iReadStatus, KErrCancel);
		iControl.ReadCancel();
		}
	else if (iReader)
		{
		iReader->ReadComplete(KErrCancel);
		iReader = NULL;
		iControl.ReadCancel();
		}
	}

EXPORT_C void CGuiConsole::Write(const TDesC &aDes)
	{
	iControl.Write(aDes);
	}

EXPORT_C TPoint CGuiConsole::CursorPos() const
	{
	return iControl.CursorPos();
	}

EXPORT_C void CGuiConsole::SetCursorPosAbs(const TPoint &aPoint)
	{
	iControl.SetCursorPosAbs(aPoint);
	}

EXPORT_C void CGuiConsole::SetCursorPosRel(const TPoint &aPoint)
	{
	iControl.SetCursorPosRel(aPoint);
	}

EXPORT_C void CGuiConsole::SetCursorHeight(TInt aPercentage)
	{
	iControl.SetCursorHeight(aPercentage);
	}

EXPORT_C void CGuiConsole::SetTitle(const TDesC &aTitle)
	{
	iControl.SetTitle(aTitle);
	}

EXPORT_C void CGuiConsole::ClearScreen()
	{
	iControl.ClearScreen();
	}

EXPORT_C void CGuiConsole::ClearToEndOfLine()
	{
	iControl.ClearToEndOfLine();
	}

EXPORT_C TSize CGuiConsole::ScreenSize() const
	{
	return iControl.ScreenSize();
	}


EXPORT_C TKeyCode CGuiConsole::KeyCode() const
	{
	return iControl.KeyCode();
	}


EXPORT_C TUint CGuiConsole::KeyModifiers() const
	{
	return iControl.KeyModifiers();
	}

EXPORT_C TInt CGuiConsole::Extension_(TUint aExtensionId, TAny*&, TAny* a1)
	{
	if (aExtensionId == ConsoleAttributes::KSetConsoleAttributesExtension)
		{
		ConsoleAttributes::TAttributes* attributes = (ConsoleAttributes::TAttributes*)a1;
		return iControl.SetAttributes(attributes->iAttributes, attributes->iForegroundColor, attributes->iBackgroundColor);
		}
	return KErrNotSupported;
	}

void CGuiConsole::ReadComplete(TInt aStatus)
	{
	if (iReadStatus)
		{
		User::RequestComplete(iReadStatus, aStatus);
		}
	if (iReader)
		{
		CConsoleProxySession* reader = iReader;
		iReader = NULL;
		reader->ReadComplete(aStatus);
		}
	}
	
void CGuiConsole::Open()
	{
	++iRefCount;
	}

void CGuiConsole::Close()
	{
	--iRefCount;
	if (iRefCount==0)
		{
		delete this;
		}
	}

CConsoleBase* CGuiConsole::Console()
	{
	return this;
	}

void CGuiConsole::Read(CConsoleProxySession& aSession)
	{
	if (iReader || iReadStatus)
		{
		aSession.ReadComplete(KErrInUse);
		}
	else
		{
		iReader = &aSession;
		iControl.Read(*this);
		}
	}

CGuiConsole::~CGuiConsole()
	{
	iControl.Closed();
	}

//______________________________________________________________________________
//						TBufferPosition
TBufferPosition::TBufferPosition(const CConsoleControl& aConsole, TPoint aPosition)
	: iConsole(aConsole), iPoint(aPosition)
	{
	}

TBufferPosition::TBufferPosition(const CConsoleControl& aConsole, TInt aX, TInt aY)
	: iConsole(aConsole), iPoint(aX, aY)
	{
	}

TBufferPosition::TBufferPosition(const TConsCursorPosition& aCursorPosition)
	: iConsole(aCursorPosition.iConsole)
	, iPoint(aCursorPosition.iPoint + aCursorPosition.iConsole.CursorWindowPosition())
	{
	}

TBufferPosition::TBufferPosition(const TViewPosition& aViewPosition)
	: iConsole(aViewPosition.iConsole)
	, iPoint(aViewPosition.iPoint + aViewPosition.iConsole.ViewPosition())
	{
	}

	

//______________________________________________________________________________
//						TConsCursorPosition
TConsCursorPosition::TConsCursorPosition(const CConsoleControl& aConsole, TPoint aPosition)
	: iConsole(aConsole), iPoint(aPosition)
	{
	}

TConsCursorPosition::TConsCursorPosition(const CConsoleControl& aConsole, TInt aX, TInt aY)
	: iConsole(aConsole), iPoint(aX, aY)
	{
	}

TConsCursorPosition::TConsCursorPosition(const TBufferPosition& aBufferPosition)
	: iConsole(aBufferPosition.iConsole)
	, iPoint(aBufferPosition.iPoint - aBufferPosition.iConsole.CursorWindowPosition())
	{
	}

TConsCursorPosition::TConsCursorPosition(const TViewPosition& aViewPosition)
	: iConsole(aViewPosition.iConsole)
	, iPoint(aViewPosition.iPoint + aViewPosition.iConsole.ViewPosition() - aViewPosition.iConsole.CursorWindowPosition())
	{
	}

	

//______________________________________________________________________________
//						TViewPosition
TViewPosition::TViewPosition(const CConsoleControl& aConsole, TPoint aPosition)
	: iConsole(aConsole), iPoint(aPosition)
	{
	}

TViewPosition::TViewPosition(const CConsoleControl& aConsole, TInt aX, TInt aY)
	: iConsole(aConsole), iPoint(aX, aY)
	{
	}

TViewPosition::TViewPosition(const TConsCursorPosition& aCursorPosition)
	: iConsole(aCursorPosition.iConsole)
	, iPoint(aCursorPosition.iPoint + aCursorPosition.iConsole.CursorWindowPosition() - aCursorPosition.iConsole.ViewPosition())
	{
	}

TViewPosition::TViewPosition(const TBufferPosition& aBufferPosition)
	: iConsole(aBufferPosition.iConsole)
	, iPoint(aBufferPosition.iPoint - aBufferPosition.iConsole.ViewPosition())
	{
	}

//______________________________________________________________________________
//						TScreenPosition
TScreenPosition::TScreenPosition(const CConsoleControl& aConsole, TPoint aPosition)
	: iConsole(aConsole), iPoint(aPosition)
	{
	}

TScreenPosition::TScreenPosition(const CConsoleControl& aConsole, TInt aX, TInt aY)
	: iConsole(aConsole), iPoint(aX, aY)
	{
	}

TScreenPosition::TScreenPosition(const TViewPosition& aViewPosition)
	: iConsole(aViewPosition.iConsole)
	, iPoint(aViewPosition.iPoint.iX * aViewPosition.iConsole.GlyphSize().iWidth, aViewPosition.iPoint.iY * aViewPosition.iConsole.GlyphSize().iHeight)
	{
	}

//______________________________________________________________________________
//						RAttributeMap

RAttributeMap::RAttributeMap()
	: iAttributes(KAttributeMapGranularity, _FOFF(TAttributes, iPosition))
	{
	}

void RAttributeMap::Close()
	{
	iAttributes.Close();
	}

void RAttributeMap::Reset()
	{
	iAttributes.Reset();
	}

void RAttributeMap::AddL(TInt aPosition, TInt aLineWidth, const ConsoleAttributes::TAttributes& aAttributes)
	{
	// Take a copy of the attributes array to make cleanup easier.
	RArray<TAttributes> attributes;
	CleanupClosePushL(attributes);
	const TInt numAttributes = iAttributes.Count();
	for (TInt i = 0; i < numAttributes; ++i)
		{
		attributes.AppendL(iAttributes[i]);
		}

	if (attributes.Count() == 0)
		{
		// This is the first set of attributes to be added. Add a default set before them.
		attributes.AppendL(TAttributes(0, DefaultAttributes()));
		}

	TAttributes attr(aPosition, aAttributes);
	TInt pos;
	TInt err = attributes.FindInSignedKeyOrder(attr, pos);
	if (err == KErrNone)
		{
		// Attribute already exists at this position - replace.
		attributes[pos] = attr;
		}
	else
		{
		// No attribute found at aPosition.
		// pos is set to the index _after_ aPosition. This must be at least 1 because the first slot in the array is always taken with defaults or something else.
		// Set pos to the index _before_ aPosition.
		--pos;

		// Check earlier attributes to see if they match.
		if (!(attributes[pos] == attr))
			{
			// No match - insert new attributes.
			attributes.InsertInSignedKeyOrderL(attr);

			// Get the position of the newly inserted attributes.
			attributes.FindInSignedKeyOrder(attr, pos);
			}
		}

	const TAttributes& newAttributes = attributes[pos];
	if ((pos == (attributes.Count() - 1)) && (newAttributes.iPosition < (aLineWidth - 1)) && !(newAttributes == TAttributes(0, DefaultAttributes())))
		{
		// New attributes are the last in the list, and not at the end of the line and not defaults.
		// Add another set of defaults after them.
		attributes.AppendL(TAttributes(newAttributes.iPosition + 1, DefaultAttributes()));
		}

	iAttributes.Close();
	iAttributes = attributes;
	CleanupStack::Pop(&attributes);
	}

void RAttributeMap::RemoveFrom(TInt aPosition)
	{
	for (TInt i = (iAttributes.Count() - 1); i >= 0; --i)
		{
		if (iAttributes[i].iPosition >= aPosition)
			{
			iAttributes.Remove(i);
			}
		}
	}

TInt RAttributeMap::NumberOfBlocks() const
	{
	if (iAttributes.Count() == 0)
		{
		return 1; // If no attributes have been added, give the entire line an implict default set of attributes.
		}
	return iAttributes.Count();
	}

void RAttributeMap::GetBlock(TInt aIndex, TInt aLineWidth, TInt& aStartPosition, TInt& aEndPosition, ConsoleAttributes::TAttributes& aAttributes) const
	{
	if (iAttributes.Count() == 0)
		{
		ASSERT(aIndex == 0);
		aStartPosition = 0;
		aEndPosition = aLineWidth;
		aAttributes = DefaultAttributes();
		}
	else
		{
		const TAttributes& attributes = iAttributes[aIndex];
		aAttributes = attributes.iAttributes;
		aStartPosition = attributes.iPosition;

		if ((iAttributes.Count() == 1) || (aIndex == (iAttributes.Count() - 1)))
			{
			aEndPosition = aLineWidth;
			}
		else
			{
			aEndPosition = iAttributes[aIndex + 1].iPosition;
			}
		}
	}

RAttributeMap::TAttributes::TAttributes(TInt aPosition, const ConsoleAttributes::TAttributes& aAttributes)
	: iPosition(aPosition), iAttributes(aAttributes)
	{
	}

TBool RAttributeMap::TAttributes::operator==(const TAttributes& aAttributes) const
	{
	// Note, intentionally ignoring iPosition so ensure that existing attribute block are extended where possible.
	return ((iAttributes.iAttributes == aAttributes.iAttributes.iAttributes) && 
			(iAttributes.iForegroundColor == aAttributes.iAttributes.iForegroundColor) &&
			(iAttributes.iBackgroundColor == aAttributes.iAttributes.iBackgroundColor));
	}
