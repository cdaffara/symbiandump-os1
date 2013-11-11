// reader.h
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

#ifndef __READER_H__
#define __READER_H__

#include <e32std.h>
#include "socketuser.h"


class MReaderObserver
	{
public:
	virtual void ReadComplete(const TDesC8& aData) = 0;
	virtual void ReadError(TInt aError) = 0;
	};


class CReader : public CSocketUser
	{
public:
	enum TReadMode
		{
		EFull,
		EOneOrMore
		};
public:
	static CReader* NewL(RSocket& aSocket, MReaderObserver& aObserver);
	~CReader();
	void StartRead(TInt aLength, TReadMode aMode);
	void GetBufferLength(TInt& aLength);
	void Abort();
private:
	CReader(RSocket& aSocket, MReaderObserver& aObserver);
	void ConstructL();
private: // From CActive.
	void RunL();
	void DoCancel();
private: // From CSocketUser.
	virtual void HandleSocketConnected();
private:
	MReaderObserver& iObserver;
	TSockXfrLength iXfrLength;
	TInt iPendingLength;
	TReadMode iPendingMode;
	RBuf8 iBuffer;
	};

#endif // __READER_H__

