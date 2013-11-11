// abstract_console_writer.h
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

#ifndef __ABSTRACT_CONSOLE_WRITER_H__
#define __ABSTRACT_CONSOLE_WRITER_H__

class MAbstractConsoleWriter
	{
public:
	virtual TInt GetCursorPos(TPoint& aPos) const = 0;
	virtual TInt SetCursorPosAbs(const TPoint& aPos) = 0;
	virtual TInt SetCursorPosRel(const TPoint& aPos) = 0;
	virtual TInt SetCursorHeight(TInt aPercentage) = 0;
	virtual TInt SetTitle(const TDesC& aTitle) = 0;
	virtual TInt ClearScreen() = 0;
	virtual TInt ClearToEndOfLine() = 0;
	virtual TInt GetScreenSize(TSize& aSize) const = 0;
	virtual TInt Write(const TDesC& aDes) = 0;
	};



#endif //__ABSTRACT_CONSOLE_WRITER_H__
