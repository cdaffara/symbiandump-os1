// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file tlbschannelutils.h
// This is the header file which contains the msg data types and other utilities
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef TLBSCHANNELUTILS_H
#define TLBSCHANNELUTILS_H

#include <lbs/test/tlbsutils.h>


/* Key value used to publish messages from Test Harness to Test AGPS Module 
   and the corresponding acknowledgement */
const TUint KTH2TAGPSMessageKey = 0x0F001010;
const TUint KTH2TAGPSMessageReadAckKey = 0x0F001020;
/* Key value used to publish messages from Test AGPS Module to Test Harness  
   and the corresponding acknowledgement */
const TUint KTAGPS2THMessageKey = 0x0F001030;
const TUint KTAGPS2THMessageReadAckKey = 0x0F001040;



/** Base class for messages sent between the Test Harness and Test A-Gps Module.

This class defines the types of messages and reserves a buffer to store
information, but it can't be used itself. Instead derived classes will
implement each of the types of messages and supply accessor functions.

*/
class TT_LbsMsgBase
	{
public:
	/** List of all possible message types.
	
	Each type of message listed here must be implemented as a
	separate class that inherits from TLbsNetInternalMsgBase.
	*/
	enum TLbsMsgType
		{
		EUnknown = 0,
		EModuleRequestUpdateInit,	// TH --> AGPS-Module
		EModuleRequestTimeOut,		// TH --> AGPS-Module
		EModuleRequestOptions,		// TH --> AGPS-Module
		EModuleRequestError,		// TH --> AGPS-Module
		EModuleRequestForcedUpdate,	// TH --> AGPS-Module
		EModuleResponse,			// AGPS-Module --> TH
		EModuleImmediateMeasurements// TH --> AGPS-Module
		};

public:
	IMPORT_C TT_LbsMsgBase();
	
	IMPORT_C TLbsMsgType Type() const;

protected:
	// Derived classes set their type by using these constructors.
	IMPORT_C TT_LbsMsgBase(TLbsMsgType aType);	

private:
	TLbsMsgType iType;
	
protected:	
	// A TUint64 is used to ensure that any objects that use the buffer 
	// are at least 8byte aligned. (The compiler will organise the arrangement
	// of iBuffer within TLbsNetInternalMsgBase to be aligned to the minimum
	// for a TUint64, which is 8bytes.)
	TUint64 iBuffer[((1576 - (sizeof(TLbsMsgType))) >> 3)];
	};
	
/** 
Test A-Gps Module Request UpdateInit Message
*/
NONSHARABLE_CLASS(TT_LbsAGpsRequestUpdateInitMsg) : public TT_LbsMsgBase
	{
public:
	IMPORT_C TT_LbsAGpsRequestUpdateInitMsg(const TDesC& aConfigFileName, const TDesC& aConfigSection);
	
	IMPORT_C const TDesC& ConfigFileName() const;
	IMPORT_C const TDesC& ConfigSection() const;

private:
	TT_LbsAGpsRequestUpdateInitMsg();

private:
	struct ST_LbsAGpsRequestUpdateInit
		{
		TBuf<KMaxModuleDataBusIniFileName>	iConfigFileName; 	/** Config request data. */
		TBuf<40>							iConfigSection;  	/** Config request data. */
		};

	inline ST_LbsAGpsRequestUpdateInit& Data();
	inline const ST_LbsAGpsRequestUpdateInit& Data() const;
	};

/** 
Test A-Gps Module Request TimeOut Message
*/
NONSHARABLE_CLASS(TT_LbsAGpsRequestTimeOut) : public TT_LbsMsgBase
	{
public:
	IMPORT_C TT_LbsAGpsRequestTimeOut(const TTimeIntervalMicroSeconds& aTimeOut);
	IMPORT_C const TTimeIntervalMicroSeconds& TimeOut() const;

private:
	TT_LbsAGpsRequestTimeOut();

private:
	struct ST_LbsAGpsRequestTimeOut
		{
		TTimeIntervalMicroSeconds 	iTimeOut;	/** Module time out. */
		};	
	inline ST_LbsAGpsRequestTimeOut& Data();
	inline const ST_LbsAGpsRequestTimeOut& Data() const;
	};


