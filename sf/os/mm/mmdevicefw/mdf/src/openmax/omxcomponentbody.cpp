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
 
#include <e32base.h>
#include <e32msgqueue.h>
#include <mmf/server/mmfdatabuffer.h>
#include "omxcomponentbody.h"
#include "omxcomponentimpl.h"

const TInt KMaxMsgQueueEntries = 10;
const TInt KMaxComponentNameLength = 256;

const TInt KSymbianErrors[] =
	{
	KErrNoMemory, /*OMX_ErrorInsufficientResources*/
	KErrUnknown,
	KErrBadName,
	KErrNotFound,
	KErrGeneral, /*OMX_ErrorInvalidComponent*/
	KErrArgument, /*OMX_ErrorBadParameter*/
	KErrNotSupported, /*OMX_ErrorNotImplemented*/
	KErrUnderflow, /*OMX_ErrorUnderflow*/
	KErrOverflow, /*OMX_ErrorOverflow*/
	KErrHardwareNotAvailable, /* OMX_ErrorHardware*/
	KErrGeneral, /*OMX_ErrorInvalidState*/
  	KErrCorrupt, /*OMX_ErrorStreamCorrupt*/
  	KErrArgument, /*OMX_ErrorPortsNotCompatible*/
  	KErrHardwareNotAvailable, /*OMX_ErrorResourcesLost*/
  	KErrCompletion, /*OMX_ErrorNoMore*/ 
  	KErrGeneral, /*OMX_ErrorVersionMismatch*/
  	KErrNotReady, /*OMX_ErrorNotReady*/
  	KErrTimedOut, /*OMX_ErrorTimeout*/
  	KErrNone /*OMX_ErrorSameState*/
	};

/**
 Converts an OpenMAX error code to a Symbian error code.
 @param		aErrorType The OpenMAX error code.
 @return	The Symbian error code.
 */
TInt ConvertOmxErrorType(OMX_ERRORTYPE aErrorType)
	{
	if (aErrorType == OMX_ErrorNone)
		{
		return KErrNone;
		}
	else if (aErrorType >= OMX_ErrorInsufficientResources &&
			  aErrorType <= OMX_ErrorSameState)
		{
		return KSymbianErrors[aErrorType - OMX_ErrorInsufficientResources];
		}
	else
		{
		return KErrGeneral;
		}
	}
	
/**
 Converts a Symbian error code to an OpenMAX error code.
 @param		aError The Symbian error code.
 @return	The OpenMAX error code.
 */
OMX_ERRORTYPE ConvertSymbianErrorType(TInt aError)
	{
	OMX_ERRORTYPE err = OMX_ErrorNone;
	switch (aError)
		{
	case KErrNone:
		err = OMX_ErrorNone;
		break;
	case KErrNoMemory:
		err = OMX_ErrorInsufficientResources;
		break;
	case KErrGeneral:
		break;
	default:
		err = OMX_ErrorUndefined;
		}
	return err;
	}


COmxBufferManager::COmxBufferManager(OMX_COMPONENTTYPE* aHandle)
	: iHandle(aHandle)
	{
	}
	
COmxBufferManager::COmxBuffer::COmxBuffer()
	{
	}
	
	
COmxBufferManager::COmxBuffer* COmxBufferManager::COmxBuffer::NewL(OMX_BUFFERHEADERTYPE* aBufferHeader, CMMFBuffer* aBuffer)
	{
	COmxBuffer* self = new (ELeave) COmxBuffer;
	CleanupStack::PushL(self);
	self->ConstructL(aBufferHeader, aBuffer);
	CleanupStack::Pop(self);
	return self;
	}
	
COmxBufferManager::COmxBuffer::~COmxBuffer()
	{
	if (iOwnsMmfBuffer)
		{
		delete iMmfBuffer;
		}
	}
	
	
CMMFBuffer* COmxBufferManager::COmxBuffer::MmfBuffer() const
	{
	return iMmfBuffer;
	}
	
OMX_BUFFERHEADERTYPE* COmxBufferManager::COmxBuffer::BufferHeader() const
	{
	return iBufferHeader;
	}


MOmxInputPortCallbacks* COmxBufferManager::COmxBuffer::InputPortCallbacks() const
	{
	return iInputPortCallbacks;
	}
	
