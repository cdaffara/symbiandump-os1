// eventhandler.h
// 
// Copyright (c) 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//
#ifndef EXTRABTRACE_EVENTHANDLER_H
#define EXTRABTRACE_EVENTHANDLER_H

#include <kernel.h>

NONSHARABLE_CLASS(DExtraBtraceEventHandler) : public DKernelEventHandler
	{
public:
	DExtraBtraceEventHandler(DObject* aParent);
	~DExtraBtraceEventHandler();
	static TUint Event(TKernelEvent aEvent, TAny* a1, TAny* a2, TAny* aPrivateData);

private:
	DObject* iParent;
	};

#endif
