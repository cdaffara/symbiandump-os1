// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTTEXTNOTIFIERS_H
#define BTTEXTNOTIFIERS_H

#include <e32std.h>
#include <e32base.h>
#include <bt_sock.h>
#include <btextnotifiers.h> // contains definitions of standard Bluetooth notifiers
#include <btextnotifierspartner.h>
#include <twintnotifier.h>
#include <btdevice.h>
#include <btmanclient.h> // contains definitions of standard Bluetooth secuirty notfiers (pre-SSP)
#include <networking/panuiinterfaces.h> // for KBTPanDeviceSelectionNotifierUid

const TUid KScreenOutputChannel ={0x00000666};

/**
The maximum number of devices to show on screen at once
@internalComponent
*/
const TInt KMaxDeviceRows=12;

typedef RPointerArray<CBTDevice> RBTDevices;

enum TRegistryState
/**
The state of the registry lookup to get info about a device
@internalComponent
*/
	{
	EFinding,	//< finding device details in registry
	EGetting,	//< getting PIN from console
	};

/**
Class for containing console utility methods used by BTTextNotifiers.
*/
class BTTextNotifiersConsole
	{
public:
	static CConsoleBase* AutoSizeNewL(const TDesC& aTitle, TSize aSize);
	};
	
	
	
// -----------------
// PIN Code Entry UI
// -----------------

/**
Mixing for receiving notifications about PIN code entry from the UI.
*/
class MBTPinCodeEntryObserver
	{
public:
	virtual void MbpceoPinInputComplete(const TBTPinCode& aPin, TInt aReason) = 0;	
	};


class CBTManPinNotifierEngine;

/**
Notifier to get a PIN for pairing - This is the legacy version (i.e. not SSP aware).
@internalTechnology
*/
NONSHARABLE_CLASS(CBTManPinNotifier)
	: public CBase
	, public MNotifierBase2
	, public MBTPinCodeEntryObserver
	{
public:
	static CBTManPinNotifier* NewLC();
	
private: // from MNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	
private: // from MBTPinCodeEntryObserver
	void MbpceoPinInputComplete(const TBTPinCode& aPin, TInt aReason);

private:
	CBTManPinNotifier();
	void ConstructL();
	~CBTManPinNotifier();
	
private:
	TNotifierInfo iInfo;
	RMessage2 iMessage;
	TInt iReplySlot;
	TBool iNeedToCompleteMessage;
	CBTManPinNotifierEngine* iEngine;
	};


#ifndef __BT_TEXT_NOTIFIERS_NO_SSP__
/**
Notifier to get a PIN for pairing - This is the SSP aware version.
@internalTechnology
*/
NONSHARABLE_CLASS(CBTUIPinCodeEntry)
	: public CBase
	, public MNotifierBase2
	, public MBTPinCodeEntryObserver
	{
public:
	static CBTUIPinCodeEntry* NewLC();
	
private: // from MNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	
private: // from MBTPinCodeEntryObserver
	void MbpceoPinInputComplete(const TBTPinCode& aPin, TInt aReason);

private:
	CBTUIPinCodeEntry();
	void ConstructL();
	~CBTUIPinCodeEntry();
	
private:
	TNotifierInfo iInfo;
	RMessage2 iMessage;
	TInt iReplySlot;
	TBool iNeedToCompleteMessage;
	CBTManPinNotifierEngine* iEngine;
	};

#endif // __BT_TEXT_NOTIFIERS_NO_SSP__


class CHostResolverCacheProdder;
class CBTGetPinFromConsole;

