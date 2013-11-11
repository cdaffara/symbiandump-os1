// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements a complete CSY for physical serial ports
// 
//

/**
 @file
*/

#include <cs_port.h>
#include <d32comm.h>
#include <c32comm.h>
#include <e32hal.h>
// new header file introduced in C32 V2, old ecuart had no header file
#include "ECUART.H"
#include <c32comm_internal.h>

// This header import is new for C32 V2 but is not configured since then we don't need to configure all
// the logging we've added to this file during V2. The ecuartlog header file when pre-processed for pre-V2
// will resolve all the logging to nothing since all its log macros for pre-V2 are defined empty.
#include "ECUARTLOG.H"

const TUint KCommLowUnit=0;  //< lowest port unit number for this CSY (i.e. COMM::0)

#if defined(__WINS__)
const TUint KCommHighUnit=1; //< highest port unit number for this CSY (i.e. COMM::1)
#else // _MARM_
const TUint KCommHighUnit=0; //< highest port unit number for this CSY (i.e. COMM::0)
#endif

const TUint KDefaultBufSize = 0x100; //< size of the Tx/Rx buffers used in CCommWriter and CCommReader

_LIT(KSerialDescription, "Built in Serial ports"); //< CSY description returned by RCommServ::GetPortInfo
#define SERIAL_NAME _S("COMM")  //< name of prefix for each com port

const TInt KReaderPriority=100; //< priority for the CCommReader active object
const TInt KWriterPriority=50;  //< priority for the CCommWriter active object
const TInt KBreakerPriority=0;  //< priority for the CCommBreaker active object
const TInt KNotifyPriority=0;   //< priority for CCommNotifyBase derived active objects


//Security policy for a port.
class TPortSecurityPolicy
	{
public:
	//Port number which this policy will apply.
	TInt iPort;
	//Security policy.
	TStaticSecurityPolicy iPolicy;
	};

_LIT_SECURITY_POLICY_PASS(KDefaultPortPassPolicy);
_LIT_SECURITY_POLICY_FAIL(KPortAlwaysFailPolicy);

#define KPlaceholderPortNumber -1	//a place holder port number used to initiliase KExplicitPortSecurityPolicies when no other ports require a policy different than the default.

//Define policy for ports which are different from default policy (KDefaultPortPolicy) here.
//Default policy is EAlwaysPass but to provide example ports 0-1 are explicitly set to EAlwaysPass as well.
//Port policies must only apply for port number 0-(KMaxUnits-1).
//Add to this array for other ports that need policing with capabilities.
const TUint KExplicitPortPoliciesCount = 0;	//Count of the number of entries in KExplicitPortSecurityPolicies.  Currently only a place holder entry exists so the count = 0.
const struct TPortSecurityPolicy KExplicitPortSecurityPolicies[]=
	{
	{KPlaceholderPortNumber, _INIT_SECURITY_POLICY_FAIL}	//this is a place holder entry which should be replaced with proper port policy when required to police a port with different capabiltiies to default policy.
//	{0, _INIT_SECURITY_POLICY_C1( ECapabilityCommDD)},	//Example showing explicitly setting a policy different than default for Port 0
//	{1, _INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, ECapabilityWriteUserData)}	//Example showing explicitly setting a policy different than default for Port 1
	};


// #define _DEBUG_CONSOLE_

#if defined(_DEBUG_CONSOLE_)
#include <e32twin.h>
class RDebugConsole : public RConsole
	{
public:
	RDebugConsole();
public:
	void Printf(TRefByValue<const TDesC> aFmt,...);
	};

#define DEBUG_TRACE(m) m

#else	// (_DEBUG)

// debug trace (disabled in Release builds)
#define DEBUG_TRACE(m)
//
#endif

//
// CCommReader CCommWriter and CCommBreaker are active objects which wait on
// completion from the serial channel for various actions.
//
class CHWPort;

/**
 * base class for CCommReader and CCommWriter
 *
 */
class CCommReadWriteBase : public CActive
	{
public:
	CCommReadWriteBase(TInt aPriority, CHWPort* aParent);
	~CCommReadWriteBase();
	TInt SetServerConfig(TCommServerConfig& aConfig);
	void GetServerConfig(TCommServerConfig& aConfig) const;
	void FreeMemory();
	 // set the role of this port unit @param aRole new role
	inline void SetRole(TCommRole aRole){iRole=aRole;};
protected:
	void SetBuffersL();
	// return ETrue if partial buffering used
	TBool UsePartial() {return ((iBufFlags & KCommBufferPartial)==KCommBufferPartial);}
protected:
	HBufC8* iBuffer;  //< pointer to the Tx/Rx buffer
	TPtr8* iBuf;      //< pointer to a TPtr8 that points to the current buffer
	TUint iBufFlags;  //< contains buffer flags e.g for partial read/write
	TInt iBufSize;    //< size of the Tx/Rx buffer
	TCommRole iRole;  //< DTE or DCE role for this port unit
	CHWPort* iParent; //< pointer to the CHWPort object
	};


#if defined (_DEBUG_DEVCOMM) && defined (_DEBUG_CONSOLE_)
class CCommDebugDumper : public CActive
	{
public:
	static CCommDebugDumper* NewL(RDebugConsole &aConsole);
	virtual void RunL();
	virtual void DoCancel();
	CCommDebugDumper::~CCommDebugDumper();
	 // set the role of this port unit @param aRole new role
	inline void SetRole(TCommRole aRole){iRole=aRole;};
private:
	CCommDebugDumper(RDebugConsole &aConsole);
private:
	TCommRole iRole;         //< DTE or DCE role for this port unit
	RDebugConsole *iConsole;
	TConsoleKey iKeystroke;
	};
#endif


/**
 * Active Comm port Reader
 *
 * This class is responsible for reading data from the physical
 * comm port.
 */
class CCommReader : public CCommReadWriteBase
	{
public:
	static CCommReader* NewL(CHWPort* aParent);
	~CCommReader();
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
	//
	void Read(const TAny* aClientBuffer, TInt aLength);
	void ReadCancel();
private:
	CCommReader(CHWPort* aParent);
	void InitL();
private:
	TUint iLength;          //< number of bytes to read
	TAny* iClientBuffer;    //< pointer to Client's buffer
	TUint iBufPos;          //< position in Client's buffer to read into (write)
	TUint iBufActive;       //< TRUE if partial buffering active
	TInt iBufOneOrMoreSize; //< buffer size of ReadOneOrMore method
	};


/**
 * Active Comm port Writer
 *
 * This class is responsible for writing data to the physical
 * comm port.
 */
class CCommWriter : public CCommReadWriteBase
	{
public:
	static CCommWriter* NewL(CHWPort* aParent);
	~CCommWriter();
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
	//
	void Write(const TAny* aClientBuffer, TInt aLength);
	void WriteCancel();
	inline void NotificationEnable();
	inline void NotificationDisable();
	inline TBool IsNotificationEnabled();
private:
	CCommWriter(CHWPort* aParent);
	void InitL();
private:
	TUint iLength;              //< number of bytes to write
	TAny* iClientBuffer;        //< pointer to Client's buffer
	TUint iBufPos;              //< position in Client's buffer to read from
	TUint iBufActive;           //< TRUE if partial buffering active
	TBool iNotificationEnabled; //< true if notification is enabled
	};



/**
 * Active Comm port Breaker
 *
 * This class is responsible for handling breaks on the physical
 * comm port.
 */
class CCommBreaker : public CActive
	{
public:
	static CCommBreaker * NewL(CHWPort* aParent);
	~CCommBreaker();
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
	//
	void Break(TInt aTime);
	 // set the role of this port unit @param aRole new role
	inline void SetRole(TCommRole aRole){iRole=aRole;};
private:
	CCommBreaker(CHWPort* aParent);
private:
	TCommRole iRole;  //< DTE or DCE role for this port unit
	CHWPort* iParent; //< pointer to the CHWPort object
	};


/**
 * Base class for Notifiers
 *
 */
class CCommNotifyBase : public CActive
	{
public:
	 // set the role of this port unit @param aRole new role
	inline void SetRole(TCommRole aRole){iRole=aRole;};
protected:
	CCommNotifyBase(CHWPort* aParent);
	~CCommNotifyBase();
protected:
	TCommRole iRole;  //< DTE or DCE role for this port unit
	CHWPort* iParent; //< pointer to the CHWPort object
	};


/**
 * This class is responsible for notifying the client when
 * the signal control lines are changing, including breaks.
 */
class CCommSignalNotifier : public CCommNotifyBase
	{
public:
	CCommSignalNotifier(CHWPort* aParent);
	~CCommSignalNotifier();
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
	//
	void Start();
	void NotifySignal(TUint aSignalMask);
	void NotifySignalCancel();
	void NotifyBreak();
	void NotifyBreakCancel();
private:
	TUint iSignals;     //< bitmask with the new signal after notification completed
	TUint iSignalMask;  //< bitmask with the signals to be notified
	TInt iNotifyCount;	//< increased when either NotifySignals or NotifyBreak is using this class
						//< decreased when RunL is called. Ensures that RComm::NotifyBreak will not
						//< interfere with operation of RComm::NotifySignalsChange
	};


