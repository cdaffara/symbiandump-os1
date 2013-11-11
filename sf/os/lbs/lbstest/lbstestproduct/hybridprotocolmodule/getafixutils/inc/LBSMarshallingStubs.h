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
// LBSMarshallingStubs.h
//
/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __LBS_GETAFIX_TEST_PROP_PIPE_MARSHALLER_H__
#define __LBS_GETAFIX_TEST_PROP_PIPE_MARSHALLER_H__


#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetclasstypes.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpsmeasurement.h>

#include <lbs/test/proppipe.h>
//#include "lbsnetprotocolstub.h"

class LBSTestMarshaller
	{
public:
	//----------------------------------------------------------------------------------
	// Write operations
	// TLbsNetClassTypeBase derived classes
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsNetSessionId& aParam);
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestPrivacy& aParam);
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestQuality& aParam);
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestOptionsBase& aParam); 
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsNetPosMethod& aParam);
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsNetPosCapabilities& aParam);
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsNetPosRequestMethod& aParam);
	
	// TLbsLocClassTypeBase derived classes
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TLbsExternalRequestInfo& aParam);

	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TPositionInfoBase& aParam);
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, RLbsAssistanceDataBuilderSet& aParam);
	
	// Basic types		
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TDesC16& aParam);
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TInt& aParam);
	
	// Handle the typedef'd types: TLbsAsistanceDataGroup, TLbsNetProtocolServiceMask
	// 			   and enum types: TLbsNetProtocolService, TLbsPrivacyResponse
	//							   TLbsSystemStatus
	IMPORT_C static void WriteL(CWritePropPipe& aPipe, const TUint32& aParam);

	//----------------------------------------------------------------------------------
	// Write operations
	// TLbsNetClassTypeBase derived classes
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetSessionId* &aParam,CMemoryManager* iMemManager);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetPosRequestPrivacy* &aParam,CMemoryManager* iMemManager);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetPosRequestQuality* &aParam,CMemoryManager* iMemManager);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetPosRequestOptionsBase* &aParam,CMemoryManager* iMemManager);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetPosRequestOptions* &aParam,CMemoryManager* iMemManager);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetPosMethod* &aParam,CMemoryManager* iMemManager);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetPosCapabilities* &aParam,CMemoryManager* iMemManager);	
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsNetPosRequestMethod* &aParam,CMemoryManager* iMemManager);

	// TLbsLocClassTypeBase derived classes
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TLbsExternalRequestInfo* &aParam, CMemoryManager* iMemManager);

	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TPositionInfoBase* &aParam,CMemoryManager* iMemManager);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, RLbsAssistanceDataBuilderSet* &aParam,CMemoryManager* iMemManager);

	IMPORT_C static void ReadL (CReadPropPipe& aPipe, HBufC16* &aParam);
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TInt& aParam);
	
	// Handle the typedef'd types: TLbsAsistanceDataGroup, TLbsNetProtocolServiceMask
	// 			   and enum types: TLbsNetProtocolService, TLbsPrivacyResponse
	//							   TLbsSystemStatus
	IMPORT_C static void ReadL (CReadPropPipe& aPipe, TUint32& aParam);
 	};
#endif // __LBS_GETAFIX_TEST_PROP_PIPE_MARSHALLER_H__
