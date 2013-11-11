// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

NONSHARABLE_CLASS(CWsConsole) : public CConsoleBase
	{
public:
// Pure virtuals from CConsoleBase
	TInt Create(const TDesC& aTitle,TSize aSize);
	void Read(TRequestStatus& aStatus);
	void ReadCancel();
	void Write(const TDesC& aDes);
	TPoint CursorPos() const;
	void SetCursorPosAbs(const TPoint& aPoint);
	void SetCursorPosRel(const TPoint& aPoint);
	void SetCursorHeight(TInt aPercentage);
	void SetTitle(const TDesC& aTitle);
	void ClearScreen();
	void ClearToEndOfLine();
	TSize ScreenSize() const;
	TKeyCode KeyCode() const;
	TUint KeyModifiers() const;
private:
	TRequestStatus* iStatus;
	};

//
// CWsConsole
//

TInt CWsConsole::Create(const TDesC&,TSize )
	{
	return(KErrNone);
	}

void CWsConsole::Read(TRequestStatus& aStatus)
	{
	aStatus=KRequestPending;
	iStatus=&aStatus;
	User::RequestComplete(iStatus,KErrNone);
	}

void CWsConsole::ReadCancel()
	{
	User::RequestComplete(iStatus,KErrCancel);
	}

void CWsConsole::Write(const TDesC&)
	{
	}

TPoint CWsConsole::CursorPos() const
	{
	return(TPoint());
	}

void CWsConsole::SetCursorPosAbs(const TPoint&)
	{
	}

void CWsConsole::SetCursorPosRel(const TPoint&)
	{
	}

void CWsConsole::SetCursorHeight(TInt )
	{
	}

void CWsConsole::SetTitle(const TDesC&)
	{
	}

void CWsConsole::ClearScreen()
	{
	}

void CWsConsole::ClearToEndOfLine()
	{
	}

TSize CWsConsole::ScreenSize() const
	{
	return(TSize(32,20));
	}

TKeyCode CWsConsole::KeyCode() const
	{
	return(EKeyNull);
	}

TUint CWsConsole::KeyModifiers() const
	{
	return(0);
	}

EXPORT_C TAny* NewConsole()
	{
	return new CWsConsole;
	}
