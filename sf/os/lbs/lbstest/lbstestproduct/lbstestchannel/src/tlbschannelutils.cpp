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
// This is the source file which contains the msg data types and other utilities
// 
//

#include "tlbschannelutils.h"


//
// Base Test Message Class
//


EXPORT_C TT_LbsMsgBase::TLbsMsgType TT_LbsMsgBase::Type() const
	{
	return iType;
	}
	
EXPORT_C TT_LbsMsgBase::TT_LbsMsgBase() :
	iType(EUnknown)
	{
	// Check that the internal buffer is 8-byte aligned.
	__ASSERT_COMPILE((_FOFF(TT_LbsMsgBase, iBuffer) & 0x40) == 0x0);
	}

EXPORT_C TT_LbsMsgBase::TT_LbsMsgBase(TLbsMsgType aType):
	iType(aType)
	{
	}
	
//
// Test A-Gps Module Request Update Initialization Message
//
EXPORT_C TT_LbsAGpsRequestUpdateInitMsg::TT_LbsAGpsRequestUpdateInitMsg(const TDesC& aConfigFileName, const TDesC& aConfigSection)
																		: TT_LbsMsgBase(EModuleRequestUpdateInit)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsRequestUpdateInit) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsRequestUpdateInit;
		
	ST_LbsAGpsRequestUpdateInit& data = Data();

	data.iConfigFileName = aConfigFileName;	
	data.iConfigSection = aConfigSection;	
	}
	
EXPORT_C const TDesC& TT_LbsAGpsRequestUpdateInitMsg::ConfigFileName()  const
	{
	return Data().iConfigFileName;
	}

EXPORT_C const TDesC& TT_LbsAGpsRequestUpdateInitMsg::ConfigSection()  const
	{
	return Data().iConfigSection;
	}

inline TT_LbsAGpsRequestUpdateInitMsg::ST_LbsAGpsRequestUpdateInit& TT_LbsAGpsRequestUpdateInitMsg::Data()
	{
	return reinterpret_cast<ST_LbsAGpsRequestUpdateInit&>(iBuffer);
	}
	
inline const TT_LbsAGpsRequestUpdateInitMsg::ST_LbsAGpsRequestUpdateInit& TT_LbsAGpsRequestUpdateInitMsg::Data() const
	{
	return reinterpret_cast<const ST_LbsAGpsRequestUpdateInit&>(iBuffer);
	}
	
	
//
// Test A-Gps Module Request TimeOut Message
//

EXPORT_C TT_LbsAGpsRequestTimeOut::TT_LbsAGpsRequestTimeOut(const TTimeIntervalMicroSeconds& aTimeOut)
															: TT_LbsMsgBase(EModuleRequestTimeOut)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsRequestTimeOut) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsRequestTimeOut;
		
	ST_LbsAGpsRequestTimeOut& data = Data();

	data.iTimeOut = aTimeOut;
	}

EXPORT_C const TTimeIntervalMicroSeconds& TT_LbsAGpsRequestTimeOut::TimeOut() const
	{
	return Data().iTimeOut;
	}	

inline TT_LbsAGpsRequestTimeOut::ST_LbsAGpsRequestTimeOut& TT_LbsAGpsRequestTimeOut::Data()
	{
	return reinterpret_cast<ST_LbsAGpsRequestTimeOut&>(iBuffer);
	}
	
inline const TT_LbsAGpsRequestTimeOut::ST_LbsAGpsRequestTimeOut& TT_LbsAGpsRequestTimeOut::Data() const
	{
	return reinterpret_cast<const ST_LbsAGpsRequestTimeOut&>(iBuffer);
	}	

//
// Test A-Gps Module Request Options
//
EXPORT_C TT_LbsAGpsRequestOptions::TT_LbsAGpsRequestOptions(TLbsHybridModuleOptions aModuleOption) 
															: TT_LbsMsgBase(EModuleRequestOptions)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsRequestOptions) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsRequestOptions;
		
	ST_LbsAGpsRequestOptions& data = Data();

	data.iModuleOption = aModuleOption;
	}

EXPORT_C TT_LbsAGpsRequestOptions::TT_LbsAGpsRequestOptions(TLbsHybridModuleOptions aModuleOption, 
															TBool aModuleValue) 
															: TT_LbsMsgBase(EModuleRequestOptions)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsRequestOptions) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsRequestOptions;
		
	ST_LbsAGpsRequestOptions& data = Data();

	data.iModuleOption = aModuleOption;
	data.iModuleValue = aModuleValue;
	}
	

EXPORT_C TLbsHybridModuleOptions TT_LbsAGpsRequestOptions::ModuleOption() const
	{
	return Data().iModuleOption;
	}
	
EXPORT_C TBool TT_LbsAGpsRequestOptions::ModuleValue() const
	{
	return Data().iModuleValue;
	}
	
inline TT_LbsAGpsRequestOptions::ST_LbsAGpsRequestOptions& TT_LbsAGpsRequestOptions::Data()
	{
	return reinterpret_cast<ST_LbsAGpsRequestOptions&>(iBuffer);
	}
	
