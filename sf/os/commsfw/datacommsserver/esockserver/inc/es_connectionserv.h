// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RConnectionServ: client access to the comms framework management plane
// 
//

/**
 @file
 @publishedPartner
 @released
*/


#ifndef ES_CONNECTION_SERV_H
#define ES_CONNECTION_SERV_H

#include <e32std.h>
#include <es_sock.h>
#include <comms-infras/cs_connservparams.h>
#include <comms-infras/cs_connservparams_internal.h>
#include <comms-infras/es_connectionservparameterbundle.h>

namespace ConnectionServ
{

class TAvailability;
class CConnectionServParameterBundle;




class MAccessPointNotificationObserver
/**
 Interface to react to access point notifications
 */
	{
public:
	// Called when a notification occurs. Ownership of aResult is handed to the implementer of this method
	//  .. so if you want to keep aResult after this method returns, just store the pointer.
	//   When you've finished looking at aResult, you must delete it.
	//
	virtual void AccessPointNotification(CConnectionServParameterBundle* aResult) = 0;

	// Called when there is an error. No further notifications will occur after this
	//  function is called.
	//
	virtual void AccessPointNotificationError(TInt aErrCode) = 0;
	};



class CConnectionServImpl;

class RConnectionServ : public RCommsSession
/**
 Provides access to the comms management plane.
 */
	{
public:

	friend class CConnectionServImpl;


// HOUSEKEEPING

	IMPORT_C RConnectionServ();
	
	/**
	Opens a new RConnServer session.
	
	@param aTierId Id of the Tier to connect to.
		To use the instance  the client connects to a Tier, depending on the technology they need to address.
		All actions are then performed in the context of that Tier until Close is called.
		The Tier Id may be Null which indicates all tiers.  At the moment no functionality supports the Null value.
	@return KErrNone if successful, otherwise another of the system wide error codes.
	*/
	IMPORT_C TInt Connect(TUint aTierId);


	/**
	Ends the session and ends the association with the Tier that was specified in Connect.
	*/
	IMPORT_C void Close();


// ACCESS POINT QUERY

	/**
	Retrieve information from the Tier(s) specified in the Connect function about the availability of one or more services.

	aQuery is an in parameter. It must be created and deleted (after the request has completed!) by the caller of this method.
		The Parameter Bundle contains 1 Parameter Set Container (at present).
		The Parameter Set Container contains 1 or more query Parameter Sets to specify the Access Point or group of Access Points it wishes
			to retrieve information about.

	aResult is an out parameter. It must be created and deleted (after the request has completed!) by the caller of this method.
		The Connection Server populates this bundle with information about the access points that matched the query.

	The time taken to perform this action depends on the implementation in the Tier Manager stack plug-in that serves the tier this session is attached to.
	Hence it's an asynchronous request.
	
	It delegates to active objects to do its work, and hence the passed-in TRequestStatus is completed by the thread that calls this function.
	Therefore it is not possible to make this call act "synchronously" by just using WaitForRequest on the following line
	 to the call to this function. Instead you'd have to kick off a worker thread to perform this call,
	 then use your main thread to do the waiting for the completion.
	This should only be a problem for test code, as all "real-world" apps are event based anyway so would supply an active object for
	 this request, therefore would return control to the active scheduler which can then do what it needs to complete the request.
	*/
	IMPORT_C void AccessPointStatusQuery(const CConnectionServParameterBundle& aQuery, CConnectionServParameterBundle& aResult, TRequestStatus& aStatus);


	/**
	Tell the Connection Server we are cancelling our status query request. If no query is active, does nothing.
	
	This function is synchronous, in that it ensures the cancel has gone all the way through to the server before 
	it returns.
	*/
	IMPORT_C void CancelAccessPointStatusQuery();



// ACCESS POINT NOTIFICATION

	/**
 	As for AccessPointStatusQuery, but will notify of changes to availability of requested Access Point(s).

	aQuery is an in parameter. It must be created and deleted (after the request has completed!) by the caller of this method.
		The Parameter Bundle contains 1 Parameter Set Container (at present).
		The Parameter Set Container contains 1 or more query Parameter Sets to specify the Access Point or group of Access Points it wishes
			to retrieve information about.

	The aQuery defines a set of access points.
		As soon as this "view" is established on the server, an empty bundle gets reported to the observer
	    (see second argument, aObserver) to denote that all the notifications are all registered on the server side.
	After that, Access Points entering or leaving the set get reported to the observer.
		This is at least the case for the generic tier parameters.
		Tiers with advanced parameters (e.g. signal strength) may define their own criteria as to when to notify
			(e.g. signal strength crossing some threshold by some amount)
		Queueing of multiple events happens automatically on the server side.

	aObserver refers to a client-defined implementation of the MAccessPointNotificationObserver interface -
	    this defines an AccessPointNotification(CConnectionServParameterBundle* aResult) method (which is called on each
		result back from the connection server), and an AccessPointNotificationError() method which is called in case of
		an error.

        Notifications are performed by a worker active object. Therefore you must have an active scheduler to use this API,
        and a return of control to the active scheduler is required before any notification or error will be received via your observer.
	*/
	IMPORT_C void AccessPointNotification(const CConnectionServParameterBundle& aQuery, MAccessPointNotificationObserver& aObserver);

	/**
	Tell the Connection Server we are cancelling our notification session. If no notification session is active, does nothing.
	
	This function is synchronous, in that it ensures the cancel has gone all the way through to the server before 
	it returns.
	*/
	IMPORT_C void CancelAccessPointNotification();



private:

	CConnectionServImpl* iConnectionServImpl;
	};


} // namespace ConnectionServ



#endif // ES_CONNECTION_SERV_H