NONSHARABLE_CLASS(CBTManPinNotifierEngine) : public CActive
	{
public:
	static CBTManPinNotifierEngine* NewL(MBTPinCodeEntryObserver& aObserver);
	~CBTManPinNotifierEngine();
	
	void StartLegacyPinEntryL(const TDesC8& aBuffer);
	void StartPinEntryL(const TDesC8& aBuffer);
	void Stop();
	
	TPtrC8 LegacyUpdateL(const TDesC8& aBuffer);
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	
	void HostResolverCacheInfoReceived(const TNameEntry& aResult);
	void PinInputComplete(const TBTPinCode& aPin, TInt aReason);

private:
	CBTManPinNotifierEngine(MBTPinCodeEntryObserver& aObserver);
	void DoStartPinEntryL();
	void StoreNameUpdateL(const TDesC8& aBuffer);
	void PerformNameUpdateL(TInt aResult);
	
private: // from CActive
	void RunL();
	void DoCancel();
	void ConstructL();
	
private:
	MBTPinCodeEntryObserver& iObserver;	// the notifier to signal when we have a PIN
	
	CHostResolverCacheProdder* iHostResolverCacheProdder;
	CBTDevice* iDevice;			// the device for which we're prompting for a PIN
	CBTGetPinFromConsole* iGetPinFromConsole;	// responsible for getting the PIN from the user
	RBTRegServ iRegistry;
	RBTRegistry iRegistryView;
	TBTRegistrySearch iRegistrySearch;
	TRegistryState iRegistryState;
	CBTRegistryResponse* iResponse;
	
	TBTDevAddr						iDeviceAddress;
	TBTDeviceName					iDeviceName;
	TUint							iPasskeyMinLength;
	TBool							iLocallyInitiated;
	TBool							iStrongKeyRequired;
	TUint							iRecommendedPasskeyMinLength;
	};

NONSHARABLE_CLASS(CHostResolverCacheProdder): public CActive
/** 
Get CoD from HostResolver class
@internalComponent
*/
	{
public:
	static CHostResolverCacheProdder* NewL(CBTManPinNotifierEngine* aParent);
	static CHostResolverCacheProdder* NewLC(CBTManPinNotifierEngine* aParent);
	~CHostResolverCacheProdder();
	TInt Start(const TBTDevAddr& aBDAddr);
	
private:
	CHostResolverCacheProdder(CBTManPinNotifierEngine* aParent);
	void DoCancel(){iHostResolver.Cancel();};
	void RunL();

private:	
 	CBTManPinNotifierEngine* iParent;
	RHostResolver iHostResolver;
	TNameEntry iResult;
	TInquirySockAddr iAddr;
	};

/**
Display a console and get a PIN response
@internalComponent
*/
NONSHARABLE_CLASS(CBTGetPinFromConsole) 
	: public CActive
	{
public:
	static CBTGetPinFromConsole* NewL(CBTManPinNotifierEngine* aParent, const CBTDevice& aDevice,
									  TUint aPasskeyMinLength, TBool aInternallyInitiated, TBool aStringKeyRequired, 
									  TUint aRecommendedPasskeyMinLength);
	~CBTGetPinFromConsole();
	
	void GetPinL();
	void DeviceNameRetrieved(const TPtrC aName, TInt aResult);
	void CoDRetrieved(TBTDeviceClass& aCoD);
	
private: // from CActive
	void RunL();
	void DoCancel();

#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	static TInt AutoNotifierCallBack(TAny *aConsolePin);
#endif
	
protected:
	CBTGetPinFromConsole(CBTManPinNotifierEngine* aPtr, TUint aPasskeyMinLength, TBool aInternallyInitiated, TBool aStrongKeyRequired, TUint aRecommendedPasskeyMinLength);
	
private:
	void ConstructL(const CBTDevice& aDevice);
	void IssueRequestL();
	
private:
	const CBTDevice* iDevice;
	CBTManPinNotifierEngine* iParent;	// the class to notify when we have a PIN code
	CConsoleBase* iConsole;	// console for interaction with the user
	TBTPinCode iPin;
	TInt iInsert;
	TUint	iPasskeyMinLength;
	TBool	iInternallyInitiated;
	TBool	iStrongKeyRequired;
	TUint	iRecommendedPasskeyMinLength;
	
#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	CAsyncCallBack* iAutoNotifierCallback;
#endif
	};

