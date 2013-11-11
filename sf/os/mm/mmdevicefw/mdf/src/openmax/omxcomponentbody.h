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
// Implementation of the Class COmxComponent
// 
//

#ifndef OMXPROCESSINGUNITBODY_H
#define OMXPROCESSINGUNITBODY_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>
#include <omxprocessingunit.h>
#include "omxcomponentimpl.h"

enum TOmxPortPanics
	{
	EObserverNotSet,
	EBodyNotCreated
	};

class CMMFBuffer;
class MMdfInputPortObserver;
class MMdfOutputPortObserver;

class COmxProcessingUnit::CBody : public CBase
	{
public:
	virtual ~CBody();
	static CBody* NewL(const TDesC8& aComponentName, 
						MOmxPuCallbacks& aPuCallbacks, 
						COmxProcessingUnit* aParent, 
						const MMdfProcessingUnitObserver& aObserver);

	// Processing unit calls
	TInt GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts);
	TInt GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts);
	TInt Configure(const TPuConfig& aConfigurationSetup);
	TInt GetConfig(TPuConfig& aConfigurationSetup);
	void Initialize();
	TInt Pause();
	void Stop();
	void Execute();
	TProcessingUnitState State();
	TInt CreateCustomInterface(TUid aUid);
	TAny* CustomInterface(TUid aUid);
	
	// OMX Processing Unit calls	
	TInt AddInputPort(MMdfInputPort* aInputPort);
	TInt AddOutputPort(MMdfOutputPort* aOutputPort);
	void SetPuState(TProcessingUnitState aState);	
	MMdfProcessingUnitObserver* Observer();

	// OMX calls 
	TInt GetComponentVersion(const TDesC8& aComponentName, OMX_VERSIONTYPE* aComponentVersion, OMX_VERSIONTYPE* aSpecVersion, OMX_UUIDTYPE* aComponentUUID);
	TInt SendCommand(OMX_COMMANDTYPE aCmd, TUint aParam1, TAny* aCmdData);
	TInt GetParameter(OMX_INDEXTYPE aParamIndex, TAny* aComponentParameterStructure);
	TInt SetParameter(OMX_INDEXTYPE aIndex, TAny* aComponentParameterStructure);
	TInt GetConfig(OMX_INDEXTYPE aIndex, TAny* aValue);
	TInt SetConfig(OMX_INDEXTYPE aIndex, TAny* aValue);
	TInt GetExtensionIndex(const TDesC8& aParameterName, OMX_INDEXTYPE* aIndexType);
	TInt GetState(OMX_STATETYPE* aState);
	TInt ComponentTunnelRequest(TUint aPortInput, OMX_HANDLETYPE aOutput, TUint aPortOutput, OMX_TUNNELSETUPTYPE* aTunnelSetup);
	TInt UseBuffer(CMMFBuffer* aBuffer, TUint aPortIndex);
	CMMFBuffer* AllocateBufferL(TUint aPortIndex, TUint aSizeBytes);
	TInt FreeBuffer(CMMFBuffer* aBuffer);
	TInt EmptyThisBuffer(const CMMFBuffer* aBuffer, MOmxInputPortCallbacks* aCallback);
	TInt FillThisBuffer(CMMFBuffer* aBuffer, MOmxOutputPortCallbacks* aCallback);
	TInt SetCallbacks(MOmxPuCallbacks& aPuCallbacks);
	
	TInt EventHandler(OMX_HANDLETYPE aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1,
		 TUint32 aData2, TAny* aExtraInfo);
private:
	CBody();
	void ConstructL(const TDesC8& aComponentName, 
						MOmxPuCallbacks& aPuCallbacks, 
						COmxProcessingUnit* aBody,
						const MMdfProcessingUnitObserver& aObserver);
						
	
	
private:

	void GetExtensionIndexL(const TDesC8& aParameterName, OMX_INDEXTYPE* aIndexType);
	// Processing Unit Data
	CMdfProcessingUnit* iParent;
	MMdfProcessingUnitObserver* iObserver;
	TProcessingUnitState 		iPuState;
	RPointerArray<MMdfInputPort> iInputPorts;
	RPointerArray<MMdfOutputPort> iOutputPorts;	


	COmxBufferManager* iBufferManager;
	OMX_COMPONENTTYPE* iHandle;
	COmxCallbacks* iCallbacks;

	};
	
// Prototypes for OMX callback handler functions
OMX_ERRORTYPE EventHandler(OMX_OUT OMX_HANDLETYPE aComponent, 
					OMX_OUT TAny* aAppData,
        			OMX_OUT OMX_EVENTTYPE aEvent, 
        			OMX_OUT TUint32 aData1,
        			OMX_OUT TUint32 aData2,
        			OMX_OUT TAny* aExtra);
        			
OMX_ERRORTYPE EmptyBufferDone(
       OMX_HANDLETYPE aComponent,
       TAny* aAppData,
       OMX_BUFFERHEADERTYPE* aBuffer);
        
OMX_ERRORTYPE FillBufferDone(
       OMX_HANDLETYPE aComponent,
       TAny* aAppData,
       OMX_BUFFERHEADERTYPE* aBuffer);


#endif // OMXPROCESSINGUNITBODY_H
