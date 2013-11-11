// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cons.h>
#include <e32keys.h>
#include <e32base.h>
#include <w32std.h>
#include <gdi.h>
#include <bitstd.h>
#include <bitdev.h>

const TInt KGroupId = 0x100039e7;

class CWsConsoleMux;
class CWsConsole;

// Active object to listen to Wserv events. Only used when
// application thread has installed an active scheduler
//
NONSHARABLE_CLASS(CWsKeyReader): public CActive
	{
public:
	CWsKeyReader(CWsConsoleMux* aMux);
	void MakeRequest();

protected:
	void RunL();
	void DoCancel();

private:
	CWsConsoleMux* iMux; //uses
	};

// Master console to dispatch key events to relevant console in situation where 
// a single application thread owns multiple instances of console (CConsoleBase).
// Reference counted as only a single instance of this class will be created
// for each thread.
//
NONSHARABLE_CLASS(CWsConsoleMux): public CBase
	{
public:
	static TInt Open();
	static void Close();
	static CWsConsoleMux* Static();
	~CWsConsoleMux();
	
	void MakeRequest(CWsConsole* aConsole);
	void CancelRequest(CWsConsole* aConsole);
	void RequestComplete();

	inline RWsSession& Session();
	inline RWindowGroup& Group();
	inline CWindowGc& Gc();
	inline CWsScreenDevice& Screen();

private:
	static CWsConsoleMux* NewL();
	CWsConsoleMux();
	void ConstructL();

private:
	TInt iRefCount;
	RWsSession iWs;
	RWindowGroup iGroup;
	CWsScreenDevice* iScr;
	CWindowGc* iGc;

	CWsConsole* iConsole;
	CWsKeyReader* iReader;
	};

// Console that uses direct Wserv API. Each console will own a window and 
// a bitmap for its backing store, but will be sharing Wserv session, window group,
// screen device and window drawing context
//
NONSHARABLE_CLASS(CWsConsole): public CConsoleBase
	{
	friend class CWsConsoleMux;

public:
	CWsConsole();
	virtual ~CWsConsole();
	
	//from CConsoleBase 
	virtual TInt Create(const TDesC &aTitle,TSize aSize);
	virtual void Read(TRequestStatus &aStatus);
	virtual void ReadCancel();
	virtual void Write(const TDesC &aDes);
	virtual TPoint CursorPos() const;
	virtual void SetCursorPosAbs(const TPoint &aPoint);
	virtual void SetCursorPosRel(const TPoint &aPoint);
	virtual void SetCursorHeight(TInt aPercentage);
	virtual void SetTitle(const TDesC &aTitle);
	virtual void ClearScreen();
	virtual void ClearToEndOfLine();
	virtual TSize ScreenSize() const;
	virtual TKeyCode KeyCode() const;
	virtual TUint KeyModifiers() const;

private:	
	// for CWsConsoleMux
	TRequestStatus* ReadStatus();
	void SetKey(TUint aCode, TUint aModifier);

	// internal
	void ConstructL();
	void DrawNow();
	void Scroll();
	TPoint WritePos();
	void CarriageReturn();
	void LineFeed();
	void Left();
	void Right();
	void WriteChar(TText);
	
private:
	// window stuff
	RWindow iWin;
	TSize iWindowSize;

	// back buffer
	CFbsBitmap* iBitmap;
	CFbsBitGc* iBitGc;
	CFbsBitmapDevice* iBitDev;

	// console stuff in chars dimension
	TPoint iCursorPos; //The position of the cursor is on the baseline
	TSize iSizeInChars;

	// font stuff
	CFont* iFont;
	TSize iCharSize;
	TInt iFontAscent;
	TInt iFontDescent;
	
	TKeyCode iKeyCode;
	TUint iModifiers;
	TTextCursor iTextCursor;
	
	// master console
	CWsConsoleMux* iMux;
	TRequestStatus* iReadStatus;
	};

//
// CWsKeyReader implementation
//
//
//
CWsKeyReader::CWsKeyReader(CWsConsoleMux* aMux):
	CActive(CActive::EPriorityStandard),
	iMux(aMux)	
	{
	CActiveScheduler::Add(this);
	}

void CWsKeyReader::MakeRequest()
	{
	SetActive();
	iMux->Session().EventReady(&iStatus);
	}

void CWsKeyReader::RunL()
	{
	// let master console decide what to do
	iMux->RequestComplete();
	}

void CWsKeyReader::DoCancel()
	{
	iMux->Session().EventReadyCancel();
	}