class CBTManAuthNotifierEngine;
NONSHARABLE_CLASS(CBTManAuthNotifier): public CBase, public MNotifierBase2
	{
public:
	static CBTManAuthNotifier* NewLC();
	~CBTManAuthNotifier();
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	void AuthorisationComplete(TBool aDecision, TInt aReason);

private:
	void ConstructL();

private:
	TNotifierInfo iInfo;
	CBTManAuthNotifierEngine* iEngine;
	RMessage2 iMessage;
	TInt iReplySlot;
	TBool iNeedToCompleteMessage;
	};
	
class CBTGetAuthFromConsole;
NONSHARABLE_CLASS(CBTManAuthNotifierEngine) : public CActive
/**
The implementation of authorisation notifier plugin.
@note Separate class to avoid CActive and MNotifyBase Cancel name clash.
*/
	{
public:
	static CBTManAuthNotifierEngine* NewL(CBTManAuthNotifier& aParent);
	void StartAuthorisationL(const TDesC8& aBuffer);
	void AuthorisationComplete(TBool aDecision, TInt aReason);
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	~CBTManAuthNotifierEngine();
	void Stop();

private:
	CBTManAuthNotifierEngine(CBTManAuthNotifier& aParent);
	void RunL();
	void DoCancel();

private:
	CBTGetAuthFromConsole* iGetAuthFromConsole;
	RBTRegServ iRegistry;
	RBTRegistry	iRegistryView;
	TBTRegistrySearch iRegistrySearch;
	TRegistryState iRegistryState;
	CBTRegistryResponse* iResponse;
	CBTDevice* iDevice;
	TBTAuthorisationParamsPckg iAuthPckg;
	CBTManAuthNotifier&	iParent;
	};

NONSHARABLE_CLASS(CBTGetAuthFromConsole) : public CActive
/**
Display a console and get a Y/N response
@internalComponent
*/
	{
public:
	static CBTGetAuthFromConsole* NewL(CBTManAuthNotifierEngine* aParent, const CBTDevice& aDevice, TUid aUid);
	static CBTGetAuthFromConsole* NewLC(CBTManAuthNotifierEngine* aParent, const CBTDevice& aDevice, TUid aUid);
	~CBTGetAuthFromConsole();
	void AuthoriseL();
	void RunL();
	void DoCancel();
	void DeviceNameRetrieved(const TDesC& aName, TInt aResult);
	
#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	static TInt AutoNotifierCallBack(TAny *aConsoleAuth);
#endif

protected:
	CBTGetAuthFromConsole(CBTManAuthNotifierEngine* aParent);
	
private:
	TInt GetDeviceName(const TBTDevAddr& aBDAddr, TDes& aName);
	void ConstructL(const CBTDevice& aDevice, TUid aUid);
	void IssueRequest();
	
private:
	TUid iServiceUid;
	const CBTDevice* iDevice;
	CBTManAuthNotifierEngine* iParent;
	CConsoleBase* iConsole;

#ifdef __BT_TEXT_NOTIFIERS_AUTO__
	CAsyncCallBack* iAutoNotifierCallback;
#endif
	};

NONSHARABLE_CLASS(CBTManDeviceSelectionNotifier)
	: public CBase
	, public MNotifierBase2
	{
public:
	enum TUserResult
		{
		EScanStateSelected,
		EScanStateContinue,
		EScanStateCancelled
		};
public:
	static CBTManDeviceSelectionNotifier* NewLC();
	~CBTManDeviceSelectionNotifier();
	TKeyCode GetResponseL(TRefByValue<const TDesC> aPrompt, TInt aWidth, TInt aHeight);
	void GetAddressL(TBTDevAddr& aBDAddr);
	void GetNameL(TBTDeviceName& aName);
	//void GetClassL(TBTDeviceClass& aClass);
	void DisplayErrorMessageL();
	
private: // from MNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	
private:
	CBTManDeviceSelectionNotifier();
	void ConstructL();
	TNotifierInfo iInfo;
	TBTDevAddr iBDAddr;
	TBTDeviceName iName;
	TBTDeviceClass iClass;
	};

