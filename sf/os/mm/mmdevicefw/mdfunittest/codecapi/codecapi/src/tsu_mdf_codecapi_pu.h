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
// Header file: PU related tests for PREQ1024 MDF Codec API.
// 
//

/**
 @file tsu_mdf_codecapi_pu.h
*/

#ifndef TSU_MDF_CODECAPI_PU_H
#define TSU_MDF_CODECAPI_PU_H

#include "tsu_mdf_codecapi_step.h"
#include <mmf/server/mmfhwdevice.h>
#include <mdf/mdfprocessingunit.h>

/**
 * 
 * Abstract base class for PU loading
 *
 * @class RTestStepCodecApiPuBase 
 *
 */
class RTestStepCodecApiPuBase 
  : public RTestStepCodecApi,
    public MMdfProcessingUnitObserver
	{
public:
	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */) { }
	virtual void ExecuteComplete(const CMdfProcessingUnit* /* aPu */, TInt /* aErrorCode */) { }

	virtual TVerdict DoTestStepL() = 0;
	
protected:
	CMdfProcessingUnit* iPu;
	
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0200-HP
 * Loads a PU directly
 *
 * @class RTestStepCodecApiPuCreate 
 *
 */
class RTestStepCodecApiPuCreate 
  : public RTestStepCodecApiPuBase
	{
public:
	RTestStepCodecApiPuCreate();
	virtual TVerdict DoTestStepL();
	};
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0260-HP
 * Gets a PU's custom interface
 *
 * @class RTestStepCodecApiPuCustomInterface 
 *
 */
class RTestStepCodecApiPuCustomInterface 
  : public RTestStepCodecApiPuCreate
	{
public:
	RTestStepCodecApiPuCustomInterface();
	virtual TVerdict DoTestStepL();
	};
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0201-HP
 * Gets a PU's ports
 *
 * @class RTestStepCodecApiPuGetPorts 
 *
 */
class RTestStepCodecApiPuGetPorts 
  : public RTestStepCodecApiPuBase
	{
public:
	RTestStepCodecApiPuGetPorts();
	virtual TVerdict DoTestStepL();

private:
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	};
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0202-HP
 * Configures a PU
 *
 * @class RTestStepCodecApiPuConfig 
 *
 */
class RTestStepCodecApiPuConfig
  : public RTestStepCodecApiPuBase
	{
public:
	RTestStepCodecApiPuConfig();
	virtual TVerdict DoTestStepL();

private:
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	};
	
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0210-HP
 * Create buffers on a PU's ports
 *
 * @class RTestStepCodecApiPuCreateBuffers 
 *
 */
