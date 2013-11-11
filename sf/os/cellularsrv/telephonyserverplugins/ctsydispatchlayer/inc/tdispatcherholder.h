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

/**
 @file
 @internalAll 
*/

#ifndef __TDISPATCHERHOLDER_H_
#define __TDISPATCHERHOLDER_H_

// Forward declarations

class CCallControlDispatcher;
class CPhoneDispatcher;
class CSecurityDispatcher;
class CPhonebookEnDispatcher;
class CCellBroadcastDispatcher;
class CPhonebookOnDispatcher;
class CPhonebookDispatcher;
class CSimDispatcher;
class CSmsDispatcher;
class CCallControlMultipartyDispatcher;
class CSupplementaryServicesDispatcher;
class CPacketServicesDispatcher;
class CSatDispatcher;

/**
 * Holds references to dispatcher objects.
 */
class TDispatcherHolder
	{
public:
	
	TDispatcherHolder(
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
						CSatDispatcher& aCtsyDispatcherSatDispatcher
					);

	CCallControlDispatcher& GetCallControlDispatcher() { return iCtsyDispatcherCallControlDispatcher; }
	CPhoneDispatcher& GetPhoneDispatcher() { return iCtsyDispatcherPhoneDispatcher; }
	CSecurityDispatcher& GetSecurityDispatcher() { return iCtsyDispatcherSecurityDispatcher; }
	CPhonebookEnDispatcher& GetPhonebookEnDispatcher() { return iCtsyDispatcherPhonebookEnDispatcher; }
	CCellBroadcastDispatcher& GetCellBroadcastDispatcher() { return iCtsyDispatcherCellBroadcastDispatcher; }
	CPhonebookOnDispatcher& GetPhonebookOnDispatcher() { return iCtsyDispatcherPhonebookOnDispatcher; }
	CPhonebookDispatcher& GetPhonebookDispatcher() { return iCtsyDispatcherPhonebookDispatcher; }
	CSimDispatcher& GetSimDispatcher() { return iCtsyDispatcherSimDispatcher; }
	CSmsDispatcher& GetSmsDispatcher() { return iCtsyDispatcherSmsDispatcher; }
	CCallControlMultipartyDispatcher& GetCallControlMultipartyDispatcher() { return iCtsyDispatcherCallControlMultipartyDispatcher; }
	CSupplementaryServicesDispatcher& GetSupplementaryServicesDispatcher() { return iCtsyDispatcherSupplementaryServicesDispatcher; }
	CPacketServicesDispatcher& GetPacketServicesDispatcher() { return iCtsyDispatcherPacketServicesDispatcher; }
	CSatDispatcher& GetSatDispatcher() { return iCtsyDispatcherSatDispatcher; }


private:

	CCallControlDispatcher& iCtsyDispatcherCallControlDispatcher;
	CPhoneDispatcher& iCtsyDispatcherPhoneDispatcher;
	CSecurityDispatcher& iCtsyDispatcherSecurityDispatcher;
	CPhonebookEnDispatcher& iCtsyDispatcherPhonebookEnDispatcher;
	CCellBroadcastDispatcher& iCtsyDispatcherCellBroadcastDispatcher;
	CPhonebookOnDispatcher& iCtsyDispatcherPhonebookOnDispatcher;
	CPhonebookDispatcher& iCtsyDispatcherPhonebookDispatcher;
	CSimDispatcher& iCtsyDispatcherSimDispatcher;
	CSmsDispatcher& iCtsyDispatcherSmsDispatcher;
	CCallControlMultipartyDispatcher& iCtsyDispatcherCallControlMultipartyDispatcher;
	CSupplementaryServicesDispatcher& iCtsyDispatcherSupplementaryServicesDispatcher;
	CPacketServicesDispatcher& iCtsyDispatcherPacketServicesDispatcher;
	CSatDispatcher& iCtsyDispatcherSatDispatcher;


	}; // class TDispatcherHolder

#endif // __TDISPATCHERHOLDER_H_
