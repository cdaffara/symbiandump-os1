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
//

/**
 @file
 @internalComponent
*/

#ifndef HCICOREPLUGINIMPL_H
#define HCICOREPLUGINIMPL_H

// Includes for parent classes
#include <bluetooth/hci/corehciplugin.h>
#include <bluetooth/hci/qdpplugininterfacefinder.h>
#include <bluetooth/hci/hcicommandallocator.h>
#include <bluetooth/hci/hcidataframer.h>
#include <bluetooth/hci/hctldataobserver.h>
#include <bluetooth/hci/hctleventobserver.h>
#include <bluetooth/hci/controllerstateobserver.h>
#include <bluetooth/hci/corehci.h>
#include <bluetooth/hci/hctlchannelobserver.h>
#include <bluetooth/hci/hciclientusage.h>

#include <bluetooth/hardresetinitiator.h>

class MHCTLPowerInterface;
class CHCIServer;
class CHCTLBase;
class CControllerInitialisationPlugin;
class MHCTLInterface;
class CHciUtil;

/**
An implementation of the CHCICorePlugin.
This also directly implements the interfaces it "exports" 
through the CHCICorePlugin::Interface function.
*/
class CCoreHCIPluginImpl : public CCoreHCIPlugin,
						   public MHardResetInitiator,
						   public MQdpPluginInterfaceFinder,
						   public MHCICommandAllocator,
						   public MHCIDataFramer,
						   public MHCTLDataObserver,
						   public MHCTLEventObserver,
						   public MControllerStateObserver,
						   public MCoreHci,
						   public MHCTLChannelObserver,
						   public MHCIClientUsageCallback
	{
public:
	static CCoreHCIPluginImpl* NewL();
	virtual TAny* Interface(TUid aUid);
	~CCoreHCIPluginImpl();

	TInt SetPower(TBTPowerState aState, TDesC8* aBuf);
	TInt GetPower(TBTPowerState& aState, TDesC8* aBuf);

	void OpenClientReference();
	void CloseClientReference();
	
private:
	// MHardResetInitiator
	virtual void MhriStartHardReset();
	
	// MHCICommandAllocator
	virtual CHctlCommandFrame* MhcaNewFrameL();

	// MHCIDataFramer
	virtual CHctlAclDataFrame* MhdfNewAclDataFrameL(TUint16 aSize);
	virtual void MhdfFormatAclData(CHctlAclDataFrame& aFrame, THCIConnHandle aConnH, TAclPacketBoundaryFlag aBoundaryFlag, TAclPacketBroadcastFlag aBroadcastFlag, const TDesC8& aData);
	virtual CHctlSynchronousDataFrame* MhdfNewSynchronousDataFrameL(TUint8 aSize);
	virtual void MhdfFormatSynchronousData(CHctlSynchronousDataFrame& aFrame, THCIConnHandle aConnH, const TDesC8& aData);

	// MHCTLDataObserver
	virtual void MhdoProcessAclData(const TDesC8& aData);
	virtual void MhdoProcessSynchronousData(const TDesC8& aData);

	// MHCTLEventObserver
	virtual void MheoProcessEvent(const TDesC8& aEvent);

	// MControllerStateObserver
	virtual void McsoProcessPowerChange(TInt aError, TControllerChangeType aChangeType, TBTPowerState aState);
	virtual void McsoProcessHardResetPhaseChange(TInt aError, TControllerChangeType aChangeType, TBTHardResetState aState);

	// MQdpPluginInterfaceFinder
	virtual TAny* MqpifQdpPluginInterface(const TUid aUid);

	// MCoreHci
	virtual TInt MchSetOption(TUint aName, const TDesC8& aData);
	virtual TInt MchGetOption(TUint aName, TDes8& aData);
	virtual void MchIoctl(TUint aLevel, TUint aName, TDes8* aOption, TAny* aStackSAP);
	virtual void MchCancelIoctl(TUint aLevel, TUint aName, TAny* aStackSAP) ;
	virtual void MchSetHCICommandQueue(MHCICommandQueue& aHCICommandQueue);
	virtual void MchSetPhysicalLinksState(MPhysicalLinksState& aPhysicalLinksState) ;
	virtual void MchSetHardResetInitiator(MHardResetInitiator& aHcHardResetInitiator) ;
	virtual void MchSetDataEventObserver(MHCIDataEventObserver& aHCIDataEventObserver) ;
	virtual void MchSetDataObserver(MHCIDataObserver& aHCIDataObserver) ;
	virtual void MchSetCommandEventObserver(MHCICommandEventObserver& aHCICommandEventObserver) ;
	virtual void MchSetChannelObserver(MHCTLChannelObserver& aHCIChannelObserver) ;
	virtual void MchSetControllerStateObserver(MControllerStateObserver& aHCIPowerStateObserver) ;
	virtual TAny* MchHctlPluginInterface(TUid aUid);
	
	// MHCTLChannelObserver
	virtual void MhcoChannelOpen(THCITransportChannel aChannels);
	virtual void MhcoChannelClosed(THCITransportChannel aChannels);

	// MHCIClientUsageCallback
	virtual void MhcucSetClientUsage(MHCIClientUsage& aClientUsage);
	
private:
	void ConstructL();

private:	
	void RequestHardResetFromStack();
	
private:
	/**
	The instance that will be used to request a hard reset from the stack
	(Stack will call back CoreHci method to actaully perform the hard reset once it has done all the preparations).
	*/
	MHardResetInitiator* iStackHardResetRequester;
	
	/**
	The instance that will be pass onto the HCTL a request to perform a hard reset on the host controller.
	*/
	MHardResetInitiator* iHCTLHardResetPerformer;

	/**
	The instance that will be sent the details of Controller State Events received by the HCI Layer.
	*/
	MControllerStateObserver* iControllerStateObserver;

	/**
	The instance that will be sent the details of Data Events received by the HCI Layer.
	*/
	MHCIDataEventObserver* iDataEventObserver;

	/**
	The instance that will be sent the data of Command Events received by the HCI Layer.
	*/
	MHCICommandEventObserver* iCommandEventObserver;

	/**
	The instance that will be used to communicate with the command queue.
	*/
	MHCICommandQueue* iHCICommandQueue;

	/**
	The instance that will be used to communicate with the HCTL's power controller.
	*/
	MHCTLPowerInterface* iHCTLPowerInterface;

	/**
	The instance used to communicate changes in the state of the HCTL channel.
	*/
	MHCTLChannelObserver* iHCTLChannelObserver;

	/**
	Used to forward user plane data to the stack.
	*/
	MHCIDataObserver* iHCIDataObserver;

	/**
	Provides access to physical link information from the stack.
	*/ 
	MPhysicalLinksState* iPhysicalLinksState;

	/**
	The instance of the HCI server that is used in this implementation
	*/
	CHCIServer*	iHCIServer;
	
	/**
	The HCTL plugin loaded by this core HCI instance
	*/
	CHCTLBase* iHCTLPlugin;

	/**
	The initialisation plugin loaded by this core HCI instance
	*/
	CControllerInitialisationPlugin* iInitialisationPlugin;		

	/**
	Used to indicate when an external client is using the HCI
	*/
	MHCIClientUsage* iClientUsage;
	
	/**
	HCI Utility Library
	*/
	CHciUtil* iHciUtil;
	
	/**
	Size of HCTL header / trailer (octets)
	*/
	TUint iHCTLCommandHeaderSize;
	TUint iHCTLCommandTrailerSize;

	TUint iHCTLACLHeaderSize;
	TUint iHCTLACLTrailerSize;
	
	TUint iHCTLSyncDataHeaderSize;
	TUint iHCTLSyncDataTrailerSize;
	};

#endif //HCICOREPLUGINIMPL_H
