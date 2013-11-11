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
//

/**
 @file
 @internalComponent
*/


#ifndef __NET_SUBSCRIBER_H__
#define __NET_SUBSCRIBER_H__

#include <e32base.h>
#include <e32std.h>
#include "NetSubscribe.h"
#include "NetSignalEvent.h"

namespace NetSubscribe
{

class TEventClientData 
/**
 * A client's subscribtion data. Class takes event data class + event id + signal
 * handler. It's up to the type of subscription as to what transport layer & signal type
 * is being used 
 * e.g. signal types are as follows - public/subscribe, Active Object, request status,..
 * transport layers - message queue, client/server, RProperty,...
 *
 * @see TEvent
 * @see TSignalHandler
 * @internalComponent
 */
	{
	friend class TEventDataArray;
	
public:
	TEventClientData( TEventBase& aEvent ) :
		iEvent(aEvent)
		{
		}

	void SignalL( const Meta::SMetaData* aData )
		{
		(Event()->iHandler)( Event()->iThis, aData );
		};
	void SignalErrorL( TInt aError );

	TEvent* Event() const
		{
		return static_cast<TEvent*>(&iEvent);
		}
		
protected:
	TEventBase& iEvent;
	};

class TEventDataArray : public RArray<TEventClientData>
	{
public:
	~TEventDataArray()
		{
		Close();
		}
	TInt Find( const TEventBase* aEvent );
	TInt RegisterL( const TEventClientData& aData );
	TInt DeRegister( const TEventBase* aEvent );
	void DispatchL( const Meta::SMetaData* aData, TInt aErr );
	};

class XSignal
/**
 * The class abstracts physical signal source (active abject, status request,...)
 * The class is owned by CNetSubscribe::iSignals and represents a signal that can have registered
 * multiple client handlers
 *
 * @see TEvent
 * @see TSignalHandler
 * @see TSubscribtionData
 * @internalComponent
 */
	{
	public:
		virtual ~XSignal();
		/*
		called by the specific signal handler to catch the event and demarshal data 
		before forwarding call to the client.
		*/
//		virtual void SignalL() = 0; //not needed at the moment see the commend above the
//CSignalActive declaration
		/**
		called by CNetSubscribe when deregistering event returns number of events(callbacks) still
		registered for the signal.
		*/
		TInt DeRegisterClient(TEventBase* aEvent)
			{
			return iData.DeRegister(aEvent);
			}
		/**
		called by CNetSubscribe when registering event returns number of events(callbacks)
		registered for the signal.
		*/
		TInt RegisterClientL( TEventClientData& aData )
			{
			iData.RegisterL( aData );
			return iData.Count();
			}

		const SSignalId& GetSignalId() const
			{
			return iSignalId;
			}

	protected:
		XSignal(const SSignalId& aSignalId) :
			iSignalId(aSignalId)
			{
			}

	protected:
		const SSignalId iSignalId;
		TEventDataArray iData;
	};

//the class layout in NetSubscriber is missing an abstarction of dispatcher at the moment.
//(the same way as a signal is abstracted by SSignal class). Therefore the cocncrete signal class
//(e.g ~CPSSignal) must be aware of underlying method of capturing the signal (e.g this class).
//The dispatcher abstraction should be added as new concrete signals & transport will appear.
class CSignalActive : public CActive, public XSignal
/**
 * class catches a signal raised by active scheduler framework
 *
 * @see TEvent
 * @see TSignalHandler
 * @see TSubscribtionData
 * @internalComponent
 */
	{
public:
	CSignalActive(const SSignalId& iSignalId) :
		XSignal(iSignalId),
		CActive(EPriorityNormal)
		{
		CActiveScheduler::Add(this);
		}

protected:
//	virtual void RunL();
	
   virtual TInt RunError(TInt aError);
	};

} // namespace NetSubscribe

	
#endif // __NET_SUBSCRIBER_H__

