// writer.h
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

#ifndef __WRITER_H__
#define __WRITER_H__

#include <e32std.h>
#include "socketuser.h"


class MWriterObserver
	{
public:
	virtual void WriteComplete(TInt aError) = 0;
	};


class CWriter : public CSocketUser
	{
public:
	static CWriter* NewL(RSocket& aSocket, MWriterObserver& aObserver);
	~CWriter();
	void StartWrite(const TDesC8& aDes);
	void Abort();
private:
	CWriter(RSocket& aSocket, MWriterObserver& aObserver);
private: // From CActive.
	void RunL();
	void DoCancel();
private: // From CSocketUser.
	virtual void HandleSocketConnected();
private:
	MWriterObserver& iObserver;
	const TDesC8* iPendingDes;
	};

#endif // __WRITER_H__

