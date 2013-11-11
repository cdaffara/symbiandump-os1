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
// Packet ETel API v1.0
// Header file for asynchronous list retrieval classes
// 
//

/**
 @file
*/

#ifndef _PCKTRETRIEVE_H_
#define _PCKTRETRIEVE_H_

#include <etelpckt.h>
#include <pcktlist.h>
#include <mmretrieve.h>

class CRetrievePcktMbmsMonitoredServices : public CAsyncRetrieveWithClientIds
/** 
Reads MBMS monitored service list from the phone.

Enables the client to retrieve all the monitored service list in a single call. 
A CPcktMbmsMonitoredServiceList object is returned to the client. 
Due to the probable time taken to retrieve a list, clients must use an asynchronous 
approach.  

@publishedPartner
@released
*/
	{
public:
	IMPORT_C static CRetrievePcktMbmsMonitoredServices* NewL(RPacketService& aService);
	IMPORT_C ~CRetrievePcktMbmsMonitoredServices();
	IMPORT_C CPcktMbmsMonitoredServiceList* RetrieveListL();
	IMPORT_C void Start(TRequestStatus& aReqStatus);
protected:
	CRetrievePcktMbmsMonitoredServices(RPacketService& aService);
	void ConstructL();
	virtual void RestoreListL();
private:
	virtual void CancelReq(TInt aIpc1, TInt aIpc2);
	virtual void Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2);	
private:
	/**
	 * Holds the list of services retrieved from the TSY
	 */
	CPcktMbmsMonitoredServiceList* iResults;
	/**
	 * Holds a reference to packet service object
	 */
	RPacketService& iService;
	};
	
NONSHARABLE_CLASS(CRetrievePcktMbmsSessionList) : public CAsyncRetrieveVariableLengthBufferV2
/** 
 * It is used to retrieve MBMS sessions from TSY.
 * It inherits from CAsyncRetrieveVariableLengthBufferV2, active object derived class that
 * enables retrieval of data of variable length - length of data is retrieved by
 * the first IPC it issues, and actual data via second IPC.
 * @publishedPartner
 * @released
*/
	{
public:
	IMPORT_C static CRetrievePcktMbmsSessionList* NewL(RPacketMbmsContext& aPcktMbmsContext, RPacketMbmsContext::CMbmsSession& aSessionIdList);
	IMPORT_C void Start(TRequestStatus& aReqStatus);
protected:
	virtual void Get(TInt aIpc, TRequestStatus& aReqStatus, TDes8& aDes1, TDes8& aDes2);
	virtual void CancelReq(TInt aIpc1, TInt aIpc2);
protected:
	virtual void RestoreListL();	
	CRetrievePcktMbmsSessionList(RPacketMbmsContext& aPcktMbmsContext, RPacketMbmsContext::CMbmsSession& aSessionIdList);
protected:
	/**
	 * Holds reference to MBMS context.
	 */
	RPacketMbmsContext& iPcktMbmsContext;
	/**
	 * Holds the list of MBMS sessions of a MBMS service
	 */
	RPacketMbmsContext::CMbmsSession& iSessionIdList;
	/**
	 * Holds the session and subsession handles for server side authentication.
	 */
	TPckgBuf<TClientId> iId;
	};
	
#endif // _PCKTRETRIEVE_H_
	
