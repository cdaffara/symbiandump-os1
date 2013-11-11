// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// spud context element
// 
//

/**
 @file 
 @internalComponent
*/

#include "cspudcontextelem.h"

/**
@param aId - pdp context id
@param aFactory - reference to etel driver factory
@param aPdpFsmInterface reference to pdp fsm interface
@return new spud context element
*/
CSpudContextElem* CSpudContextElem::NewL(TContextId aId, 
									CEtelDriverFactory& aFactory, 
									CPdpFsmInterface& aPdpFsmInterface)
	{
	CSpudContextElem *me = new(ELeave) CSpudContextElem();
	CleanupStack::PushL(me);
	me->ConstructL(aId, aFactory, aPdpFsmInterface);
	CleanupStack::Pop(me);
	return me;
	}

/** 
@param aId - pdp context id
@param aFactory - reference to etel driver factory
@param aPdpFsmInterface reference to pdp fsm interface
*/
void CSpudContextElem::ConstructL(TContextId aId, 
								CEtelDriverFactory& aFactory, 
								CPdpFsmInterface& aPdpFsmInterface)
	{
	iContext = CEtelDriverContext::NewL(aId, aId == KPrimaryContextId?SpudMan::EPrimary:SpudMan::ESecondary, aFactory);
	iPdpStatusChangeNotifier = new(ELeave) CPdpStatusChangeNotifier(aId, 
													iContext->PacketContext(), 
													aPdpFsmInterface);
	iQoSChangeNotifier = new(ELeave) CQoSChangeNotifier(aId, 
													iContext->PacketQoS(), 
													aPdpFsmInterface);
	iPdpConfigChangeNotifier = CPdpConfigChangeNotifier::NewL(aId, 
													iContext->PacketContext(), 
													aPdpFsmInterface);
	}

CSpudContextElem::~CSpudContextElem()
	{
	delete iPdpStatusChangeNotifier;
	delete iQoSChangeNotifier;
	delete iPdpConfigChangeNotifier;
	delete iContext;
	}

/** starts all notifiers */
void CSpudContextElem::Start()
	{
	iPdpStatusChangeNotifier->Start();
	iQoSChangeNotifier->Start();
	iPdpConfigChangeNotifier->Start();
	}
	
/** cancels all notifiers */
void CSpudContextElem::Cancel()
	{
	iPdpStatusChangeNotifier->Cancel();
	iQoSChangeNotifier->Cancel();
	iPdpConfigChangeNotifier->Cancel();
	}
	
	
	/**
@param aId - pdp context id
@param aFactory - reference to etel driver factory
@param aPdpFsmInterface reference to pdp fsm interface
@return new spud context element
*/
CMbmsContextElem* CMbmsContextElem::NewL(TContextId aId, 
									CEtelDriverFactory& aFactory, 
									CPdpFsmInterface& aPdpFsmInterface)
	{
	CMbmsContextElem *me = new(ELeave) CMbmsContextElem();
	CleanupStack::PushL(me);
	me->ConstructL(aId, aFactory, aPdpFsmInterface);
	CleanupStack::Pop(me);
	return me;
	}

/** 
@param aId - pdp context id
@param aFactory - reference to etel driver factory
@param aPdpFsmInterface reference to pdp fsm interface
*/
void CMbmsContextElem::ConstructL(TContextId aId, 
								CEtelDriverFactory& aFactory, 
								CPdpFsmInterface& aPdpFsmInterface)
	{
	iContext = CEtelDriverContext::NewL(aId, SpudMan::EMbms, aFactory);
	iMbmsPdpStatusChangeNotifier = new(ELeave) CMbmsPdpStatusChangeNotifier(aId, 
													iContext->MbmsPacketContext(), 
													aPdpFsmInterface);
	
	iMbmsPdpConfigChangeNotifier = new(ELeave) CMbmsPdpConfigChangeNotifier(aId, 
													iContext->MbmsPacketContext(), 
													aPdpFsmInterface);
	}

CMbmsContextElem::~CMbmsContextElem()
	{
	delete iMbmsPdpStatusChangeNotifier;
	delete iMbmsPdpConfigChangeNotifier;
	delete iContext;
	}

/** starts all notifiers */
void CMbmsContextElem::Start()
	{
	iMbmsPdpStatusChangeNotifier->Start();
	iMbmsPdpConfigChangeNotifier->Start();
	}
	
/** cancels all notifiers */
void CMbmsContextElem::Cancel()
	{
	iMbmsPdpStatusChangeNotifier->Cancel();
	iMbmsPdpConfigChangeNotifier->Cancel();
	}
	
	
	
	