/**
 * this class is responsible for notifying the client when
 * there are some incoming data available.
 */
class CCommAvailableDataNotifier : public CCommNotifyBase
	{
public:
	CCommAvailableDataNotifier(CHWPort* aParent);
	~CCommAvailableDataNotifier();
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
	//
	void Start();
	};


/**
 * this class is responsible for notifying the client when
 * the flow control is changing.
 */
class CCommFlowControlChangeNotifier : public CCommNotifyBase
	{
public:
	CCommFlowControlChangeNotifier(CHWPort* aParent);
	~CCommFlowControlChangeNotifier();
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
	//
	void Start();
private:
	TFlowControl iFlowControl; //< current flow control settings
	};


/**
 * this class is responsible for notifying the client when
 * the configuration is changing.
 */
class CCommConfigChangeNotifier : public CCommNotifyBase
	{
public:
	CCommConfigChangeNotifier(CHWPort* aParent);
	~CCommConfigChangeNotifier();
	void Start();
	// from CActive
	virtual void DoCancel();
	virtual void RunL();
private:
	TCommNotificationPckg iConfig; //< current configuration package
	};


/**
 * CPort is the object which interfaces to the commserver
 */
class CHWPort : public CPort
	{
public:
	static CHWPort* NewL(TUint aUnit);
private:
	 CHWPort();
public:
	virtual void StartRead(const TAny* aClientBuffer,TInt aLength);
	virtual void ReadCancel();
	virtual TInt QueryReceiveBuffer(TInt& aLength) const;
	virtual void ResetBuffers(TUint aFlags);
	virtual void StartWrite(const TAny* aClientBuffer,TInt aLength);
	virtual void WriteCancel();
	virtual void Break(TInt aTime);
	virtual void BreakCancel();
	virtual TInt GetConfig(TDes8& aDes) const;
	virtual TInt SetConfig(const TDesC8& aDes);
	virtual TInt SetServerConfig(const TDesC8& aDes);
	virtual TInt GetServerConfig(TDes8& aDes);
	virtual TInt GetCaps(TDes8& aDes);
	virtual TInt GetSignals(TUint& aSignals);
	virtual TInt SetSignalsToMark(TUint aSignals);
	virtual TInt SetSignalsToSpace(TUint aSignals);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aSignals);
	virtual void Destruct();
	virtual void FreeMemory();
	virtual void NotifySignalChange(TUint aSignalMask);
	virtual void NotifySignalChangeCancel();
	virtual void NotifyConfigChange();
	virtual void NotifyConfigChangeCancel();
	virtual void NotifyFlowControlChange();
	virtual void NotifyFlowControlChangeCancel();
	virtual void NotifyBreak();
	virtual void NotifyBreakCancel();
	virtual void NotifyDataAvailable();
	virtual void NotifyDataAvailableCancel();
	virtual void NotifyOutputEmpty();
	virtual void NotifyOutputEmptyCancel();
	virtual TInt GetFlowControlStatus(TFlowControl& aFlowControl);
	virtual TInt GetRole(TCommRole& aRole);
	virtual TInt SetRole(TCommRole aRole);

	virtual ~CHWPort();
#if defined (_DEBUG_DEVCOMM)
	virtual void DoDumpDebugInfo(const RMessage2 &aMessage);
#endif
	RBusDevComm& DTEPort();
	RBusDevCommDCE& DCEPort();
private:
	CCommReader* iReader;                                       //< pointer to the Active Reader
	CCommWriter* iWriter;                                       //< pointer to the Active Writer
	CCommBreaker* iBreaker;                                     //< pointer to the Active Breaker
	CCommSignalNotifier* iSignalNotifier;                       //< pointer to the Active Signal Notifier
	CCommAvailableDataNotifier* iDataAvailableNotifier;         //< pointer to the Active Data available notifier
	CCommFlowControlChangeNotifier* iFlowControlChangeNotifier; //< pointer to the Active Flow Control notifier
	CCommConfigChangeNotifier* iConfigChangeNotifier;           //< pointer to the Active Config Change notifier

	RBusDevComm iPort;           //< interface to the logical Comm port device (DTE role)
	RBusDevCommDCE iPortDCE;     //< interface to the logical Comm port device (DCE role)
	TCommRole iRole;             //< DTE or DCE role for this port unit
	TUint iPortName;             //< contains the port unit number (i.e. 0 for COMM::0)
	TBool iEarlyWriteCompletion; //< ETrue if early write completion enabled
#if defined (_DEBUG_CONSOLE_)
#if defined (_DEBUG_DEVCOMM)
	CCommDebugDumper *iDumper;
#endif
public:
	RDebugConsole iConsole;
#endif
	};


/**
 * "Entry point object" makes the objects which do the work
 */
class CHWPortFactory : public CSerial
	{
public:
    CHWPortFactory();
	virtual CPort* NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo &aSerialInfo);

public:	//from CSerial
	TSecurityPolicy PortPlatSecCapability(TUint aPort) const;
	};


/**
 * this class handles the 'Read one or more' method
 * for ports with role DTE
 */
class RMaxComm : public RBusDevComm
	{
public:
	static void ReadOneOrMore(RBusDevComm& aComm, TRequestStatus& aStatus, TDes8& aDes, TInt aLen);
	};


void RMaxComm::ReadOneOrMore(RBusDevComm& aComm, TRequestStatus& aStatus, TDes8& aDes, TInt aLen)
/**
 * reads one or more bytes from the physical DTE com port
 *
 * @param aComm   handle to the logical com port device
 * @param aStatus handle for asyncronous communications
 * @param aDes    buffer to be read into
 * @param aLen    max number of bytes to read
 */
    {
	LOGECUART2(_L8("RMaxComm::ReadOneOrMore(), (max. bytes to read) aLen = %d"), aLen);
	RMaxComm& r = (RMaxComm&)aComm;
	aLen = -aLen; // Note: negative length here means ReadOneOrMore
	              //       see RComm::ReadOneOrMore in CC_CLI.CPP
	r.DoRequest(ERequestRead, aStatus, &aDes, &aLen);
    }


/**
 * this class handles the 'Read one or more' method
 * for ports with role DCE
 */
class RMaxCommDCE : public RBusDevCommDCE
	{
public:
	static void ReadOneOrMore(RBusDevCommDCE& aComm, TRequestStatus& aStatus, TDes8& aDes, TInt aLen);
	};


void RMaxCommDCE::ReadOneOrMore(RBusDevCommDCE& aComm, TRequestStatus& aStatus, TDes8& aDes, TInt aLen)
/**
 * reads one or more bytes from the physical DCE com port
 *
 * @param aComm   handle to the logical com port device
 * @param aStatus handle for asyncronous communications
 * @param aDes    buffer to be read into
 * @param aLen    max number of bytes to read
 */
    {
	LOGECUART2(_L8("RMaxCommDCE::ReadOneOrMore(), (max. bytes to read) aLen = %d"), aLen);
	RMaxCommDCE& r = (RMaxCommDCE&)aComm;
	aLen = -aLen; // Note: negative length here means ReadOneOrMore
	              //       see RComm::ReadOneOrMore in CC_CLI.CPP
	r.DoRequest(ERequestRead, aStatus, &aDes, &aLen);
    }




void Fault(TCommFault aFault)
/**
 * Panic the comm module (us)
 *
 * @param aFault Panic code
 */
	{
	LOGECUART3(_L8("Fault(), (TCommFault) aFault = %d (%S)"), aFault, &TECUARTLOG::CommFaultStr(aFault));
	_LIT(KCsyPanicCategory, "CHWComm" );
	User::Panic(KCsyPanicCategory, aFault);
	}


//
// implementation of CCommReadWriteBase
//


CCommReadWriteBase::CCommReadWriteBase(TInt aPriority, CHWPort* aParent)
/**
 * C'tor
 *
 * @param aPriority priority of the Active Object
 * @param aParent   pointer to the owner, the CHWPort object
 */
	:CActive(aPriority)
	,iBufSize(KDefaultBufSize)
	,iRole(ECommRoleDTE)
	,iParent(aParent)
	{
	LOGECUART1(_L8("CCommReadWriteBase::CCommReadWriteBase()"));
	}


CCommReadWriteBase::~CCommReadWriteBase()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommReadWriteBase::~CCommReadWriteBase()"));
	delete iBuffer;
	delete iBuf;
	}


