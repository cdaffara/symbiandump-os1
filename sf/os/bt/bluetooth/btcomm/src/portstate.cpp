// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Together, these classes and the CBTPortProxy implement the 
// State pattern.  The states themselves are implemented using 
// the Flyweight pattern.  Each state is a Flyweight object
// and CBTPortStateFactory is manager of these objects.  As a 
// result of being a flyweight, no state object may have a state 
// that can't be shared between all possible users of the state.
// 
//

#include <bluetooth/logger.h>
#include <cs_port.h>
#include "btcomm.h"
#include "btstate.h"
#ifdef DEBUG
#include "btcommutil.h"
#endif

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_COMM);
#endif

CBTPortStateFactory::CBTPortStateFactory()
/**
	CBTPortStateFactory constructor nulls all the iState ptrs in here.
**/
	{
	LOG_FUNC
	iStates.Reset();
	}

CBTPortStateFactory::~CBTPortStateFactory()
/**
	CBTPortStateFactory destructor deletes all states.
**/
	{
	LOG_FUNC
	iStates.DeleteAll();
	}

CBTPortStateFactory* CBTPortStateFactory::NewL()
/**
	This creates the static CSY state objects.
	Declared private so only a friend (CBTPortFactory)
	can instantiate one of these.
**/
	{
	LOG_STATIC_FUNC
	CBTPortStateFactory *psf;
	psf=new (ELeave) CBTPortStateFactory();
	CleanupStack::PushL(psf);
	psf->InitL();
	CleanupStack::Pop();
	return psf;
	}

void CBTPortStateFactory::InitL()
/**
	This method creates the static instances of the CSY state objects.
**/
	{
	LOG_FUNC
	iStates[EIdle]						=new (ELeave) TBTPortStateIdle(this);
	iStates[ELoadingProtocol]			=new (ELeave) TBTPortStateLoadingProtocol(this);
	iStates[EDiscovering]				=new (ELeave) TBTPortStateDiscovering(this);
	iStates[ESDPConnected]				=new (ELeave) TBTPortStateSDPConnected(this);
	iStates[ESDPServiceQuery]			=new (ELeave) TBTPortStateSDPServiceQuery(this);
	iStates[ESDPAttribListRetrieved]	=new (ELeave) TBTPortStateSDPAttributeListRetrieved(this);
	iStates[EConnecting]				=new (ELeave) TBTPortStateConnecting(this);
	iStates[EOpen]						=new (ELeave) TBTPortStateOpen(this);
	iStates[EClosing]					=new (ELeave) TBTPortStateClosing(this);
	iStates[EError]						=new (ELeave) TBTPortErrorState(this);
	iStates[ESDPServiceIDListRetrieved] =new (ELeave) TBTPortStateServiceIDListRetrieved(this);
	}

TBTPortState& CBTPortStateFactory::GetState(const TCSYState aState)
/**
	Returns ptr to the state object corresponding to the TCSYState passed in.
**/
	{
	LOG_FUNC
	__ASSERT_DEBUG(aState != EMaxCSYStates, BTCommUtil::Panic(EBTCommPortStateOutOfBounds));
	return *iStates[aState];
	}

TInt CBTPortStateFactory::StateIndex(const TBTPortState* aState) const
	{
	LOG_FUNC
	TInt state;
	for (state = 0; state < EMaxCSYStates; state++)
		{
		if (iStates[state] == aState)
			{
			return state;
			}
		}
	
	return KUnknownState;
	}

