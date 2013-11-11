// pipe.h
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

#ifndef __PIPE_H__
#define __PIPE_H__

#include <e32base.h>
#include "endpoint.h"
#include "readwrite.h"


class CIoPipe : public CIoEndPoint
	{
public:
	static CIoPipe* NewLC();
	~CIoPipe();
public: // From CIoObject.
	virtual TBool IsType(RIoHandle::TType aType) const;
public: // From MIoReadEndPoint.
	virtual void IorepReadL(MIoReader& aReader);
public: // From MIoWriteEndPoint.
	virtual void IowepWriteL(MIoWriter& aWriter);
	virtual void IowepWriteCancel(MIoWriter& aWriter);
private: // From CIoEndPoint.
	virtual void HandleReaderDetached(MIoReader& aReader);
	virtual void ForegroundReaderChanged();
	virtual void HandleWriterDetached(MIoWriter& aWriter);
private:
	CIoPipe();
	void DoCopy();
	void CheckReady();
private:
	RPointerArray<MIoWriter> iPendingWriters;
	};

#endif // __PIPE_H__

