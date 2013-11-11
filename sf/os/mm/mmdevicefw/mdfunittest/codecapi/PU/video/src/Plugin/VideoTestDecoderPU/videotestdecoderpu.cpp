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
#include <mmf/devvideo/devvideopuconfig.h>
#include "videotestdecoderpu.hrh"
#include "videotestdecoderpu.h"

const TInt KMaxFrameSizeX = 640;
const TInt KMaxFrameSizeY = 480;
// For TVideoPictureHeader structure default settings
const TInt KMinusOne = -1;
const TInt KZero = 0;
// For Port indexes
const TInt KZerothPort = 0;
const TInt KFirstPort = 1;

enum TVideoTestDecoderPuPanics
	{
	EObserverNotSet,
	};
	
// ------------------------------------------------------------------------------------------	
// CVideoTestDecoderProcessor
	
CVideoTestDecoderProcessor::CVideoTestDecoderProcessor(CVideoTestDecoderProcessingUnit& aParent) 
	: CActive(EPriorityNormal), iParent(aParent)
	{
	}
	
CVideoTestDecoderProcessor::~CVideoTestDecoderProcessor()
	{
	Cancel();
	}
	

CVideoTestDecoderProcessor* CVideoTestDecoderProcessor::NewL(CVideoTestDecoderProcessingUnit& aParent)
	{
	CVideoTestDecoderProcessor* self = new (ELeave) CVideoTestDecoderProcessor(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CVideoTestDecoderProcessor::ConstructL()
	{
	CActiveScheduler::Add(this);	
	}

void CVideoTestDecoderProcessor::DoSetActive(TBool aRequest) 
	{
	SetActive();
	if(aRequest) 
		{
		iStatus = KRequestPending;
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}


void CVideoTestDecoderProcessor::Trigger() 
	{
	if (!IsActive())
		{
		DoSetActive(ETrue);
		}
	}
void CVideoTestDecoderProcessor::RunL()
	{
	iParent.ProcessL();	
	}

void CVideoTestDecoderProcessor::DoCancel()
	{
	}

// ------------------------------------------------------------------------------------------	
// MMdfInputPort

TInt CVideoTestDecoderProcessingUnit::MipConfigure(const TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}
	
TInt CVideoTestDecoderProcessingUnit::MipGetConfig(TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}



void CVideoTestDecoderProcessingUnit::MipInitialize()
	{
	}

void CVideoTestDecoderProcessingUnit::MipSetObserver(const MMdfInputPortObserver& aObserver)
	{
	iInputPortObserver = const_cast<MMdfInputPortObserver*>(&aObserver);
	}

TInt CVideoTestDecoderProcessingUnit::MipTunnelRequest(const MMdfOutputPort& /* aOutputPortToBeConnectedTo */,
	TTunnelFlags& /* aTunnelFlags */, TSupplierType& /* aSupplierType */)
	{
	return KErrNotSupported;
	}

void CVideoTestDecoderProcessingUnit::MipWriteData(CMMFBuffer& aInputBuffer)
	{
	TInt err = iInputBuffers.Append(&aInputBuffer);
	if (err == KErrNone)
		{
		if (iState == EProcessingUnitExecuting
		    && iOutputBuffers.Count() > 0 
		    && !iProcessor->IsActive())
			{
			iProcessor->DoSetActive(ETrue);
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

TBool CVideoTestDecoderProcessingUnit::MipIsTunnelled() const
	{
	return EFalse;	
	}

TInt CVideoTestDecoderProcessingUnit::MipIndex() const
	{
	return KZerothPort;
	}

CMMFBuffer* CVideoTestDecoderProcessingUnit::MipCreateBuffer(TInt /*aBufferSize*/)
	{
	return NULL;
	}

	
TInt CVideoTestDecoderProcessingUnit::MipUseBuffer(CMMFBuffer& /*aBuffer*/)
	{
	// this should not return an error
	return KErrNone;
	}

TInt CVideoTestDecoderProcessingUnit::MipFreeBuffer(CMMFBuffer* /*aBuffer*/)
	{
	return KErrNotSupported;
	}

void CVideoTestDecoderProcessingUnit::MipDisconnectTunnel()
	{
	// not applicable - the decoder is a dumb device
	}


void CVideoTestDecoderProcessingUnit::MipRestartTunnel()
	{
	// not applicable - the decoder is a dumb device
	}


TUint32 CVideoTestDecoderProcessingUnit::MipBufferSize() const
	{
	return iInputBufferSize;
	}

TInt CVideoTestDecoderProcessingUnit::MipCreateCustomInterface(TUid aUid)
	{
	return CreateCustomInterface(aUid);
	}
	
TAny* CVideoTestDecoderProcessingUnit::MipCustomInterface(TUid aUid)
	{
	return CustomInterface(aUid);
	}

// ------------------------------------------------------------------------------------------
// MMdfOutputPort
	
TInt CVideoTestDecoderProcessingUnit::MopConfigure(const TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}
	
TInt CVideoTestDecoderProcessingUnit::MopGetConfig(TPuConfig& /* aConfig */)
	{
	return KErrNotSupported;
	}

void CVideoTestDecoderProcessingUnit::MopInitialize()
	{
	}

void CVideoTestDecoderProcessingUnit::MopSetObserver(const MMdfOutputPortObserver& aObserver)
	{
	iOutputPortObserver = const_cast<MMdfOutputPortObserver*>(&aObserver);
	}

CMMFBuffer* CVideoTestDecoderProcessingUnit::MopCreateBuffer(TInt /* aBufferSize */)
	{
	// CVideoTestDecoderProcessingUnit does not own or create buffers
	return NULL;
	}
	
TInt CVideoTestDecoderProcessingUnit::MopUseBuffer(CMMFBuffer& /* aBuffer */) 
	{
	// CVideoTestDecoderProcessingUnit does not own or create buffers
	// BUT this should not return an error
	return KErrNone;
	}

TInt CVideoTestDecoderProcessingUnit::MopFreeBuffer(CMMFBuffer* /* aBuffer */) 
	{
	// CVideoTestDecoderProcessingUnit does not own or create buffers
	return KErrNotSupported;
	}

TInt CVideoTestDecoderProcessingUnit::MopTunnelRequest(const MMdfInputPort& /* aInputPortToBeConnectedTo */,
	TTunnelFlags& /* aTunnelFlags */, TSupplierType& /* aSupplierType */)
	{
	return KErrNotSupported;
	}

void CVideoTestDecoderProcessingUnit::MopReadData(CMMFBuffer& aOutputBuffer)
	{
	TInt err = iOutputBuffers.Append(&aOutputBuffer);

	if (err == KErrNone)
		{
		if (iState == EProcessingUnitExecuting
		    && iInputBuffers.Count() > 0 
		    && !iProcessor->IsActive())
			{
			iProcessor->DoSetActive(ETrue);
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

TBool CVideoTestDecoderProcessingUnit::MopIsTunnelled() const
	{
	return EFalse;	
	}

TInt CVideoTestDecoderProcessingUnit::MopIndex() const
	{
	return KFirstPort;
	}
	
void CVideoTestDecoderProcessingUnit::MopDisconnectTunnel()
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
	
void CVideoTestDecoderProcessingUnit::MopRestartTunnel()
	{
	}
	
TUint32 CVideoTestDecoderProcessingUnit::MopBufferSize() const
	{
	return iOutputBufferSize;
	}

TInt CVideoTestDecoderProcessingUnit::MopCreateCustomInterface(TUid aUid)
	{
	return CreateCustomInterface(aUid);
	}
	
TAny* CVideoTestDecoderProcessingUnit::MopCustomInterface(TUid aUid)
	{
	return CustomInterface(aUid);
	}

// ------------------------------------------------------------------------------------------
// custom interface

TInt CVideoTestDecoderProcessingUnit::CreateCustomInterface(TUid /*aUid*/)
	{
	return KErrNotSupported;	
	}
	
TAny* CVideoTestDecoderProcessingUnit::CustomInterface(TUid /*aUid*/)
	{
	return NULL;
	}

// ------------------------------------------------------------------------------------------
// CVideoTestDecoderProcessingUnit Implementation

CVideoTestDecoderProcessingUnit* CVideoTestDecoderProcessingUnit::NewL()
	{
	CVideoTestDecoderProcessingUnit* self = new (ELeave) CVideoTestDecoderProcessingUnit;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CVideoTestDecoderProcessingUnit::CVideoTestDecoderProcessingUnit()
	{
	iFrameRate = 0;
	iFrameSize = TSize(KMaxFrameSizeX, KMaxFrameSizeY);
	iImageFormat.iDataFormat = (TImageDataFormat)0;
	}
	
void CVideoTestDecoderProcessingUnit::ConstructL()
	{
	// don't create the decoder itself until we initialize
	
	iProcessor = CVideoTestDecoderProcessor::NewL(*this);
	iState = EProcessingUnitLoaded;
	}
	
CVideoTestDecoderProcessingUnit::~CVideoTestDecoderProcessingUnit()
	{
	if(iProcessor) 
		{
		iProcessor->Cancel();
		}
	iInputBuffers.Close();
	iOutputBuffers.Close();

	delete iProcessor;
	}

TInt CVideoTestDecoderProcessingUnit::Create(const MMdfProcessingUnitObserver& aObserver)
	{
	iObserver = const_cast<MMdfProcessingUnitObserver*>(&aObserver);
	return KErrNone;
	}

TInt CVideoTestDecoderProcessingUnit::GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts)
	{
	return aComponentInputPorts.Append(this);
	}


TInt CVideoTestDecoderProcessingUnit::GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts)
	{
	return aComponentOutputPorts.Append(this);
	}

TInt CVideoTestDecoderProcessingUnit::Configure(const TPuConfig& aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidDevVideoPlayConfig))
		{
		const TDevVideoPlayPuConfig* config = TPuConfigVideoPlayback::GetStructure(aConfig);	
		// NB: in the decoder, the frame rate and size will come from the file itself.
		iImageFormat = config->iImageFormat;		
		return KErrNone;
		}
	else if (aConfig.Uid() == TUid::Uid(KUidDevVideoHeaderConfig))
		{
		const TDevVideoHeaderPuConfig* config = TPuConfigVideoHeader::GetStructure(aConfig);	
		// we support basic bitstream data only
		if(config->iDataUnitType != EDuArbitraryStreamSection) 
			{
			return KErrNotSupported;
			}
	
		if(config->iDataUnitEncapsulation != EDuElementaryStream) 
			{
			return KErrNotSupported;
			}

		TRAPD(err, ProcessHeaderL(config->iHeaderData));
		return err;
		}
	return KErrNotSupported;
	}
	
	
void CVideoTestDecoderProcessingUnit::ProcessHeaderL(const TDesC8& aHeader)
	{
	const TTestDecoderHeaderStructure* header = reinterpret_cast<const TTestDecoderHeaderStructure*>(aHeader.Ptr());
	if (header->iHeaderMarker==KIDHeaderMarker)
		{
		iFrameSize.iWidth = header->iFrameWidth;
		iFrameSize.iHeight = header->iFrameHeight;	
		
		iOutputBufferSize = iFrameSize.iWidth*iFrameSize.iHeight*3;
		iInputBufferSize = iOutputBufferSize + sizeof(TTestDecoderHeaderStructure);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	
	}
	
TInt CVideoTestDecoderProcessingUnit::GetConfig(TPuConfig& aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidDevVideoPictureHeaderConfig))
		{
		TVideoPictureHeader* header = TPuConfigVideoPictureHeader::GetStructure(aConfig);
		
		header->iSizeInMemory = iFrameSize;
		header->iDisplayedRect = TRect(iFrameSize);
		header->iOptions = KZero;
		header->iProfile = KMinusOne;
		header->iLevel = KMinusOne;
		header->iVersion = KMinusOne;
		header->iOptional = NULL;
		header->iIsRandomAccessible = KZero;

		return KErrNone;
		}
	return KErrNotSupported;
	}

void CVideoTestDecoderProcessingUnit::Initialize()
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(_L("CVideoTestDecoderProcessingUnit:: panic"), EObserverNotSet));
	
	TInt err = KErrNone;
			
	iObserver->InitializeComplete(this, err);
	iState = EProcessingUnitIdle;
	}

void CVideoTestDecoderProcessingUnit::Execute()
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(_L("CVideoTestDecoderProcessingUnit"), EObserverNotSet));

	if(iState != EProcessingUnitInvalid) 
		{
		iState = EProcessingUnitExecuting;
		if (!iProcessor->IsActive() && iInputBuffers.Count() > 0 && iOutputBuffers.Count() > 0)
			{
			iProcessor->DoSetActive(ETrue);
			}
		}
	}

TInt CVideoTestDecoderProcessingUnit::Pause()
	{
	iState = EProcessingUnitPaused;
	return KErrNone;	
	}

void CVideoTestDecoderProcessingUnit::Stop()
	{
	iState = EProcessingUnitIdle;	
	}

TProcessingUnitState CVideoTestDecoderProcessingUnit::State()
	{
	return iState;
	}

// -----------------------------------------------------------------------------
// processor callback

void CVideoTestDecoderProcessingUnit::ProcessL()
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
				// stub decoder : we do nothing but write the contents
				// of the input buffer to the output buffer.
								
 				TDes8& inputBufferDes = (static_cast<CMMFDataBuffer*>(iCurrentInputBuffer))->Data();
 				TDes8& outputBufferDes = (static_cast<CMMFDataBuffer*>(iCurrentOutputBuffer))->Data();
 				
				iCurrentOutputBuffer->SetLastBuffer(iCurrentInputBuffer->LastBuffer());

 				if (inputBufferDes.Length()==0)
 					{
 					// if the length is zero, it definitely is the last bufvfer
 					iCurrentOutputBuffer->SetLastBuffer(ETrue);
 					outputBufferDes.SetLength(0);
 					}
 				else
 					{
	 				TPtr8 input = inputBufferDes.MidTPtr(sizeof(TTestDecoderHeaderStructure));
	 				outputBufferDes.Copy(input);
					
					iCurrentInputBuffer->SetPosition(inputBufferDes.Length());
 					}
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
	IMPLEMENTATION_PROXY_ENTRY(KUidVideoTestDecoderPu,	CVideoTestDecoderProcessingUnit::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
    
// -----------------------------------------------------------------------------
// end