TInt CCommReadWriteBase::SetServerConfig(TCommServerConfig& aConfig)
/**
 * Set the port to use partial reads/writes or the bungee buffer
 *
 * @param aConfig new Comm server configurations
 * @return TInt error code. KErrNone for sucess
 */
	{
	LOGECUART1(_L8("CCommReadWriteBase::SetServerConfig()"));
	TCommServerConfigV01& c = aConfig();
	TInt res = KErrNone;
	if (c.iBufFlags & KCommBufferPartial)
		{
		TInt bufSave = iBufSize;
		iBufSize = c.iBufSize;
		TRAP(res, SetBuffersL();)
		if (res==KErrNone)
			iBufFlags = c.iBufFlags;
		else
			iBufSize = bufSave;
		}
	return res;
	}


void CCommReadWriteBase::GetServerConfig(TCommServerConfig& aConfig) const
/**
 * read the comm server buffer config from the reader
 *
 * @param aConfig reference to Comm server configurations to be changed
 */
	{
	LOGECUART1(_L8("CCommReadWriteBase::GetServerConfig()"));
	aConfig().iBufFlags = iBufFlags;
	aConfig().iBufSize = iBufSize;
	}


void CCommReadWriteBase::FreeMemory()
/**
 * Reduce allocation levels by order of the comm server
 */
	{
	LOGECUART1(_L8("CCommReadWriteBase::FreeMemory()"));
	TRAP_IGNORE(SetBuffersL());
	}


void CCommReadWriteBase::SetBuffersL()
/**
 * Attempt to free up some memory
 *
 * @leave This function may leave in case of OOM
 */
	{
	LOGECUART1(_L8("CCommReadWriteBase::SetBuffersL()"));
	if (!IsActive())
		{
		TInt allocLen = Align4(iBufSize);
		delete iBuffer;
		delete iBuf;
		iBuf = NULL;   // set to NULL, in case new leaves
		iBuffer = NULL;
		iBuffer = HBufC8::NewMaxL(allocLen);
		iBuf = new (ELeave) TPtr8((TText8*)iBuffer->Des().Ptr(), allocLen, allocLen);
		}
	}


//
// implementation of CCommReader
//


CCommReader* CCommReader::NewL(CHWPort* aParent)
/**
 * static function, create and init a comm reader active object
 *
 * @param aParent pointer to the CHWPort object
 * @return a newly created CCommReader object
 */
	{
	LOGECUART1(_L8("CCommReader::NewL()"));
	CCommReader* c = new (ELeave) CCommReader(aParent);
	CleanupStack::PushL(c);
	c->InitL();
	CleanupStack::Pop();
	CActiveScheduler::Add(c);
	return c;
	}


CCommReader::~CCommReader()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommReader::~CCommReader()"));
	Cancel();
	}


void CCommReader::DoCancel()
/**
 * Cancel an outstanding request
 */
	{
	LOGECUART1(_L8("CCommReader::DoCancel()"));
	__ASSERT_ALWAYS(IsActive(), Fault(ECancelNotOutstanding));
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().ReadCancel();
	else
		iParent->DCEPort().ReadCancel();
	}


void CCommReader::RunL()
/**
 * Read request has completed
 */
	{
	LOGECUART2(_L8("CCommReader::RunL(), iStatus = %d"), iStatus.Int());
	DEBUG_TRACE((iParent->iConsole.Printf(_L("CCommReader::RunL\n\r"))));

	TInt len = iLength;

	DEBUG_TRACE((iParent->iConsole.Printf(_L("Write %x bytes to 0x%x+%x\n\r"),iBuf->Size(),iPendingRead.Ptr0(),iBufPos)));
	iParent->IPCWrite(iClientBuffer,*iBuf, iBufPos);

	if (iBufActive)
		{
		iBufPos+=iBufSize;
		TInt left;

		if (iBufOneOrMoreSize)
			{
			//
			//	We are in ReadOneOrMore HELL
			//
			left=iBufOneOrMoreSize-iBufPos;
			if (left<iBufSize)
				{
				len=left;
				iBufActive=EFalse;
				}
			else
				len=iBufSize;
			}
		else
			{
			left=len-iBufPos;
			if (left<iBufSize)
				{
				len=left;
				iBufActive=EFalse;
				}
			else
				len=iBufSize;
			}
		//
		//	Next block
		//
		iBuf->SetLength(0);

		LOGECUART2(_L8("Read %d bytes from serial"), len);
		DEBUG_TRACE((iParent->iConsole.Printf(_L("read %x bytes from serial\n\r"),len)));
		if (iRole==ECommRoleDTE)
			iParent->DTEPort().Read(iStatus,*iBuf,len);
		else
			iParent->DCEPort().Read(iStatus,*iBuf,len);;
		SetActive();
		return;
		}
	LOGECUART1(_L8("Read Completed"));
	DEBUG_TRACE((iParent->iConsole.Write(_L("readCompleted \n\r"))));

	iParent->ReadCompleted(iStatus.Int());
	}



void CCommReader::Read(const TAny* aClientBuffer, TInt aLength)
/**
 * resize the buffer and queue a read
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength number of bytes to read
 */
	{
	LOGECUART2(_L8("CCommReader::Read(), aLength (number of Bytes to read) = %d"), aLength);
	iClientBuffer=(TAny*)aClientBuffer;
	iLength=aLength;
	TInt& len=aLength;
	if (len==0)
		{
		iParent->ReadCompleted(KErrNone);
		return;
		}

	TBool doOneOrMore=EFalse;
	if (len<0)
		{
		doOneOrMore=ETrue;
		len=-len;
		}


	if ((UsePartial() && len>iBufSize) || (UsePartial()==EFalse && len>iBuf->MaxLength()))
		{
		if (UsePartial()==EFalse)
			{
			TInt allocLen=Align4(len);

			TRAPD(res, iBuffer=iBuffer->ReAllocL(allocLen));
			if(res != KErrNone)
				{
				delete iBuffer;
				iBuffer=NULL;
				LOGECUART1(_L8("Read Completed"));
				DEBUG_TRACE((iParent->iConsole.Write(_L("readCompleted \n\r"))));
				iParent->ReadCompleted(res);
				return;
				}

			delete iBuf;
			iBuf=NULL;
			iBuf=new TPtr8((TText8*)iBuffer->Des().Ptr(), allocLen, allocLen);
			if (iBuf == NULL)
				{
				iParent->ReadCompleted(KErrNoMemory);
				return;
				}
			}
		else
			{
			iBufActive=ETrue;
			iBufPos=0;
			len=iBufSize;
			}
		}


	iBuf->SetLength(0);
	if (iBufActive)
		{
		if (doOneOrMore)
			{
			if (iRole==ECommRoleDTE)
				{
				RBusDevComm port = iParent->DTEPort();
				iBufOneOrMoreSize = port.QueryReceiveBuffer();
				if (iBufOneOrMoreSize < iBuffer->Length())
					{
					RMaxComm::ReadOneOrMore(port, iStatus, *iBuf, len);
					iBufActive = EFalse; // Not needed
					}
				else
					{
					//	Just do normal reads on it
					LOGECUART2(_L8("Read %d bytes from serial"), len);
					DEBUG_TRACE((iParent->iConsole.Printf(_L("read %x bytes from serial\n\r"),len)));
					port.Read(iStatus,*iBuf,len);
					}
				}
			else
				{
				RBusDevCommDCE port = iParent->DCEPort();
				iBufOneOrMoreSize=port.QueryReceiveBuffer();
				if (iBufOneOrMoreSize<iBuffer->Length())
					{
					RMaxCommDCE::ReadOneOrMore(port,iStatus,*iBuf,len);
					iBufActive=EFalse; // Not needed
					}
				else
					{
					//	Just do normal reads on it
					LOGECUART2(_L8("Read %d bytes from serial"), len);
					DEBUG_TRACE((iParent->iConsole.Printf(_L("read %x bytes from serial\n\r"),len)));
					port.Read(iStatus,*iBuf,len);
					}
				}
			}
		else
			{
			LOGECUART2(_L8("Read %d bytes from serial"), len);
			DEBUG_TRACE((iParent->iConsole.Printf(_L("read %x bytes from serial\n\r"),len)));
			if (iRole==ECommRoleDTE)
				iParent->DTEPort().Read(iStatus,*iBuf,len);
			else
				iParent->DCEPort().Read(iStatus,*iBuf,len);
			}
		}
	else
		{
		if (iRole==ECommRoleDTE)
			{
			RBusDevComm port = iParent->DTEPort();
			if (doOneOrMore)
				RMaxComm::ReadOneOrMore(port,iStatus,*iBuf,len);
			else
				{
				LOGECUART2(_L8("Read %d bytes from serial"), len);
				DEBUG_TRACE((iParent->iConsole.Printf(_L("read %x bytes from serial\n\r"),len)));
				port.Read(iStatus,*iBuf,len);
				}
			}
		else
			{
			RBusDevCommDCE port = iParent->DCEPort();
			if (doOneOrMore)
				RMaxCommDCE::ReadOneOrMore(port,iStatus,*iBuf,len);
			else
				{
				LOGECUART2(_L8("Read %d bytes from serial"), len);
				DEBUG_TRACE((iParent->iConsole.Printf(_L("read %x bytes from serial\n\r"),len)));
				port.Read(iStatus,*iBuf,len);
				}
			}
		}

	SetActive();
	}


