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
// Definitions of types used to simplify data transfer between
// client-server and publish and subscribe.
// 
//

/**
 @file
 @internalTechnology
 @released
*/
#ifndef __LBSPRIVACYCONTROLLERDATA_H__
#define __LBSPRIVACYCONTROLLERDATA_H__

#include <lbspositioninfo.h> // For TPositionInfo
#include "lbscommoninternaldatatypes.h" // For TLbsNetSessionIdInt
#include <lbs/lbsloccommon.h>

class TPrivacyRequestParams
    {
public:    
	TLbsNetSessionIdInt			iSessionId;  
	TLbsExternalRequestInfo2	iRequestInfo;
	TLbsNetPosRequestPrivacyInt	iRequestPrivacy;
    };

typedef TPckg<TPrivacyRequestParams> TPrivacyRequestParamsPckg;
typedef TPckgC<TPrivacyRequestParams> TPrivacyRequestParamsPckgC;

class TPrivacyResponseParams
	{
public:	
	TLbsNetSessionIdInt iSessionId;
	CLbsPrivacyController::TRequestVerificationResult iResult;
	} ;

class TPrivacyCancelParams
	{
public:	
	TLbsNetSessionIdInt iSessionId;
	} ;


class TLocationUpdateParams
	{
public:
	TLbsNetSessionIdInt iSessionId;	
	TPositionInfo	iPositionInfo;
	};
	
typedef TPckg<TLocationUpdateParams> TLocationUpdateParamsPckg;
typedef TPckgC<TLocationUpdateParams> TLocationUpdateParamsPckgC;

class TLbsSessionCompleteParams
	{
public:
	TLbsNetSessionIdInt iSessionId;
	TInt iReason;	
	};

typedef TPckg<TLbsSessionCompleteParams> TLbsSessionCompleteParamsPckg;
typedef TPckgC<TLbsSessionCompleteParams> TLbsSessionCompleteParamsPckgC;


enum TNrhServerMessageType
	{
	ENrhServerMessageAck = KErrNone,
	ENrhServerPrivacyRequest = 999,
	ENrhServerLocationUpdate,
	ENrhServerLbsSessionComplete
	};

enum TPrivacyControllerParamsIndex
	{
	EPrivacyRequestParams,
	ELocationUpdateParams,
	ELbsSessionCompleteParams	
	};	

#endif // __LBSPRIVACYCONTROLLERDATA_H__