class RTestStepCodecApiPuCreateBuffers
  : public RTestStepCodecApiPuBase
	{
public:
	RTestStepCodecApiPuCreateBuffers();
	virtual TVerdict DoTestStepL();

private:
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0211-HP
 * Use buffers on a PU's ports
 *
 * @class RTestStepCodecApiPuUseBuffers 
 *
 */
class RTestStepCodecApiPuUseBuffers
  : public RTestStepCodecApiPuBase
	{
public:
	RTestStepCodecApiPuUseBuffers();
	virtual TVerdict DoTestStepL();

private:
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0220-HP
 * Initialises a PU
 *
 * @class RTestStepCodecApiPuInit 
 *
 */
class RTestStepCodecApiPuInit
  : public RTestStepCodecApiPuBase
	{
public:
	RTestStepCodecApiPuInit();
	virtual TVerdict DoTestStepL();

	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

private:
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	TBool iInitComplete;
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0230-HP
 * Executes a PU directly
 *
 * @class RTestStepCodecApiPuExecute 
 *
 */
class RTestStepCodecApiPuExecute
  : public RTestStepCodecApiPuBase,
    public MMdfInputPortObserver,
    public MMdfOutputPortObserver
	{
public:
	RTestStepCodecApiPuExecute();
	virtual TVerdict DoTestStepL();

	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	
	// MMdfInputPortObserver
	virtual void MipoWriteDataComplete(const MMdfInputPort* aInputPort,
		CMMFBuffer* aBuffer, TInt aErrorCode);
	virtual void MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode);
	virtual void MipoRestartTunnelComplete(const MMdfInputPort* aInputPort,
		TInt aErrorCode);

	// MMdfOutputPortObserver
	virtual void MopoReadDataComplete(const MMdfOutputPort* aOutputPort, 
		CMMFBuffer* aBuffer, TInt aErrorCode);
	virtual void MopoDisconnectTunnelComplete(const MMdfOutputPort* aOutputPort,
		TInt aErrorCode);
	virtual void MopoRestartTunnelComplete(const MMdfOutputPort* aOutputPort,
		TInt aErrorCode);	

protected:
	TInt ReadTestFileInBuffer();

protected:
	RPointerArray<MMdfInputPort> iInputPortsArray;
	RPointerArray<MMdfOutputPort> iOutputPortsArray;
	HBufC8* iSourceFile;
	TBool iInitComplete;
	TBool iExecuteComplete;
	TBool iReadDataComplete;
	TBool iWriteDataComplete;
	TBool iReadDataOk;
	TBool iWriteDataOk;
	
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0240-HP
 * Pause and restart a PU
 *
 * @class RTestStepCodecApiPuPauseRestart 
 *
 */
class RTestStepCodecApiPuPauseRestart
  : public RTestStepCodecApiPuExecute
	{
public:
	RTestStepCodecApiPuPauseRestart();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0250-HP
 * Stop and restart a PU
 *
 * @class RTestStepCodecApiPuStopRestart 
 *
 */
class RTestStepCodecApiPuStopRestart
  : public RTestStepCodecApiPuExecute
	{
public:
	RTestStepCodecApiPuStopRestart();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0231-HP
 * Executes a PU directly using tunnelled ports
 *
 * @class RTestStepCodecApiPuExecuteTunnel 
 *
 */
class RTestStepCodecApiPuExecuteTunnel
  : public RTestStepCodecApiPuExecute
	{
public:
	RTestStepCodecApiPuExecuteTunnel();
	virtual TVerdict DoTestStepL();

	// MMdfProcessingUnitObserver
	virtual void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	virtual void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	
	// MMdfInputPortObserver
	virtual void MipoWriteDataComplete(const MMdfInputPort* aInputPort,
		CMMFBuffer* aBuffer, TInt aErrorCode);

	// MMdfOutputPortObserver
	virtual void MopoReadDataComplete(const MMdfOutputPort* aOutputPort, 
		CMMFBuffer* aBuffer, TInt aErrorCode);

protected:
	RPointerArray<MMdfInputPort> iPcmInputPortsArray;
	RPointerArray<MMdfInputPort> iSrcSinkInputPortsArray;
	RPointerArray<MMdfOutputPort> iPcmOutputPortsArray;
	RPointerArray<MMdfOutputPort> iSrcSinkOutputPortsArray;
	CMdfProcessingUnit* iPcmPu;
	CMdfProcessingUnit* iSrcSinkPu;
	TInt iWriteDataNumCallbacks;
	TBool iWriteDataPcmOk;
	TBool iWriteDataSrcSinkOk;
	};
				
/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0241-HP
 * Pause and restart a PU using tunnelled ports
 *
 * @class RTestStepCodecApiPuPauseRestartTunnel 
 *
 */
class RTestStepCodecApiPuPauseRestartTunnel
  : public RTestStepCodecApiPuExecuteTunnel
	{
public:
	RTestStepCodecApiPuPauseRestartTunnel();
	virtual TVerdict DoTestStepL();
	};

/**
 * 
 * Test Case : MM-MDF-CODECAPI-U-0251-HP
 * Stop and restart a PU using tunnelled ports
 *
 * @class RTestStepCodecApiPuStopRestartTunnel 
 *
 */
class RTestStepCodecApiPuStopRestartTunnel
  : public RTestStepCodecApiPuExecuteTunnel
	{
public:
	RTestStepCodecApiPuStopRestartTunnel();
	virtual TVerdict DoTestStepL();
	};

#endif // TSU_MDF_CODECAPI_PU_H