inline const TT_LbsAGpsRequestOptions::ST_LbsAGpsRequestOptions& TT_LbsAGpsRequestOptions::Data() const
	{
	return reinterpret_cast<const ST_LbsAGpsRequestOptions&>(iBuffer);
	}	
	
//
// Test A-Gps Module Request Error
//
EXPORT_C TT_LbsAGpsRequestError::TT_LbsAGpsRequestError(TInt aError)
														: TT_LbsMsgBase(EModuleRequestError)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsRequestError) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsRequestError;
		
	ST_LbsAGpsRequestError& data = Data();

	data.iError = aError;
	}

EXPORT_C TInt TT_LbsAGpsRequestError::Error() const
	{
	return Data().iError;
	}
	
inline TT_LbsAGpsRequestError::TT_LbsAGpsRequestError::ST_LbsAGpsRequestError& TT_LbsAGpsRequestError::Data()
	{
	return reinterpret_cast<ST_LbsAGpsRequestError&>(iBuffer);
	}
	
inline const TT_LbsAGpsRequestError::ST_LbsAGpsRequestError& TT_LbsAGpsRequestError::Data() const
	{
	return reinterpret_cast<const ST_LbsAGpsRequestError&>(iBuffer);
	}	

//
// Test A-Gps Module Immediate Measurements
//
EXPORT_C TT_LbsAGpsImmediateMeasurements::TT_LbsAGpsImmediateMeasurements(TInt aImmediateMeasurements)
														: TT_LbsMsgBase(EModuleImmediateMeasurements)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsImmediateMeasurements) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsImmediateMeasurements;
		
	ST_LbsAGpsImmediateMeasurements& data = Data();

	data.iImmediateMeasurements = aImmediateMeasurements;
	}

EXPORT_C TInt TT_LbsAGpsImmediateMeasurements::ImmediateMeasurements() const
	{
	return Data().iImmediateMeasurements;
	}
	
inline TT_LbsAGpsImmediateMeasurements::TT_LbsAGpsImmediateMeasurements::ST_LbsAGpsImmediateMeasurements& TT_LbsAGpsImmediateMeasurements::Data()
	{
	return reinterpret_cast<ST_LbsAGpsImmediateMeasurements&>(iBuffer);
	}
	
inline const TT_LbsAGpsImmediateMeasurements::ST_LbsAGpsImmediateMeasurements& TT_LbsAGpsImmediateMeasurements::Data() const
	{
	return reinterpret_cast<const ST_LbsAGpsImmediateMeasurements&>(iBuffer);
	}	




//
// Test A-Gps Module Request Forced Update
//

EXPORT_C TT_LbsAGpsRequestForcedUpdate::TT_LbsAGpsRequestForcedUpdate()
														: TT_LbsMsgBase(EModuleRequestForcedUpdate)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsRequestForcedUpdate) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsRequestForcedUpdate;
		
	ST_LbsAGpsRequestForcedUpdate& data = Data();
	}

inline TT_LbsAGpsRequestForcedUpdate::ST_LbsAGpsRequestForcedUpdate& TT_LbsAGpsRequestForcedUpdate::Data()
	{
	return reinterpret_cast<ST_LbsAGpsRequestForcedUpdate&>(iBuffer);
	}
	
inline const TT_LbsAGpsRequestForcedUpdate::ST_LbsAGpsRequestForcedUpdate& TT_LbsAGpsRequestForcedUpdate::Data() const
	{
	return reinterpret_cast<const ST_LbsAGpsRequestForcedUpdate&>(iBuffer);
	}
	
//
// Test A-Gps Module Response Message
//

EXPORT_C TT_LbsAGpsResponseMsg::TT_LbsAGpsResponseMsg(const TModuleResponseType& 	aResponse)
							  						: TT_LbsMsgBase(EModuleResponse)
	{
	// Must make sure that the data to be stored is not bigger than the buffer.
	__ASSERT_COMPILE(sizeof(ST_LbsAGpsResponse) <= sizeof(iBuffer));

	// In-place construction of the data member
	new(&Data()) ST_LbsAGpsResponse;
		
	ST_LbsAGpsResponse& data = Data();

	data.iResponseType = aResponse;
	}

EXPORT_C TT_LbsAGpsResponseMsg::TModuleResponseType TT_LbsAGpsResponseMsg::ResponseType() const
	{
	return Data().iResponseType;
	}
			
inline TT_LbsAGpsResponseMsg::ST_LbsAGpsResponse& TT_LbsAGpsResponseMsg::Data()
	{
	return reinterpret_cast<ST_LbsAGpsResponse&>(iBuffer);
	}
	
inline const TT_LbsAGpsResponseMsg::ST_LbsAGpsResponse& TT_LbsAGpsResponseMsg::Data() const
	{
	return reinterpret_cast<const ST_LbsAGpsResponse&>(iBuffer);
	}	