MOmxOutputPortCallbacks* COmxBufferManager::COmxBuffer::OutputPortCallbacks() const
	{
	return iOutputPortCallbacks;
	}
	
void COmxBufferManager::COmxBuffer::SetInputPortCallbacks(MOmxInputPortCallbacks* aCallbacks)
	{
	iInputPortCallbacks = aCallbacks;
	}
	
void COmxBufferManager::COmxBuffer::SetOutputPortCallbacks(MOmxOutputPortCallbacks* aCallbacks)
	{
	iOutputPortCallbacks = aCallbacks;
	}
	
// look up the corresponding buffer
COmxBufferManager::COmxBuffer* COmxBufferManager::FindBuffer(const CMMFBuffer* aBuffer) const
	{
	COmxBuffer* buffer = NULL;
	for (TInt i=0;i<iBuffers.Count() && !buffer;i++)
		{
		if (iBuffers[i]->MmfBuffer() == aBuffer)
			{
			buffer = iBuffers[i];
			}
		}
	return buffer;
	}
	
COmxBufferManager::COmxBuffer* COmxBufferManager::FindBuffer(OMX_BUFFERHEADERTYPE* aBuffer) const
	{
	return reinterpret_cast<COmxBuffer*>(aBuffer->pAppPrivate);
	}

	
	
 TInt COmxBufferManager::UseBuffer(CMMFBuffer& aBuffer, TUint aPortIndex)
	{
	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer.Type()))
		{
		OMX_BUFFERHEADERTYPE* buffer;
		CMMFDataBuffer& dataBuffer = static_cast<CMMFDataBuffer&>(aBuffer);
				
        TDes8& aBufferDes = dataBuffer.Data();
        OMX_ERRORTYPE error = iHandle->UseBuffer(static_cast<OMX_HANDLETYPE>(iHandle), &buffer, aPortIndex, (void*)&aBuffer, aBufferDes.MaxLength(), const_cast<TUint8*>(aBufferDes.Ptr()));
    	if (error != OMX_ErrorNone)
    		{
    		return ConvertOmxErrorType(error);
    		}
		TRAPD(err, StoreBufferL(buffer, &aBuffer));
		return err;
		}
	else
		{
		return KErrNotSupported;
		}
	}


 CMMFBuffer* COmxBufferManager::AllocateBufferL(TUint aPortIndex, TUint aSizeBytes)
	{
	OMX_BUFFERHEADERTYPE* buffer;
	OMX_ERRORTYPE error = iHandle->AllocateBuffer(static_cast<OMX_HANDLETYPE>(iHandle), &buffer, aPortIndex, NULL, aSizeBytes);
	User::LeaveIfError(ConvertOmxErrorType(error));

	StoreBufferL(buffer,NULL); // transfers ownership

	// return the newly created buffer
	return FindBuffer(buffer)->MmfBuffer();
	}
	

 TInt COmxBufferManager::FreeBuffer(CMMFBuffer* aBuffer)
	{
	COmxBuffer* buffer;
	for (TInt i=0;i<iBuffers.Count();i++)
		{
		buffer = iBuffers[i];
		if (buffer->MmfBuffer() == aBuffer)
			{
			iBuffers.Remove(i);
			OMX_ERRORTYPE err = iHandle->FreeBuffer(static_cast<OMX_HANDLETYPE>(iHandle), 0, buffer->BufferHeader());
			delete buffer;
			return err;
			}
		}
	return KErrNotFound;
	}
	
void COmxBufferManager::COmxBuffer::ConstructL(OMX_BUFFERHEADERTYPE* aBufferHeader, CMMFBuffer* aBuffer)
	{
	
	// Now if CMMFBuffer is NULL, this is been called from allocate buffer, and we need to 
	// Allocate a ptr buffer to correspond to the buffer created by OMX
	ASSERT(aBufferHeader);
	iBufferHeader = aBufferHeader;	
	if (aBuffer == NULL)
		{
		TPtr8 ptr(iBufferHeader->pBuffer, iBufferHeader->nFilledLen, iBufferHeader->nAllocLen);
		CMMFBuffer* mmfBuffer = CMMFPtrBuffer::NewL(ptr);
		iMmfBuffer = mmfBuffer;
		iOwnsMmfBuffer = ETrue;
		}
	else
		{
		iMmfBuffer = aBuffer;
		}
		
	// store pointer to element in array
	iBufferHeader->pAppPrivate = this;
	}



