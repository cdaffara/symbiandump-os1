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
// Header file: OpenMax related tests for PREQ1024 OpenMax Vorbis PU.
// 
//

/**
 @file tsu_mdf_omxvorbiscodecs_omx.h
*/

#ifndef TSU_MDF_OMXVORBISCODECS_OMX_H
#define TSU_MDF_OMXVORBISCODECS_OMX_H

#include "tsu_mdf_omxvorbiscodecs_step.h"
#include <mmf/server/mmfhwdevice.h>
#include <mdf/mdfprocessingunit.h>
#include <oggvorbiscodec/oggutil.h>

class RTestStepOmxVorbisCodecs;

/**
 * 
 * Test Case : MM-MDF-OMXVORBISCODECS-U-0020-HP
 * General testing relating to OpenMax layer
 *
 * @class RTestStepOmxVorbisCodecOmx 
 *
 */
class RTestStepOmxVorbisCodecOmx
  : public RTestStepOmxVorbisCodecs,
  	public MMdfProcessingUnitObserver,
    public MMdfInputPortObserver,
    public MMdfOutputPortObserver
	{
public:
	// MMdfProcessingUnitObserver
	void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);

	// MMdfInputPortObserver
	virtual void MipoWriteDataComplete(const MMdfInputPort* /*aInputPort */,
		CMMFBuffer* /* aBuffer */, TInt /* aErrorCode */) { }
	virtual void MipoDisconnectTunnelComplete(const MMdfInputPort* /*aInputPort */,
		TInt /* aErrorCode */) { }
	virtual void MipoRestartTunnelComplete(const MMdfInputPort* /*aInputPort */,
		TInt /* aErrorCode */) { }
	
	// MMdfOutputPortObserver
	virtual void MopoReadDataComplete(const MMdfOutputPort* /* aOutputPort */,
		CMMFBuffer* /* aBuffer */, TInt /* aErrorCode */) { }
	virtual void MopoDisconnectTunnelComplete(const MMdfOutputPort* /* aOutputPort */,
		TInt /* aErrorCode */) { }
	virtual void MopoRestartTunnelComplete(const MMdfOutputPort* /* aOutputPort */,
		TInt /* aErrorCode */) { }

	RTestStepOmxVorbisCodecOmx();
	virtual TVerdict DoTestStepL();

private:
	TVerdict iVerdict;
	};

#endif // TSU_MDF_OMXVORBISCODECS_OMX_H


