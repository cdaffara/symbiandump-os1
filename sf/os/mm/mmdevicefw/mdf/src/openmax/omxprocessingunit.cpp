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

#include <mdf/mdfpuconfig.h>
#include "omxcomponentbody.h"
#include <omxprocessingunit.h>
#include <omxinputport.h>
#include <omxoutputport.h>
#include <mmf/server/taskconfig.h>

/**
Default constructor.
*/
EXPORT_C COmxProcessingUnit::COmxProcessingUnit()
	{ 
	}

/**
Destructor.
*/
EXPORT_C COmxProcessingUnit::~COmxProcessingUnit()
	{
	delete iBody;
	}

/**
Safe constructing method. 
@param aComponentName The name of the component to be contructed.
@param aProcessingUnitObserver Reference to the obsever for this Processing Unit.
*/
EXPORT_C void COmxProcessingUnit::ConstructL(const TDesC8& aComponentName, const MMdfProcessingUnitObserver& aProcessingUnitObserver)
	{
	iBody = CBody::NewL(aComponentName, *this, this, aProcessingUnitObserver);
	}

/**.
@see CMdfProcessingUnit::Create
*/	
EXPORT_C TInt COmxProcessingUnit::Create(const MMdfProcessingUnitObserver& /* aProcessingUnitObserver */)
	{
	return KErrNotSupported;
	}

/**.
@see CMdfProcessingUnit::GetInputPorts
*/
EXPORT_C TInt COmxProcessingUnit::GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts)
	{
	return iBody->GetInputPorts(aComponentInputPorts);
	}

/**
@see CMdfProcessingUnit::GetOutputPorts
*/
EXPORT_C TInt COmxProcessingUnit::GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts)
	{
	return iBody->GetOutputPorts(aComponentOutputPorts);
	}
	
/**
@see CMdfProcessingUnit::Initialize
@see MMdfProcessingUnitObserver::InitializeComplete()
*/
EXPORT_C void COmxProcessingUnit::Initialize()
	{
	iBody->Initialize();
	}

/**
@see CMdfProcessingUnit::Execute
*/
EXPORT_C void COmxProcessingUnit::Execute()
	{
	iBody->Execute();
	}
	
/**
@see CMdfProcessingUnit::State
*/
EXPORT_C TProcessingUnitState COmxProcessingUnit::State()
	{
	return iBody->State();
	}

/**
@see CMdfProcessingUnit::Configure
*/
EXPORT_C TInt COmxProcessingUnit::Configure(const TPuConfig& aConfig)
	{
	return iBody->Configure(aConfig);
	}

/**
@see CMdfProcessingUnit::GetConfig
*/
EXPORT_C TInt COmxProcessingUnit::GetConfig(TPuConfig& aConfig)
	{
	return iBody->GetConfig(aConfig);
	}

/**
@see CMdfProcessingUnit::Pause
*/	
EXPORT_C TInt COmxProcessingUnit::Pause()
	{
	return iBody->Pause();
	}

/**
@see CMdfProcessingUnit::Stop
*/
EXPORT_C void COmxProcessingUnit::Stop()
	{
	iBody->Stop();
	}

EXPORT_C TInt COmxProcessingUnit::MopcEventHandler(OMX_HANDLETYPE aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1,
	TUint32 aData2, TAny* aExtraInfo)
	{
	return iBody->EventHandler(aComponent, aEvent, aData1, aData2, aExtraInfo);
	}

/**
@see CMdfProcessingUnit::CreateCustomInterface
*/
EXPORT_C TInt COmxProcessingUnit::CreateCustomInterface(TUid aUid)
	{
	return iBody->CreateCustomInterface(aUid);	
	}

/**
@see CMdfProcessingUnit::CustomInterface
*/	
EXPORT_C TAny* COmxProcessingUnit::CustomInterface(TUid aUid)
	{
 	return iBody->CustomInterface(aUid);
	}	

/**
Add an input port to the component
@param aInputPort A pointer to the input port
@return KErrNone if successful or one of the system wide error codes
*/	
EXPORT_C TInt COmxProcessingUnit::AddInputPort(MMdfInputPort* aInputPort)
	{
	return iBody->AddInputPort(aInputPort);
	}

/**
Add an output port to the component
@param aOutputPort A pointer to the output port
@return KErrNone if successful or one of the system wide error codes
*/	
EXPORT_C TInt COmxProcessingUnit::AddOutputPort(MMdfOutputPort* aOutputPort)
	{
	return iBody->AddOutputPort(aOutputPort);
	}
	
/**
Return the observer interface for the component
@return The observer interface
*/	
EXPORT_C const MMdfProcessingUnitObserver* COmxProcessingUnit::Observer()
	{
	return iBody->Observer();
	}

