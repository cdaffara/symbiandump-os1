// wsp_protocolfamily.h
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

#ifndef __WSP_PROTOCOLFAMILY_H__
#define __WSP_PROTOCOLFAMILY_H__

#include <es_prot.h>
#include "wsp_factory.h"


class CWinSockProtocolFamily : public CProtocolFamilyBase
	{
public:
	~CWinSockProtocolFamily();	
	static CWinSockProtocolFamily* NewL();
	TInt Install();
	CProtocolBase* NewProtocolL(TUint aSockType,TUint aProtocol);
	TUint ProtocolList(TServerProtocolDesc*& aProtocolList);
private:
	CWinSockProtocolFamily();
	TBool RealTcpipActiveL() const;
private:
	RWin32Factory* iWin32Factory;
	};


#endif // __WSP_PROTOCOLFAMILY_H__
