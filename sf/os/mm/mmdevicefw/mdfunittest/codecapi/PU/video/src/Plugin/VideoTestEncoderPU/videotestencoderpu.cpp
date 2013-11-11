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

#include <e32debug.h>
#include <ecom/implementationproxy.h>
#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>
#include <mdf/mdfpuconfig.h>
#include <mmf/server/mmfdatabuffer.h>
#include "videotestencoderpu.hrh"
#include "videotestencoderpu.h"

// For Port indexes
const TInt KZerothPort = 0;
const TInt KFirstPort = 1;


enum TVideoTestEncoderPuPanics
	{
	EObserverNotSet
	};
	
// ------------------------------------------------------------------------------------------	
// CVideoTestEncoderProcessor
	
CVideoTestEncoderProcessor::CVideoTestEncoderProcessor(CVideoTestEncoderProcessingUnit& aParent) 
	: CActive(EPriorityNormal), iParent(aParent)
	{
	}
	
CVideoTestEncoderProcessor::~CVideoTestEncoderProcessor()
	{
	Cancel();
	}

CVideoTestEncoderProcessor* CVideoTestEncoderProcessor::NewL(CVideoTestEncoderProcessingUnit& aParent)
	{
	CVideoTestEncoderProcessor* self = new (ELeave) CVideoTestEncoderProcessor(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CVideoTestEncoderProcessor::ConstructL()
	{
	CActiveScheduler::Add(this);	
	}

void CVideoTestEncoderProcessor::DoSetActive(TBool aRequest) 
	{
	SetActive();
	if(aRequest) 
		{
		iStatus = KRequestPending;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}


void CVideoTestEncoderProcessor::Trigger() 
	{
	if (!IsActive())
		{
		DoSetActive(ETrue);
		}
	}

void CVideoTestEncoderProcessor::RunL()
	{
	iParent.ProcessL();	
	}

void CVideoTestEncoderProcessor::DoCancel()
	{
	}

// ------------------------------------------------------------------------------------------	
// MMdfInputPort

TInt CVideoTestEncoderProcessingUnit::MipConfigure(const TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}


TInt CVideoTestEncoderProcessingUnit::MipGetConfig(TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}

void CVideoTestEncoderProcessingUnit::MipInitialize()
	{
	}

void CVideoTestEncoderProcessingUnit::MipSetObserver(const MMdfInputPortObserver& aObserver)
	{
	iInputPortObserver = const_cast<MMdfInputPortObserver*>(&aObserver);
	}

TInt CVideoTestEncoderProcessingUnit::MipTunnelRequest(const MMdfOutputPort& /* aOutputPortToBeConnectedTo */,
	TTunnelFlags& /* aTunnelFlags */, TSupplierType& /* aSupplierType */)
	{
	return KErrNotSupported;
	}

void CVideoTestEncoderProcessingUnit::MipWriteData(CMMFBuffer& aInputBuffer)
	{
	TInt err = iInputBuffers.Append(&aInputBuffer);
	if (err == KErrNone)
		{
		if (iState == EProcessingUnitExecuting
		    && iOutputBuffers.Count() > 0 
		    && !iProcessor->IsActive())
			{
			iProcessor->Trigger();
			}
		}
	else
		{
		// the device can't append the buffer
		iState = EProcessingUnitInvalid;
		if (iInputPortObserver)
			{
			iInputPortObserver->MipoWriteDataComplete(this, &aInputBuffer, err);
			}
		}
	}

TBool CVideoTestEncoderProcessingUnit::MipIsTunnelled() const
	{
	return EFalse;	
	}

TInt CVideoTestEncoderProcessingUnit::MipIndex() const
	{
	return KZerothPort;
	}

CMMFBuffer* CVideoTestEncoderProcessingUnit::MipCreateBuffer(TInt /* aBufferSize */)
	{
	return NULL;
	}

	
TInt CVideoTestEncoderProcessingUnit::MipUseBuffer(CMMFBuffer& /* aBuffer */)
	{
	// this should not return an error
	return KErrNone;
	}

	
TInt CVideoTestEncoderProcessingUnit::MipFreeBuffer(CMMFBuffer* /* aBuffer */) 
	{
	// CVideoTestEncoderProcessingUnit does not own or create buffers
	return KErrNotSupported;
	}

void CVideoTestEncoderProcessingUnit::MipDisconnectTunnel()
	{
	// not applicable - the encoder is a dumb device
	}


void CVideoTestEncoderProcessingUnit::MipRestartTunnel()
	{
	// not applicable - the encoder is a dumb device
	}


TUint32 CVideoTestEncoderProcessingUnit::MipBufferSize() const
	{
	return iInputBufferSize;
	}

TInt CVideoTestEncoderProcessingUnit::MipCreateCustomInterface(TUid aUid)
	{
	return CreateCustomInterface(aUid);
	}
	
TAny* CVideoTestEncoderProcessingUnit::MipCustomInterface(TUid aUid)
	{
	return CustomInterface(aUid);
	}

// ------------------------------------------------------------------------------------------
// MMdfOutputPort
	
TInt CVideoTestEncoderProcessingUnit::MopConfigure(const TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}

TInt CVideoTestEncoderProcessingUnit::MopGetConfig(TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}

void CVideoTestEncoderProcessingUnit::MopInitialize()
	{
	}

void CVideoTestEncoderProcessingUnit::MopSetObserver(const MMdfOutputPortObserver& aObserver)
	{
	iOutputPortObserver = const_cast<MMdfOutputPortObserver*>(&aObserver);
	}

CMMFBuffer* CVideoTestEncoderProcessingUnit::MopCreateBuffer(TInt /* aBufferSize */)
	{
	// CVideoTestEncoderProcessingUnit does not own or create buffers
	return NULL;
	}
	
TInt CVideoTestEncoderProcessingUnit::MopUseBuffer(CMMFBuffer& /* aBuffer */) 
	{
	// CVideoTestEncoderProcessingUnit does not own or create buffers
	// BUT this should not return an error
	return KErrNone;
	}

TInt CVideoTestEncoderProcessingUnit::MopFreeBuffer(CMMFBuffer* /* aBuffer */) 
	{
	// CVideoTestEncoderProcessingUnit does not own or create buffers
	return KErrNotSupported;
	}

TInt CVideoTestEncoderProcessingUnit::MopTunnelRequest(const MMdfInputPort& /* aInputPortToBeConnectedTo */,
	TTunnelFlags& /* aTunnelFlags */, TSupplierType& /* aSupplierType */)
	{
	return KErrNotSupported;
	}

void CVideoTestEncoderProcessingUnit::MopReadData(CMMFBuffer& aOutputBuffer)
	{
	TInt err = iOutputBuffers.Append(&aOutputBuffer);

	if (err == KErrNone)
		{
		if (iState == EProcessingUnitExecuting
		    && iInputBuffers.Count() > 0 
		    && !iProcessor->IsActive())
			{
			iProcessor->Trigger();
			}
		}
	else
		{
		// the device can't append the buffer
		iState = EProcessingUnitInvalid;
		if (iOutputPortObserver)
			{
			iOutputPortObserver->MopoReadDataComplete(this, &aOutputBuffer, err);
			}
		}
	}

TBool CVideoTestEncoderProcessingUnit::MopIsTunnelled() const
	{
	return EFalse;	
	}

TInt CVideoTestEncoderProcessingUnit::MopIndex() const
	{
	return KFirstPort;
	}
	
void CVideoTestEncoderProcessingUnit::MopDisconnectTunnel()
	{
	// Find the last buffer in the array and set it as the 'LastBuffer'
	TInt c = iOutputBuffers.Count();
	if(c > 0)
		{
		(iOutputBuffers[c-1])->SetLastBuffer(ETrue);		
		}
	// Still have to send callback and cancel driver
	iOutputPortObserver->MopoDisconnectTunnelComplete(this, KErrNone);				
	}
	
void CVideoTestEncoderProcessingUnit::MopRestartTunnel()
	{
	}
	
TUint32 CVideoTestEncoderProcessingUnit::MopBufferSize() const
	{
	return iOutputBufferSize;
	}

TInt CVideoTestEncoderProcessingUnit::MopCreateCustomInterface(TUid aUid)
	{
	return CreateCustomInterface(aUid);
	}
	
TAny* CVideoTestEncoderProcessingUnit::MopCustomInterface(TUid aUid)
	{
	return CustomInterface(aUid);
	}

// ------------------------------------------------------------------------------------------
// custom interface

TInt CVideoTestEncoderProcessingUnit::CreateCustomInterface(TUid /* aUid */)
	{
	return KErrNotSupported;	
	}
	
TAny* CVideoTestEncoderProcessingUnit::CustomInterface(TUid /* aUid */)
	{
	return NULL;
	}

// ------------------------------------------------------------------------------------------
// CVideoTestEncoderProcessingUnit Implementation

CVideoTestEncoderProcessingUnit* CVideoTestEncoderProcessingUnit::NewL()
	{
	CVideoTestEncoderProcessingUnit* self = new (ELeave) CVideoTestEncoderProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CVideoTestEncoderProcessingUnit::CVideoTestEncoderProcessingUnit()
	{
	iFrameRate = 0;
	iFrameSize = TSize(0,0);
	iImageFormat.iDataFormat = (TImageDataFormat)0;
	}
	
void CVideoTestEncoderProcessingUnit::ConstructL()
	{
	// don't create the encoder itself until we initialize
	
	iProcessor = CVideoTestEncoderProcessor::NewL(*this);
	iState = EProcessingUnitLoaded;
	}
	
CVideoTestEncoderProcessingUnit::~CVideoTestEncoderProcessingUnit()
	{
	if(iProcessor) 
		{
		iProcessor->Cancel();
		}
	iInputBuffers.Close();
	iOutputBuffers.Close();

	delete iProcessor;
	}

TInt CVideoTestEncoderProcessingUnit::Create(const MMdfProcessingUnitObserver& aObserver)
	{
	iObserver = const_cast<MMdfProcessingUnitObserver*>(&aObserver);
	return KErrNone;
	}

TInt CVideoTestEncoderProcessingUnit::GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts)
	{
	return aComponentInputPorts.Append(this);
	}


TInt CVideoTestEncoderProcessingUnit::GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts)
	{
	return aComponentOutputPorts.Append(this);
	}
	
TInt CVideoTestEncoderProcessingUnit::Configure(const TPuConfig& aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidDevVideoRecordConfig))
		{
		const TDevVideoRecordPuConfig* config = TPuConfigDevVideoRecord::GetStructure(aConfig);	
		iFrameRate = config->iFrameRate;
		iFrameSize = config->iFrameSize;
		iImageFormat = config->iImageFormat;
		
		iInputBufferSize = iFrameSize.iWidth * iFrameSize.iHeight*3; // 4k
		iOutputBufferSize = sizeof(TTestDecoderHeaderStructure) + iInputBufferSize;

		return KErrNone;
		}
	return KErrNotSupported;
	}
	