NONSHARABLE_CLASS(CBTManDeviceScanner): public CBase
	{
public:
	enum TScanState
		{
		EScanStateUnfinished,
		EScanStateCancelled,
		EScanStateSelected
		};
public:
	static CBTManDeviceScanner* NewL();
	~CBTManDeviceScanner();
	TBool GetByScanL(TBTDevAddr& aBDAddr, TBTDeviceName& aName, TBTDeviceClass& aClass);
private:
	CBTManDeviceScanner();
	void ConstructL();

	void NotifyErrorL(TRefByValue<const TDesC> aErrorNotification);
	void ResetDeviceLists();
	void EnsureConsoleExistsL();
	void StartWaitMessageL();
	void StopWaitMessage();
	void AddDiscoveredDeviceL(RBTDevices& aDeviceArray, const TNameEntry& aNameEntry);
//	void AddDiscoveredDeviceL(CBTDevInfoArray& aDevInfoArray, TNameEntry aNameEntry);
	void SetupDeviceParams(RHostResolver aHr, TBTNamelessDevice& aDevInfo);
	void SetupDeviceParams(const THostName& aName, TBTNamelessDevice& aDevInfo);
	void PrintDiscoveredDevice(TUint aCount, CBTDevice& aDevice);
	void RubOutWaitMessage();
	void PrintDiscoveredDevice();
	TBool DeviceNumberFromUserL(TBool allowMore);
	void DisplayNoDeviceMessageL(TBool isMore);
	void CheckUnfinishedL();

private:
	CConsoleBase*		iConsole;
	CConsoleBase*		iWaitConsole;
	TPtrC				iWaitMessage;
	RBTDevices			iDevices;
	RPointerArray<TNameEntry>		iNoNameDevices;
//	CBTDevInfoArray* iDevsWithName;

	TUint iPrintedCounter;
	TUint iChosenDeviceIndex;
	TUint iResultCountWithName;
	TUint iResultCountNoName;
	TBTDeviceResponseParams iDevPms;

	TScanState iScanState;
	};

//
// BT PAN profile device discovery notifier classes
//

class MBTPanDeviceSelectionNotify
/**
Notification interface from PAN device selection engine to the notifier
@internalComponent
*/
	{
public:
	/**
	Remote device(s) have been selected
	*/
	virtual void DeviceSelectionComplete(TBTDeviceList& aDevices) = 0;
	virtual void DeviceSelectionError(TInt aError) = 0;
	
protected:
	~MBTPanDeviceSelectionNotify() {};
	};

class CBTPanDeviceSelectionNotifierEngine;
class CBTPanDeviceSelectionNotifier
	: public CBase
	, public MNotifierBase2
	, private MBTPanDeviceSelectionNotify
/**
An example notifier for Bluetooth PAN profile device selection.  This allows the selection of 
multiple devices.
@internalTechnology
*/
	{
public:
	static CBTPanDeviceSelectionNotifier* NewLC();
	~CBTPanDeviceSelectionNotifier();

	// MNotifierBase2 methods
	virtual void Release();
	virtual TNotifierInfo RegisterL();
	virtual TNotifierInfo Info() const;
	virtual TPtrC8 StartL(const TDesC8& aBuffer);
	virtual void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	virtual void Cancel();
	virtual TPtrC8 UpdateL(const TDesC8& aBuffer);

	// MBTPanDeviceSelectionNotify methods
	virtual void DeviceSelectionComplete(TBTDeviceList& aDevices);
	virtual void DeviceSelectionError(TInt aError);

private:
	CBTPanDeviceSelectionNotifier();
	void ConstructL();

public:

private:
	TNotifierInfo iInfo;			//< Info about this notifier
	RMessage2 iMessage;				//< Client message to signal when device selection is complete
	TBool iMessageOutstanding;		//< Have we got a message outstanding that needs to be completed?
	TInt iReplySlot;				//< The slot in the message that contains the reply buffer

	CBTPanDeviceSelectionNotifierEngine* iEngine;	//< Engine to do the actual device scanning and selection
	};