/**
Sets the internal PU State
@param aPuState The internal state of the component
*/	
EXPORT_C void COmxProcessingUnit::SetPuState(TProcessingUnitState aPuState)
	{
	iBody->SetPuState(aPuState);
	}
	
// Calls to OmxComponent

/**
	Requests a current parameter from the OpenMAX Processing Unit.
	@param	aParamIndex						The index of the structure to be filled.
	@param	aComponentParameterStructure	A pointer to structure to be filled by the component.
	@return	An error code indicating if the function call was successful.
			KErrNone on success, otherwise another of the system-wide error codes.
*/
EXPORT_C TInt COmxProcessingUnit::OmxGetParameter(OMX_INDEXTYPE aParamIndex, TAny* aComponentParameterStructure)
	{
	return iBody->GetParameter(aParamIndex, aComponentParameterStructure);
	}

/**
	Instructs the OpenMAX Processing Unit to use the buffer passed in the function's argument. 
	@param  aBuffer		The buffer to be used.
	@param 	aPortIndex	The port that the given buffer should be associated with.	  
	@return 			An error code indicating if the function call was successful. KErrNone
						on success, otherwise another of the system-wide error codes.
*/	
EXPORT_C TInt COmxProcessingUnit::OmxUseBuffer(CMMFBuffer* aBuffer, TUint32 aPortIndex)
	{
	return iBody->UseBuffer(aBuffer, aPortIndex);
	}

/**
	Instructs the OpenMAX Processing Unit to allocate a buffer.  Ownership is transferred to the caller.
	@param	aPortIndex	The Port Index that the new buffer is associated with.
	@param  aSizeBytes	The size of the buffer to be allocated, in bytes.		  
	@return A pointer to the newly created buffer.
*/
EXPORT_C CMMFBuffer* COmxProcessingUnit::OmxAllocateBufferL(TUint32 aPortIndex, TUint32 aSizeBytes)
	{
	return iBody->AllocateBufferL(aPortIndex, aSizeBytes);
	}

/**
	Instructs the OpenMAX Processing Unit to free the buffer passed in the function's argument. 
	@param  aBuffer		The buffer to be freed
	@return 			An error code indicating if the function call was successful. KErrNone
						on success, otherwise another of the system-wide error codes.
*/
EXPORT_C TInt COmxProcessingUnit::OmxFreeBuffer(CMMFBuffer* aBuffer)
	{
	return iBody->FreeBuffer(aBuffer);
	}

/**
	Sends an full buffer to an input port of a component.
	@param	aBuffer		The buffer to be emptied.
	@param	aObserver	The observer of the input port that is to be notified.
	@return				An error code indicating if the function call was successful. KErrNone
						on success, otherwise another of the system-wide error codes.
*/
EXPORT_C TInt COmxProcessingUnit::OmxEmptyThisBuffer(const CMMFBuffer* aBuffer, MOmxInputPortCallbacks* aObserver)
	{
	return iBody->EmptyThisBuffer(aBuffer, aObserver);
	}

/**
	Sends an empty buffer to an output port of a component.
	@param	aBuffer		The buffer to be filled.
	@param	aObserver	The observer of the output port that is to be notified.
	@return				An error code indicating if the function call was successful. KErrNone
						on success, otherwise another of the system-wide error codes.
*/
EXPORT_C TInt COmxProcessingUnit::OmxFillThisBuffer(CMMFBuffer* aBuffer, MOmxOutputPortCallbacks* aObserver)
	{
	return iBody->FillThisBuffer(aBuffer, aObserver);
	}
	
/**
	Sends a command to the OpenMAX Processing Unit.
	@param	aCmd		The command for the component to execute.	
	@param	aParam1		The parameter for the command to be executed.
	@param	aCmdData	A pointer to the data for the command.
	@return	An error code indicating if the function call was successful.
			KErrNone on success, otherwise another of the system-wide error codes.
*/
EXPORT_C TInt COmxProcessingUnit::OmxSendCommand(OMX_COMMANDTYPE aCmd, TUint32 aParam1, TAny* aCmdData)
	{
	return iBody->SendCommand(aCmd, aParam1, aCmdData);	
	}

/**
	Sends a parameter from the OpenMAX Processing Unit.
	@param	aParamIndex						The index of the structure to be sent.
	@param	aComponentParameterStructure	A pointer to structure to be used by the component.
	@return	An error code indicating if the function call was successful.
			KErrNone on success, otherwise another of the system-wide error codes.
*/
EXPORT_C TInt COmxProcessingUnit::OmxSetParameter(OMX_INDEXTYPE aParamIndex, TAny* aComponentParameterStructure)
	{
	return iBody->SetParameter(aParamIndex, aComponentParameterStructure);	
	}



