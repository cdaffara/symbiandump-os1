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

#include <btmanclient.h>
#include <btdevice.h>


#ifndef _REGISTRY_HELPERS_H
#define _REGISTRY_HELPERS_H

//
// Classes apertaining to MBTRegistryTaskNotifier //
//

class CBTInquiryMgr;
class CRegistrySession;
class CBTRegistryHelperBase;

enum TRegistryUpdateStatus
	{
	ENoChange,
	EModification,
	EAddition
	};

class MBTRegistryTaskNotifier
	{
public:
	virtual void RegistryTaskComplete(CBTRegistryHelperBase* /*aHelper*/,
									  TInt /*aResult*/) {};
	virtual void RegistryTaskComplete(CBTRegistryHelperBase* /*aHelper*/,
									  const TBTNamelessDevice& /*aDevice*/,
									  TInt /*aResult*/)=0;
  	virtual void RegistryTaskComplete(CBTRegistryHelperBase* /*aHelper*/,
  									  TRegistryUpdateStatus /*aModified*/,
  									  TInt /*aResult*/)=0;
	};

NONSHARABLE_CLASS(CBTRegistryHelperBase) : public CActive
	{
public:
	~CBTRegistryHelperBase();
	void DetachParent();
	
protected:
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	CBTRegistryHelperBase(MBTRegistryTaskNotifier& aParent, CRegistrySession& aRegSess);
	virtual void ConstructL();

	virtual TBool IsRegistryTaskCompleteL();
	virtual void Notify(TInt aError);
	
public:
	TSglQueLink 				iLink;
protected:
	CRegistrySession&			iRegSess;
	TBTNamelessDevice			iDevice;
	MBTRegistryTaskNotifier*	iParent;
	RBTRegistry					iRegistry;
	};



NONSHARABLE_CLASS(CBTDeviceGetter): public CBTRegistryHelperBase
	{
public:
	static CBTDeviceGetter* NewL(CRegistrySession& aRegSess,
								  MBTRegistryTaskNotifier& aParent);
	void Start(const TBTDevAddr& aAddress);
private:
	CBTDeviceGetter(MBTRegistryTaskNotifier& aParent, CRegistrySession& aRegSess);

	void Notify(TInt aError);
	};

NONSHARABLE_CLASS(CBTLinkKeyDeleter): public CBTRegistryHelperBase
	{
public:
	static CBTLinkKeyDeleter* NewL(CRegistrySession& aRegSess,
										  MBTRegistryTaskNotifier& aParent);
	void Start(const TBTDevAddr& aAddr);
private:
	CBTLinkKeyDeleter(MBTRegistryTaskNotifier& aParent, CRegistrySession& aRegSess);
	};

NONSHARABLE_CLASS(CBTDeviceModifier): public CBTRegistryHelperBase
	{
public:
	static CBTDeviceModifier* NewL(CRegistrySession& aRegSess, MBTRegistryTaskNotifier& aParent, CBTInquiryMgr& aInquiryMgr);
	void Start(const TBTNamelessDevice& aDevice, TBool aCanAddDevice);
	~CBTDeviceModifier();
	
private:
	CBTDeviceModifier(MBTRegistryTaskNotifier& aParent, CBTInquiryMgr& aInquiryMgr, CRegistrySession& aRegSess);
	void Notify(TInt aError);
	
private: // from CBTRegistryHelperBase
	TBool IsRegistryTaskCompleteL();
	
private:
	enum TModifierState
		{
		EFindingDevice,
		EAddingDevice,
		EModifyingDevice,
		EModifyingName,
		};
	
private:
	CBTInquiryMgr&			iInquiryMgr; // for updating of device name
	TModifierState			iState;
	CBTDevice* 				iDeviceWithName;
	TBool					iCanAddDevice;
	TBTRegistrySearch		iRegistrySearch; // Used for checking if an entry exists for a specified address. 
	TRegistryUpdateStatus	iRegistryUpdateStatus;	//indicate the registry modification
	};

NONSHARABLE_CLASS(CBTDeviceNameChanger): public CBTRegistryHelperBase
	{
public:
	static CBTDeviceNameChanger* NewL(CRegistrySession& aRegSess,
								  MBTRegistryTaskNotifier& aParent);
	void Start(const TBTDevAddr& aAddress, const TBTDeviceName8& aName);
private:
	CBTDeviceNameChanger(MBTRegistryTaskNotifier& aParent, CRegistrySession& aRegSess);
	TBTDevAddr		iAddr;
	TBTDeviceName8	iName;
	};
	
	
//
// Classes apertaining to MBtPairedDeviceNotifier //
//

class MBTPairedDeviceNotifier
	{
public:
	virtual void MbpdnAddPairedDevices(const RBTDeviceArray& aAddrs) = 0;
	virtual void MbpdnErrorInGettingPairedDevices(TInt aError) = 0;
	};


NONSHARABLE_CLASS(CBTPairedBDAddrGetter): public CActive
	{
public:
	~CBTPairedBDAddrGetter();
	static CBTPairedBDAddrGetter* NewL(MBTPairedDeviceNotifier& aNotifier, RBTRegServ& aRegServ);

	void Retrieve();

private:
	CBTPairedBDAddrGetter(MBTPairedDeviceNotifier& aNotifier, RBTRegServ& aRegServ);
	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);
	void ConstructL();

private:
	enum TBTPairedBDAddrGetterState
		{
		EReady,
		ECreatingView,
		EUsingResponse,
		};
private:
	MBTPairedDeviceNotifier&	iNotifier;
	RBTRegistry					iView;
	RBTRegServ&					iRegistryServer;
	CBTRegistryResponse*		iResponseHandler;
	TBTRegistrySearch			iRegistrySearch;
	TBTPairedBDAddrGetterState	iState;
	};


#endif // _REGISTRY_HELPERS_H
