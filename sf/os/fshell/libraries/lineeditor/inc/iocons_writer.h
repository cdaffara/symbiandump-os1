// iocons_writer.h
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


#ifndef __IOCONS_WRITER_H__
#define __IOCONS_WRITER_H__

#include <fshell/abstract_console_writer.h>
class RIoConsoleWriteHandle;

class TIoConsWriterAdaptor : public MAbstractConsoleWriter
	{
public:
	IMPORT_C TIoConsWriterAdaptor(RIoConsoleWriteHandle& aWriteHandle);

	virtual TInt GetCursorPos(TPoint& aPos) const;
	virtual TInt SetCursorPosAbs(const TPoint& aPos);
	virtual TInt SetCursorPosRel(const TPoint& aPos);
	virtual TInt SetCursorHeight(TInt aPercentage);
	virtual TInt SetTitle(const TDesC& aTitle);
	virtual TInt ClearScreen();
	virtual TInt ClearToEndOfLine();
	virtual TInt GetScreenSize(TSize& aSize) const;
	virtual TInt Write(const TDesC& aDes);
private:
	RIoConsoleWriteHandle& iWriteHandle;
	};

#endif //__IOCONS_WRITER_H__
