// null.h
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

#include <e32base.h>
#include "endpoint.h"
#include "readwrite.h"


class CIoNull : public CIoEndPoint
	{
public:
	static CIoNull* NewLC();
	~CIoNull();
public: // From CIoObject.
	virtual TBool IsType(RIoHandle::TType aType) const;
public: // From MIoReadEndPoint.
	virtual void IorepReadL(MIoReader& aReader);
public: // From MIoWriteEndPoint.
	virtual void IowepWriteL(MIoWriter& aWriter);
	virtual void IowepWriteCancel(MIoWriter& aWriter);
private:
	CIoNull();
	};
