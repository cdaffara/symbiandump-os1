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

#ifndef CSPUDCONTEXTELEM_H
#define CSPUDCONTEXTELEM_H

#include "ceteldrivercontext.h"
#include "cpdpstatuschangenotifier.h"
#include "cqoschangenotifier.h"
#include "cpdpconfigchangenotifier.h"

class CEtelDriverFactory;

/* base class for contexts*/
class CContextElemBase: public CBase
	{
	public:
	virtual void Start()=0;
	virtual void Cancel()=0;
	virtual void ConstructL(TContextId aId, CEtelDriverFactory& aFactory, CPdpFsmInterface& aPdpFsmInterface)=0;
	
	CEtelDriverContext*			iContext;
	};
	
/** structure to keep context object and associated notifiers */
class CSpudContextElem : public CContextElemBase
	{
	public:
		static CSpudContextElem* NewL(TContextId aId, CEtelDriverFactory& aFactory, CPdpFsmInterface& aPdpFsmInterface);
		virtual ~CSpudContextElem();
		void ConstructL(TContextId aId, CEtelDriverFactory& aFactory, CPdpFsmInterface& aPdpFsmInterface);
		void Start();
		void Cancel();
	private:	
		/** pdp status change notifier */
		CPdpStatusChangeNotifier*	iPdpStatusChangeNotifier;
		/** QoS profile change notifier */
		CQoSChangeNotifier*			iQoSChangeNotifier;
		/** pdp config  change notifier */
		CPdpConfigChangeNotifier*	iPdpConfigChangeNotifier;
	};

/** structure to keep mbms context object and associated notifiers */	
class CMbmsContextElem : public CContextElemBase
	{
	public:
		static CMbmsContextElem* NewL(TContextId aId, CEtelDriverFactory& aFactory, CPdpFsmInterface& aPdpFsmInterface);
		virtual ~CMbmsContextElem();
		void ConstructL(TContextId aId, CEtelDriverFactory& aFactory, CPdpFsmInterface& aPdpFsmInterface);
		void Start();
		void Cancel();
	private:
		/** pdp status change notifier */
		CMbmsPdpStatusChangeNotifier*	iMbmsPdpStatusChangeNotifier;
		/** pdp config  change notifier */
		CMbmsPdpConfigChangeNotifier*	iMbmsPdpConfigChangeNotifier;
	};
	

#endif // CSPUDCONTEXTELEM_H
