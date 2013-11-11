// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// The factory class which is used to instantiate the MbufGobbler flow.
//  (data plane)
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//
/**
 @file
 @internalComponent 
*/

#include "mbufgobblerflowfactory.h"
#include "mbufgobblerflow.h"
#include "mbufgobblerlog.h"

// =====================================================================================
//
// MbufGobbler Flow Factory
//

CMbufGobblerFlowFactory* CMbufGobblerFlowFactory::NewL(TAny* aConstructionParameters)
/**
Constructs a Default SubConnection Flow Factory

@param aConstructionParameters construction data passed by ECOM

@returns pointer to a constructed factory
*/
	{
	CMbufGobblerFlowFactory* ptr = new (ELeave) CMbufGobblerFlowFactory(TUid::Uid(CMbufGobblerFlowFactory::EUid), *(reinterpret_cast<ESock::CSubConnectionFlowFactoryContainer*>(aConstructionParameters)));
	
	return ptr;
	}


CMbufGobblerFlowFactory::CMbufGobblerFlowFactory(TUid aFactoryId, ESock::CSubConnectionFlowFactoryContainer& aParentContainer)
	: CSubConnectionFlowFactoryBase(aFactoryId, aParentContainer)
/**
Default SubConnection Flow Factory Constructor

@param aFactoryId ECOM Implementation Id
@param aParentContainer Object Owner
*/
	{
	}


ESock::CSubConnectionFlowBase* CMbufGobblerFlowFactory::DoCreateFlowL(ESock::CProtocolIntfBase* aProtocol, ESock::TFactoryQueryBase& aQuery)
	{
	const ESock::TDefaultFlowFactoryQuery& query = static_cast<const ESock::TDefaultFlowFactoryQuery&>(aQuery);
	ESock::CSubConnectionFlowBase *temp = CMbufGobblerFlow::NewL(*this, query.iSCprId, aProtocol);
	return temp;
	}

