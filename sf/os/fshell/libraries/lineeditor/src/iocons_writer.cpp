// iocons_writer.cpp
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
#include "iocons_writer.h"
#include <fshell/iocli.h>

//______________________________________________________________________________
//						TIoConsWriterAdaptor
EXPORT_C TIoConsWriterAdaptor::TIoConsWriterAdaptor(RIoConsoleWriteHandle& aWriteHandle)
	: iWriteHandle(aWriteHandle)
	{
	}

TInt TIoConsWriterAdaptor::GetCursorPos(TPoint& aPos) const
	{
	return iWriteHandle.GetCursorPos(aPos);
	}


TInt TIoConsWriterAdaptor::SetCursorPosAbs(const TPoint& aPos)
	{
	return iWriteHandle.SetCursorPosAbs(aPos);
	}

TInt TIoConsWriterAdaptor::SetCursorPosRel(const TPoint& aPos)
	{
	return iWriteHandle.SetCursorPosRel(aPos);
	}

TInt TIoConsWriterAdaptor::SetCursorHeight(TInt aPercentage)
	{
	return iWriteHandle.SetCursorHeight(aPercentage);
	}

TInt TIoConsWriterAdaptor::SetTitle(const TDesC& aTitle)
	{
	return iWriteHandle.SetTitle(aTitle);
	}

TInt TIoConsWriterAdaptor::ClearScreen()
	{
	return iWriteHandle.ClearScreen();
	}

TInt TIoConsWriterAdaptor::ClearToEndOfLine()
	{
	return iWriteHandle.ClearToEndOfLine();
	}

TInt TIoConsWriterAdaptor::GetScreenSize(TSize& aSize) const
	{
	return iWriteHandle.GetScreenSize(aSize);
	}


TInt TIoConsWriterAdaptor::Write(const TDesC& aDes)
	{
	return iWriteHandle.Write(aDes);
	}

	
