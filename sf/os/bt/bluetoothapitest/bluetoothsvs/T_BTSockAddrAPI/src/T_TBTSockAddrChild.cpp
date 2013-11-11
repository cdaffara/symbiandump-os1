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


#include "T_TBTSockAddrChild.h"

// Test socket Address class
T_TBTSockAddrChild::T_TBTSockAddrChild()
	: TBTSockAddr()
	{	
	}
	
T_TBTSockAddrChild::T_TBTSockAddrChild(const TSockAddr &aSockAddr)
	: TBTSockAddr(aSockAddr)
	{
			
	}

T_TBTSockAddrChild& T_TBTSockAddrChild::Cast(const TSockAddr &aAddr)
	{
	TBTSockAddr &tmp = TBTSockAddr::Cast(aAddr);
	return *((T_TBTSockAddrChild *)&tmp);
	}
		
void T_TBTSockAddrChild::T_SetUserLen(TInt aLen)
	{
	SetUserLen(aLen);	
	}

TUint8* T_TBTSockAddrChild::T_UserPtr()
	{
	return UserPtr();
	}

TAny * T_TBTSockAddrChild::T_EndBTSockAddrPtr()
	{
	return 	EndBTSockAddrPtr();
	}

	
// EOF
