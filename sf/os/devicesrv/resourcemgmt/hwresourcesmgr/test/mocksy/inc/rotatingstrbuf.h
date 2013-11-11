// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef rotatingstrbuf_h
#define rotatingstrbuf_h

#include <e32base.h>

class CRotatingStrBuf : public CBase
	{
public:
	static CRotatingStrBuf* NewL(TInt aSize);
	virtual ~CRotatingStrBuf();
public:
	void Put(const TDesC& aText);
	HBufC* Get();

private:
	CRotatingStrBuf();
	void ConstructL(TInt aSize);

private:
	class TEntry
		{
	public:
		HBufC* 	iText;
		TDblQueLink iLink;
		};
		
	TDblQue<TEntry> iQueue;
	};
	
#endif