class MBTPanDeviceDiscoveryNotify
/**
Notification interface for when new devices are discovered
@internalComponent
*/
	{
public:
	/**
	A new device has been found
	@param aNewDevice Information about the new device
	@note aNewDevice must be copied if you want to use it after this method completes
	*/
	virtual void NewDeviceFoundL(TNameRecord& aNewDevice) = 0;
	
	/**
	The search for devices has completed
	*/
	virtual void DeviceSearchComplete(TInt aError) = 0;
	
protected:
	~MBTPanDeviceDiscoveryNotify() {};
	};

class CBTPanDeviceSelectionNotifierDeviceDiscoverer;
class CBTPanDeviceSelectionNotifierEngine : public CActive, private MBTPanDeviceDiscoveryNotify
/**
Engine to display discovered devices to the user and get them to select some
@internalComponent
*/
	{
public:
	static CBTPanDeviceSelectionNotifierEngine* NewL(MBTPanDeviceSelectionNotify& aNotify);
	~CBTPanDeviceSelectionNotifierEngine();
	
	void PromptForDevicesL(const TDesC8& aBuffer);
	void Stop();
	
	// CActive methods
	virtual void RunL();
	virtual void DoCancel();

	// MBTPanDeviceDiscoveryNotify methods
	virtual void NewDeviceFoundL(TNameRecord& aNewDevice);
	virtual void DeviceSearchComplete(TInt aError);

private:
	CBTPanDeviceSelectionNotifierEngine(MBTPanDeviceSelectionNotify& aNotify);
	void ConstructL();
	
	void DisplayInstructions(TPtr& aDisplayBuf);
	TInt DisplayDevice(TUint aDeviceNumber, TPtr& aDisplayBuf);
	void Redraw();
	void PackageSelectedDevicesAndCompleteNotifier();
	void CompleteNotifierWithError(TInt aError);
	void AbortEverything();
	
	TInt NumberOfDevicesSelected();

public:

private:
	class TBTDeviceSelectionInfo
		{
	public:
		TBool iSelected;
		TNameRecord iNameRecord;
		};
		
	RPointerArray<TBTDeviceSelectionInfo> iDevices;
	
	CConsoleBase* iConsole;
	HBufC* iDisplayBuf;					//< Buffer into which to put text for display
	TPtr iDisplayPtr;
	TUint iCurrentlySelectedDevice;		//< The index of the device to which the cursor is currently pointing
	TUint iPositionOfTopDeviceInList;	//< The index of the device which is at the top of the list currently being displayed to the user
	TBool iDeviceSearchComplete;		//< Has the device search completed?
	
	TBool iAborting;					//< Flag to stop us going round in circles as AOs complete with KErrCancel and signal us
	
	CBTPanDeviceSelectionNotifierDeviceDiscoverer* iDeviceDiscoverer;
	MBTPanDeviceSelectionNotify& iNotify;	//< the class to be notified when the devices have been selected
	};
	
class CBTPanDeviceSelectionNotifierDeviceDiscoverer : public CActive
/**
Discovers remote devices and notifies the engine about them
@internalComponent
*/
	{
public:
	static CBTPanDeviceSelectionNotifierDeviceDiscoverer* NewL(MBTPanDeviceDiscoveryNotify& iNotify);
	~CBTPanDeviceSelectionNotifierDeviceDiscoverer();
	
	void StartDeviceDiscovery();
	
	// CActive methods
	virtual void RunL();
	virtual void DoCancel();

private:
	CBTPanDeviceSelectionNotifierDeviceDiscoverer(MBTPanDeviceDiscoveryNotify& iNotify);
	void ConstructL();

public:

private:
	RSocketServ iSockServ;					//< Socket server session
	RHostResolver iHostResolver;			//< Host resolver to discover devices
	TNameEntry iNameEntry;					//< Name entry into which to return results
	TInquirySockAddr iInquiryAddr;			//< The inquiry address to use to discover devices

	MBTPanDeviceDiscoveryNotify& iNotify;	//< The class to notify when a new device is found
	};

