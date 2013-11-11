/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//
// LBSParamLogger.h
//

#ifndef __LBS_GETAFIX_TEST_PARAMETER_LOGGER_H__
#define __LBS_GETAFIX_TEST_PARAMETER_LOGGER_H__

#include <e32std.h>
#include <lbs/lbsnetprotocolbase.h>

class TLbsNetSessionId;
class TLbsNetPosRequestPrivacy;
class TLbsNetPosRequestQuality;
class TLbsNetPosRequestOptionsBase;
class TLbsNetPosMethod;
class TLbsNetPosCapabilities;
class TLbsNetPosRequestMethod;
class TLbsExternalRequestInfo;
class TPositionInfoBase;
class RLbsAssistanceDataBuilderSet;
class TLbsExternalRequestInfo;
class TLbsLocRequestQuality;
class TLbsGpsOptions;
class TLbsTransmitPositionOptions;



class LBSParamLogger
	{
public:
	// Log a method name
	IMPORT_C static void Method(const TPtrC& aDirection, const TPtrC& aMethod);
	
	// Flush any buffers
	IMPORT_C static void Flush();
	
	// TLbsNetClassTypeBase derived classes
	IMPORT_C static void Log(const TPtrC& aName, const TLbsNetSessionId& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsNetPosRequestPrivacy& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsNetPosRequestQuality& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsNetPosRequestOptionsBase& aParam); 
	IMPORT_C static void Log(const TPtrC& aName, const TLbsNetPosMethod& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsNetPosCapabilities& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsNetPosRequestMethod& aParam);
	
	// TLbsLocClassTypeBase derived classes
	IMPORT_C static void Log(const TPtrC& aName, const TLbsExternalRequestInfo& aParam);

	IMPORT_C static void Log(const TPtrC& aName, const TPositionInfoBase& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const RLbsAssistanceDataBuilderSet& aParam);
	
	// enum types: TLbsNetProtocolService, TLbsPrivacyResponse, TLbsSystemStatus
	IMPORT_C static void Log(const TPtrC& aName, MLbsNetworkProtocolObserver::TLbsNetProtocolService aParam);
	IMPORT_C static void Log(const TPtrC& aName, CLbsNetworkProtocolBase::TLbsPrivacyResponse aParam);
	IMPORT_C static void Log(const TPtrC& aName, CLbsNetworkProtocolBase::TLbsSystemStatus aParam);
	
	// TLbsLocClassTypeBase derived classes
//IMPORT_C static void Log(const TPtrC& aName, const TLbsExternalRequestInfo& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsLocRequestQuality& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsGpsOptions& aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TLbsTransmitPositionOptions& aParam);
	
	// Basic types
	IMPORT_C static void Log(const TPtrC& aName, const TTime& aParam);
	IMPORT_C static void Log(const TPtrC& aName, TInt aParam);
	IMPORT_C static void Log(const TPtrC& aName, TUint32 aParam);
	IMPORT_C static void Log(const TPtrC& aName, TInt32 aParam);
	IMPORT_C static void Log(const TPtrC& aName, const TDesC16& aParam);
	};




#ifdef _ENABLE_TEST_LOGGING
	#define INBOUND  _L("<-")
	#define OUTBOUND _L("->")
	
	#define LBSTESTLOG_METHOD(d,m)				    { LBSParamLogger::Method(d, _L(m));	LBSParamLogger::Flush();	}
	#define LBSTESTLOG_METHOD1(d,m,p1)				{ LBSParamLogger::Method(d, _L(m));	LBSParamLogger::Log(_L(#p1), p1); LBSParamLogger::Flush();	}
	#define LBSTESTLOG_METHOD2(d,m,p1,p2)    		{ LBSParamLogger::Method(d, _L(m)); LBSParamLogger::Log(_L(#p1), p1); LBSParamLogger::Log(_L(#p2), p2); LBSParamLogger::Flush();	}
	#define LBSTESTLOG_METHOD3(d,m,p1,p2,p3)		{ LBSParamLogger::Method(d, _L(m));	LBSParamLogger::Log(_L(#p1), p1); LBSParamLogger::Log(_L(#p2), p2); LBSParamLogger::Log(_L(#p3), p3); LBSParamLogger::Flush();	}
	#define LBSTESTLOG_METHOD4(d,m,p1,p2,p3,p4)		{ LBSParamLogger::Method(d, _L(m)); LBSParamLogger::Log(_L(#p1), p1); LBSParamLogger::Log(_L(#p2), p2); LBSParamLogger::Log(_L(#p3), p3); LBSParamLogger::Log(_L(#p4), p4);	LBSParamLogger::Flush(); }
	#define LBSTESTLOG_METHOD5(d,m,p1,p2,p3,p4,p5)	{ LBSParamLogger::Method(d, _L(m)); LBSParamLogger::Log(_L(#p1), p1); LBSParamLogger::Log(_L(#p2), p2); LBSParamLogger::Log(_L(#p3), p3); LBSParamLogger::Log(_L(#p4), p4);	LBSParamLogger::Log(_L(#p5), p5); LBSParamLogger::Flush(); }
#else
	#define INBOUND
	#define OUTBOUND
	
	#define LBSTESTLOG_METHOD(d,m)
	#define LBSTESTLOG_METHOD1(d,m,p1)
	#define LBSTESTLOG_METHOD2(d,m,p1,p2)
	#define LBSTESTLOG_METHOD3(d,m,p1,p2,p3)
	#define LBSTESTLOG_METHOD4(d,m,p1,p2,p3,p4)
	#define LBSTESTLOG_METHOD5(d,m,p1,p2,p3,p4,p5)
#endif // _DISABLE_TEST_LOGGING

#endif // __LBS_GETAFIX_TEST_PARAMETER_LOGGER_H__
