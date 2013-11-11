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
// Header file: More load related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_load2.h
*/

#ifndef TSU_MDF_CODECAPI_LOAD2_H
#define TSU_MDF_CODECAPI_LOAD2_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/codecapiuids.hrh>
#include <mdf/mdfpuloader.h>

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0011-HP
 * Loads a PU directly
 *
 * @class RTestStepCodecApiLoadPUDirect 
 *
 */
class RTestStepCodecApiLoadPUDirect 
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	RTestStepCodecApiLoadPUDirect();
	virtual TVerdict DoTestStepL();
	
private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	CMdfProcessingUnit* iPu;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0010-HP
 * Loads a PU directly by Uid
 *
 * @class RTestStepCodecApiLoadPUDirectByUid 
 *
 */
class RTestStepCodecApiLoadPUDirectByUid 
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	RTestStepCodecApiLoadPUDirectByUid();
	virtual TVerdict DoTestStepL();
	
private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	CMdfProcessingUnit* iPu;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0020-HP
 * Loads a PU and attempts to setup a tunnel between ports
 *
 * @class RTestStepCodecApiLoadPUDirectByUid 
 *
 */
class RTestStepCodecApiTunnelSetup 
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	RTestStepCodecApiTunnelSetup();
	virtual TVerdict DoTestStepL();
	
private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	CMdfProcessingUnit* iPcmPu;
	CMdfProcessingUnit* iSrcSinkPu;
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0060-HP
 * Loads a codec using CMdfHwDeviceAdapter direct
 *
 * @class RTestStepCodecApiLoadAdapterDirect 
 *
 */
class RTestStepCodecApiLoadAdapterDirect : public RTestStepCodecApi,  public MMMFHwDeviceObserver
	{
public:
	virtual TInt FillThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/) {return 0;}
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& /*aHwDataBuffer*/) {return 0;}
	virtual TInt MsgFromHwDevice(TUid /*aMessageType*/, const TDesC8& /*aMsg*/) {return 0;}
	virtual void Stopped() {}
	virtual void Error(TInt /*aError=KErrNone*/) {}

	RTestStepCodecApiLoadAdapterDirect();
	virtual TVerdict DoTestStepL();

private:
	TUid iHwDeviceAdapterDtorKey;
	};

//
// negative tests

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1011-HP
 * Attempt to load a PU with invalid FourCC codes
 *
 * @class RTestStepCodecApiLoadPUDirectInvalid 
 *
 */
class RTestStepCodecApiLoadPUDirectInvalid 
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	RTestStepCodecApiLoadPUDirectInvalid();
	virtual TVerdict DoTestStepL();
	
private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	CMdfProcessingUnit* iPu;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1010-HP
 * Attempt to load a PU with invalid Uid
 *
 * @class RTestStepCodecApiLoadPUDirectByUidInvalid 
 *
 */
class RTestStepCodecApiLoadPUDirectByUidInvalid 
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	RTestStepCodecApiLoadPUDirectByUidInvalid();
	virtual TVerdict DoTestStepL();
	
private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	CMdfProcessingUnit* iPu;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1020-HP
 * Attempts to setup a tunnel between ports which cannot be tunnelled
 *
 * @class RTestStepCodecApiLoadPUDirectByUidInvalid 
 *
 */
class RTestStepCodecApiTunnelSetupInvalid 
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	RTestStepCodecApiTunnelSetupInvalid();
	virtual TVerdict DoTestStepL();
	
private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	CMdfProcessingUnit* iPcmPu;
	CMdfProcessingUnit* iSrcSinkPu;
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	};
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-1110-HP
 * PDEF111147
 *
 * @class RTestStepCodecApiLoadPUMemLeak
 *
 */
class RTestStepCodecApiLoadPUMemLeak
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	RTestStepCodecApiLoadPUMemLeak();
	virtual TVerdict DoTestStepL();
	
private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	CMdfProcessingUnit* iPu;
	};

#endif // TSU_MDF_CODECAPI_LOAD2_H