TInt CVideoTestEncoderProcessingUnit::GetConfig(TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}

	
void CVideoTestEncoderProcessingUnit::Initialize()
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(_L("CVideoTestEncoderProcessingUnit:: panic"), EObserverNotSet));
			
	iObserver->InitializeComplete(this, KErrNone);
	iState = EProcessingUnitIdle;
	}

void CVideoTestEncoderProcessingUnit::Execute()
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(_L("CVideoTestEncoderProcessingUnit"), EObserverNotSet));

	if(iState != EProcessingUnitInvalid) 
		{
		iState = EProcessingUnitExecuting;
		if (!iProcessor->IsActive() && iInputBuffers.Count() > 0 && iOutputBuffers.Count() > 0)
			{
			iProcessor->Trigger();
			}
		}
	}

TInt CVideoTestEncoderProcessingUnit::Pause()
	{
	iState = EProcessingUnitPaused;
	return KErrNone;	
	}

void CVideoTestEncoderProcessingUnit::Stop()
	{
	iState = EProcessingUnitIdle;	
	}

TProcessingUnitState CVideoTestEncoderProcessingUnit::State()
	{
	return iState;
	}

// -----------------------------------------------------------------------------
// processor callback

void CVideoTestEncoderProcessingUnit::ProcessL()
	{
	if (iProcessor->iStatus == KErrNone)
		{
		if (iInputBuffers.Count() > 0 && iOutputBuffers.Count() > 0)
			{
			iCurrentInputBuffer = iInputBuffers[0];
			iCurrentOutputBuffer = iOutputBuffers[0];
			iInputBuffers.Remove(0);
			iOutputBuffers.Remove(0);
			
			if (CMMFBuffer::IsSupportedDataBuffer(iCurrentInputBuffer->Type())
			    && CMMFBuffer::IsSupportedDataBuffer(iCurrentOutputBuffer->Type()))
				{
				// stub encoder : we do nothing but write the contents
				// of the input buffer to the output buffer.
								
 				TDesC8& inputBufferDes = (static_cast<CMMFDataBuffer*>(iCurrentInputBuffer))->Data();
 				TDes8& outputBufferDes = (static_cast<CMMFDataBuffer*>(iCurrentOutputBuffer))->Data();
 				
 				outputBufferDes.SetLength(outputBufferDes.MaxLength());
 				TTestDecoderHeaderStructure* header = reinterpret_cast<TTestDecoderHeaderStructure*>(&outputBufferDes[0]);
 				header->iHeaderMarker = KIDHeaderMarker;
 				header->iFrameWidth = iFrameSize.iWidth;
 				header->iFrameHeight = iFrameSize.iHeight;
 				TPtr8 output = outputBufferDes.MidTPtr(sizeof(TTestDecoderHeaderStructure));
 				output.Copy(inputBufferDes);

 				if (iInputPortObserver)
					{		
					iInputPortObserver->MipoWriteDataComplete(this, iCurrentInputBuffer, iProcessor->iStatus.Int());
					}
				if (iOutputPortObserver)
					{		
					iOutputPortObserver->MopoReadDataComplete(this, iCurrentOutputBuffer, iProcessor->iStatus.Int());
					}
				}
			}
		// Only process one buffer at a time, if more available, retrigger AO
		if (iInputBuffers.Count() > 0 && iOutputBuffers.Count() > 0)
			{
			iProcessor->Trigger();
			}
		}
	}

// -----------------------------------------------------------------------------
// ECOM Implementation table entry	
	
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidVideoTestEncoderPu,	CVideoTestEncoderProcessingUnit::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
    
// -----------------------------------------------------------------------------
// end
