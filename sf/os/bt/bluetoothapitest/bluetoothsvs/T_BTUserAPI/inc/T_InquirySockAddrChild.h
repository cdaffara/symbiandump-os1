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


#if (!defined __T_INQUIRY_SOCKADDR_CHILD_H__)
#define __T_INQUIRY_SOCKADDR_CHILD_H__
	
// EPOC includes
#include <bt_sock.h>
#include <es_sock.h>

class T_InquirySockAddrChild : public TInquirySockAddr
	{
public:

	T_InquirySockAddrChild();
	T_InquirySockAddrChild(const TSockAddr &aSockAddr);
	static T_InquirySockAddrChild& Cast(const TSockAddr &aSockAddr);
	TUint8	T_FormatTypeField();
	};

#endif /* __T_INQUIRY_SOCKADDR_CHILD_H__ */
