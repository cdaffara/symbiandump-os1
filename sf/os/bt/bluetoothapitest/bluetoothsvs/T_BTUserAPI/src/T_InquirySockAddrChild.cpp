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


#include "T_InquirySockAddrChild.h"

// Test TInquirySockAddr class

T_InquirySockAddrChild::T_InquirySockAddrChild()
	: TInquirySockAddr()
	{
	}


T_InquirySockAddrChild::T_InquirySockAddrChild(const TSockAddr	&aSockAddr)
	: TInquirySockAddr(aSockAddr)
	{
	}
	
	
T_InquirySockAddrChild&	T_InquirySockAddrChild::Cast(const TSockAddr	&aSockAddr)
	{
	TInquirySockAddr &tmp = TInquirySockAddr::Cast(aSockAddr);
	return *((T_InquirySockAddrChild *)&tmp);
	}
	
	
TUint8 T_InquirySockAddrChild::T_FormatTypeField()
	{
	return FormatTypeField();
	}
