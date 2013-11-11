// wsp_active.h
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __WSP_ACTIVE_H__
#define __WSP_ACTIVE_H__

#include <e32base.h>
#include "wsp_def.h"

class CWin32Scheduler;

class CWin32ActiveObject : public CBase
	{
protected:
	CWin32ActiveObject(CWin32Scheduler& aScheduler);
	~CWin32ActiveObject();
	void ConstructL();
	void ReleaseEventHandle();
	virtual void Run() = 0;
protected:
	WIN32_HANDLE iEvent;
	CWin32Scheduler& iScheduler;
private:
	friend class CWin32Scheduler;
	TSglQueLink iLink;
	};


#endif // __WSP_ACTIVE_H__
