// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTMANCLIENT_H
#define BTMANCLIENT_H

#include <e32std.h>
#include <e32base.h>
#include <btdevice.h>

static const TInt KErrBluetoothRegistryCorrupt = -6501;

/** The type of device class search to perform.

The enumeration is of masks. Valid combinations are:

	EMajorDevice only
	EMajorDevice & EMinorDevice

Note: EMinorDevice may not be used on its own.
@publishedAll
@released
*/
enum TBTDeviceClassSearch
	{
	EMajorDevice = 0x01,/*!< Indicates the major device class search criteria. */
	EMinorDevice = 0x02,/*!< Indicates the minor device class search criteria. */
	};


NONSHARABLE_CLASS(TBTRegistrySearch)
	/** Set search criteria on the Bluetooth Registry.

	The search logic is "AND", so if device bonded is set, and code set to
	headsets the search results contain all the bonded headsets in the registry.

	Other things can be added to the search criteria e.g. recently used headsets
	can easil be found by adding in the LastUsed criterion.
	@publishedAll
	@released
	*/
	{
	friend class RBTDbQuery;
public:
	IMPORT_C TBTRegistrySearch();
	IMPORT_C TBTRegistrySearch(const TBTRegistrySearch& aSearch);
	IMPORT_C void Reset();
	IMPORT_C void FindAll();
	IMPORT_C void FindAddress(const TBTDevAddr& aAddress);
	IMPORT_C void FindBonded();
	IMPORT_C void FindTrusted();
	IMPORT_C void FindCoD(const TBTDeviceClass& aClass);
	IMPORT_C void FindCoD(const TBTDeviceClass& aClass, TBTDeviceClassSearch aPref);
	IMPORT_C void FindSinceSeen(const TTime& aLastSeen);
	IMPORT_C void FindSinceUsed(const TTime& aLastUsed);
	IMPORT_C void FindBluetoothName(const TDesC8& aName);
	IMPORT_C void FindFriendlyName(const TDesC& aName);
	IMPORT_C void FindCurrentProcessOwned();
	IMPORT_C void FindUiCookie(TUint32 aUiCookie);
	IMPORT_C void FindUiCookie(TUint32 aUiCookie, TUint32 aUiCookieMask);
	IMPORT_C TBTRegistrySearch& operator=(const TBTRegistrySearch& aSearch);
private:
	enum TSearchMask
		{
		EAddress	  = 0x0001,
		EBonded		  = 0x0002,
		ETrusted	  = 0x0004,
		ECoD		  = 0x0008,
		ECoDMajorDev  = 0x0010,
		ECoDMinorDev  = 0x0020,
		ECoDService   = 0x0040, // not used at present
		ELastSeen	  = 0x0080,
		ELastUsed	  = 0x0100,
		EBTName		  = 0x0200,
		EFriendlyName = 0x0400,
		EUiCookie	  = 0x2000,
		EProcess	  = 0x4000,
		EAll		  = 0x8000,
		// Top bit reserved for if the class needs to be extended.
		EExtension	  = 0x80000000
		};
private:
	TUint			iSearchMask;	// what we actually have set to search on
	TBTDevAddr		iDeviceAddress;
	TBTDeviceClass	iDeviceClass;
	TTime			iLastSeen;
	TTime			iLastUsed;
	TPtrC8			iBluetoothName;	
	TPtrC			iFriendlyName;	
	TUid			iCurrentProcessSID;
	TUint32			iUiCookie;
	TUint32			iUiCookieMask;
	
	//TUint32	iPadding1; // used for UI Cookie
	//TUint32	iPadding2; // used for UI Cookie mask.
	};


NONSHARABLE_CLASS(RBTMan) : public RSessionBase
    /** Creates a sessions over which the Bluetooth security manager is accessible.

    A session must be used in order to access a subsession, RBTManSubSession,
	that allows access to the Security Manager. The following example creates and
	connects to the security manager session.

    @code
	RBTMan secMan;
	User::LeaveIfError(secMan.Connect());
	@endcode
	 
	Sessions available thus far are Registry-like
	@publishedAll
	@released
	*/
	{
public:
	IMPORT_C RBTMan();
	IMPORT_C TInt Connect();
	IMPORT_C TVersion Version() const;
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2;	
	};

struct TBTManClientServerMessage
/**
This is to only be used by phone manufacturers, not by application developers. 
@publishedAll
*/
	{
public:
	TRequestStatus* iClientStatusToCancel;
	TBool			iClientBusy;
	};

