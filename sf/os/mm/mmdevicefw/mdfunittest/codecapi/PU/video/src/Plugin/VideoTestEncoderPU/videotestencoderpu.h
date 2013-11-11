// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef VIDEOTESTENCODERPU_H
#define VIDEOTESTENCODERPU_H

#include <e32msgqueue.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>
#include <mdf/mdfcommon.h>
#include <mmf/devvideo/devvideopuconfig.h>

class CVideoTestEncoderProcessingUnit;
class CMMFBuffer;


const TInt KIDHeaderMarker = 0x01020304;

class TTestDecoderHeaderStructure
	{
public:
	TUint32 iHeaderMarker;
	TUint32 iFrameWidth;
	TUint32 iFrameHeight;
	};


// --------------------------------------------------------------------
// CVideoTestEncoderProcessor : active object for CVideoTestEncoderProcessingUnit

class CVideoTestEncoderProcessor 
	: public CActive
	{
public:	
	static CVideoTestEncoderProcessor* NewL(CVideoTestEncoderProcessingUnit& aParent);
	~CVideoTestEncoderProcessor();
	void RunL();
	void DoCancel();
	void DoSetActive(TBool aRequest);
	void Trigger();
private:	
	CVideoTestEncoderProcessor(CVideoTestEncoderProcessingUnit& aParent);
	void ConstructL();
private:
	CVideoTestEncoderProcessingUnit& 		iParent;
	};

// --------------------------------------------------------------------
// CVideoTestEncoderProcessingUnit

// this is a stub video encoder processing unit

class CVideoTestEncoderProcessingUnit 
	: public CMdfProcessingUnit,
	  public MMdfInputPort,
	  public MMdfOutputPort
	  
	{
public:		

	// MMdfInputPort
	TInt MipConfigure(const TPuConfig& aConfiguration);
	TInt MipGetConfig(TPuConfig& aConfiguration);
	void MipInitialize();
	void MipSetObserver(const MMdfInputPortObserver& aInputPortObserver);
	CMMFBuffer* MipCreateBuffer(TInt aBufferSize);
	TInt MipUseBuffer(CMMFBuffer& aBuffer);
	TInt MipFreeBuffer(CMMFBuffer* aBuffer);
	TInt MipTunnelRequest(const MMdfOutputPort& aOutputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
	void MipWriteData(CMMFBuffer& aInputBuffer);
	void MipDisconnectTunnel();
	void MipRestartTunnel();
	TBool MipIsTunnelled() const;
	TInt MipIndex() const;
	TUint32 MipBufferSize() const;
	TInt MipCreateCustomInterface(TUid aUid);
	TAny* MipCustomInterface(TUid aUid);

	// from MMdfOutputPort
	TInt MopConfigure(const TPuConfig&  aConfiguration);
	TInt MopGetConfig(TPuConfig& aConfiguration);
	void MopInitialize();
	void MopSetObserver(const MMdfOutputPortObserver& aOutputPortObserver);
	virtual CMMFBuffer* MopCreateBuffer(TInt aBufferSize);
	TInt MopUseBuffer(CMMFBuffer& aBuffer);
	TInt MopFreeBuffer(CMMFBuffer* aBuffer);
	TInt MopTunnelRequest(const MMdfInputPort& aInputPortToBeConnectedTo,
		TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
	void MopReadData(CMMFBuffer& aOutputBuffer);
	void MopDisconnectTunnel();
	void MopRestartTunnel();
	TBool MopIsTunnelled() const;
	TInt MopIndex() const;
	TUint32 MopBufferSize() const;
	TInt MopCreateCustomInterface(TUid aUid);
	TAny* MopCustomInterface(TUid aUid);

	// from both MMdfInputPort and MMdfOutputPort
	TInt CreateCustomInterface(TUid aUid);
	TAny* CustomInterface(TUid aUid);

	// CVideoTestEncoderProcessingUnit
	static CVideoTestEncoderProcessingUnit* NewL();
	~CVideoTestEncoderProcessingUnit();
	TInt Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver);
	TInt GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts);
	TInt GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts);
	TInt Configure(const TPuConfig& aConfigurationSetup);
	TInt GetConfig(TPuConfig& aConfiguration);
	void Initialize();
	void Execute(); 
	TInt Pause();
	void Stop();
	TProcessingUnitState State();
	
	void ProcessL();
	MMdfProcessingUnitObserver* Observer();
	
private:
	CVideoTestEncoderProcessingUnit();	
	void ConstructL();

private:
	MMdfProcessingUnitObserver* 	iObserver;
	MMdfInputPortObserver* 			iInputPortObserver;
	MMdfOutputPortObserver* 		iOutputPortObserver;
	
	TProcessingUnitState 			iState;
	CVideoTestEncoderProcessor*		iProcessor;
	
	CMMFBuffer* 					iCurrentInputBuffer;
	RPointerArray<CMMFBuffer> 		iInputBuffers;
	TUint 							iInputBufferSize;

	CMMFBuffer* 					iCurrentOutputBuffer;
	RPointerArray<CMMFBuffer> 		iOutputBuffers;
	TUint 							iOutputBufferSize;

	TUncompressedVideoFormat		iImageFormat;
	TSize							iFrameSize;
	TUint							iFrameRate;
	};

// --------------------------------------------------------------------
	
#endif // VIDEOTESTENCODERPU_H