//
// CWsConsoleMux implementation
//
//
//
TInt CWsConsoleMux::Open()
	{
	CWsConsoleMux* mux = (CWsConsoleMux*)Dll::Tls();
	
	// not the first time, simply increment refcount
	if (mux)
		{
		++mux->iRefCount;
		return KErrNone;
		}

	// first time, let's create master console and
	// stick it to TLS
	TRAPD(err, mux = CWsConsoleMux::NewL());
	if (err != KErrNone)
		{
		return err;
		}

	err = Dll::SetTls(mux);
	if (err != KErrNone)
		{
		delete mux;
		return err;
		}

	++mux->iRefCount;			
	return KErrNone;
	}

void CWsConsoleMux::Close()
	{
	CWsConsoleMux* mux = (CWsConsoleMux*)Dll::Tls();
	if (!mux)
		{
		return;
		}
		
	// destroy master console if this is the last use from
	// this thread and reset TLS
	if (--mux->iRefCount == 0)
		{
		delete mux;
		Dll::FreeTls();
		}
	}

CWsConsoleMux* CWsConsoleMux::Static()
	{
	return (CWsConsoleMux*)Dll::Tls();
	}

CWsConsoleMux::CWsConsoleMux()
	{
	}
	
CWsConsoleMux::~CWsConsoleMux()
	{
	delete iReader;
	delete iGc;
	delete iScr;
	iGroup.Close();
	iWs.Close();
	}

CWsConsoleMux* CWsConsoleMux::NewL()
	{
	CWsConsoleMux* mux = new(ELeave) CWsConsoleMux;
	CleanupStack::PushL(mux);
	mux->ConstructL();
	CleanupStack::Pop(mux);
	
	return mux;
	}
	
void CWsConsoleMux::ConstructL()
	{
	TInt err = iWs.Connect();
	User::LeaveIfError(err);
	
	iScr = new(ELeave) CWsScreenDevice(iWs);
	err = iScr->Construct();
	User::LeaveIfError(err);
	
	iGc = new(ELeave) CWindowGc(iScr);
	err = iGc->Construct();
	User::LeaveIfError(err);
	
	iGroup = RWindowGroup(iWs);
	err = iGroup.Construct(KGroupId, ETrue);
	User::LeaveIfError(err);
	
	// we will be using active object to listen to Wserv events
	// only if this thread already has active scheduler
	//
	if (CActiveScheduler::Current())
		{
		iReader = new(ELeave) CWsKeyReader(this);
		}
	}

RWsSession& CWsConsoleMux::Session()
 	{
 	return iWs;
 	}
 	
RWindowGroup& CWsConsoleMux::Group()
	{
	return iGroup;
	}
	
CWindowGc& CWsConsoleMux::Gc()
	{
	return *iGc;
	}
	
CWsScreenDevice& CWsConsoleMux::Screen()
	{
	return *iScr;
	}

void CWsConsoleMux::MakeRequest(CWsConsole* aConsole)
	{
	if (!iReader)
		{
		// client does not have scheduler, ideally we should use
		// secondary thread to make this asynchronous.
		//
		// however, we can get away with this because application 
		// that doesn't use active scheduler will be calling asynchronous console read 
		// and immediately followed by WaitForRequest e.g.
		//
		// while (1)
		//	{
		//	...
		//	console->Read(status);
		//	User::WaitForRequest(status);
		//	...
		//	}
		//
		TWsEvent event;

		// keep going until we got key event
		do
			{
			TRequestStatus s;
			s = KRequestPending;
			iWs.EventReady(&s);
			User::WaitForRequest(s);
			
			iWs.GetEvent(event);
			}
		while (event.Type() != EEventKey);
		
		aConsole->SetKey(event.Key()->iCode, event.Key()->iModifiers);
		TRequestStatus* pS = aConsole->ReadStatus();
		User::RequestComplete(pS, KErrNone);
		}
	else
		{
		iConsole = aConsole;
		iReader->MakeRequest();
		}
	}

// called from key listener RunL
//
void CWsConsoleMux::RequestComplete()
	{
	TWsEvent event;
	iWs.GetEvent(event);

	if (event.Type() == EEventKey)
		{
		iConsole->SetKey(event.Key()->iCode, event.Key()->iModifiers);
		TRequestStatus* pS = iConsole->ReadStatus();
		User::RequestComplete(pS, KErrNone);
		iConsole = NULL;
		}
	else
		{
		// keep going until we got key event
		//
		iReader->MakeRequest();
		}
	}

