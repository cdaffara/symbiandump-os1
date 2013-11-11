// Copyright (c) 1999-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Defines the LinkMgr protocol class
// 
//

#ifndef LINKMGR_H
#define LINKMGR_H

#include <e32property.h>
#include <es_prot.h>
#include <bt_sock.h>
#include "notification.h"
#include <btextnotifiers.h>
#include <bluetooth/hci/hcishared.h>
#include <bluetooth/hci/hcitypes.h>
#include "btfeatures.h"
#include "secman.h"
#include "linkmuxer.h"
#include "bt.h"
#include "btconsts.h"
#include "internaltypes.h"
#include <bluetooth/lmoptions.h>
#include <es_prot_internal.h>
#include <bt_subscribe_partner.h>


class CBTProtocolFamily;
class CLinkMgrProtocol;
class CBTInquiryMgr;
class CHCIFacade;
class CLinkMuxer;
class CACLDataQController;
class CBTSecMan;
class CPhysicalLinksManager;
class CACLLinkStateFactory;
class CSyncLinkStateFactory;
class CSubscriber;
class CVendorSAP;
class CHCIDirectAccessSAPsManager;
class CEirManServer;
class CEirPublisherTxPowerLevel;
class CEirPublisherLocalName;
class CBrEdrControllerConfiguration;

NONSHARABLE_CLASS(CRegistrySession)
	: public CBase
	{
friend class CLinkMgrProtocol;
public:	
	void Open();
	void Close();
	
	RBTRegServ& RegServ();
	
private: // for use by CLinkMgrProtocol
	static CRegistrySession* NewL(CLinkMgrProtocol& aLinkMgrProtocol);
	~CRegistrySession();
	
	CRegistrySession(CLinkMgrProtocol& aLinkMgrProtocol);
	void ConstructL();
	
private:
	TInt				iAccessCount;
	RBTRegServ			iRegServ;
	CLinkMgrProtocol&	iLinkMgrProtocol;
	};

NONSHARABLE_CLASS(TBTTrackedLocalDevice) : public TBTLocalDevice
	{
public:
	void SetAddress(const TBTDevAddr& aAddr);
	void SetDeviceClass(TUint32 aCod);
	void SetDeviceName(const TDesC8& aName);
	void SetScanEnable(THCIScanEnable aEnable);
	void SetLimitedDiscoverable(TBool aOn);
	void SetPowerSetting(TUint8 aPowerSetting);
	void SetAFHChannelAssessmentMode(TBool aOn);
	void SetAcceptPairedOnlyMode(TBool aOn);
	
	void Modify(TBTLocalDevice& aLocalDeviceSettings);
	void ResetChangesMask();
	
private:
	void StoreChange(TUint8 aChange);

private:
	TUint8 iChangedSettings;
	};

NONSHARABLE_CLASS(CRegistryUpdater) : public CActive
	{
public:
	~CRegistryUpdater();
	CRegistryUpdater(RBTLocalDevice& aLocalDevice, TBTTrackedLocalDevice& aSettings);
	void Update();
private:
	void RunL();
	void DoCancel();
private:
	RBTLocalDevice&			iLocalDevice;
	TBTTrackedLocalDevice& 	iStackSettings;
	TBTLocalDevice  		iRegistrySettings;
	TBool					iRepeatUpdate;
	};