void CCommReader::ReadCancel()
/**
 * Cancel a read
 */
	{
	LOGECUART1(_L8("CCommReader::ReadCancel()"));
	if(IsActive())
		{
		Cancel();
		if(iBuf->Length()) // Copy any data to client
			iParent->IPCWrite(iClientBuffer, *iBuf, iBufPos); // todo check return value ?
		}
	}


CCommReader::CCommReader(CHWPort* aParent)
/**
 * C'tor - pass priority up to CActive
 */
	:CCommReadWriteBase(KReaderPriority, aParent)
	{
	}


void CCommReader::InitL()
/**
 * Allocate buffers
 */
	{
	SetBuffersL();
	}


//
// implementation of CCommWriter
//


CCommWriter* CCommWriter::NewL(CHWPort* aParent)
/**
 * static function, create and init a comm writer active object
 *
 * @param aParent pointer to the CHWPort object
 * @return a newly created CCommWriter object
 */
	{
	LOGECUART1(_L8("CCommWriter::NewL()"));
	CCommWriter* c = new (ELeave) CCommWriter(aParent);
	CleanupStack::PushL(c);
	c->InitL();
	CleanupStack::Pop();
	CActiveScheduler::Add(c);
	return c;
	}


CCommWriter::~CCommWriter()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommWriter::~CCommWriter()"));
	Cancel();
	}


void CCommWriter::DoCancel()
/**
 * Cancel a pending write
 */
	{
	LOGECUART1(_L8("CCommWriter::DoCancel()"));
	__ASSERT_ALWAYS(IsActive(), Fault(ECancelNotOutstanding));
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().WriteCancel();
	else
		iParent->DCEPort().WriteCancel();
	}


void CCommWriter::RunL()
/**
 * a write has completed
 */
	{
	LOGECUART2(_L8("CCommWriter::RunL(), iStatus = %d"), iStatus.Int());
	DEBUG_TRACE((iParent->iConsole.Write(_L("CCommWriter::RunL\n\r"))));
	if (iBufActive)
		{
		//
		//	Did the last chunk get through OK ?
		//
		if (iStatus.Int() != KErrNone)
			{
			iBufActive=EFalse;
			iBufPos=0;
			iParent->WriteCompleted(iStatus.Int());
			return;
			}

		TInt len=iLength;
		TInt left=len-iBufPos;
		//
		//	Read the next chunk into our buffer
		//
		DEBUG_TRACE((iParent->iConsole.Printf(_L("Read %x bytes from 0x%x+%x\n\r"),iBuf->Size(),iPendingWrite.Ptr0(),iBufPos)));
		TInt res;
		if((res=iParent->IPCRead(iClientBuffer,*iBuf, iBufPos))!=KErrNone)
			{
			iParent->WriteCompleted(res);
			return;
			}
		//
		//	Still need buffering ?
		//
		if (left<iBufSize)
			{
			iBufActive=EFalse;
			iBufPos=0;
			len=left;
			}
		else
			{
			iBufPos+=iBufSize;
			len=iBufSize;
			}
		iBuf->SetLength(len);
		LOGECUART2(_L8("Write %x bytes to serial"), len);
		DEBUG_TRACE((iParent->iConsole.Printf(_L("Write %x bytes to serial\n\r"),len)));
		if (iRole==ECommRoleDTE)
			iParent->DTEPort().Write(iStatus,*iBuf,len);
		else
			iParent->DCEPort().Write(iStatus,*iBuf,len);
		SetActive();
		return;
		}
	LOGECUART1(_L8("Write Completed"));
	DEBUG_TRACE((iParent->iConsole.Write(_L("writeCompleted \n\r"))));

	if (iNotificationEnabled)
		{
		iParent->NotifyOutputEmptyCompleted(iStatus.Int());
		iNotificationEnabled=EFalse;
		}
	iParent->WriteCompleted(iStatus.Int());
	}


void CCommWriter::Write(const TAny* aClientBuffer, TInt aLength)
/**
 * resize the buffer and queue a write
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength number of bytes to write
 */
	{
	LOGECUART2(_L8("CCommWriter::Write(), aLength = %d"), aLength);
	iClientBuffer = (TAny*)aClientBuffer;
	iLength = aLength;

	TInt& len = aLength;
	if (len==0)
		{
		if (iRole==ECommRoleDTE)
			iParent->DTEPort().Write(iStatus, TPtrC8(NULL,0));
		else
			iParent->DCEPort().Write(iStatus, TPtrC8(NULL,0));
		}
	else
		{
		if ((UsePartial() && len>iBufSize) || (UsePartial()==EFalse && len>iBuf->MaxLength()))
			{
			if (UsePartial()==EFalse)
				{
				TInt allocLen=Align4(len);

				TRAPD(res, iBuffer=iBuffer->ReAllocL(allocLen));
				if(res!=KErrNone)
					{
					delete iBuffer;
					iBuffer=NULL;
					iParent->WriteCompleted(res);
					return;
					}

				delete iBuf;
				iBuf=NULL;
				iBuf = new TPtr8((TText8*)iBuffer->Des().Ptr(), allocLen, allocLen);
				if (iBuf == NULL)
					{
					iParent->WriteCompleted(KErrNoMemory);
					return;
					}
				}
			else
				{
				iBufActive=ETrue;
				len=iBufSize;
				iBufPos=len;
				}
			}

		iBuf->SetLength(len);
		LOGECUART4(_L8("Read %d bytes from 0x%x+%x"), iBuf->Size(), aClientBuffer, iBufPos);
		DEBUG_TRACE((iParent->iConsole.Printf(_L("Read %x bytes from 0x%x+%x\n\r"),iBuf->Size(),aClientBuffer,iBufPos)));
		TInt res;
		if((res=iParent->IPCRead(aClientBuffer,*iBuf))!=KErrNone)
			{
			iParent->WriteCompleted(res);
			return;
			}
		LOGECUART2(_L8("Write %x bytes to serial"), len);
		DEBUG_TRACE((iParent->iConsole.Printf(_L("Write %x bytes to serial\n\r"),len)));

		if (iRole==ECommRoleDTE)
			iParent->DTEPort().Write(iStatus, *iBuf, len);
		else
			iParent->DCEPort().Write(iStatus, *iBuf, len);
		}

	SetActive();
	}


void CCommWriter::WriteCancel()
/**
 * Cancel a write
 */
	{
	LOGECUART1(_L8("CCommWriter::WriteCancel()"));
	Cancel();
	}


void CCommWriter::NotificationEnable()
/**
 * enables notification when output buffer is empty.
 * activated when a clients calls RComm::NotifyOutputEmpty
 */
	{
	LOGECUART1(_L8("CCommWriter::NotificationEnable()"));
	iNotificationEnabled = ETrue;
	}


void CCommWriter::NotificationDisable()
/**
 * disables notification when output buffer is empty
 * deactivated when a clients calls RComm::NotifyOutputEmptyCancel
 */
	{
	LOGECUART1(_L8("CCommWriter::NotificationDisable()"));
	iNotificationEnabled = EFalse;
	}


TBool CCommWriter::IsNotificationEnabled()
/**
 * returns ETrue if notification on output buffer empty is enabled
 */
	{
	LOGECUART2(_L8("CCommWriter::IsNotificationEnabled(), iNotificationEnabled = %d"), iNotificationEnabled);
	return iNotificationEnabled;
	}


CCommWriter::CCommWriter(CHWPort* aParent)
/**
 * C'Tor
 */
	:CCommReadWriteBase(KWriterPriority, aParent)
	{
	}


void CCommWriter::InitL()
/**
 * Allocate buffers
 */
	{
	SetBuffersL();
	}


//
// implementation of CCommBreaker
//


CCommBreaker* CCommBreaker::NewL(CHWPort* aParent)
/**
 * static function, create a comm breaker object
 *
 * @param aParent pointer to the CHWPort object
 * @return a newly created CCommBreaker object
 */
	{
	LOGECUART1(_L8("CCommBreaker::NewL()"));
	CCommBreaker* c = new (ELeave) CCommBreaker(aParent);
	CActiveScheduler::Add(c);
	return c;
	}


CCommBreaker::~CCommBreaker()
/**
 * D'Tor
 */
	{
	LOGECUART1(_L8("CCommBreaker::~CCommBreaker()"));
	Cancel();
	}


void CCommBreaker::DoCancel()
/**
 * Cancel a pending break.
 */
	{
	LOGECUART1(_L8("CCommBreaker::DoCancel()"));
	__ASSERT_ALWAYS(IsActive(), Fault(ECancelNotOutstanding));
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().BreakCancel();
	else
		iParent->DCEPort().BreakCancel();
	}


