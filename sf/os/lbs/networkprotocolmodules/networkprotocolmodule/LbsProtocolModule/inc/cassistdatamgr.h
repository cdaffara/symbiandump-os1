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
// This file defines the class that manages assistance data
// during active location requests.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CASSISTDATAMGR_H__
#define __CASSISTDATAMGR_H__

#include <e32base.h>
#include <lbs/lbsassistancedatabuilderset.h>

/** Assistance Data Manager class.

This class manages assistance data requests and responses. It oversees the
process whereby the data that is requested by LBS is retrieved from the 
network, or associated error responses are provided.

Note: In this first implementation there is no provision for requesting
additional assistance data on behalf of LBS i.e. only the default data
set provided by the network is returned to LBS. Future enhancements could
introduce support for requesting additional data and handling any network
interactions relating to errors and cancellations.
*/
NONSHARABLE_CLASS(CAssistDataMgr) : public CBase
	{

public:

	static CAssistDataMgr* NewL();
	~CAssistDataMgr();

	void  Error(TInt aReason);
	void  StoreData(const RLbsAssistanceDataBuilderSet& aData);
	TBool ProcessDataRequest(const TLbsAsistanceDataGroup& aDataRequestMask);

	const RLbsAssistanceDataBuilderSet& ValidData();
	const TLbsAsistanceDataGroup& RequestMask() const;
	const TLbsAsistanceDataGroup& ValidGroupMask() const;
	const TLbsAsistanceDataGroup& ErrorGroupMask() const;
	const TLbsAsistanceDataGroup& RequestErrorMask() const;

	TBool IsDataToBeRequested() const;
	TBool IsDataToBeReported() const;
	TBool IsErrorToBeReported() const;
	TBool IsRequestErrorToBeReported() const;
	TInt  ErrorReason() const;

	void  SetDataNotReceived();
	void  SendingRequest();
	void  RequestSent();
	void  DataReported();
	void  ErrorReported();
	void  RequestErrorReported();
	void  ResetData();

private:

	CAssistDataMgr();
	void ConstructL();


private:

	/** Requested data mask
	This reflects the set of data requested by a client.
	Additional data requests from LBS are added to this mask.
	If an error is reported then the problematic items are 
	removed from the set i.e. they should not be re-requested.
	*/
	TLbsAsistanceDataGroup iDataRequestMask;

	/** Missing data mask
	This shows the set of data requested by a client that is missing
	from the received data. It is used when re-requesting data.
	*/
	TLbsAsistanceDataGroup iMissingDataMask;

	/** Erroneous data mask
	This shows the set of data requested by a client that has been received
	with errors identified.
	*/
	TLbsAsistanceDataGroup iErrorDataMask;

	/** Valid data mask
	This shows the set of data requested by a client that has been received
	without an error.
	*/
	TLbsAsistanceDataGroup iValidDataMask;

	/** Erroneous request data mask
	This shows invalid parts of a request from a client.
	*/
	TLbsAsistanceDataGroup iRequestErrorMask;

	/** Valid assistance data storage
	This is the stored set of valid assistance data.
	*/
	RLbsAssistanceDataBuilderSet iValidData;

	/** Assistance data response error.
	This indicates any general assistance data error that was reported.
	*/
	TInt iDataResponseError;
	
	/** Indicates if an assistance data response is expected from network.
	*/
	TBool iExpectingResponse;
	
	/** Indicates if an assistance data response must be sent to LBS.
	*/
	TBool iIsResponseWaiting;
	
	/** Indicates if data response (or error) has been reported by network.
	*/
	TBool iIsDataReported;

	};

#endif // __CASSISTDATAMGR_H__
