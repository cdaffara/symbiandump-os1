/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


#ifndef T_RSOCKET_DATA_H
#define T_RSOCKET_DATA_H

//	User Includes
#include "DataWrapperBase.h"
#include "BreathSaver.h"
//	EPOC includes
#include <datawrapper.h>
#include <w32std.h>
#include <e32std.h>

#include <es_sock.h> // TSockAddr address 
#include <in_sock.h> // TCP/IP Protocol and address family


// For ETel API.
#include <etelmm.h>


class CT_RSocketData: public CDataWrapperBase
	{
public:
	/**
	* Two phase constructor
	*/
	static CT_RSocketData*	NewL();
	
	virtual TAny*	GetObject();
	virtual TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	
	~CT_RSocketData();

public:
	void RunL(CActive* aActive, TInt aIndex);

protected:
	/**
	* Protected constructor. First phase construction
	*/
	CT_RSocketData();

	void ConstructL();
		
private:
	void InitialiseL();
	void CloseSocketServer();

	void DoCmdOpenL(const TTEFSectionName& aSection);
	void DoCmdInfoL(const TTEFSectionName& aSection);
	void DoCmdConnectL(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void DoCmdShutDown(const TInt aAsyncErrorIndex);
	void DoCmdClose();
	void DoCmdRecvOneOrMore(const TTEFSectionName& aSection);
	void DoCmdWrite(const TInt aAsyncErrorIndex);
	void DoCmdSendBytesToSocket(const TTEFSectionName& aSection);
	void DoCmdWriteRequest(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void DoCmdUtilityStartFile(const TTEFSectionName& aSection);
	void DoCmdUtilityStartDealer(const TTEFSectionName& aSection);
	void DoCmdUtilityEndFile(const TTEFSectionName& aSection);
	void DoCmdUtilityCalculateDownloadStats(const TTEFSectionName& aSection);
	void DoCmdUtilityCheckHSDPAL(const TTEFSectionName& aSection);
	void DoCmdResponseFromServer(const TTEFSectionName& aSection);
	
	//Helpers
    TBool CheckHSDPAIndicator();
    TBool CheckHSDPASupportL(RMobilePhone &aMobilePhone);
    TBool CheckHSDPAUsageL(RMobilePhone &aMobilePhone);
	
	//Upload
	void DoCmdUtilityBuildRequest(const TTEFSectionName& aSection);
	void DoCmdUtilityEndRequest();
	///End Upload
	TInt SendBytesToSocket(TInt aBytes, TInt aDuration);
	TReal ThroughputInKiloBits( TTimeIntervalMicroSeconds aDuration, TInt aBytes );
	TReal ThroughputInMegaBits( TTimeIntervalMicroSeconds aDuration, TInt aBytes );
	TInt SetThroughput( const TReal aThroughput );

private:
	/**
	 * RSocket
	 */    
    RSocket* 	iSocket;
    /**
     * CActive callback for asynchronous functions
     */
    CActiveCallback* iActiveCallback;
    /**
	 * Name entry stores name record for IPC.
	 */
	TNameEntry iNameEntry;
	/**
	 * Timestamps 
	 */
	TTime iEnd;
	TTime iStart;
	/**
	 * Total bytes received
	 */
	TInt iTotalReceived;
	
	/**
	 * Helper class that is used to save data.
	 */
    CBreathSaver* iBreathSaver;
    
    /**
     * TBuf used to send http requests
     */
    TBuf8<456> iRequest;
    
    TUid iPropertyCategory;
    TUint iPropertyKey;
    TInt iMaxPropertyLength;
    
	};

#endif //T_RSOCKET_DATA_H

