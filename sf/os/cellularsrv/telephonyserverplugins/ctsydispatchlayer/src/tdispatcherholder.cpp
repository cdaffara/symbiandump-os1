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
//

#include "tdispatcherholder.h"

TDispatcherHolder::TDispatcherHolder(
				CCallControlDispatcher& aCtsyDispatcherCallControlDispatcher, 
				CPhoneDispatcher& aCtsyDispatcherPhoneDispatcher, 
				CSecurityDispatcher& aCtsyDispatcherSecurityDispatcher, 
				CPhonebookEnDispatcher& aCtsyDispatcherPhonebookEnDispatcher, 
				CCellBroadcastDispatcher& aCtsyDispatcherCellBroadcastDispatcher, 
				CPhonebookOnDispatcher& aCtsyDispatcherPhonebookOnDispatcher, 
				CPhonebookDispatcher& aCtsyDispatcherPhonebookDispatcher, 
				CSimDispatcher& aCtsyDispatcherSimDispatcher, 
				CSmsDispatcher& aCtsyDispatcherSmsDispatcher, 
				CCallControlMultipartyDispatcher& aCtsyDispatcherCallControlMultipartyDispatcher, 
				CSupplementaryServicesDispatcher& aCtsyDispatcherSupplementaryServicesDispatcher, 
				CPacketServicesDispatcher& aCtsyDispatcherPacketServicesDispatcher, 
				CSatDispatcher& aCtsyDispatcherSatDispatcher)
	: 	iCtsyDispatcherCallControlDispatcher(aCtsyDispatcherCallControlDispatcher), 
		iCtsyDispatcherPhoneDispatcher(aCtsyDispatcherPhoneDispatcher), 
		iCtsyDispatcherSecurityDispatcher(aCtsyDispatcherSecurityDispatcher), 
		iCtsyDispatcherPhonebookEnDispatcher(aCtsyDispatcherPhonebookEnDispatcher), 
		iCtsyDispatcherCellBroadcastDispatcher(aCtsyDispatcherCellBroadcastDispatcher), 
		iCtsyDispatcherPhonebookOnDispatcher(aCtsyDispatcherPhonebookOnDispatcher), 
		iCtsyDispatcherPhonebookDispatcher(aCtsyDispatcherPhonebookDispatcher), 
		iCtsyDispatcherSimDispatcher(aCtsyDispatcherSimDispatcher), 
		iCtsyDispatcherSmsDispatcher(aCtsyDispatcherSmsDispatcher), 
		iCtsyDispatcherCallControlMultipartyDispatcher(aCtsyDispatcherCallControlMultipartyDispatcher), 
		iCtsyDispatcherSupplementaryServicesDispatcher(aCtsyDispatcherSupplementaryServicesDispatcher), 
		iCtsyDispatcherPacketServicesDispatcher(aCtsyDispatcherPacketServicesDispatcher), 
		iCtsyDispatcherSatDispatcher(aCtsyDispatcherSatDispatcher)
	
	{
	}

