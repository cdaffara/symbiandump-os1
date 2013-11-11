// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @publishedPartner
*/

#ifndef BTBUSDEVCOMM_H
#define BTBUSDEVCOMM_H

#include <d32comm.h>

class RBtBusDevComm : public RBusDevComm
	{
	public:
		inline TInt Open(TInt aUnit); // override
	};

/**
This is an override of the standard (non-virtual) RBusDevComm::Open(TInt).  Its
purpose is to open with a thread-relative handle instead
of a process-relative one.  This will allow the stack to recover from
a Panic, as the handle will be cleared up now when the thread dies.
*/
inline TInt RBtBusDevComm::Open(TInt aUnit)
	{return(DoCreate(KRBusDevComm,VersionRequired(),aUnit,NULL,NULL,EOwnerThread));}

#endif //BTBUSDEVCOMM_H