void CCommBreaker::RunL()
/**
 * a break has completed. forward the news to the parent
 */
	{
	TInt status = iStatus.Int();
	LOGECUART2(_L8("CCommBreaker::RunL(), iStatus = %d"), status);
	iParent->BreakCompleted(status);
	}


void CCommBreaker::Break(TInt aTime)
/**
 * Queue a break operation
 *
 * @param aTime Length of break in micro seconds
 */
	{
	LOGECUART2(_L8("CCommBreaker::Break(), aTime (length of break in micro secs) = %d"), aTime);
	if (aTime==0)
		{
		iParent->BreakCompleted(KErrNone);
		}
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().Break(iStatus, aTime);
	else
		iParent->DCEPort().Break(iStatus, aTime);
	SetActive();
	}


CCommBreaker::CCommBreaker(CHWPort* aParent)
/**
 * C'Tor
 *
 * @param aParent pointer to the CHWPort object
 */
	:CActive(KBreakerPriority)
	,iRole(ECommRoleDTE)
	,iParent(aParent)
	{
	}


//
// implementation of CCommNotifyBase
//


CCommNotifyBase::CCommNotifyBase(CHWPort* aParent)
/**
 * C'tor
 *
 * @param aParent pointer to the CHWPort object
 */
	:CActive(KNotifyPriority)
	,iRole(ECommRoleDTE)
	,iParent(aParent)
	{
	CActiveScheduler::Add(this);
	}


CCommNotifyBase::~CCommNotifyBase()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommNotifyBase::~CCommNotifyBase()"));
	}


//
// implementation of CCommSignalNotifier
//


CCommSignalNotifier::CCommSignalNotifier(CHWPort* aParent)
/**
 * C'tor
 *
 * @param aParent pointer to the CHWPort object
 */
	:CCommNotifyBase(aParent)
	{
	LOGECUART1(_L8("CCommSignalNotifier::CCommSignalNotifier()"));
	}


CCommSignalNotifier::~CCommSignalNotifier()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommSignalNotifier::~CCommSignalNotifier()"));
	}


void CCommSignalNotifier::DoCancel()
/**
 * Cancel a pending notification.
 */
	{
	LOGECUART1(_L8("CCommSignalNotifier::DoCancel()"));
	__ASSERT_ALWAYS(IsActive(), Fault(ECancelNotOutstanding));
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().NotifySignalChangeCancel();
	else
		iParent->DCEPort().NotifySignalChangeCancel();
	}


void CCommSignalNotifier::RunL()
/**
 * a notification has completed
 */
	{
	LOGECUART2(_L8("CCommSignalNotifier::RunL(), iStatus = %d"), iStatus.Int());
	if (iStatus.Int())
		{
		iParent->SignalChangeCompleted(iSignals,iStatus.Int());
		iParent->BreakNotifyCompleted(iStatus.Int());
		iSignalMask = 0;
		iNotifyCount = 0;
		return;
		}

	if (iSignals & ((KSignalDTEOutputs|KSignalDTEInputs)*KSignalChanged))	// these don't include BREAK
		{
		iNotifyCount--;
		iParent->SignalChangeCompleted(iSignals,iStatus.Int());
		iSignalMask &= (KSignalBreak);	// mask out everything but the BREAK signal
		}
	if (iSignals & KBreakChanged)
		{
		iNotifyCount--;
		iParent->BreakNotifyCompleted(iStatus.Int());
		iSignalMask&=(~KSignalBreak);
		}
	__ASSERT_ALWAYS(iNotifyCount>=0, Fault(ENegativeCount));
	if (iNotifyCount>0)
		Start();
	}

void CCommSignalNotifier::Start()
/**
 * start the notification
 */
	{
	LOGECUART1(_L8("CCommSignalNotifier::Start()"));
	if (IsActive())
		Cancel();
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().NotifySignalChange(iStatus, iSignals, iSignalMask);
	else
		iParent->DCEPort().NotifySignalChange(iStatus, iSignals, iSignalMask);
	SetActive();
	}


void CCommSignalNotifier::NotifySignal(TUint aSignalMask)
/**
 * notify client when signal changes
 *
 * @param aSignalMask bitmask with the signals to be notified on
 */
	{
	LOGECUART2(_L8("CCommSignalNotifier::NotifySignal(), aSignalMask = %d"), aSignalMask);
	TUint reqSigs = aSignalMask & ~KSignalBreak;
	if (!reqSigs)
		{// User has only asked to be notified of break.
		iParent->SignalChangeCompleted(iSignals, KErrArgument);
		return;
		}
	iNotifyCount++;
	iSignalMask |= reqSigs;
	Start();
	}


void CCommSignalNotifier::NotifySignalCancel()
/**
 * cancel an outstanding signal change notification
 */
	{
	LOGECUART1(_L8("CCommSignalNotifier::NotifySignalCancel()"));
	Cancel();
	iSignalMask &= (KSignalBreak);	// set mask to zero, excluding BREAK
	if (--iNotifyCount>0)	// NotifyBreak may still be outstanding
		Start();
	}


void CCommSignalNotifier::NotifyBreak()
/**
 * notify client if break occurs
 */
	{
	LOGECUART1(_L8("CCommSignalNotifier::NotifyBreak()"));
	iNotifyCount++;
	iSignalMask |= KSignalBreak;
	Start();
	}


void CCommSignalNotifier::NotifyBreakCancel()
/**
 * cancel an outstanding break notification
 */
	{
	LOGECUART1(_L8("CCommSignalNotifier::NotifyBreakCancel()"));
	Cancel();
	iSignalMask &= (~KSignalBreak);	// mask out the BREAK signal
	if (--iNotifyCount>0)					// NotifySignals may still be outstanding
		Start();
	}


//
// implementation of CCommAvailableDataNotifier
//


CCommAvailableDataNotifier::CCommAvailableDataNotifier(CHWPort* aParent)
/**
 * C'tor
 *
 * @param aParent pointer to the CHWPort object
 */
	:CCommNotifyBase(aParent)
	{
	}


CCommAvailableDataNotifier::~CCommAvailableDataNotifier()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommAvailableDataNotifier::~CCommAvailableDataNotifier()"));
	}


void CCommAvailableDataNotifier::DoCancel()
/**
 * Cancel a pending data available notification.
 */
	{
	__ASSERT_ALWAYS(IsActive(), Fault(ECancelNotOutstanding));
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().NotifyReceiveDataAvailableCancel();
	else
		iParent->DCEPort().NotifyReceiveDataAvailableCancel();
	}


void CCommAvailableDataNotifier::RunL()
/**
 * the notification has completed, and data is available
 */
	{
	LOGECUART2(_L8("CCommAvailableDataNotifier::RunL(), iStatus = %d"), iStatus.Int());
	iParent->NotifyDataAvailableCompleted(iStatus.Int());
	}


void CCommAvailableDataNotifier::Start()
/**
 * start notification on data available
 */
	{
	LOGECUART1(_L8("CCommAvailableDataNotifier::Start()"));
	if (iRole==ECommRoleDTE)
		iParent->DTEPort().NotifyReceiveDataAvailable(iStatus);
	else
		iParent->DCEPort().NotifyReceiveDataAvailable(iStatus);
	SetActive();
	}


//
// implementation of CCommFlowControlChangeNotifier
//


CCommFlowControlChangeNotifier::CCommFlowControlChangeNotifier(CHWPort* aParent)
/**
 * C'tor
 *
 * @param aParent pointer to the CHWPort object
 */
	:CCommNotifyBase(aParent)
	{
	}


CCommFlowControlChangeNotifier::~CCommFlowControlChangeNotifier()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommFlowControlChangeNotifier::~CCommFlowControlChangeNotifier()"));
	}


void CCommFlowControlChangeNotifier::DoCancel()
/**
 * Cancel a pending flow control change notification.
 */
	{
	LOGECUART1(_L8("CCommFlowControlChangeNotifier::DoCancel()"));
	__ASSERT_ALWAYS(iRole==ECommRoleDCE,Fault(EWrongRole));
	__ASSERT_ALWAYS(IsActive(),Fault(ECancelNotOutstanding));
	iParent->DCEPort().NotifyFlowControlChangeCancel();
	}


void CCommFlowControlChangeNotifier::RunL()
/**
 *	The device driver does not give the new value of flow control status when the notification
 *	completes so we go and ask for it. If this impedes performance, it may be worth taking the
 *  plunge and working out the value ourselves - it should just be the opposite (ON/OFF) each time
 *  it completes.
 */
	{
	LOGECUART2(_L8("CCommFlowControlChangeNotifier::RunL(), iStatus = %d"), iStatus.Int());
	__ASSERT_ALWAYS(iRole==ECommRoleDCE, Fault(EWrongRole));
	iParent->DCEPort().GetFlowControlStatus(iFlowControl);
	iParent->FlowControlChangeCompleted(iFlowControl,iStatus.Int());
	}


