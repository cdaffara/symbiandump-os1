// nullcons.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include <e32std.h>
#include <e32cons.h>
#include <e32keys.h>

NONSHARABLE_CLASS(CNullConsole) : public CConsoleBase
	{
public:
	CNullConsole();
	virtual ~CNullConsole();
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
	};

CNullConsole::CNullConsole()
	{
	}

CNullConsole::~CNullConsole()
	{
	}

TInt CNullConsole::Create(const TDesC& /*aTitle*/, TSize /*aSize*/)
	{
	return(KErrNone);
	}

void CNullConsole::Read(TRequestStatus &aStatus)
	{
	TRequestStatus *pS=(&aStatus);
	User::RequestComplete(pS, KErrNotSupported);
	}

void CNullConsole::ReadCancel()
	{
	}

void CNullConsole::Write(const TDesC& /*aDes*/)
	{
	}

TPoint CNullConsole::CursorPos() const
	{
	return TPoint(0,0);
	}

void CNullConsole::SetCursorPosAbs(const TPoint& /*aPoint*/)
	{
	}

void CNullConsole::SetCursorPosRel(const TPoint& /*aPoint*/)
	{
	}

void CNullConsole::SetCursorHeight(TInt /*aPercentage*/)
	{
	}

void CNullConsole::SetTitle(const TDesC& /*aTitle*/)
	{
	}

void CNullConsole::ClearScreen()
	{
	}

void CNullConsole::ClearToEndOfLine()
	{
	}

TSize CNullConsole::ScreenSize() const
	{
	return TSize(10,10);
	}

TKeyCode CNullConsole::KeyCode() const
	{
	return EKeyNull;
	}

TUint CNullConsole::KeyModifiers() const
	{
	return 0;
	}

extern "C" EXPORT_C TAny *NewConsole()
	{
	return(new CNullConsole);
	}