void CWsConsoleMux::CancelRequest(CWsConsole* aConsole)
	{
	if (iReader && iConsole == aConsole)
		{
		iReader->Cancel();
		}
	}
	
//
// CWsConsole implementation
//
//
//
CWsConsole::CWsConsole()
	{
	}

CWsConsole::~CWsConsole()
	{
	if(iBitGc)
		{
		iBitGc->DiscardFont();
		}
	if(iBitDev)
		{
		iBitDev->ReleaseFont(iFont);
		}

	delete iBitGc;
	delete iBitDev;
	delete iBitmap;	
	iWin.Close();
	iMux->Session().Flush();

	CWsConsoleMux::Close();
	}

TInt CWsConsole::Create(const TDesC& /*aTitle*/, TSize /*aSize*/)
	{
	// we must not push this object to cleanup stack during construction because
	// caller will explicitly call delete on this object when Create() returns error
	//
	TRAPD(err, ConstructL());
	return err;
	}
	
void CWsConsole::ConstructL()
	{
	TInt err = CWsConsoleMux::Open();
	User::LeaveIfError(err);

	iMux = CWsConsoleMux::Static();
	
	iWindowSize = iMux->Screen().SizeInPixels();
	
	iBitmap = new(ELeave) CFbsBitmap;
	err = iBitmap->Create(iWindowSize, EGray2);
	User::LeaveIfError(err);

	iBitDev = CFbsBitmapDevice::NewL(iBitmap);
	err = iBitDev->CreateContext(iBitGc);
	User::LeaveIfError(err);

	iBitGc->SetPenColor(KRgbBlack);
	iBitGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
	iBitGc->SetBrushColor(KRgbWhite);
	iBitGc->Clear();

	TFontSpec fs(_L("DejaVu Sans Mono"), 12);
	err = iBitDev->GetNearestFontToDesignHeightInPixels(iFont, fs);
	User::LeaveIfError(err);

	iBitGc->UseFont(iFont);

	// uses monospace font to get uniform glpyh size
	iCharSize.iHeight = iFont->FontMaxAscent() + iFont->FontMaxDescent();
	iCharSize.iWidth = iFont->MaxCharWidthInPixels();
	iFontAscent = iFont->FontMaxAscent();
	iFontDescent= iFont->FontMaxDescent();

	// cursor represent char position in character-based area
	// eshell is using 0,0 based
	iCursorPos = TPoint(1,1);
	// console size in number of characters and lines
	iSizeInChars.iWidth = iWindowSize.iWidth / iCharSize.iWidth;
	iSizeInChars.iHeight = iWindowSize.iHeight / iCharSize.iHeight;
	
	iTextCursor.iType = TTextCursor::ETypeRectangle;
	iTextCursor.iHeight = iCharSize.iHeight;
	iTextCursor.iAscent = iFontAscent;
	iTextCursor.iWidth = iCharSize.iWidth;
	iTextCursor.iFlags = 0;
	iTextCursor.iColor = KRgbWhite;


	iWin = RWindow(iMux->Session());
	err = iWin.Construct(iMux->Group(), (TInt)this);
	User::LeaveIfError(err);
		
	iWin.Activate();
	iMux->Session().Flush();
	}

TRequestStatus* CWsConsole::ReadStatus()
	{
	return iReadStatus;
	}

void CWsConsole::SetKey(TUint aCode, TUint aModifier) 
	{
	iKeyCode = (TKeyCode)aCode;
	iModifiers = aModifier;
	}

TPoint CWsConsole::WritePos()
	{
	return TPoint((iCursorPos.iX - 1) * iCharSize.iWidth, (iCursorPos.iY - 1) * iCharSize.iHeight + iFontAscent);
	}
	
void CWsConsole::Read(TRequestStatus &aStatus)
	{
	iReadStatus = &aStatus;
	*iReadStatus = KRequestPending;
	iMux->MakeRequest(this);
	}

void CWsConsole::ReadCancel()
	{
	iMux->CancelRequest(this);
	iReadStatus = NULL;
	}

void CWsConsole::CarriageReturn()
	{
	iCursorPos.iX = 1;
	}

void CWsConsole::LineFeed()
	{
	CarriageReturn();
	if (iCursorPos.iY < iSizeInChars.iHeight)
		{
		++iCursorPos.iY;
		}
	else
		{
		Scroll();
		}
	}