// Store OMX buffer pointer
void COmxBufferManager::StoreBufferL(OMX_BUFFERHEADERTYPE* aBufferHeader, CMMFBuffer* aBuffer)
	{
	COmxBuffer* buf = COmxBuffer::NewL(aBufferHeader, aBuffer);
	CleanupStack::PushL(buf);
	iBuffers.AppendL(buf);
	CleanupStack::Pop(buf);
	}
	
	
 TInt COmxBufferManager::EmptyThisBuffer(const CMMFBuffer* aBuffer, MOmxInputPortCallbacks* aObserver)
	{
	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		COmxBuffer* omxBuffer = FindBuffer(aBuffer);
		if (!omxBuffer)
			{
			return KErrNotFound;
			}

		omxBuffer->SetInputPortCallbacks(aObserver);
		OMX_BUFFERHEADERTYPE* bufferHeader = omxBuffer->BufferHeader();
		const CMMFDataBuffer* buf = static_cast<const CMMFDataBuffer*>(aBuffer);
		const TDesC8& des = buf->Data();
		bufferHeader->nFilledLen = des.Length();
		bufferHeader->nFlags = 0;
		if (aBuffer->LastBuffer())
			{	
			bufferHeader->nFlags |= OMX_BUFFERFLAG_EOS;
			}
		else
			{
			bufferHeader->nFlags &= ~OMX_BUFFERFLAG_EOS;
			}
		return ConvertOmxErrorType(iHandle->EmptyThisBuffer(static_cast<OMX_HANDLETYPE>(iHandle), bufferHeader));
		}
	else
		{
		return KErrNotSupported;
		}
	}


 TInt COmxBufferManager::FillThisBuffer(CMMFBuffer* aBuffer, MOmxOutputPortCallbacks* aObserver)
	{
	if (CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		COmxBuffer* omxBuffer = FindBuffer(aBuffer);
		if (!omxBuffer)
			{
			return KErrNotFound;
			}
		omxBuffer->SetOutputPortCallbacks(aObserver);
		OMX_BUFFERHEADERTYPE* bufferHeader = omxBuffer->BufferHeader();
		
		bufferHeader->nFilledLen = 0;
		// clear last buffer flag
		bufferHeader->nFlags &= ~OMX_BUFFERFLAG_EOS;
		return ConvertOmxErrorType(iHandle->FillThisBuffer(static_cast<OMX_HANDLETYPE>(iHandle), bufferHeader));
		}
	else
		{
		return KErrNotSupported;
		}
	}

	
	
COmxBufferManager::~COmxBufferManager()
	{
	for (TInt i=0;i<iBuffers.Count();i++)
		{
		COmxBuffer* omxBuffer = iBuffers[i];
		iHandle->FreeBuffer(static_cast<OMX_HANDLETYPE>(iHandle), 0, omxBuffer->BufferHeader());
		delete omxBuffer;
		}
	iBuffers.Close();
	}
	
//  Implementation of the Class COmxComponent