/** 
Test A-Gps Module Immediate Measurements Message
*/
NONSHARABLE_CLASS(TT_LbsAGpsImmediateMeasurements) : public TT_LbsMsgBase
	{
public:
	IMPORT_C TT_LbsAGpsImmediateMeasurements(TInt aImmediateMeasurements);
	IMPORT_C TInt ImmediateMeasurements() const;

private:
	TT_LbsAGpsImmediateMeasurements();

private:
	struct ST_LbsAGpsImmediateMeasurements
		{
		TInt iImmediateMeasurements;				/** Immediate Measurements */
		};	

	inline ST_LbsAGpsImmediateMeasurements& Data();
	inline const ST_LbsAGpsImmediateMeasurements& Data() const;
	};


/** 
Test A-Gps Module Request Options Message
*/
NONSHARABLE_CLASS(TT_LbsAGpsRequestOptions) : public TT_LbsMsgBase
	{
public:
	IMPORT_C TT_LbsAGpsRequestOptions(TLbsHybridModuleOptions aModuleOption);
	IMPORT_C TT_LbsAGpsRequestOptions(TLbsHybridModuleOptions aModuleOption, TBool aOptionValue);

	IMPORT_C TLbsHybridModuleOptions ModuleOption() const;
	IMPORT_C TBool ModuleValue() const;

private:
	TT_LbsAGpsRequestOptions();

private:
	struct ST_LbsAGpsRequestOptions
		{
		TLbsHybridModuleOptions 	iModuleOption; 	// Module option
		TBool	 					iModuleValue; 	// Value for the module option
		};	

	inline ST_LbsAGpsRequestOptions& Data();
	inline const ST_LbsAGpsRequestOptions& Data() const;
	};

/** 
Test A-Gps Module Request Error Message
*/
NONSHARABLE_CLASS(TT_LbsAGpsRequestError) : public TT_LbsMsgBase
	{
public:
	IMPORT_C TT_LbsAGpsRequestError(TInt aOptions);
	IMPORT_C TInt Error() const;

private:
	TT_LbsAGpsRequestError();

private:
	struct ST_LbsAGpsRequestError
		{
		TInt iError;				/** Module Error Code */
		};	

	inline ST_LbsAGpsRequestError& Data();
	inline const ST_LbsAGpsRequestError& Data() const;
	};

/** 
Test A-Gps Module Request Forced Update Message

This message forces the test A-Gps module to send
the next position update from the update array
now, even if there is no outstanding request from LBS.
*/
NONSHARABLE_CLASS(TT_LbsAGpsRequestForcedUpdate) : public TT_LbsMsgBase
	{
public:
	IMPORT_C TT_LbsAGpsRequestForcedUpdate();

private:
	struct ST_LbsAGpsRequestForcedUpdate
		{
		TInt iDummy;				/** Dummy value; unused */
		};	

	inline ST_LbsAGpsRequestForcedUpdate& Data();
	inline const ST_LbsAGpsRequestForcedUpdate& Data() const;
	};
	
/** 
Test A-Gps Module Response Message
*/
NONSHARABLE_CLASS(TT_LbsAGpsResponseMsg) : public TT_LbsMsgBase
	{
public:

    enum TModuleResponseType
		{
		EModuleResponseOk,
		EModuleErr
		};
		
	IMPORT_C TT_LbsAGpsResponseMsg(const TModuleResponseType& 	aResponse);

	IMPORT_C TModuleResponseType ResponseType() const;

private:
	TT_LbsAGpsResponseMsg();

private:

	struct ST_LbsAGpsResponse
		{
		TModuleResponseType iResponseType;
		};
	
	inline ST_LbsAGpsResponse& Data();
	inline const ST_LbsAGpsResponse& Data() const;
	};	
	
#endif // TLBSCHANNELUTILS_H
