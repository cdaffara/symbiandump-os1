// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef OMXPROCESSINGUNIT_H
#define OMXPROCESSINGUNIT_H

#include <e32std.h>
#include <e32base.h>
#include <mdf/mdfprocessingunit.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

class COmxInputPort;
class COmxOutputPort;

/**
OpenMAX Processing Unit observer class.
*/
class MOmxPuCallbacks
	{
public:
	virtual TInt MopcEventHandler(OMX_HANDLETYPE aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2, TAny* aExtraInfo) =0;
	};

/**
OpenMAX Input Port observer class.
*/
class MOmxInputPortCallbacks
	{
public:
	/**
	 * Returns the buffer that has been emptied.
	 * @param 	aComponent
	 *	   	The handle to the OpenMAX component sending the callback.
	 * @param 	aBuffer
	 * 		The buffer returned by the OpenMAX component.
	 * @return	
	 *		KErrNone, if successful; otherwise one of the other system-wide error codes. 
	 *
	 */
	virtual TInt EmptyBufferDone(OMX_HANDLETYPE aComponent, const CMMFBuffer* aBuffer) =0;	
	};


/**
OpenMAX Output Port observer class.
*/
class MOmxOutputPortCallbacks
	{
public:
	/**
	 * Returns the buffer that has been filled.
	 * @param 	aComponent
	 *	   	The handle to the OpenMAX component sending the callback.
	 * @param 	aBuffer
	 * 		The buffer returned by the OpenMAX component.
	 * @return	
	 *		KErrNone, if successful; otherwise one of the other system-wide error codes. 
	 */
	virtual TInt FillBufferDone(OMX_HANDLETYPE aComponent, CMMFBuffer* aBuffer) =0;
	};

/**
OpenMAX Processing Unit class.
*/
class COmxProcessingUnit : public CMdfProcessingUnit, public MOmxPuCallbacks
	{
public:
	class CBody;
public:
	// from CMdfProcessingUnit
	IMPORT_C TInt Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver);
	IMPORT_C TInt GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts);
	IMPORT_C TInt GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts);
	IMPORT_C TInt Configure(const TPuConfig& aConfigurationSetup);
	IMPORT_C TInt GetConfig(TPuConfig& aConfigurationSetup);
	IMPORT_C void Initialize();
	IMPORT_C void Execute ();
	IMPORT_C TInt Pause ();
	IMPORT_C void Stop ();
	IMPORT_C TProcessingUnitState State();
	IMPORT_C TInt CreateCustomInterface(TUid aUid);
	IMPORT_C TAny* CustomInterface(TUid aUid);
	IMPORT_C ~COmxProcessingUnit();

	// from MOmxPuCallbacks
	IMPORT_C TInt MopcEventHandler(OMX_HANDLETYPE aComponent, OMX_EVENTTYPE aEvent, TUint32 aData1, TUint32 aData2, TAny* aExtraInfo);

	// Accessors for Omx Component
	IMPORT_C TInt OmxGetComponentVersion(const TDesC8& aComponentName, OMX_VERSIONTYPE* aComponentVersion, OMX_VERSIONTYPE* aSpecVersion, OMX_UUIDTYPE* aComponentUUID);
	IMPORT_C TInt OmxSendCommand(OMX_COMMANDTYPE aCmd, TUint32 aParam1, TAny* aCmdData);
	IMPORT_C TInt OmxGetParameter(OMX_INDEXTYPE aParamIndex, TAny* aComponentParameterStructure);
	IMPORT_C TInt OmxSetParameter(OMX_INDEXTYPE aIndex, TAny* aComponentParameterStructure);
	IMPORT_C TInt OmxGetConfig(OMX_INDEXTYPE aIndex, TAny* aValue);
	IMPORT_C TInt OmxSetConfig(OMX_INDEXTYPE aIndex, TAny* aValue);
	IMPORT_C TInt OmxGetExtensionIndex(const TDesC8& aParameterName, OMX_INDEXTYPE* aIndexType);
	IMPORT_C TInt OmxGetState(OMX_STATETYPE* aState);
	IMPORT_C TInt OmxComponentTunnelRequest(TUint32 aPortInput, OMX_HANDLETYPE aOutput, TUint32 aPortOutput, OMX_TUNNELSETUPTYPE* aTunnelSetup);
	IMPORT_C TInt OmxUseBuffer(CMMFBuffer* aBuffer, TUint32 aPortIndex);
	IMPORT_C CMMFBuffer* OmxAllocateBufferL(TUint32 aPortIndex, TUint32 aSizeBytes);
	IMPORT_C TInt OmxFreeBuffer(CMMFBuffer* aBuffer);
	IMPORT_C TInt OmxEmptyThisBuffer(const CMMFBuffer* aBuffer, MOmxInputPortCallbacks* aObserver);
	IMPORT_C TInt OmxFillThisBuffer(CMMFBuffer* aBuffer, MOmxOutputPortCallbacks* aObserver);
	IMPORT_C TInt OmxSetCallbacks(MOmxPuCallbacks& aPuCallbacks);

protected:
	IMPORT_C void ConstructL(const TDesC8& aComponentName, const MMdfProcessingUnitObserver& aProcessingUnitObserver);
	IMPORT_C COmxProcessingUnit();
	IMPORT_C TInt AddInputPort(MMdfInputPort* aInputPort);
	IMPORT_C TInt AddOutputPort(MMdfOutputPort* aOutputPort);
	IMPORT_C void SetPuState(TProcessingUnitState aState);	
	IMPORT_C const MMdfProcessingUnitObserver* Observer();
private:
	CBody*	iBody;
	};

#endif // OMXPROCESSINGUNIT_H