COmxCallbacks* COmxCallbacks::NewL(MOmxPuCallbacks& aPuCallbacks)
	{
	COmxCallbacks* self = new (ELeave) COmxCallbacks(aPuCallbacks);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
	
void COmxCallbacks::ConstructL()
	{
	OMX_CALLBACKTYPE h =
			{
			&::EventHandler,
			&::EmptyBufferDone,
			&::FillBufferDone
			};
			
	iHandle = h;
	CActiveScheduler::Add(this);

	User::LeaveIfError(iMsgQueue.CreateLocal(KMaxMsgQueueEntries));
	iMsgQueue.NotifyDataAvailable(iStatus);
	SetActive();
	}

COmxCallbacks::COmxCallbacks(MOmxPuCallbacks& aPuCallbacks)
	: CActive(EPriorityNormal),
	iPuCallbacks(&aPuCallbacks)
	{
	}
		
	
COmxCallbacks::operator OMX_CALLBACKTYPE*()
	{
	return &iHandle;
	}
	
	
void COmxCallbacks::RunL()
	{
	TOmxMessage msg;
	while (iMsgQueue.Receive(msg)==KErrNone)
		{
		switch (msg.iType)
			{
			case EEmptyBufferCallback:
				{
				MOmxInputPortCallbacks* callback = msg.iBuffer->InputPortCallbacks();
				const CMMFBuffer* buffer = msg.iBuffer->MmfBuffer();
				callback->EmptyBufferDone(msg.iComponent, buffer);
				break;
				}				

			case EFillBufferCallback:
				{
				CMMFBuffer* mmfBuffer = msg.iBuffer->MmfBuffer();
				OMX_BUFFERHEADERTYPE* bufferHeader = msg.iBuffer->BufferHeader();

				if (CMMFBuffer::IsSupportedDataBuffer(mmfBuffer->Type()))
					{
					CMMFDataBuffer* dataBuffer = static_cast<CMMFDataBuffer*>(mmfBuffer);
					TDes8& aBufferDes = dataBuffer->Data();
					aBufferDes.SetLength(bufferHeader->nFilledLen);
					mmfBuffer->SetLastBuffer(bufferHeader->nFlags & OMX_BUFFERFLAG_EOS);
					}				
				else
					{
					ASSERT(EFalse);
					}
				MOmxOutputPortCallbacks* callback = msg.iBuffer->OutputPortCallbacks();
				callback->FillBufferDone(msg.iComponent, mmfBuffer);
				break;
				}				
			case EEventCallback:
				{
				iPuCallbacks->MopcEventHandler(msg.iComponent, 
											msg.iEventParams.iEvent, 
											msg.iEventParams.iData1, 
											msg.iEventParams.iData2, 
											msg.iEventParams.iExtra);
				break;
				}				
			default:
				{
				// This is an invalid state
				ASSERT(EFalse);
				}					
			};
		}

	// setup for next callbacks		
	iStatus = KRequestPending;
	iMsgQueue.NotifyDataAvailable(iStatus);
	SetActive();	
	}
	
COmxCallbacks::~COmxCallbacks()
	{
	Cancel();	
	iMsgQueue.Close();
	}
	
	
void COmxCallbacks::DoCancel()
	{
	if (iMsgQueue.Handle()!=NULL)
		{
		iMsgQueue.CancelDataAvailable();
		}
	}
	
TInt COmxCallbacks::FillBufferDone(OMX_HANDLETYPE aComponent, COmxBufferManager::COmxBuffer* aBuffer)
	{
	TOmxMessage message;
	message.iType = EFillBufferCallback;
	message.iComponent = aComponent;
	message.iBuffer = aBuffer;
	return iMsgQueue.Send(message);
	}
	
TInt COmxCallbacks::EmptyBufferDone(OMX_HANDLETYPE aComponent, COmxBufferManager::COmxBuffer* aBuffer)
	{
	TOmxMessage message;
	message.iType = EEmptyBufferCallback;
	message.iComponent = aComponent;
	message.iBuffer = aBuffer;
	return iMsgQueue.Send(message);
	}
	
TInt COmxCallbacks::EventHandler(OMX_HANDLETYPE aComponent, TEventParams aEventParams)
	{
	TOmxMessage message;
	message.iType = EEventCallback;
	message.iComponent = aComponent;
	message.iEventParams = aEventParams;
	return iMsgQueue.Send(message);
	}


 COmxProcessingUnit::CBody::CBody()
	{
	iPuState = EProcessingUnitInvalid;
	}


COmxProcessingUnit::CBody::~CBody()
	{
	delete iBufferManager;
	delete iCallbacks;
	
	iInputPorts.Close();
	iOutputPorts.Close();

	::OMX_FreeHandle((OMX_HANDLETYPE)iHandle);
	}

COmxProcessingUnit::CBody* COmxProcessingUnit::CBody::NewL(const TDesC8& aComponentName, 
															MOmxPuCallbacks& aPuCallbacks, 
															COmxProcessingUnit* aParent,
															const MMdfProcessingUnitObserver& aObserver)
	{
	CBody* self = new (ELeave) CBody;
	CleanupStack::PushL(self);
	self->ConstructL(aComponentName, aPuCallbacks, aParent, aObserver);
	CleanupStack::Pop(self);
	return self;
	}
	
	
void COmxProcessingUnit::CBody::ConstructL(const TDesC8& aComponentName, 
											MOmxPuCallbacks& aPuCallbacks, 
											COmxProcessingUnit* aParent,
											const MMdfProcessingUnitObserver& aObserver)
	{
	iCallbacks = COmxCallbacks::NewL(aPuCallbacks);
	
	iParent = aParent;
	iObserver = const_cast<MMdfProcessingUnitObserver*>(&aObserver);

	OMX_ERRORTYPE errorType;
	OMX_CALLBACKTYPE* omxCallbacks = *iCallbacks;
	TBuf8<KMaxComponentNameLength> buf;
	buf.Copy(aComponentName);
	const char* name = reinterpret_cast<const char*>(buf.PtrZ());
	errorType = ::OMX_GetHandle((OMX_HANDLETYPE*)&iHandle, const_cast<char*>(name), iCallbacks, omxCallbacks);

	User::LeaveIfError(ConvertOmxErrorType(errorType));
	// Create the BufferManager class to look after the buffering
	iBufferManager = new (ELeave) COmxBufferManager(iHandle);
	SetPuState(EProcessingUnitLoaded);
	}



TInt COmxProcessingUnit::CBody::GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts)
	{
	TInt err = KErrNone;
	for (TInt i=0; i < iInputPorts.Count() && err == KErrNone; i++ )
		{
		err = aComponentInputPorts.Append(iInputPorts[i]);
		}
	return err;
	}


