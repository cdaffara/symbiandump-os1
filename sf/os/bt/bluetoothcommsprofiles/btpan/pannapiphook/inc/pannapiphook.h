// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef PANNAPIPHOOK_H
#define PANNAPIPHOOK_H

#include <posthook.h>

static const TUint KProtocolPanNapIpHook = 1001;

//
// CPanNapIpHook
//
NONSHARABLE_CLASS(CPanNapIpHook) : public CProtocolPosthook 
	{
public:
	static CPanNapIpHook* NewL();
	~CPanNapIpHook();

	TInt ApplyL(RMBufHookPacket &aPacket, RMBufRecvInfo &aInfo);
    void NetworkAttachedL();

	void Identify(TServerProtocolDesc* aProtocolDesc) const;
	static void Describe(TServerProtocolDesc& aEntry);

private:
	CPanNapIpHook() {}

private:
	MInterfaceManager* iManager;
	};

//
// CPanNapIpHookFamily
//
NONSHARABLE_CLASS(CPanNapIpHookFamily) : public CProtocolFamilyBase
	{
public:
	CPanNapIpHookFamily();
	~CPanNapIpHookFamily();
	TInt Install();
	TInt Remove();
	TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
	CProtocolBase* NewProtocolL(TUint /*aSockType*/, TUint aProtocol);
	};

#endif // PANNAPIPHOOK_H