void CWsConsole::Left()
	{
	if (iCursorPos != TPoint(1,1))
		{
		if (iCursorPos.iX == 1)
			{
			iCursorPos.iX += iSizeInChars.iWidth;
			--iCursorPos.iY;
			}
		--iCursorPos.iX;
		}
	}

void CWsConsole::Right()
	{
	++iCursorPos.iX;
	if (iCursorPos.iX > iSizeInChars.iWidth)
		{
		LineFeed();
		}
	}

void CWsConsole::Write(const TDesC& aDes)
	{
	const TInt length = aDes.Length();
	if (length == 0)
		{
		return;
		}
	
	for (TInt i=0; i<length; ++i)
		{
		switch (aDes[i])
			{
			case 0x07:
				RDebug::Print(_L("WSCON: End of command line!"));
				break;
			case 0x0a:
				LineFeed();
				break;
			case 0x0d:
				CarriageReturn();
				break;
			default:
				WriteChar(aDes[i]);
				break;
			}
		}
	
	DrawNow();
	}

void CWsConsole::WriteChar(TText aChar)
	{
	TBuf<1> s;
	s.Append(aChar);
	// use draw text box to clear the glpyh box
	TPoint writePos = WritePos();
	TRect r(writePos.iX, writePos.iY-iFontAscent,writePos.iX+iCharSize.iWidth, writePos.iY+iFontDescent);
	iBitGc->DrawText(s, r, iFontAscent);
	Right();
	}

TPoint CWsConsole::CursorPos() const
	{
	// eshell is using 0,0 based
	return iCursorPos - TPoint(1,1);
	}

void CWsConsole::SetCursorPosAbs(const TPoint& aPoint)
	{
	// eshell is using 0,0 based
	iCursorPos = aPoint + TPoint(1,1);
	iMux->Group().SetTextCursor(iWin, WritePos(), iTextCursor);
	}

void CWsConsole::SetCursorPosRel(const TPoint& aPoint)
	{
	iCursorPos += aPoint;
	iMux->Group().SetTextCursor(iWin, WritePos(), iTextCursor);
	}

void CWsConsole::SetCursorHeight(TInt aPercentage)
	{
	if (aPercentage == 0)
		{
		// none
		iTextCursor.iHeight = 0;
		}
	else if (aPercentage == 100)
		{
		// insert
		iTextCursor.iHeight = iCharSize.iHeight;
		iTextCursor.iAscent = iFontAscent;
		}
	else
		{
		// normal
		iTextCursor.iHeight = 1;
		iTextCursor.iAscent = 0;
		}

	iMux->Group().SetTextCursor(iWin, WritePos(), iTextCursor);
	}

void CWsConsole::SetTitle(const TDesC& /*aTitle*/)
	{
	}

void CWsConsole::ClearScreen()
	{
	iBitGc->Clear();
	iCursorPos.iX = iCursorPos.iY = 1;
	DrawNow();
	}

void CWsConsole::ClearToEndOfLine()
	{
	iMux->Group().CancelTextCursor();
	TPoint writePos = WritePos();
	TRect r(writePos.iX, writePos.iY-iFontAscent, iWindowSize.iWidth, writePos.iY+iFontDescent);
	iBitGc->Clear(r);
	DrawNow();
	iMux->Group().SetTextCursor(iWin, WritePos(), iTextCursor);
	}

TSize CWsConsole::ScreenSize() const
	{
	return iSizeInChars;
	}

TKeyCode CWsConsole::KeyCode() const
	{
	return iKeyCode;
	}

TUint CWsConsole::KeyModifiers() const
	{
	return iModifiers;
	}

void CWsConsole::DrawNow()
	{
	iWin.Invalidate();
	iWin.BeginRedraw();
	iMux->Gc().Activate(iWin);
	iMux->Gc().BitBlt(TPoint(0,0), iBitmap);
	iMux->Gc().Deactivate();
	iWin.EndRedraw();
	iMux->Session().Flush();
	}

void CWsConsole::Scroll()
	{
	TRect r(0, iCharSize.iHeight, iWindowSize.iWidth, iSizeInChars.iHeight * iCharSize.iHeight);

	iBitGc->CopyRect(TPoint(0, -iCharSize.iHeight), r);
	// clear last line
	TRect rect(0, WritePos().iY-iFontAscent, iWindowSize.iWidth, iWindowSize.iHeight);
	iBitGc->Clear(rect);
	}

extern "C" EXPORT_C TAny *NewConsole()
	{
	return new CWsConsole;
	}
