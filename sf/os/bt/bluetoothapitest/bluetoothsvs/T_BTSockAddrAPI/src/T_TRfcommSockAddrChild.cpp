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


#include "T_TRfcommSockAddrChild.h"

// Test socket Address class

T_TRfcommSockAddrChild::T_TRfcommSockAddrChild()
	: TRfcommSockAddr()
	{	
	}
	
T_TRfcommSockAddrChild::T_TRfcommSockAddrChild(const TSockAddr &aSockAddr)
	: TRfcommSockAddr(aSockAddr)
	{
			
	}

T_TRfcommSockAddrChild& T_TRfcommSockAddrChild::Cast(const TSockAddr &aAddr)
	{
	TRfcommSockAddr &tmp = TRfcommSockAddr::Cast(aAddr);
	return *((T_TRfcommSockAddrChild *)&tmp);
	}	
	
void T_TRfcommSockAddrChild::T_SetUserLen(TInt aLen)
	{
	SetUserLen(aLen);	
	}

TUint8* T_TRfcommSockAddrChild::T_UserPtr()
	{
	return UserPtr();
	}

TAny* T_TRfcommSockAddrChild::T_EndBTSockAddrPtr()
	{
	return 	EndBTSockAddrPtr();
	}
