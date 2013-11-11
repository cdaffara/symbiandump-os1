// endpoint.h
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

#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

#include <e32base.h>
#include "readwrite.h"


class CIoEndPoint : public CIoObject, public MIoReadEndPoint, public MIoWriteEndPoint
	{
public:
	~CIoEndPoint();
public: // From MIoEndPoint:
	virtual TName IoepName() const;
	virtual TBool IoepIsType(RIoHandle::TType aType) const;
public: // From MIoReadEndPoint.
	virtual void IorepAttachL(MIoReader& aReader, RIoEndPoint::TReadMode aMode);
	virtual void IorepDetach(MIoReader& aReader);
	virtual void IorepSetForegroundReaderL(MIoReader& aReader);
	virtual TBool IorepIsForegroundL(const MIoReader& aReader) const;
private: // From MIoWriteEndPoint.
	virtual void IowepAttachL(MIoWriter& aWriter);
	virtual void IowepDetach(MIoWriter& aWriter);
private: // New.
	virtual TInt HandleReaderAttached(MIoReader& aReader);
	virtual void HandleReaderDetached(MIoReader& aReader);
	virtual void ForegroundReaderChanged();
	virtual TInt HandleWriterAttached(MIoWriter& aWriter);
	virtual void HandleWriterDetached(MIoWriter& aWriter);
protected:
	virtual void HandleForegroundReaderChanged();
protected:
	CIoEndPoint();
	TInt NumAttachedReaders() const;
	MIoReader* AttachedReader(TInt aIndex = 0);
	TInt NumAttachedWriters() const;
private:
	TInt FindReader(MIoReader& aReader) const;
	TInt FindReader(const MIoReader& aReader) const;
private:
	TInt iAttachedWriterCount;
	RPointerArray<MIoReader> iAttachedReaders;
	};

#endif // __ENDPOINT_H__