TInt COmxProcessingUnit::CBody::GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts)
	{
	TInt err = KErrNone;
	for (TInt i=0; i < iOutputPorts.Count() && err == KErrNone; i++ )
		{
		err = aComponentOutputPorts.Append(iOutputPorts[i]);
		}
	return err;
	}
	

void COmxProcessingUnit::CBody::Initialize()
	{
	// if the state is not Loaded, we should not accept this call
	if (State() != EProcessingUnitLoaded)
		{	
		Observer()->InitializeComplete(iParent, KErrNotReady); 
		}
	else
		{
		// initialize each of the ports in turn		
		for (TInt i=0; i < iInputPorts.Count(); i++ )
			{
			iInputPorts[i]->MipInitialize();
			}
		
	for (TInt i=0; i < iOutputPorts.Count(); i++ )
		{
		iOutputPorts[i]->MopInitialize();
		}

		// instruct the OMX component to go into the Idle state
		SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
		SetPuState(EProcessingUnitInitializing);
		}	
	}

void COmxProcessingUnit::CBody::Execute()
	{
	
	SendCommand(OMX_CommandStateSet, OMX_StateExecuting, NULL);
	iPuState = EProcessingUnitExecuting;
	}
		
TProcessingUnitState COmxProcessingUnit::CBody::State()
	{
	return iPuState;
	}
	
TInt COmxProcessingUnit::CBody::EventHandler(OMX_HANDLETYPE /*aComponent*/, OMX_EVENTTYPE aEvent, TUint32 /*aData1*/,
		 TUint32 aData2, TAny* /*aExtraInfo*/)
	{	
	switch (aEvent)
		{
		
		case OMX_EventCmdComplete:
			{
		
			switch (aData2)
				{
				case OMX_StateIdle:
					if (iPuState == EProcessingUnitInitializing)
						{
						Observer()->InitializeComplete(iParent, KErrNone);
						}
					else
						{
						Observer()->ExecuteComplete(iParent, KErrNone);
						}
					SetPuState(EProcessingUnitIdle);
					
					break;
				};
			break;
			}
		case OMX_EventBufferFlag:
			{
			SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);
			}
		}
	return KErrNone;
	}

// Base Versions are not supported
TInt COmxProcessingUnit::CBody::Configure(const TPuConfig& /*aConfig*/)
	{
	return KErrNotSupported;
	}

	
// Base Versions are not supported
TInt COmxProcessingUnit::CBody::GetConfig(TPuConfig& /*aConfig*/)
	{
	return KErrNotSupported;
	}


TInt COmxProcessingUnit::CBody::Pause ()
	{
	return KErrNone;
	}