NONSHARABLE_CLASS(RBTManSubSession) : public RSubSessionBase
    /** A subsession for the RBTMan session.

    RBTManSubSession provides the actual access to the security manager.

    Contains basic subssession functionality.
	@publishedAll
	@released
	*/
	{
public:
	/** Used to close the subsession. */
	virtual void Close() = 0; // All subsessions must provide Close overrides
	IMPORT_C void CancelRequest(TRequestStatus& aStatus);
	void LocalComplete(TRequestStatus& aStatus, TInt aErr);

protected:
	IMPORT_C RBTManSubSession();
	TBool IsBusy() const;
	void SetBusy(TRequestStatus& aStatus);
	
protected:
	TPckgBuf<TBTManClientServerMessage>	iClientServerMsg;

	// This data padding has been added to help prevent future binary compatibility breaks
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used	
	TUint32     iPadding1; 
	TUint32     iPadding2;  
	};

NONSHARABLE_CLASS(RBTRegServ) : protected RBTMan
    /** Registry access session.

    It has a RBTMan session as implementation.
	@released
	@publishedAll
	*/
	{
public:
	IMPORT_C RBTRegServ();
	IMPORT_C TInt Connect();
	IMPORT_C void Close();
	IMPORT_C RBTMan& Session();
	IMPORT_C TInt SetHeapFailure(TInt aType,TInt aRate);
	IMPORT_C TInt ResourceCount();
	
private:
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 
	};

// other functional sessions can derive from RBTMan

typedef RPointerArray<CBTDevice> RBTDeviceArray; /*!<RPointerArray of CBTDevices*/
typedef TPckgBuf<TBTNamelessDevice> TBTNamelessDevicePckgBuf; /*!<packagebuf of TBTNamelessDevice*/
typedef TPckgC<TBTDevAddr> TBTDevAddrPckg;	/*!<const package (not buf) of TBTDevAddr*/
typedef TPckgBuf<TBTDevAddr> TBTDevAddrPckgBuf; /*!<packagebuf of TBTDevAddr*/
typedef TPckgBuf<TBTRegistrySearch> TBTRegistrySearchPckgBuf; /*!<packagebuf of TBTRegistrySearch*/

class RBTRegistry;


NONSHARABLE_CLASS(CBTRegistryResponse) : public CActive
    /** Retrieves a set of results from the Registry.
	 
	This is a helper class that retrieves the set of results from a view set-up in the registry.
	A view is set-up in the registry using RBTRegistry::CreateView.
	
	@see RBTRegistry
	@publishedAll
	@released
	*/
	{
public:
	IMPORT_C static CBTRegistryResponse* NewL(RBTRegistry& aView);
	IMPORT_C void Start(TRequestStatus& aClientStatus);
	IMPORT_C RBTDeviceArray& Results();
	IMPORT_C ~CBTRegistryResponse();
private:
	CBTRegistryResponse(RBTRegistry& aView);
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();
	void DoGet();
	void ConstructL();
	enum TState {EGettingSize, EGettingResults};
private:
	RBTRegistry&		iView;
	RBTDeviceArray		iArray;
	HBufC8*				iResponseBuf;
	TPtr8				iResponsePtr;
	TState				iState;
	TRequestStatus*		iClientStatus;
	};

NONSHARABLE_CLASS(RBTRegistry) : public RBTManSubSession
    /** Creates and opens a subsession on the BT Registry Server for remote devices.
	
	This subsession allows details of remote devices to be updated, modified, examined etc.
	@publishedAll
	@released
	*/
	{
public:
	IMPORT_C RBTRegistry();
	IMPORT_C TInt Open(RBTRegServ& aSession);
	IMPORT_C void AddDeviceL(const CBTDevice& aDeviceDetails, TRequestStatus& aStatus);
	IMPORT_C void GetDevice(TBTNamelessDevice& aDevice, TRequestStatus& aStatus); // nameless device
	IMPORT_C void ModifyDevice(const TBTNamelessDevice& aDevice, TRequestStatus& aStatus); //namesless device
	IMPORT_C void UnpairDevice(const TBTDevAddr& aAddress, TRequestStatus& aStatus);
	IMPORT_C void ModifyFriendlyDeviceNameL(const TBTDevAddr& aAddress, const TDesC& aNewName, TRequestStatus& aStatus);
	IMPORT_C void ModifyBluetoothDeviceNameL(const TBTDevAddr& aAddress, const TDesC8& aNewName, TRequestStatus& aStatus);
	IMPORT_C void CreateView(const TBTRegistrySearch& aSearch, TRequestStatus& aStatus);
	IMPORT_C void UnpairAllInView(TRequestStatus& aStatus);
	IMPORT_C void DeleteAllInView(TRequestStatus& aStatus);	// depends on client process capabilities
	IMPORT_C void Close();
	IMPORT_C TInt CloseView();
	IMPORT_C void NotifyViewChange(TRequestStatus& aStatus);
	

	void PreLoad(TRequestStatus& aStatus);
	void GetResults(TPtr8& aResultBuf, TRequestStatus& aStatus);
private:
	CBufFlat*	iSendBuffer;	// lazily constructed
	TPtrC8		iSendBufferPtr;
	TBTRegistrySearchPckgBuf	iSearchPckg;
	TPckg<TBTNamelessDevice>	iDevicePckg;
	TPckgBuf<TBTDevAddr>		iAddrBuf;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};