void CCommFlowControlChangeNotifier::Start()
/**
 * start notification on flow control change
 *
 * @note Only supported for DCE role
 */
	{
	LOGECUART1(_L8("CCommFlowControlChangeNotifier::Start()"));
	if (iRole==ECommRoleDTE)
		iParent->FlowControlChangeCompleted(iFlowControl, KErrNotSupported);
	else
		{
		iParent->DCEPort().NotifyFlowControlChange(iStatus);
		SetActive();
		}
	}


//
// implementation of CCommConfigChangeNotifier
//


CCommConfigChangeNotifier::CCommConfigChangeNotifier(CHWPort* aParent)
/**
 * C'tor
 *
 * @param aParent pointer to the CHWPort object
 */
	:CCommNotifyBase(aParent)
	{
	LOGECUART1(_L8("CCommConfigChangeNotifier::CCommConfigChangeNotifier()"));
	}


CCommConfigChangeNotifier::~CCommConfigChangeNotifier()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CCommConfigChangeNotifier::~CCommConfigChangeNotifier()"));
	}


void CCommConfigChangeNotifier::Start()
/**
 * start notification on config change
 *
 * @note Only supported for DCE role
 */
	{
	LOGECUART1(_L8("CCommConfigChangeNotifier::Start()"));
	if (iRole==ECommRoleDTE)
		iParent->ConfigChangeCompleted(iConfig, KErrNotSupported);
	else
		{
		iParent->DCEPort().NotifyConfigChange(iStatus, iConfig);
		SetActive();
		}
	}


void CCommConfigChangeNotifier::DoCancel()
/**
 * Cancel a pending config change notification.
 */
	{
	LOGECUART1(_L8("CCommConfigChangeNotifier::DoCancel()"));
	__ASSERT_ALWAYS(iRole==ECommRoleDCE,Fault(EWrongRole));
	__ASSERT_ALWAYS(IsActive(),Fault(ECancelNotOutstanding));
	iParent->DCEPort().NotifyConfigChangeCancel();
	}


void CCommConfigChangeNotifier::RunL()
/**
 * the config change notification has completed,
 * forward the news to the client.
 */
	{
	LOGECUART2(_L8("CCommConfigChangeNotifier::RunL(), iStatus = %d"), iStatus.Int());
	iParent->ConfigChangeCompleted(iConfig, iStatus.Int());
	}


void CloseObject(TAny* aObject)
/**
 * Close an object from the cleanup stack
 *
 * @param aObject pointer to the object to close
 */
	{
	LOGECUART1(_L8("CloseObject()"));
	((CObject*)aObject)->Close();
	}


//
// implementation of CHWPort
//


CHWPort* CHWPort::NewL(TUint aUnit)
/**
 * Make a new CPort for the comm server
 *
 * @param aUnit unit number of comm port (i.e. 0 for COMM::0)
 * @return a newly created CHWPort object
 */
	{
	LOGECUART2(_L8("CHWPort::NewL(), aUint = %d (unit number of comm port)"), aUnit);
	CHWPort* p = new (ELeave) CHWPort;
	TCleanupItem closePort(CloseObject, p);

	CleanupStack::PushL(closePort);

	p->iReader=CCommReader::NewL(p);
	p->iWriter=CCommWriter::NewL(p);
	p->iBreaker=CCommBreaker::NewL(p);
	p->iSignalNotifier=new (ELeave) CCommSignalNotifier(p);
	p->iDataAvailableNotifier=new (ELeave) CCommAvailableDataNotifier(p);
	p->iFlowControlChangeNotifier=new (ELeave) CCommFlowControlChangeNotifier(p);
	p->iConfigChangeNotifier=new (ELeave) CCommConfigChangeNotifier(p);

	TName name;
	name.Format(_L("%d"), aUnit);
	p->SetName(&name); // todo check return value?

	// the C32 server does not check for the port number beeing to big.
	// this is up to the CSY. If the aUnit number passed to the Logical
	// device driver is greater than KMaxUnits, the kernel will Panic.
	// We want to avoid this by gracefully leaving with Not Supported instead.
	if(aUnit >= (TUint)KMaxUnits)
		User::Leave(KErrNotSupported);

	p->iPortName = aUnit;
#if defined (_DEBUG_CONSOLE_)
	name.Format(_L("Comm::%d"),aUnit);
	p->iConsole.SetTitle(name);
#if defined (_DEBUG_DEVCOMM)
	p->iDumper=CCommDebugDumper::NewL(p->iConsole);
#endif
#endif
	CleanupStack::Pop();
	return p;
	}


CHWPort::CHWPort()
/**
 * C'Tor
 */
	{
	}


void CHWPort::StartRead(const TAny* aClientBuffer, TInt aLength)
/**
 * Queue a read
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength number of bytes to read
 */
	{
	LOGECUART2(_L8("CHWPort::StartRead(), aLength = %d"), aLength);
	DEBUG_TRACE((iConsole.Write(_L("DoRead \n\r"))));
	if(iDataAvailableNotifier->IsActive())
		{
		ReadCompleted(KErrInUse);
		return;
		}
	iReader->Read(aClientBuffer, aLength);
	}


void CHWPort::ReadCancel()
/**
 * Cancel a read
 */
	{
	LOGECUART1(_L8("CHWPort::ReadCancel()"));
	iReader->ReadCancel();
	}


TInt CHWPort::QueryReceiveBuffer(TInt& aLength) const
/**
 * Get the size of the receive buffer from the real serial port
 *
 * @param aLength reference to where the size will be written to
 * @return KErrNone always
 */
	{
	LOGECUART1(_L8("CHWPort::QueryReceiveBuffer()"));
	// casting away constness with (CHWPort *)this
	if (iRole==ECommRoleDTE)
		aLength=((CHWPort *)this)->iPort.QueryReceiveBuffer();
	else
		aLength=((CHWPort *)this)->iPortDCE.QueryReceiveBuffer();
	return KErrNone;
	}


void CHWPort::ResetBuffers(TUint)
/**
 * reset Tx and Rx buffers
 *
 * @note both Tx and Rx buffers are reset here
 */
	{
	LOGECUART1(_L8("CHWPort::ResetBuffers()"));
	if (iRole==ECommRoleDTE)
		iPort.ResetBuffers();
	else
		iPortDCE.ResetBuffers();
	}


void CHWPort::StartWrite(const TAny* aClientBuffer, TInt aLength)
/**
 * Queue a write
 *
 * @param aClientBuffer pointer to the Client's buffer
 * @param aLength number of bytes to write
 */
	{
	LOGECUART2(_L8("CHWPort::StartWrite(), aLength = %d"), aLength);
	DEBUG_TRACE((iConsole.Write(_L("DoWrite \n\r"))));
	iWriter->Write(aClientBuffer,aLength);
	}


void CHWPort::WriteCancel()
/**
 * Cancel a pending write
 */
	{
	LOGECUART1(_L8("CHWPort::WriteCancel()"));
	iWriter->WriteCancel();
	}


void CHWPort::Break(TInt aTime)
/**
 * Queue a break
 *
 * @param aTime Length of break in micro seconds
 */
	{
	LOGECUART2(_L8("CHWPort::Break(), aTime = %d"), aTime);
	iBreaker->Break(aTime);
	}


void CHWPort::BreakCancel()
/**
 * Cancel a pending break
 */
	{
	LOGECUART1(_L8("CHWPort::BreakCancel()"));
	iBreaker->Cancel();
	}


TInt CHWPort::GetConfig(TDes8& aDes) const
/**
 * Pass a config request
 *
 * @param aDes config will be written to this descriptor
 * @return KErrNone always
 */
	{
	LOGECUART1(_L8("CHWPort::GetConfig()"));
	if (iRole==ECommRoleDTE)
		((CHWPort*)this)->iPort.Config(aDes);
	else
		((CHWPort*)this)->iPortDCE.Config(aDes);
	return KErrNone;
	}


TInt CHWPort::SetConfig(const TDesC8& aDes)
/**
 * Set up the Comm LDD/PDD
 *
 * @param aDes descriptor containing the new config
 * @return TInt error code
 */
	{
	LOGECUART1(_L8("CHWPort::SetConfig()"));
	if (aDes.Length()<(TInt)sizeof(TCommConfigV01))
		return KErrGeneral;
	TCommConfigV01 config(*(TCommConfigV01*)aDes.Ptr());

	if (config.iHandshake & KConfigWriteBufferedComplete)
		{
		if (iWriter->IsNotificationEnabled())
			return KErrAccessDenied;
		iEarlyWriteCompletion = ETrue;
		}
	else
		{
		iEarlyWriteCompletion = EFalse;
		}
	if (iRole==ECommRoleDTE)
		return iPort.SetConfig(aDes);
	else
		return iPortDCE.SetConfig(aDes);
	}