void COmxProcessingUnit::CBody::Stop()
	{
	SendCommand(OMX_CommandStateSet, OMX_StateIdle, NULL);	
	
	TInt err = KErrNone;
	for (TInt i=0; i < iInputPorts.Count() && err == KErrNone; i++ )
		{
		iInputPorts[i]->MipDisconnectTunnel();
		}

	for (TInt i=0; i < iOutputPorts.Count() && err == KErrNone; i++ )
		{
		iOutputPorts[i]->MopDisconnectTunnel();
		}

	SetPuState(EProcessingUnitIdle);	
	}

TInt COmxProcessingUnit::CBody::CreateCustomInterface(TUid /*aUid*/)
	{
	return KErrNotSupported;
	}


TAny* COmxProcessingUnit::CBody::CustomInterface(TUid /*aUid*/)
	{
 	return NULL;
	}
	
TInt COmxProcessingUnit::CBody::AddInputPort(MMdfInputPort* aInputPort)
	{
	if (iInputPorts.Find(aInputPort)>=0)
		return KErrAlreadyExists;
	return iInputPorts.Append(aInputPort);
	}
	
TInt COmxProcessingUnit::CBody::AddOutputPort(MMdfOutputPort* aOutputPort)
	{
	if (iOutputPorts.Find(aOutputPort)>=0)
		return KErrAlreadyExists;
	return iOutputPorts.Append(aOutputPort);
	}
	
MMdfProcessingUnitObserver* COmxProcessingUnit::CBody::Observer()
	{
	return iObserver;
	}
	
void COmxProcessingUnit::CBody::SetPuState(TProcessingUnitState aPuState)
	{
	iPuState = aPuState;
	}

TInt COmxProcessingUnit::CBody::GetComponentVersion(const TDesC8& /*aComponentName*/, OMX_VERSIONTYPE* /*aComponentVersion*/, OMX_VERSIONTYPE* /*aSpecVersion*/, OMX_UUIDTYPE* /*aComponentUUID*/)
	{
	return KErrNotSupported;
	}

TInt COmxProcessingUnit::CBody::SendCommand(OMX_COMMANDTYPE aCmd, TUint aParam, TAny* aCmdData)
	{
	OMX_ERRORTYPE error = iHandle->SendCommand(static_cast<OMX_HANDLETYPE>(iHandle), aCmd, aParam, aCmdData);
	return ConvertOmxErrorType(error);
	}


TInt COmxProcessingUnit::CBody::GetParameter(OMX_INDEXTYPE aParamIndex, TAny* aComponentParameterStructure)
	{
	OMX_ERRORTYPE error = iHandle->GetParameter(static_cast<OMX_HANDLETYPE>(iHandle), aParamIndex, aComponentParameterStructure);
	return ConvertOmxErrorType(error);
	}


TInt COmxProcessingUnit::CBody::SetParameter(OMX_INDEXTYPE aIndex, TAny* aComponentParameterStructure)
	{
	OMX_ERRORTYPE error = iHandle->SetParameter(static_cast<OMX_HANDLETYPE>(iHandle), aIndex, aComponentParameterStructure);
	return ConvertOmxErrorType(error);
	}

TInt COmxProcessingUnit::CBody::GetConfig(OMX_INDEXTYPE aIndex, TAny* aValue)
	{
	OMX_ERRORTYPE error = iHandle->GetConfig(static_cast<OMX_HANDLETYPE>(iHandle), aIndex, aValue);
	return ConvertOmxErrorType(error);
	}

TInt COmxProcessingUnit::CBody::SetConfig(OMX_INDEXTYPE aIndex, TAny* aValue)
	{
	OMX_ERRORTYPE error = iHandle->SetConfig(static_cast<OMX_HANDLETYPE>(iHandle), aIndex, aValue);
	return ConvertOmxErrorType(error);
	}

TInt COmxProcessingUnit::CBody::GetExtensionIndex(const TDesC8& aParameterName, OMX_INDEXTYPE* aIndexType)
	{
	HBufC8* buf = HBufC8::New(aParameterName.Length()+1);
	if (buf == NULL)
		{
		return KErrNoMemory;
		}
	else
		{
		// Create a zero terminated version of the paramter name
		*buf = aParameterName;
		TPtr8 ptr = buf->Des();
		TUint8* cstring = const_cast<TUint8*>(ptr.PtrZ());
		OMX_ERRORTYPE error = iHandle->GetExtensionIndex(static_cast<OMX_HANDLETYPE>(iHandle), reinterpret_cast<char*>(cstring), aIndexType);
		// delete the created memory - note no leaving functions so CleanupStack not used
		delete buf;
		return ConvertOmxErrorType(error);
		}
	}


