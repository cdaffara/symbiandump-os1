// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__PROTOCOLFAMILY_H__)
#define __PROTOCOLFAMILY_H__

#include "es_ptestinternalsocket.h"
#include <es_prot.h>
#include <comms-infras/nifif.h>
#include <dns_qry.h>

//---------------------------------------------------------------------------------------------------------

NONSHARABLE_CLASS(CTestIntSocketProtocolFamily): public CProtocolFamilyBase
	{
private:
	CTestIntSocketProtocolFamily();	
public:
	static CTestIntSocketProtocolFamily * NewL();
	void ConstructL();
	TInt Install();
	TInt Remove();
	CProtocolBase * NewProtocolL(TUint aSockType,TUint aProtocol);
	TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
private:
	void Construct();
private:
	TServerProtocolDesc iProtocolDescs[1];
	};

#endif //__PROTOCOLFAMILY_H__