TInt CHWPort::SetServerConfig(const TDesC8& aDes)
/**
 * Set the port to use partial reads/writes or the bungee buffer
 *
 * @param aDes package with new server configurations
 * @return TInt error code
 */
	{
	LOGECUART1(_L8("CHWPort::SetServerConfig()"));
	if (aDes.Length()<(TInt)sizeof(TCommServerConfigV01))
		return KErrGeneral;
	TCommServerConfig c(*(TCommServerConfigV01*)aDes.Ptr());
	TInt r=0;
	if ((r=iReader->SetServerConfig(c))==KErrNone)
		r=iWriter->SetServerConfig(c);
	return r;
	}


TInt CHWPort::GetServerConfig(TDes8& aDes)
/**
 * Get the server configs
 *
 * @param aDes server configs will be written to this descriptor
 * @return TInt error code
 */
	{
	LOGECUART1(_L8("CHWPort::GetServerConfig()"));
	if (aDes.Length()<(TInt)sizeof(TCommServerConfigV01))
		return KErrGeneral;
	TCommServerConfig c;

	iReader->GetServerConfig(c);
	aDes=c;

	return KErrNone;
	}


TInt CHWPort::GetCaps(TDes8& aDes)
/**
 * Read capabilities from the driver
 *
 * @param aDes caps will be written to this descriptor
 * @return KErrNone always
 */
	{
	LOGECUART1(_L8("CHWPort::GetCaps()"));
	if (iRole==ECommRoleDTE)
		iPort.Caps(aDes);
	else
		iPortDCE.Caps(aDes);
	if (aDes.Length()==sizeof(TCommCapsV02))
		{
		TCommCapsV02* commcaps = (TCommCapsV02*)(aDes.Ptr());
		commcaps->iNotificationCaps |= KNotifyOutputEmptySupported;
		commcaps->iRoleCaps = KCapsRoleSwitchSupported;
		}
	return KErrNone;
	}


TInt CHWPort::GetSignals(TUint& aSignals)
/**
 * get the status of the signal pins
 *
 * @param aSignals signals will be written to this descriptor
 * @return KErrNone always
 */
	{
	LOGECUART1(_L8("CHWPort::GetSignals()"));
	if (iRole==ECommRoleDTE)
		aSignals=iPort.Signals();
	else
		aSignals=iPortDCE.Signals();
	return KErrNone;
	}


TInt CHWPort::SetSignalsToMark(TUint aSignals)
/**
 * set selected signals to high (logical 1)
 *
 * @param aSignals bitmask with the signals to be set
 * @return KErrNone always
 */
	{
	LOGECUART2(_L8("CHWPort::SetSignalsToMark(), aSignals = %d"), aSignals);
	if (iRole==ECommRoleDTE)
		iPort.SetSignals(aSignals,0);
	else
		iPortDCE.SetSignals(aSignals,0);
	return KErrNone;
	}


TInt CHWPort::SetSignalsToSpace(TUint aSignals)
/**
 * set selected signals to low (logical 0)
 *
 * @param aSignals bitmask with the signals to be cleared
 * @return KErrNone always
 */
	{
	LOGECUART2(_L8("CHWPort::SetSignalsToSpace(), aSignals = %d"), aSignals);
	if (iRole==ECommRoleDTE)
		iPort.SetSignals(0,aSignals);
	else
		iPortDCE.SetSignals(0,aSignals);
	return KErrNone;
	}


TInt CHWPort::GetReceiveBufferLength(TInt& aLength) const
/**
 * get size of Tx and Rx buffer
 *
 * @param aLength reference to where the length will be written to
 * @return KErrNone always
 */
	{
	LOGECUART1(_L8("CHWPort::GetReceiveBufferLength()"));
	// casting away constness with (CHWPort *)this
	if (iRole==ECommRoleDTE)
		aLength=((CHWPort *)this)->iPort.ReceiveBufferLength();
	else
		aLength=((CHWPort *)this)->iPortDCE.ReceiveBufferLength();
	return KErrNone;
	}


TInt CHWPort::SetReceiveBufferLength(TInt aLength)
/**
 * set the size of Tx and Rx buffer
 *
 * @param aLength new length of Tx and Rx buffer
 * @return TInt error code
 */
	{
	LOGECUART2(_L8("CHWPort::SetReceiveBufferLength(), aLength = %d"), aLength);
	if (iRole==ECommRoleDTE)
		return iPort.SetReceiveBufferLength(aLength);
	else
		return iPortDCE.SetReceiveBufferLength(aLength);
	}


void CHWPort::Destruct()
/**
 * Destruct - we must (eventually) call delete this
 */
	{
	LOGECUART1(_L8("CHWPort::Destruct()"));
	delete this;
	}


void CHWPort::FreeMemory()
/**
 * Attempt to reduce out memory foot print.
 */
	{
	LOGECUART1(_L8("CHWPort::FreeMemory()"));
	iReader->FreeMemory();
	iWriter->FreeMemory();
	}


void CHWPort::NotifySignalChange(TUint aSignalMask)
/**
 * notify client when the signals change
 *
 * @param aSignalMask bitmask with signal to be notified on
 */
	{
	LOGECUART2(_L8("CHWPort::NotifySignalChange(), aSignalMask = %d"), aSignalMask);
	iSignalNotifier->NotifySignal(aSignalMask);
	}


void CHWPort::NotifySignalChangeCancel()
/**
 * cancel an outstanding signal change notification
 */
	{
	LOGECUART1(_L8("CHWPort::NotifySignalChangeCancel()"));
	iSignalNotifier->NotifySignalCancel();
	}


void CHWPort::NotifyConfigChange()
/**
 * notify client when the configation changes
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyConfigChange()"));
	iConfigChangeNotifier->Start();
	}


void CHWPort::NotifyConfigChangeCancel()
/**
 * cancel an outstanding config change notification
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyConfigChangeCancel()"));
	iConfigChangeNotifier->Cancel();
	}


void CHWPort::NotifyFlowControlChange()
/**
 * notify client when the flow control changes
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyFlowControlChange()"));
	iFlowControlChangeNotifier->Start();
	}


void CHWPort::NotifyFlowControlChangeCancel()
/**
 * cancel an outstanding flow control change notification
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyFlowControlChangeCancel()"));
	iFlowControlChangeNotifier->Cancel();
	}


void CHWPort::NotifyBreak()
/**
 * notify client when a break occurs
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyBreak()"));
	iSignalNotifier->NotifyBreak();
	}


void CHWPort::NotifyBreakCancel()
/**
 * cancel an outstanding break notification
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyBreakCancel()"));
	iSignalNotifier->NotifyBreakCancel();
	}


void CHWPort::NotifyDataAvailable()
/**
 * notify client when data is available
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyDataAvailable()"));
	if(iReader->IsActive())
		{
		NotifyDataAvailableCompleted(KErrInUse);
		return;
		}
	iDataAvailableNotifier->Start();
	}


void CHWPort::NotifyDataAvailableCancel()
/**
 * cancel an outstanding data availalbe notification
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyDataAvailableCancel()"));
	iDataAvailableNotifier->Cancel();
	}


void CHWPort::NotifyOutputEmpty()
/**
 * notify client when output buffer is empty
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyOutputEmpty()"));
	if (iEarlyWriteCompletion)
		NotifyOutputEmptyCompleted(KErrAccessDenied);
	else
		iWriter->NotificationEnable();
	}


void CHWPort::NotifyOutputEmptyCancel()
/**
 * cancel an outstanding output empty notification
 */
	{
	LOGECUART1(_L8("CHWPort::NotifyOutputEmptyCancel()"));
	iWriter->NotificationDisable();
	}


TInt CHWPort::GetFlowControlStatus(TFlowControl& aFlowControl)
/**
 * get the flow control status
 *
 * @param aFlowControl flow control status will be written to this descriptor
 * @return TInt error code
 *
 * @note only supported for DCE role
 */
	{
	if (iRole==ECommRoleDTE)
		return KErrNotSupported;
	else
		iPortDCE.GetFlowControlStatus(aFlowControl);
	return KErrNone;
	}


TInt CHWPort::GetRole(TCommRole& aRole)
/**
 * get the role of this port unit
 *
 * @param aRole reference to where the role will be written to
 * @return KErrNone always
 */
	{
	aRole = iRole;
	return KErrNone;
	}


TInt CHWPort::SetRole(TCommRole aRole)
/**
 * set the role of this port unit
 *
 * @param aRole the new role
 * @return TInt error code
 */
	{
	iRole=aRole;
	TInt ret=KErrNone;
	if (aRole==ECommRoleDTE)
		ret = iPort.Open(iPortName);
	else
		ret = iPortDCE.Open(iPortName);
	if (ret!=KErrNone)
		{
		LOGECUART3(_L8("CHWPort::SetRole - failed with %d when attempting to open hardware port unit: %d"),ret,iPortName);
		return ret;
		}
	iReader->SetRole(aRole);
	iWriter->SetRole(aRole);
	iBreaker->SetRole(aRole);
	iSignalNotifier->SetRole(aRole);
	iDataAvailableNotifier->SetRole(aRole);
	iFlowControlChangeNotifier->SetRole(aRole);
	iConfigChangeNotifier->SetRole(aRole);

#if defined (_DEBUG_DEVCOMM) && defined (_DEBUG_CONSOLE_)
	iDumper->SetRole(aRole);
#endif
	return KErrNone;
	}


