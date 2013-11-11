// cconsolebase_writer.h
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


#ifndef __CCONSOLEBASE_WRITER_H__
#define __CCONSOLEBASE_WRITER_H__

#include <fshell/abstract_console_writer.h>
class CConsoleBase;

NONSHARABLE_CLASS(TConsoleBaseAdaptor) : public MAbstractConsoleWriter
	{
public:
	IMPORT_C TConsoleBaseAdaptor(CConsoleBase& aConsole);

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
	CConsoleBase& iConsole;
	};

#endif //__CCONSOLEBASE_WRITER_H__
