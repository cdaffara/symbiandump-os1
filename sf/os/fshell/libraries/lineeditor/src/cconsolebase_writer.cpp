// cconsolebase_writer.cpp
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

#include <e32base.h>
#include "cconsolebase_writer.h"
#include <e32cons.h>

//______________________________________________________________________________
//						TConsoleBaseAdaptor
EXPORT_C TConsoleBaseAdaptor::TConsoleBaseAdaptor(CConsoleBase& aConsole)
	: iConsole(aConsole)
	{
	}

TInt TConsoleBaseAdaptor::GetCursorPos(TPoint& aPos) const
	{
	aPos = iConsole.CursorPos();
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::SetCursorPosAbs(const TPoint& aPos)
	{
	iConsole.SetCursorPosAbs(aPos);
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::SetCursorPosRel(const TPoint& aPos)
	{
	iConsole.SetCursorPosRel(aPos);
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::SetCursorHeight(TInt aPercentage)
	{
	iConsole.SetCursorHeight(aPercentage);
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::SetTitle(const TDesC& aTitle)
	{
	iConsole.SetTitle(aTitle);
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::ClearScreen()
	{
	iConsole.ClearScreen();
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::ClearToEndOfLine()
	{
	iConsole.ClearToEndOfLine();
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::GetScreenSize(TSize& aSize) const
	{
	aSize = iConsole.ScreenSize();
	return KErrNone;
	}

TInt TConsoleBaseAdaptor::Write(const TDesC& aDes)
	{
	iConsole.Write(aDes);
	return KErrNone;
	}