NONSHARABLE_CLASS(CLinkMgrProtocol) : public CBluetoothProtocolBase
	{
public:
	// Create ourselves
	static CLinkMgrProtocol* NewL(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	static void ProtocolIdentity(TServerProtocolDesc* aDesc);
	
	void LocalOpen();
	void LocalClose();
	
	void Error(TInt anError,CProtocolBase* aSourceProtocol=NULL);

public: // from CProtocolBase
	// Ref Counting
	void Close();
	void Open();
	
private: // From CProtocolBase
	// Factories
	CHostResolvProvdBase* NewHostResolverL();
	CServProviderBase* NewSAPL(TUint aSockType);
	
	// Ref Counting
	void CloseNow();

	// Others
	void InitL(TDesC &aTag); // From ProtocolManager before all binding.
	void StartL(); // From Protocol Manager - after all binding
	void BindL(CProtocolBase* protocol, TUint id);
	void Identify(TServerProtocolDesc* aDesc)const; // Query function
	
private: // from CBluetoothProtocolBase
	TInt BearerConnectComplete(const TBTDevAddr& /*aAddr*/, CServProviderBase* /*aSAP*/) {return KErrNotSupported;};
	TInt ControlPlaneMessage(TBTControlPlaneMessage aMessage, TAny* aParam);
	
public:

	void SetInquiryAndPageScanningL();
	void SetAcceptPairedOnlyMode(TBool aOn);
	void SetSimplePairingDebugMode(TBool aOn);

	// LinkMgr specific functions
	inline const TBTDevAddr& LocalBTAddress() const;	// Get our local address
	void SetLocalBTAddress(const TBTDevAddr& aAddr);

 	void SetUIDiscovering(TInt aDiscoveringState);
 	
	inline CPhysicalLinksManager& PhysicalLinksMgr() const;
	inline CACLLinkStateFactory& ACLStateFactory() const;
	inline CSyncLinkStateFactory& SyncStateFactory() const;

	// later - change local device management/notification to be nicer...
	inline const TBTLocalDevice& LocalDevice() const;
	inline TBTTrackedLocalDevice& DesiredLocalDeviceSettings();
	void UpdateLocalDeviceName(const TDesC8& aName);
	void UpdateLocalDeviceName(TBool aSucceeded);
	void UpdateLocalDevicePower(TBTPowerState aState);
	void UpdateLocalDeviceScanEnable(TBool aSucceeded);
	void UpdateDeviceClass(TBool aSucceeded);
	void UpdateAFHChannelAssessmentMode(TBool aSucceeded);
	void UpdateLimitedDiscoverable(TBool aSucceeded);

	TBTDevHCIVersion GetHWHCIVersion() const;

	void SetLimitedDiscoverableIfChangedL(TBool aOn);
	void SetLocalVersion(THCIErrorCode aErr, TBTDevHCIVersion aHCIVersion, TBTDevLMPVersion aLMPVersion);
	void SetLocalFeatures(THCIErrorCode aErr, const TBTFeatures& aMask);
	void SetLocalCommands(THCIErrorCode aErr, const TBluetoothHciCommands& aMask);
	void SetDeviceClassL(TUint32 aCod);
	TInt SetLocalDeviceName(const TDesC8& aName);
	void SetAFHChannelAssessmentModeL(TBool aMode);
	void SetAFHHostChannelClassificationL(const TBTAFHHostChannelClassification& aChannelClassification);
	void SetUIConnecting(TBool aConnecting);
	void SetUINumPhysicalLinks(TUint aNum);
	void SetUINumSynchronousLinks(TUint aNum);
	void SetPageScanParameters(TPageScanParameterSettings aPageScanParameters);

	inline CHCIFacade& HCIFacade() const;
	inline CBTInquiryMgr& InquiryMgr() const;
	inline CLinkMuxer& LinkMuxer() const;
	inline CACLDataQController& ACLController() const;
	TInt ACLPacketMTU() const;

	inline CVendorSAP* VendorSpecificSAP() const;
	void UpdateDeviceClass(TBool aSucceeded, TUint aCoD);
	void WriteClassOfDeviceL(TUint32 aCod);
	void ClearPendingLocalDeviceSettingsCod();
	
	void UpdateInquiryResponseTxPowerLevel(TInt8 aTxPowerLevel);
	
	inline const TBTFeatures& PacketsSupportedLocally() const;
	inline TUint8 ModesSupportedLocally() const;

	void LocalSupportedFeaturesAvailable();
	
	inline TBool IsEncryptionSupportedLocally() const;
	inline TBool IsRoleSwitchSupportedLocally() const;
	inline TBool IsSecureSimplePairingSupportedLocally() const;
	inline TBool IsModeSupportedLocally(TBTLinkMode aMode) const;
	inline TBool IsEncryptionPauseResumeSupportedLocally() const;
	inline TBool IsExtendedInquiryResponseSupportedLocally() const;
	inline TBool IsRssiWithInquiryResultsSupportedLocally() const;
	
	inline TBool IsCommandSupportedLocally(TBluetoothSupportedCommandsBits aCommand) const;

private:
	CLinkMgrProtocol(CBTSecMan& aSecMan, RBTControlPlane& aControlPlane, CBTCodServiceMan& aCodMan);
	~CLinkMgrProtocol();
	void DoSetLimitedDiscoverableL(TBool aOn);
	void QueIdleTimerEntry();
	void RemoveIdleTimerEntry();
	static TInt TryToClose(TAny* aProtocol);
	void UpdateSettings();
	virtual TInt StartProtocolListening();
	virtual void StopProtocolListening();
	TInt KickDiscoverabilitySubscriber();
	void DefinePublications(THCIScanEnable aHCIScanEnable);
	void DeletePublications();
	
	void ExternalOpenInit();
	void ExternalCloseCleanup();

private:
friend class CVendorSAP;
friend class CBTCodServiceMan;
	TInt					iExternalRef;

	// Singleton objects owned
	CPhysicalLinksManager*	iPhysicalLinksMgr;

	CRegistrySession*		iRegSess;	// owns an RBTRegServ for sharing.
	RBTLocalDevice			iLocalDevice; //subsession
	TBTTrackedLocalDevice	iLocalDeviceSettings;
	TBTLocalDevice			iPendingLocalDeviceSettings;
	TBTTrackedLocalDevice			iDesiredLocalDeviceSettings;	
	
	RProperty				iProperty;

	CBTInquiryMgr*			iInquiryMgr;
	CHCIFacade*				iHCIFacade;
	CLinkMuxer*				iLinkMuxer;
	CEirManServer*			iEirManServer;

	CBrEdrControllerConfiguration*	iBrEdrControllerConfiguration;
	
	CACLLinkStateFactory*	iACLStateFactory;
	CSyncLinkStateFactory*	iSCOStateFactory;

	RPointerArray<CSubscriber>	iSubscribers;

	// protocol state objects
	TBool                   iClosePending;	// the protocol is closing
	TDeltaTimerEntry		iIdleTimerEntry;
	TBool					iIdleEntryQueued;

	// features of the local hardware that dont need persisting
	TBTDevAddr			iLocalDeviceAddress;
	TBTDevHCIVersion	iHWHCIVersion;
	TBTDevLMPVersion	iHWLMPVersion;
	TBTFeatures			iHWFeatures;
	TBluetoothHciCommands	iHWCommands;
	
	CRegistryUpdater*	iRegistryUpdater;

	// non-owned
	CProtocolBase*		iUpperProtocol;	// L2CAP
	CVendorSAP*			iVendorSpecificSAP;


	// Inquiry Response Transmit Power Level value
	TInt8 				iTxPowerLevel;
	CEirPublisherLocalName*		iLocalNamePublisher;
	CEirPublisherTxPowerLevel*	iTxPowerLevelPublisher;
	
	TInt				iDebugModeDefinitionResult;
	};

inline const TBTDevAddr& CLinkMgrProtocol::LocalBTAddress() const
/**
	Return the local BT address.
	This actually might not be set on initial startup, as it
	takes a finite time to get the local address out of the 
	hardware. But it should not matter, as this local address
	is not actually used for much (get local address ioctl
	actually re-reads it from the hardware.)
**/
	{
	return iLocalDeviceAddress;
	}

inline CPhysicalLinksManager& CLinkMgrProtocol::PhysicalLinksMgr() const
	{
	__ASSERT_DEBUG(iPhysicalLinksMgr, User::Panic(_L("linkmgr.h"), __LINE__));
	return *iPhysicalLinksMgr;
	}

inline const TBTLocalDevice& CLinkMgrProtocol::LocalDevice() const
	{
	return iLocalDeviceSettings;
	}

inline TBTTrackedLocalDevice& CLinkMgrProtocol::DesiredLocalDeviceSettings()
	{
	return iDesiredLocalDeviceSettings;
	}


inline CBTInquiryMgr& CLinkMgrProtocol::InquiryMgr() const
	{
	return *iInquiryMgr;
	}
   
inline CHCIFacade& CLinkMgrProtocol::HCIFacade() const
	{
	return *iHCIFacade;
	}

inline CLinkMuxer& CLinkMgrProtocol::LinkMuxer() const
	{
	return *iLinkMuxer;
	}

inline CACLDataQController& CLinkMgrProtocol::ACLController() const
	{
	return LinkMuxer().DataQController();
	}

inline CACLLinkStateFactory& CLinkMgrProtocol::ACLStateFactory() const
	{
	return *iACLStateFactory;
	}

inline CSyncLinkStateFactory& CLinkMgrProtocol::SyncStateFactory() const
	{
	return *iSCOStateFactory;
	}

inline CVendorSAP* CLinkMgrProtocol::VendorSpecificSAP() const
	{
	return iVendorSpecificSAP;
	}

inline TUint8 CLinkMgrProtocol::ModesSupportedLocally() const
	{
	// baseband spec:
	// mode support is --------|-------x|xx------
	// **this method returns it in lowest bits of a uint8**
	return TUint8(iHWFeatures.LinkModes());
	}

inline const TBTFeatures& CLinkMgrProtocol::PacketsSupportedLocally() const
	{
	// not masking for now - disparate bits and in future have EV packets
	return iHWFeatures;
	}

inline TBool CLinkMgrProtocol::IsModeSupportedLocally(TBTLinkMode aMode) const
	{
	// bits 6 to 8 show us modes supported
	return iHWFeatures.IsModeSupported(aMode);
	}

inline TBool CLinkMgrProtocol::IsRoleSwitchSupportedLocally() const
	{
	return iHWFeatures.IsRoleSwitchSupported();
	}

inline TBool CLinkMgrProtocol::IsEncryptionSupportedLocally() const
	{
	return iHWFeatures.IsEncryptionSupported();
	}

inline TBool CLinkMgrProtocol::IsExtendedInquiryResponseSupportedLocally() const
	{
	return iHWFeatures.IsExtendedInquiryResponseSupported();
	}

inline TBool CLinkMgrProtocol::IsRssiWithInquiryResultsSupportedLocally() const
	{
	return iHWFeatures.IsRssiWithInquiryResultsSupported();
	}

inline TBool CLinkMgrProtocol::IsSecureSimplePairingSupportedLocally() const
	{
	return iHWFeatures.IsSecureSimplePairingSupported();
	}

inline TBool CLinkMgrProtocol::IsEncryptionPauseResumeSupportedLocally() const
	{
	return iHWFeatures.IsEncryptionPauseResumeSupported();
	}

inline TBool CLinkMgrProtocol::IsCommandSupportedLocally(TBluetoothSupportedCommandsBits aCommand) const
	{
	return iHWCommands[aCommand];
	}

#endif