TInt COmxProcessingUnit::CBody::GetState(OMX_STATETYPE* aState)
	{
	OMX_ERRORTYPE error = iHandle->GetState(static_cast<OMX_HANDLETYPE>(iHandle), aState);
	return ConvertOmxErrorType(error);
	}


TInt COmxProcessingUnit::CBody::ComponentTunnelRequest(TUint aPortInput, OMX_HANDLETYPE aOutput, TUint aPortOutput, OMX_TUNNELSETUPTYPE* aTunnelSetup)
	{
	OMX_ERRORTYPE error = iHandle->ComponentTunnelRequest(static_cast<OMX_HANDLETYPE>(iHandle), aPortInput, aOutput, aPortOutput, aTunnelSetup);
	return ConvertOmxErrorType(error);
	}
	
TInt COmxProcessingUnit::CBody::UseBuffer(CMMFBuffer* aBuffer, TUint aPortIndex)
	{
	return iBufferManager->UseBuffer(*aBuffer, aPortIndex);	
	}


CMMFBuffer* COmxProcessingUnit::CBody::AllocateBufferL(TUint aPortIndex, TUint aSizeBytes)
	{
	return iBufferManager->AllocateBufferL(aPortIndex, aSizeBytes);
	}
	

TInt COmxProcessingUnit::CBody::FreeBuffer(CMMFBuffer* aBuffer)
	{
	return iBufferManager->FreeBuffer(aBuffer);
	}

TInt COmxProcessingUnit::CBody::EmptyThisBuffer(const CMMFBuffer* aBuffer, MOmxInputPortCallbacks* aObserver)
	{
	return iBufferManager->EmptyThisBuffer(aBuffer, aObserver);
	}


TInt COmxProcessingUnit::CBody::FillThisBuffer(CMMFBuffer* aBuffer, MOmxOutputPortCallbacks* aObserver)
	{
	return iBufferManager->FillThisBuffer(aBuffer, aObserver);
	}


TInt COmxProcessingUnit::CBody::SetCallbacks(MOmxPuCallbacks& /*aPuCallbacks*/)
	{
	return KErrNotSupported;
	}

// Callbacks implementation - calls back to COMxCallbacks class, which manages a queue
OMX_ERRORTYPE EventHandler(OMX_OUT OMX_HANDLETYPE aComponent, 
					OMX_OUT TAny* aAppData,
        			OMX_OUT OMX_EVENTTYPE aEvent, 
        			OMX_OUT TUint32 aData1,
        			OMX_OUT TUint32 aData2,
        			OMX_OUT TAny* aExtra)
	{
	COmxCallbacks::TEventParams eventParams;
	eventParams.iEvent = aEvent;
	eventParams.iData1 = aData1;
	eventParams.iData2 = aData2;
	eventParams.iExtra = aExtra;
	TInt error = static_cast<COmxCallbacks*>(aAppData)->EventHandler(aComponent, eventParams);
	return ConvertSymbianErrorType(error);
	}
        			
OMX_ERRORTYPE EmptyBufferDone(
       OMX_HANDLETYPE aComponent,
       TAny* aAppData,
       OMX_BUFFERHEADERTYPE* aBuffer)
	{
	COmxBufferManager::COmxBuffer* buffer = static_cast<COmxBufferManager::COmxBuffer*>(aBuffer->pAppPrivate);
	TInt error = static_cast<COmxCallbacks*>(aAppData)->EmptyBufferDone(aComponent, buffer);
	return ConvertSymbianErrorType(error);
	}
        
OMX_ERRORTYPE FillBufferDone(
       OMX_HANDLETYPE aComponent,
       TAny* aAppData,
       OMX_BUFFERHEADERTYPE* aBuffer)
	{
	COmxBufferManager::COmxBuffer* buffer = static_cast<COmxBufferManager::COmxBuffer*>(aBuffer->pAppPrivate);
	TInt error = static_cast<COmxCallbacks*>(aAppData)->FillBufferDone(aComponent, buffer);
	return ConvertSymbianErrorType(error);
	}
	
