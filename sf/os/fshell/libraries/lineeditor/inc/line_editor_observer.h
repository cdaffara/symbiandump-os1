// line_editor_observer.h
// 
// Copyright (c) 2009 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __LINE_EDITOR_OBSERVER_H__
#define __LINE_EDITOR_OBSERVER_H__

#include <e32std.h>


class CLineEditor;
class TConsoleLine;


//
// MLineEditorObserver.
//

class MLineEditorObserver
	{
public:
	virtual void LeoHandleLine(const TDesC& aLine) = 0;
	};


//
// MLineCompleter.
//

class MLineCompleter
	{
public:
	virtual void LcCompleteLineL(TConsoleLine& aLine, const TChar& aEscapeChar) = 0;
	};


#endif // __LINE_EDITOR_OBSERVER_H__
