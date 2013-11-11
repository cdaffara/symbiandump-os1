/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#if (!defined __T_TRFCOMMSOCKADDR_CHILD_H__)
#define __T_TRFCOMMSOCKADDR_CHILD_H__


#include <es_sock.h> // TSockAddr address 
#include <bt_sock.h> // Bluetooth address classes


class T_TRfcommSockAddrChild : public TRfcommSockAddr
	{
public:
	T_TRfcommSockAddrChild();
	T_TRfcommSockAddrChild(const TSockAddr &aSockAddr);
	static T_TRfcommSockAddrChild& Cast(const TSockAddr &aAddr);
	void T_SetUserLen(TInt aLen);
	TUint8* T_UserPtr();
	TAny * T_EndBTSockAddrPtr();	
	};
	
	
#endif /* __T_TRFCOMMSOCKADDR_CHILD_H__ */