CHWPort::~CHWPort()
/**
 * D'tor
 */
	{
	LOGECUART1(_L8("CHWPort::~CHWPort()"));
	delete iReader;
	delete iWriter;
	delete iBreaker;
	delete iSignalNotifier;
	delete iDataAvailableNotifier;
	delete iFlowControlChangeNotifier;
	delete iConfigChangeNotifier;

	iPort.Close();
	iPortDCE.Close();

#if defined (_DEBUG_CONSOLE_)
#if defined (_DEBUG_DEVCOMM)
	delete iDumper;
#endif
	iConsole.Close();
#endif
	}


#ifdef _DEBUG_DEVCOMM
void CHWPort::DoDumpDebugInfo(const RMessage2 &aMessage)
	{
	TCommDebugInfoPckg d;
	if (iRole==ECommRoleDTE)
		iPort.DebugInfo(d);
	else
		iPortDCE.DebugInfo(d);
	TRAPD(ret, aMessage.WriteL(0,aMessage,d) );
	aMessage.Complete(ret);
	}
#endif


RBusDevComm& CHWPort::DTEPort()
/**
 * return the handle to the logical DTE port
 */
	{
	return iPort;
	}


RBusDevCommDCE& CHWPort::DCEPort()
/**
 * return the handle to the logical DCE port
 */
	{
	return iPortDCE;
	}


//
// todo implement this - for further improvements
//
#if 0
RBusWhatever& CHWPort::Port()
/**
 * return a handle to our port
 */
	{
	if (iRole==ECommRoleDTE)
		return iPort;
	else
		return iPortDCE;
	}
#endif


//
// implementation of CHWPortFactory
//


CHWPortFactory::CHWPortFactory()
/**
 * C'tor
 */
	{
	TName name(SERIAL_NAME);
	SetName(&name); // todo check return value?
	iVersion = TVersion(KEC32MajorVersionNumber, KEC32MinorVersionNumber, KEC32BuildVersionNumber);
	}


CPort* CHWPortFactory::NewPortL(const TUint aUnit)
/**
 * Create a new port for the supplied unit number
 *
 * @param aUnit port unit number (i.e. 0 for COMM::0)
 */
	{
	LOGECUART2(_L8("CHWPortFactory::NewPortL(), aUnit = %d"), aUnit);
//	__ASSERT_ALWAYS(aUnit>=KCommLowUnit,User::Leave(KErrNotSupported));
//	__ASSERT_ALWAYS(aUnit<=KCommHighUnit,User::Leave(KErrNotSupported));
	return (CPort *)CHWPort::NewL(aUnit);
	}


void CHWPortFactory::Info(TSerialInfo& aSerialInfo)
/**
 * get info about this CSY, fill in the supplied structure.
 *
 * @param aSerialInfo where info will be written to
 */
	{
	aSerialInfo.iDescription = KSerialDescription;
	aSerialInfo.iName = SERIAL_NAME;
	aSerialInfo.iLowUnit = KCommLowUnit;
	aSerialInfo.iHighUnit = KCommHighUnit;
	}

/**
Returns capabilities for requested port
*/
TSecurityPolicy CHWPortFactory::PortPlatSecCapability(TUint aPort) const
	{
	LOGECUART2(_L8("CHWPortFactory::PortPlatSecCapability(), aPort = %d"), aPort);
	TSecurityPolicy csySecurityPolicy(KDefaultPortPassPolicy);

	if(aPort >= (TUint)KMaxUnits)	//if greater than KMaxUnits it is an invalid port so fail.
		{
		csySecurityPolicy = KPortAlwaysFailPolicy;
		}
	else //see if it is defined in explicit policy list.
		{
		for(int i = 0;i < KExplicitPortPoliciesCount; i++)
			{
			// coverity [dead_error_line] : KExplicitPortPoliciesCount is currently 0, but this could change.
			if(aPort == KExplicitPortSecurityPolicies[i].iPort)
				{
				csySecurityPolicy = KExplicitPortSecurityPolicies[i].iPolicy;
				break;
				}
			}
		}
	return csySecurityPolicy;
	}

extern "C"
	{
	IMPORT_C CSerial* LibEntry(void);   // Force export
	};


EXPORT_C CSerial* LibEntry()
/**
 * Lib main entry point
 */
	{
	LOGECUART1(_L8("LibEntry()"));
	return new CHWPortFactory;
	}


#if defined(_DEBUG_CONSOLE_)

RDebugConsole::RDebugConsole()
	{
	Create();
	Set(_L(""),TSize(64,15));

	}

void RDebugConsole::Printf(TRefByValue<const TDesC> aFmt,...)
//
// Print to a console screen.
//
	{

	VA_LIST list;
	VA_START(list,aFmt);
	TBuf<0x100> aBuf;
	aBuf.AppendFormatList(aFmt,list);
	Write(aBuf);
	}
#endif

#if defined (_DEBUG_DEVCOMM) && defined (_DEBUG_CONSOLE_)
CCommDebugDumper* CCommDebugDumper::NewL(RDebugConsole &aConsole)
	{
	CCommDebugDumper* p=new CCommDebugDumper(aConsole);
	return p;
	}

CCommDebugDumper::CCommDebugDumper(RDebugConsole &aConsole)
	:CActive(EPriorityStandard)
	{
	iRole=ECommRoleDTE;
	iConsole=&aConsole;
	CActiveScheduler::Add(this);
	SetActive();
	iConsole->Read(iKeystroke,iStatus);
	};

CCommDebugDumper::~CCommDebugDumper()
	{
	Cancel();
	}

void CCommDebugDumper::RunL()
	{
	TInt key=iKeystroke.Code();
	switch(key)
		{
		case 'd':
		case 'D':
			{
			TCommDebugInfoPckg d;
			if (iRole==ECommRoleDTE)
				iParent->DTEPort().DebugInfo(d);
			else
				iParent->DCEPort().DebugInfo(d);
			TCommDebugInfo& debug=d();
			iConsole->Printf(_L("rxbusy  : 0x%04x, rxHeld   : 0x%04x, \n\r"),debug.iRxBusy,debug.iRxHeld);
			iConsole->Printf(_L("txbusy  : 0x%04x, txHeld   : 0x%04x, \n\r"),debug.iTxBusy,debug.iTxHeld);
			iConsole->Printf(_L("drainRx : 0x%04x, fillTx   : 0x%04x\n\r"),debug.iDrainingRxBuf,debug.iFillingTxBuf);
			iConsole->Printf(_L("Txonchar: 0x%04x, TxOffchar: 0x%04x\n\r"),debug.iTxXon,debug.iTxXoff);
			iConsole->Printf(_L("RxonChar: 0x%04x, RxOffchar: 0x%04x\n\r"),debug.iRxXon,debug.iRxXoff);
			iConsole->Printf(_L("NumTX   : 0x%04x, NumRx    : 0x%04x\n\r"),debug.iTxChars,debug.iRxChars);
			iConsole->Printf(_L("TxLen   : 0x%04x, RxLen    : 0x%04x\n\r"),debug.iTxLength,debug.iRxLength);
			iConsole->Printf(_L("TxOffset: 0x%04x, RxOffset : 0x%04x\n\r"),debug.iTxOffset,debug.iRxOffset);
			iConsole->Printf(_L("TxInts  : 0x%04x, RxInts   : 0x%04x\n\r"),debug.iTxIntCount,debug.iRxIntCount);
			}
			break;
		case 's':
		case 'S':
			{
			TUint signals=0;
			if (iRole==ECommRoleDTE)
				signals=iParent->DTEPort().Signals();
			else
				signals=iParent->DCEPort().Signals();
			iConsole->Printf(_L("Signals: "));
			if (signals&KSignalCTS)
				iConsole->Printf(_L("CTS "));
			if (signals&KSignalDSR)
				iConsole->Printf(_L("DSR "));
			if (signals&KSignalDCD)
				iConsole->Printf(_L("DCD "));
			if (signals&KSignalRTS)
				iConsole->Printf(_L("RTS "));
			if (signals&KSignalDTR)
				iConsole->Printf(_L("DTR "));
			iConsole->Printf(_L("\n\r"));
			}
			break;
		default:
			break;
		}

	SetActive();
	iConsole->Read(iKeystroke,iStatus);
	};

void CCommDebugDumper::DoCancel()
	{
	iConsole->ReadCancel();
	}

#endif

// EOF - ECUART.CPP
