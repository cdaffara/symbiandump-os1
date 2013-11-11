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
// ETel  API
// Header file for  packet list classes
// 
//

/**
 @file
*/

#ifndef _PCKTLIST_H_
#define _PCKTLIST_H_

#include <etelpckt.h>
#include <mmlist.h>

class CPcktMbmsMonitoredServiceList : public CMobilePhoneEditableList<RPacketService::TMbmsServiceAvailabilityV1>
/**
An instantiation of the "read-write" list thin-template used 
to hold the Mbms Service availability list retrieved from the Network.

@publishedPartner
@released

@see CRetrievePcktMbmsMonitoredServices
*/
	{
public:
	IMPORT_C static CPcktMbmsMonitoredServiceList* NewL();
	IMPORT_C ~CPcktMbmsMonitoredServiceList();

protected:
	CPcktMbmsMonitoredServiceList();
	};

#endif
