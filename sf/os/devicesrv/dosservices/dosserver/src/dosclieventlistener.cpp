/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    Implementation for the CDosEventListenerBase class.
*
*/


#include <DosSvrServices.h>
#include "dos_debug.h"
#include <e32svr.h>

//
// ---------------------------------------------------------
// CDosEventListenerBase Constructor (Not owned session)
// ---------------------------------------------------------
//

EXPORT_C CDosEventListenerBase::CDosEventListenerBase(RDosServer& aServer)
: iNotifierList(_FOFF(CDosEventNotifier,iLink)) , iNotifierIter(iNotifierList)
{
	iDosServer = aServer;
	iOwnServer = EFalse;
	iNotifierCount = 0;
	iEventList = NULL;
}

//
// ---------------------------------------------------------
// CDosEventListenerBase Constructor (owned session)
// ---------------------------------------------------------
//

EXPORT_C CDosEventListenerBase::CDosEventListenerBase()
: iNotifierList(_FOFF(CDosEventNotifier,iLink)) , iNotifierIter(iNotifierList)
{
    // Correcting CodeScanner 'High' defect for ignoring Connect() return value
	(void)iDosServer.Connect();
	iOwnServer = ETrue;
	iNotifierCount = 0;
	iEventList = NULL;
}

//
// ---------------------------------------------------------
// CDosEventListenerBase Destructor
// ---------------------------------------------------------
//

EXPORT_C CDosEventListenerBase::~CDosEventListenerBase()
{
	if (!iNotifierList.IsEmpty())
	{
		CDosEventNotifier* item;

	    iNotifierIter.SetToFirst();
	    while ((item = iNotifierIter++) != NULL)
		{
			delete item;
		}
		iNotifierList.Reset();
	}

	if(iOwnServer)
		iDosServer.Close();

	if (iEventList) delete[] iEventList;
}

//
// ---------------------------------------------------------
// CDosEventListenerBase::StartListeningL (array of events)
// ---------------------------------------------------------
//

EXPORT_C void CDosEventListenerBase::StartListeningL(TRegisterEvent* aEventArray, TInt aEventCount)
{
	//
	// Added reset
	if (!iNotifierList.IsEmpty())
	{
		CDosEventNotifier* item;

	    iNotifierIter.SetToFirst();
	    while ((item = iNotifierIter++) != NULL)
		{
			delete item;
		}
		iNotifierList.Reset();
	}
	// End of addition
	//

	iNotifierCount = aEventCount;

	CDosEventNotifier* tmp;

	if(iEventList)
	{
	   delete iEventList;
	   iEventList = NULL;
	}

	iEventList = new TRegisterEvent[iNotifierCount];
	if(!iEventList)
		User::LeaveIfError( KErrNoMemory );

	for(TInt i=iNotifierCount-1;i>=0;i--)
	{
		tmp = CDosEventNotifier::NewL(iDosServer,this);

		iNotifierList.AddFirst(*tmp);
		iEventList[i].iEvent = aEventArray[i].iEvent;
		iEventList[i].iParamSize = aEventArray[i].iParamSize;
		iEventList[i].iQueue = aEventArray[i].iQueue;
	}

	RegisterAndListenL();
}

//
// ---------------------------------------------------------
// CDosEventListenerBase::StartListeningL (one event)
// ---------------------------------------------------------
//

EXPORT_C void CDosEventListenerBase::StartListeningL(TUint aEvent,TInt aParamSize,TQueueType aQueue)
{
	CDosEventNotifier* tmp = NULL;

	iNotifierCount = 1;
	tmp = CDosEventNotifier::NewL(iDosServer,this);

	if (iEventList)
	{
		delete iEventList;
		iEventList = NULL;
	}

	iEventList = new TRegisterEvent;
	if( !iEventList )
	    {
		User::LeaveIfError( KErrNoMemory );
	    }

	iEventList->iEvent = aEvent;
	iEventList->iParamSize = aParamSize;
	iEventList->iQueue = aQueue;

	iNotifierList.AddLast(*tmp);

	if (!iNotifierList.IsEmpty())
	{
		CDosEventNotifier* item;

	    iNotifierIter.SetToFirst();
	    while ((item = iNotifierIter++) != NULL)
		{
	    COM_TRACE_3( "[DOSSERVER]\t StartListeningL (single event): event:%d, size:%d, queue:%d", iEventList->iEvent, iEventList->iParamSize, iEventList->iQueue );
		item->StartListeningL(iEventList->iEvent, iEventList->iParamSize, iEventList->iQueue);
		}
	}
}

//
// ---------------------------------------------------------
// CDosEventListenerBase::RegisterAndListenL
// ---------------------------------------------------------
//

void CDosEventListenerBase::RegisterAndListenL()
{
	if (!iNotifierList.IsEmpty())
	{
		CDosEventNotifier* item;
		TInt i=0;

	    iNotifierIter.SetToFirst();
	    while ((item = iNotifierIter++) != NULL)
		{
		COM_TRACE_3( "[DOSSERVER]\t RegisterAndListenL: event:%d, size:%d, queue:%d", iEventList[i].iEvent,iEventList[i].iParamSize,iEventList[i].iQueue );
		item->StartListeningL(iEventList[i].iEvent,iEventList[i].iParamSize,iEventList[i].iQueue);
		i++;
		}
	}
}

//
// ---------------------------------------------------------
// CDosEventListenerBase::Stop
// ---------------------------------------------------------
//

EXPORT_C void CDosEventListenerBase::Stop()
{
	COM_TRACE_( "CDosEventListenerBase::Stop() - STOP!" );
	if (!iNotifierList.IsEmpty())
	{
		CDosEventNotifier* item;

	    iNotifierIter.SetToFirst();
	    while ((item = iNotifierIter++) != NULL)
		{
			item->Cancel();
		}
	}
}

//
// ---------------------------------------------------------
// CDosEventListenerBase::OnError
// ---------------------------------------------------------
//

EXPORT_C void CDosEventListenerBase::OnError(TInt /*aError*/, TBool& /*aStopListening*/)
{
	//Does nothing. The derived class may implement this function,
}

//
// ---------------------------------------------------------
// CDosEventListenerBase::HandleEvent
// ---------------------------------------------------------
//

void CDosEventListenerBase::HandleEventL(const TRegisterEvent& aEvent,HBufC8* aParameter)
{
    API_TRACE_( "[DOSSERVER] CDosEventListenerBase::HandleEventL(...)" );

	TAny* par = NULL;
	if(aParameter)
		par = (TAny*)aParameter->Ptr();

    COM_TRACE_2( "[DOSSERVER] CDosEventListenerBase::GeneralEventL(0x%x, 0x%x)", aEvent, par );

	GeneralEventL(aEvent,par);
}


//
// ---------------------------------------------------------
// CDosEventListenerBase Empty default implementation of
// the events.
// ---------------------------------------------------------
//
EXPORT_C void CDosEventListenerBase::GeneralEventL(const TRegisterEvent& /*aEvent*/,TAny* /*aParameter*/)
{
}
