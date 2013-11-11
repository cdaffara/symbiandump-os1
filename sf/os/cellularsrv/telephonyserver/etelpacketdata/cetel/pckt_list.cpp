// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains implementation of MBMS API list classes.
// 
//

#include "pcktlist.h"
#include "pcktptr.h"

/********************************************************************/
//
// CPcktMbmsMonitoredServiceList
// A concrete instantiation of an ETel list - holding 
// RPacketService::TMbmsServceAvailabilityV1 objects
//
/********************************************************************/
EXPORT_C CPcktMbmsMonitoredServiceList* CPcktMbmsMonitoredServiceList::NewL()
/**
Creates a new instance of a CPcktMbmsMonitoredServiceList object.

@return A pointer to the new instance.
*/
	{
	CPcktMbmsMonitoredServiceList* r = new(ELeave) CPcktMbmsMonitoredServiceList();
	return r;
	}
	
CPcktMbmsMonitoredServiceList::CPcktMbmsMonitoredServiceList()
	: CMobilePhoneEditableList<RPacketService::TMbmsServiceAvailabilityV1>()
/**
Constructor.
*/
	{
	}

EXPORT_C CPcktMbmsMonitoredServiceList::~CPcktMbmsMonitoredServiceList()
/**
Destructor.
*/
	{
	}
