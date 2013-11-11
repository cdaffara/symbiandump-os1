// object.h
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

#include <e32base.h>
#include "iocli.h"

class CIoSession;


class CIoObject : public CObject
	{
public:
	virtual TBool IsType(RIoHandle::TType aType) const = 0;
	virtual void SessionClosed(const CIoSession& aSession);
	virtual void ClosedBy(const CIoSession& aSession);
	};
