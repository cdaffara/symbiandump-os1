// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "avctpOutboundQ.h" 

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP);
#endif

using namespace SymbianAvctp;

/** 
This function Inserts the packet into the Que in BT address
Make sure the sdu goes at the end of those 
order
*/
void TAvctpOutboundQ::Insert(HAvctpOutgoingSdu& aSdu)
	{
	LOG_FUNC

	TDblQueIter<HAvctpOutgoingSdu> iter(iQue);

	// Find end of the SDUs to the same remote device
	while(iter && (*iter).BTAddr() <= aSdu.BTAddr())
		{
		iter++;
		}
		
	if(iter)
		{
		aSdu.iQueLink.AddBefore(&(*iter).iQueLink);		
		}
	else
		{
		iQue.AddLast(aSdu);		
		}
	aSdu.AddToQueue(this);
	
	iCount++;
	}