// forward declarations
class CPbapAuthNotifierConsole;
class CPbapAuthNotifierEngine;

/**
Console utility class
@internalTechnology
*/
class PbapTextNotifiersConsole
	{
public:
	static CConsoleBase* AutoSizeNewL(const TDesC& aTitle, TSize aSize);
	};

/**
PBAP OBEX authentication notifier.
Handles notification of a PBAP client challenge when attempting to connect to the server.
@internalTechnology
*/
NONSHARABLE_CLASS(CPbapAuthNotifier): public CBase, public MNotifierBase2
	{
public:
	static CPbapAuthNotifier* NewLC();
	~CPbapAuthNotifier();

	void PasswordInputComplete(const TDesC& aPassword, TInt aReason);

public:
	// from MNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);

private:
	CPbapAuthNotifier();
	void ConstructL();

private:
	TNotifierInfo iInfo;
	RMessage2 iMessage;
	TInt iReplySlot;
	TBool iNeedToCompleteMessage;
	CPbapAuthNotifierEngine* iEngine;
	};

NONSHARABLE_CLASS(CPbapAuthNotifierEngine) : public CActive
	{
public:
	static CPbapAuthNotifierEngine* NewL(CPbapAuthNotifier& aParent);
	~CPbapAuthNotifierEngine();

	void StartPasswordEntryL(const TDesC8& aBuffer);
	void Stop();
	void PasswordInputComplete(const TDesC& aPassword, TInt aReason);

private:
	CPbapAuthNotifierEngine(CPbapAuthNotifier& aParent);
	void ConstructL();

	// from CActive
	void RunL();
	void DoCancel();

private:
	enum TRegistryState
		{
		EFinding,
		EGetting
		};
private:
	CPbapAuthNotifier& iParent;
	CBTDevice* iDevice;
	CPbapAuthNotifierConsole* iAuthConsole;
	RBTRegServ iRegistry;
	RBTRegistry iRegistryView;
	TBTRegistrySearch iRegistrySearch;
	TRegistryState iRegistryState;
	CBTRegistryResponse* iResponse;
	TPbapAuthNotifierParamsPckg iParamsPckg;
	};

/**
Display a console and get a password response
@internalComponent
*/
NONSHARABLE_CLASS(CPbapAuthNotifierConsole) : public CActive
	{
public:
	static CPbapAuthNotifierConsole* NewL(CPbapAuthNotifierEngine* aParent, const CBTDevice& aDevice, const TDesC& aRealm, TBool aRealmTruncated);
	~CPbapAuthNotifierConsole();

	void GetPasswordL();

#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
	static TInt AutoNotifierCallBack(TAny *aConsolePin);
#endif

protected:
	CPbapAuthNotifierConsole(CPbapAuthNotifierEngine* aParent, const TDesC& aRealm, TBool aRealmTruncated);

private:
	void ConstructL(const CBTDevice& aDevice);
	void IssueRequestL();

	// from CActive
	void RunL();
	void DoCancel();

private:
	CPbapAuthNotifierEngine* iParent;
	const CBTDevice* iDevice;
	TBufC<KPbapAuthRealmLength> iRealm;
	TBool iRealmTruncated;
	CConsoleBase* iConsole;
	TBuf<KPbapAuthPasswordLength> iPassword;
	
#ifdef __PBAP_TEXT_NOTIFIERS_AUTO__
	CAsyncCallBack* iAutoNotifierCallback;
#endif
	};


#ifndef __BT_TEXT_NOTIFIERS_NO_SSP__

class MBTGetNCResultFromConsoleObserver
	{
public:
	virtual void NumericComparisonComplete(TBool aDecision, TInt aReason)=0;		
	};
	
