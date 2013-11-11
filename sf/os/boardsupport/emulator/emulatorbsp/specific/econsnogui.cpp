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
//

#include <e32std.h>
#include <e32cons.h>
#include <e32keys.h>

class CNoGuiConsole : public CConsoleBase
	{
public:
	CNoGuiConsole();
	virtual ~CNoGuiConsole();
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

CNoGuiConsole::CNoGuiConsole()
	{
	}

CNoGuiConsole::~CNoGuiConsole()
	{
	}

TInt CNoGuiConsole::Create(const TDesC& /*aTitle*/, TSize /*aSize*/)
	{
	return(KErrNone);
	}

void CNoGuiConsole::Read(TRequestStatus &aStatus)
	{
	TRequestStatus *pS=(&aStatus);
	User::RequestComplete(pS, KErrNotSupported);
	}

void CNoGuiConsole::ReadCancel()
	{
	}

void CNoGuiConsole::Write(const TDesC& /*aDes*/)
	{
	}

TPoint CNoGuiConsole::CursorPos() const
	{
	return TPoint(0,0);
	}

void CNoGuiConsole::SetCursorPosAbs(const TPoint& /*aPoint*/)
	{
	}

void CNoGuiConsole::SetCursorPosRel(const TPoint& /*aPoint*/)
	{
	}

void CNoGuiConsole::SetCursorHeight(TInt /*aPercentage*/)
	{
	}

void CNoGuiConsole::SetTitle(const TDesC& /*aTitle*/)
	{
	}

void CNoGuiConsole::ClearScreen()
	{
	}

void CNoGuiConsole::ClearToEndOfLine()
	{
	}

TSize CNoGuiConsole::ScreenSize() const
	{
	return TSize(10,10);
	}

TKeyCode CNoGuiConsole::KeyCode() const
	{
	return EKeyNull;
	}

TUint CNoGuiConsole::KeyModifiers() const
	{
	return 0;
	}

extern "C" EXPORT_C TAny *NewConsole()
	{
	return(new CNoGuiConsole);
	}