NONSHARABLE_CLASS(RBTLocalDevice) : public RBTManSubSession
    /** Creates and opens a subsession on BT Registry Server for local devices.

    This subsession allows the local device to be updated, modified, examined etc.
	@publishedAll
    @released
    */
	{
public:
	IMPORT_C RBTLocalDevice();
	IMPORT_C TInt Open(RBTRegServ& aSession);
	IMPORT_C TInt Get(TBTLocalDevice& aLocalDevice);
	IMPORT_C TInt Modify(const TBTLocalDevice& aLocalDevice);
	IMPORT_C void Modify(const TBTLocalDevice& aLocalDevice, TRequestStatus& aStatus);
	IMPORT_C void Close();
private:
	TPckg<TBTLocalDevice> iLocalDevicePckg;
	
	// This data padding has been added to help prevent future binary compatibility breaks	
	// Neither iPadding1 nor iPadding2 have been zero'd because they are currently not used
	TUint32     iPadding1; 
	TUint32     iPadding2; 	
	};


/**
UID of the PIN entry RNotifier plugin
This is only to be used with notifiers.
@publishedPartner
@released
*/
const TUid KBTManPinNotifierUid={0x100069c9};

/**
UID of the authorisation RNotifier plugin
This is only to be used with notifiers.
@publishedPartner
@released
*/
const TUid KBTManAuthNotifierUid={0x100069cf};



NONSHARABLE_CLASS(TBTNotifierUpdateParams)
	/** Sends parameter updates to the notifier.
	
	Struct to allow us to send a device name to the Passkey Entry and Authorisation RNotifier plugins.
	This is only to be used with notifiers.
	@publishedPartner
	@released
	*/
	{
public:
	/** The device name that has been retrieved since security procedures started. */
	TBTDeviceName iName;
	/** The result. */
	TInt iResult;
	};

NONSHARABLE_CLASS(TBTNotifierParams)
	/** The device name parameter for the security notifier.
	This is only to be used with notifiers.	
	@publishedPartner
	@released
	*/
	{
public:
    /** Device address of remote device requiring authorisation. */
	TBTDevAddr iBDAddr;
	/** The device name that has been retrieved since security procedures started. */
	TBTDeviceName iName;
	};
	
NONSHARABLE_CLASS(TBTAuthorisationParams) : public TBTNotifierParams
    /** Bluetooth authorisation parameters.
	This is only to be used with notifiers.	 
	Struct to allow us to send params from the security manager to the Authorisation RNotifier plugin.
	@publishedPartner
	@released
	*/
	{
public:
    /** UID of the service requesting authorisation. */
	TUid iUid;
	};

NONSHARABLE_CLASS(TBTPasskeyNotifierParams) : public TBTNotifierParams
	/** The passkey parameter for the security notifier.
	This is only to be used with notifiers.	
	@publishedPartner
	@released
	*/
	{
public:
	/** A min passkey length required. 0 means, no length requirement */	
	TUint iPasskeyMinLength;		
	/** true - locally initiated connection, false - remote side initiated */	
	TBool iLocallyInitiated;		
	};

// These are only to be used with notifiers.
typedef TPckgBuf<TBTNotifierUpdateParams> TBTNotifierUpdateParamsPckg; /*!< packagebuf of TBTNotifierUpdateParams*/
typedef TPckgBuf<TBTNotifierParams> TBTNotifierParamsPckg; /*!< packagebuf of TBTNotifierParams*/
typedef TPckgBuf<TBTAuthorisationParams> TBTAuthorisationParamsPckg; /*!< packagebuf of TBTAuthorisationParams*/
typedef TPckgBuf<TBTPasskeyNotifierParams> TBTPasskeyNotifierParamsPckg; /*!< packagebuf of TBTPasskeyNotifierParams*/

#endif