class CBTUINumericComparisonEngine;
NONSHARABLE_CLASS(CBTUINumericComparison):	public CBase,
											public MNotifierBase2,
											public MBTGetNCResultFromConsoleObserver
	{
public:
	static CBTUINumericComparison* NewLC();
	~CBTUINumericComparison();
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	
	// MBTGetNCResultFromConsoleObserver
	void NumericComparisonComplete(TBool aDecision, TInt aReason);

private:
	void ConstructL();

private:
	TNotifierInfo iInfo;
	CBTUINumericComparisonEngine* iEngine;
	RMessage2 iMessage;
	TInt iReplySlot;
	TBTNumericComparisonParamsPckg		iNumericComparisonParamsPckg;
	TBool iNeedToCompleteMessage;
	};

NONSHARABLE_CLASS(CBTUINumericComparisonEngine) : public CActive
	{
public:
	static CBTUINumericComparisonEngine* NewL(MBTGetNCResultFromConsoleObserver& aObserver);
	static CBTUINumericComparisonEngine* NewLC(MBTGetNCResultFromConsoleObserver& aObserver);
	~CBTUINumericComparisonEngine();

	void DoComparisonL(TBTNumericComparisonParams& aNumericComparisonParams);
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	void Stop();
	void RunL();
	void DoCancel();

protected:
	CBTUINumericComparisonEngine(MBTGetNCResultFromConsoleObserver& aObserver);
	
private:
	void ConstructL();
	void IssueRequest();
	
private:
	MBTGetNCResultFromConsoleObserver& iObserver;
	CConsoleBase* iConsole;
	};


// ----------------------
// Passkey Entry UI (SSP)
// ----------------------

class MBTGetPasskeyResultFromConsoleObserver
	{
public:
	virtual void PasskeyComplete(TInt aReason)=0;		
	};


class CBTUIPasskeyEntryEngine;

NONSHARABLE_CLASS(CBTUIPasskeyEntry)
	: public CBase
	, public MNotifierBase2
	, public MBTGetPasskeyResultFromConsoleObserver
	{
public:
	static CBTUIPasskeyEntry* NewLC();
	
private: // from MNotifierBase2
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	TPtrC8 StartL(const TDesC8& aBuffer);
	void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void Cancel();
	TPtrC8 UpdateL(const TDesC8& aBuffer);
		
private: // from MBTGetPasskeyResultFromConsoleObserver
	void PasskeyComplete(TInt aReason);

private:
	void ConstructL();
	~CBTUIPasskeyEntry();

private:
	TNotifierInfo iInfo;
	CBTUIPasskeyEntryEngine* iEngine;
	RMessage2 iMessage;
	TInt iReplySlot;
	TBTPasskeyDisplayParamsPckg			iPasskeyDisplayParamsPckg;
	TBTDevAddr iAddr;
	TBTDeviceName iName;
	TUint32 iNumericValue;
	TBool iNeedToCompleteMessage;
	};

NONSHARABLE_CLASS(CBTUIPasskeyEntryEngine)
	: public CActive
	{
public:
	static CBTUIPasskeyEntryEngine* NewL(MBTGetPasskeyResultFromConsoleObserver& aObserver);
	static CBTUIPasskeyEntryEngine* NewLC(MBTGetPasskeyResultFromConsoleObserver& aObserver);
	~CBTUIPasskeyEntryEngine();

	void DoPasskeyL(TBTPasskeyDisplayParams& aPasskeyParams);
	TPtrC8 UpdateL(const TDesC8& aBuffer);
	void Stop();
	void RunL();
	void DoCancel();

private:
	CBTUIPasskeyEntryEngine(MBTGetPasskeyResultFromConsoleObserver& aObserver);
	void ConstructL();
	void IssueRequest();
	
private:
	MBTGetPasskeyResultFromConsoleObserver& iObserver;
	CConsoleBase* iConsole;
	};


#endif // __BT_TEXT_NOTIFIERS_NO_SSP__



#endif // BTTEXTNOTIFIERS_H
