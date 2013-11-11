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

#ifndef CLTSYFACTORYV1_H
#define CLTSYFACTORYV1_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <e32def.h>
#include <ctsy/ltsy/ltsylogger.h>
#include <ctsy/ltsy/mltsydispatchfactory.h>
#include "ltsymacros.h"

// FORWARD DECLARATIONS
class CCtsyDispatcherCallback;
class CLtsyCallControlHandler;
class CLtsyPhoneHandler;
class CLtsySecurityHandler;
class CLtsyPhonebookEnHandler;
class CLtsyCellBroadcastHandler;
class CLtsyPhonebookOnHandler;
class CLtsyPhonebookHandler;
class CLtsySimHandler;
class CLtsySmsHandler;
class CLtsyCallControlMultipartyHandler;
class CLtsySupplementaryServicesHandler;
class CLtsyPacketServicesHandler;
class CLtsySatHandler;
class CReceiveThreadManager;
class CLtsyReceiveThreadEventStateHandler;

// Can be removed in real LTSY implementation
MTEST_FORWARD_DECLARE_MOCKLTSYENGINE

// CLASS DECLARATION

class CLtsyFactoryV1 : public CBase, public MLtsyDispatchFactoryV1  
	{
public:
	
	// Public constructor because this object is constructed in two phases where
	// the second phase ConstructL() is called explicitly by the creator.
	CLtsyFactoryV1(); 
	
	virtual ~CLtsyFactoryV1();

	// From MLtsyDispatchFactoryV1 (or its base class)
	virtual void Release();
	virtual TInt GetDispatchHandler(
			TLtsyDispatchInterfaceApiId aDispatchInterfaceId,
			TAny*& aDispatchInterface);
	virtual MLtsyFactoryBase::TCtsyInterfaceVersion Version();
	virtual void ConstructL(CCtsyDispatcherCallback& aCallback);
	virtual TBool IsDispatchInterfaceSupported(TInt aFuncUnitId, TLtsyDispatchInterfaceApiId aDispatchApiId);
	virtual void IsCallbackIndicatorSupported(TInt aFuncUnitId, TLtsyDispatchIndIdGroup aIdGroup, TUint32& aIndIdBitMask);
	virtual CTsySatMessagingBase* GetSatMessagingService(MCtsySatService& aSatMessagingService);
	
private:
	
	// To prevent explicit construction in this way
	void NewL() {};
	void NewLC() {};
	
private: // Owned

	// Dispatch Handlers
	CLtsyCallControlHandler* iLtsyDispatchCallControlHandler;
	CLtsyPhoneHandler* iLtsyDispatchPhoneHandler;
	CLtsySecurityHandler* iLtsyDispatchSecurityHandler;
	CLtsyPhonebookEnHandler* iLtsyDispatchPhonebookEnHandler;
	CLtsyCellBroadcastHandler* iLtsyDispatchCellBroadcastHandler;
	CLtsyPhonebookOnHandler* iLtsyDispatchPhonebookOnHandler;
	CLtsyPhonebookHandler* iLtsyDispatchPhonebookHandler;
	CLtsySimHandler* iLtsyDispatchSimHandler;
	CLtsySmsHandler* iLtsyDispatchSmsHandler;
	CLtsyCallControlMultipartyHandler* iLtsyDispatchCallControlMultipartyHandler;
	CLtsySupplementaryServicesHandler* iLtsyDispatchSupplementaryServicesHandler;
	CLtsyPacketServicesHandler* iLtsyDispatchPacketServicesHandler;
	CLtsySatHandler* iLtsyDispatchSatHandler;
	
	CReceiveThreadManager* iReceiveThreadManager;
	CLtsyReceiveThreadEventStateHandler* iReceiveThreadEventStateHandler;
	
	// Can be removed in real LTSY implementation.
	MTEST_DECLARE_MOCKLTSYENGINE
	//
	
	}; // class CLtsyFactoryV1

#endif // CLTSYFACTORYV1_H
