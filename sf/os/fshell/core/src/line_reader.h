// line_reader.h
// 
// Copyright (c) 2006 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __LINE_READER_H__
#define __LINE_READER_H__


class MLineReaderObserver
	{
public:
	virtual void HandleLine(const TDesC& aLineData) = 0;
	virtual void HandleLineReadError(TInt aError) = 0;
	};


#endif // __LINE_READER_H__
