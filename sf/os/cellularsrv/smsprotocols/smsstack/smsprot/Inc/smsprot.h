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
// Declares the CSmsProtocol, CSmsProtocolFamily, CSmsProvider and
// other classes used internally by them.
// Conditional includes
// 
//

/**
 @file
 @internalAll
*/

#ifndef SMSPROT_H
#define SMSPROT_H

#include <es_sock.h>
#include <es_prot.h>
#include <etelmm.h>
#include <es_ini.h>
#include <mmlist.h>
#include <cdblen.h>

#include "smsuset.h"
#include "smsuaddr.H"
#include "smspcomm.h"
#include "smspproc.h"
#include "nbbackuprestoreint.h"

class CSmsProvider;
class CFacadeSmsReassemblyStore;
class CSmsSegmentationStore;
class CLogWrapper;
class CSmsModemNotification;
class CSmsPDURead;
class CSmsPhoneEnumeration;
class CSmsMessageWrite;
class CSmsPDUDelete;
class CSmsReadParams;
class CSmsWriteParams;
class CSmspWriteQueue;
class CQueuedSmsMessage;
class CSmspDeleteQueue;
class CSmspSendQueue;
class CSmsPhoneInitialization;
class CSmsProtocolBootTimer;
class CBackupAndRestore;
class CSmspSetBearer;

/**
 *  CProtocolFamilyBase derived class which identifies the protocols
 *  we can create, and provides an overidden factory method for
 *  creating them.
 *  @internalComponent
 */
class CSmsProtocolFamily : public CProtocolFamilyBase
    {
public:
    static CSmsProtocolFamily * NewL();
    TInt Install();
    TInt Remove();
    CProtocolBase * NewProtocolL(TUint aSockType,TUint aProtocol);
    TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
private:
    CSmsProtocolFamily();
    };

/**
 *  @internalComponent
 */
IMPORT_C CProtocolFamilyBase* InstallSMS();	// Force export

/**
 *  The main class for this protocol which handles requests
 *  from clients contending for services from the phone.
 *  @internalComponent
 */
class CSmsProtocol : public CProtocolBase,
                     public MSmsComm, 
                     public MNBBackupAndRestoreObserver
	{
friend class CSmsProvider;

public:
    static CSmsProtocol *NewL();
    virtual ~CSmsProtocol();

// Calls from the socket server which we implement
    CServProviderBase *NewSAPL(TUint aSocketType);
    void InitL(TDesC& aTag);
    void StartL(void);
    void Identify(TServerProtocolDesc *)const;
    
// Calls from the socket server which we don't implement
    virtual CHostResolvProvdBase *NewHostResolverL();
    virtual CServiceResolvProvdBase *NewServiceResolverL();
    virtual CNetDBProvdBase* NewNetDatabaseL();

// Calls from the socket server and other protocols which we just panic
    void BindL(CProtocolBase *aProtocol, TUint aId);
    virtual void BindToL(CProtocolBase *aProtocol);
    TInt Send(TDes8& aDes, TSockAddr *aTo,TSockAddr *aFrom,CProtocolBase* aSourceProtocol);
    void Process(TDes8& aDes,TSockAddr *aFrom,TSockAddr *to,CProtocolBase* aSourceProtocol);
    TInt GetOption(TUint aLevel,TUint aName,TDes8 &aOption,CProtocolBase* aSourceProtocol);
    TInt SetOption(TUint aLevel,TUint aName,const TDesC8& aOption,CProtocolBase* aSourceProtocol);
    void Error(TInt aError,CProtocolBase* aSourceProtocol);

// Functions called for WAP binding or for SAP creation
    virtual void AddSmsMessageObserverL(MSmsMessageObserver& aObserver);
    virtual TInt BindSmsMessageObserver(MSmsMessageObserver& aObserver,const TSmsAddr& aSmsAddr);
    virtual void RemoveSmsMessageObserver(const MSmsMessageObserver& aObserver);
    virtual TBool SmsAddrIsAlreadyUsed(const MSmsMessageObserver* aObserver,const TSmsAddr& aSmsAddr)const;

// Functions called by SAPs (e.g. WAPPROT)
    virtual void SendSmsMessage(CSmsMessage* aSmsMessage,MSmsMessageObserver& aObserver,TUint aOptions);
    virtual void CancelSendSmsMessage(MSmsMessageObserver& aObserver,TBool aCancelInternally=ETrue);
    virtual void EnumeratePhone(MSmsMessageObserver& aObserver);
    virtual void CancelEnumeratePhone(MSmsMessageObserver& aObserver);
    
    void WriteSmsMessage(CSmsMessage* aSmsMessage,MSmsMessageObserver& aObserver);
    void CancelWriteSmsMessage(MSmsMessageObserver& aObserver);
    
    virtual void DeleteSmsMessage(CSmsMessage* aSmsMessage,MSmsMessageObserver& aObserver);
    virtual void CancelDeleteSmsMessage(MSmsMessageObserver& aObserver);
    virtual void ReadSmsParameters(MSmsMessageObserver& aObserver);
    virtual void CancelReadSmsParams();
    virtual void WriteSmsParameters(CMobilePhoneSmspList* aMobilePhoneSmspList, MSmsMessageObserver& aObserver);
    virtual void CancelWriteSmsParams();

// MSmsComm Notifications used by active objects
    TInt ProcessMessageL(const CSmsMessage& aSmsMessage,const TSmsAddr* aOriginalSmsAddr,const CSmsMessage* aOriginalSmsMessage,TDes& aDes);
    void DeletePDUs(const CArrayFix<TGsmSmsSlotEntry>& aSlotArray, MSmsMessageObserver* aObserver);
    void ModemNotificationCompleted(TInt aStatus,RPhone::TModemDetection aNewState);
    void DiskSpaceMonitorStateChange(TSmsDiskSpaceMonitorStatus aStatus);
    void MessageSendCompleted(TInt aStatus);
    void PhoneEnumerationCompleted(TInt aStatus);

	inline RFs& FileSession();
	inline virtual TBool ObserverIsPresent(const MSmsMessageObserver& aObserver) const;
	const RMobilePhone::TMobilePhoneNetworkInfoV1& NetworkInfo() const;
    TBool NetworkInfoAvailable() const;
    // new function which deletes msg from the reassemblystore
    // by passing them the smsmessage class
    void DeleteSMSFromReaStoreL(const CSmsMessage& aSmsMessage);
    void MessageReadedSuccessfully();
    TBool AllocateLocalAddress(TSmsAddr& aAddr);
    TInt ExternalizeEnumeratedMessagesL(CSmsProvider& aProvider,TInt& aCount);
    static TBool IsAppPortSMS(const CSmsMessage& aSmsMessage);

protected:
    void CloseNow();

private:
    CSmsProtocol();
    
    TInt ObserverIndex(const MSmsMessageObserver& aObserver) const;
    void OrderSmsMessageObserver(const MSmsMessageObserver& aObserver);
    
    MSmsMessageObserver* MatchSmsAddressToObserver(const TSmsAddr& aAddr);
    MSmsMessageObserver* MatchSmsMessageToObserver(const CSmsMessage& aSmsMessage);
    static TBool IsMatch(const TSmsAddr& aSmsAddr, const CSmsMessage& aSmsMessage);
    static TBool MatchInformationElement(const CSmsMessage& aSmsMessage, CSmsInformationElement::TSmsInformationElementIdentifier aIeVal);
    void MatchCompleteSmsMessagesToObserverL(MSmsMessageObserver& aObserver,CArrayFix<TInt>& aIndexArray);
    static TBool MatchApplicationPort(const CSmsMessage& aSmsMessage,TUint aPort,TBool a16Bit);
    
    void NotifyMessageObservers(TInt aStatus);

 	TBool IsWapSMS(const CSmsMessage& smsmessage );

    inline TBool ClosingDown() const;
    inline void SetClosingDown(TBool aClosingDown);
    void CloseNowWrap();
    
    void PowerUp();			// Handles modem turning on or backup finishing
    void DoPowerUpL();
    void PowerDown();		// Handles modem turning off or backup starting
    void ProcessCompleteSmsMessagesL();
    TInt CheckPoweredUp() const;
    virtual void HandleBackupOrRestoreStartingL();
    virtual void HandleBackupOrRestoreCompleteL();
    void ReadConfigurableSettingsL(); // from smswap.sms.esk -> [customTimeoutSettings]

private:
    ///< Enumerated flag values for states of the protocol
    enum TSmsProtocolFlags
        {
        ESmsProtocolClosingDown=0x01			///< In the process of shutting down
        };
    enum
        {
        KNumSARStoreAccessesBeforePurging=8	///< Number of accesses to the segmentation & reassembly stores before purging
        };
    
    enum TSmsProtocolState
        {
        EPoweredDown,	///< SmsProt is not initialised and the phone is not swiched on
        EPoweredUp		///< SmsProt is powered up, phone switched on, ready for action
        } iState;

private:
    CArrayPtrFlat<MSmsMessageObserver> iSmsMessageObserverList;	///< List of current observers

    RTelServer iTelServer;                              ///< A session with the telephony server
    TBuf<KCommsDbSvrMaxFieldLength> iGsmTsyName;        ///< Name of the TSY
    RMobilePhone iGsmPhone;                             ///< ETEL phone subsession for intialisation and detection events
    RMobilePhone iEnumerationPhone;                     ///< ETEL phone subsession for enumerating the phone
    RMobilePhone iWritePhone;                           ///< ETEL phone subsession for writing messages
    RMobileSmsMessaging iSmsMessaging;                  ///< ETEL phone subsession for advanced messaging services

    TSmsSettings iSmsSettings;                          ///< Global SMS Protocol settings
    RMobileSmsMessaging::TMobileSmsCapsV1 iMobileSmsCaps;   // SMS messaging capabilities of the phone
    
    TInt iFlags;										///< Flags indicating current state of the protocol
    
    RFs iFs;                                            ///< A session with the file server
    CFacadeSmsReassemblyStore* iReassemblyStore;        ///< Pointer to the reassembly store
    TInt iNumReassemblyStoreAccesses;                   ///< Counter for number of accesses to the reassembly store
    CSmsSegmentationStore* iSegmentationStore;          ///< Pointer to the segmentation store
    TInt iNumSegmentationStoreAccesses;                 ///< Counter for number of accesses to the segmentation store
    CSmsModemNotification* iSmsModemNotification;		///< Pointer to object for handling modem detection events
    RPhone::TModemDetection iModemDetection;			///< The current modem detection state
    

    CSmsPDURead* iSmsPDURead;                           ///< Pointer to object for receiving SMS PDUs from ETEL

    CSmsMonitorDiskSpace* iSmsMonitorDiskSpace;         ///< Pointer to object for monitoring of disk space
    
    MSmsMessageObserver*    iPhoneEnumerationObserver;  ///< Pointer to the observer of any current enumeration
    CSmsPhoneEnumeration*   iSmsPhoneEnumeration;       ///< Pointer to object for enumerating messages on the phone
    
    CSmspSendQueue*     iSendQueue;                     ///< Queue of messages to send
    CSmspWriteQueue*    iWriteQueue;                    ///< Pointer to object for writing messages to the phone
    CSmspDeleteQueue*   iDeleteQueue;                   ///< Queue of PDUs to delete
    
    CSmsReadParams*    iSmsReadParams;                  ///< Pointer to object for reading SMS parameters
    CSmsWriteParams*   iSmsWriteParams;                 ///< Pointer to object for writing SMS parameters
    
    CSmsPhoneInitialization*   iSmsPhoneInitialization;
    CBackupAndRestore*         iBackupRestoreSession;
    CSmspSetBearer*            iSetBearer;              ///< Pointer to bearer object, used by message sender but cancelled from protocol
    CSmspReceiveMode*          iReceiveMode;            ///< Pointer to receive mode object, used by PDU reader but cancelled from protocol
    
    TInt iNext8BitPort;
    TInt iNext16BitPort;
    CSmsProtocolBootTimer*  iBootTimer;                 ///< Pointer to the CSmsProtocolBootTimer object
    };

/**
 *  A CServProviderBase derived class providing service access points to the
 *  SMS Protocol.
 *  @internalComponent
 */
class CSmsProvider : public CServProviderBase, public MSmsMessageObserver
	{
public:
    static CSmsProvider* NewL(CSmsProtocol& aProtocol);
    virtual ~CSmsProvider();
public:
// pure virtuals from CServProviderBase
    void Start();
    void LocalName(TSockAddr& aAddr) const;
    TInt SetLocalName(TSockAddr& aAddr);
    void RemName(TSockAddr& aAddr) const;
    TInt SetRemName(TSockAddr& aAddr);
    TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption) const;
    void Ioctl(TUint aLevel,TUint aName,TDes8* aOption);
    void CancelIoctl(TUint aLevel,TUint aName);
    TInt SetOption(TUint aLevel,TUint aName,const TDesC8 &aOption);
    TInt Write(RMBufChain& aBufChain, TUint aOptions, TSockAddr* aAddr=NULL);
    TInt GetData(RMBufChain& aBufChain, TUint aLength, TUint aOptions, TSockAddr* aAddr=NULL);
    void ActiveOpen();
    void ActiveOpen(const TDesC8& aConnectionData);
    TInt PassiveOpen(TUint aQueSize);
    TInt PassiveOpen(TUint aQueSize,const TDesC8& aConnectionData);
    void Shutdown(TCloseType aOption);
    void Shutdown(TCloseType aOption,const TDesC8& aDisconnectionData);
    void AutoBind();

//  From MSmsMessageObserver
    const TSmsAddr& GetLocalAddress() const;
    void SetLocalAddress(const TSmsAddr& aSmsAddr);
    void ModemNotificationCompleted(TInt aStatus);
    void MessageSendCompleted(TInt aStatus);  //  aStatus is EPOC error or -(KFailureCauseOffset+failurecause)
    TInt MessageReceived(const CSmsMessage& aSmsMessage,TDes& aDes);  //  must return EPOC error or -(KFailureCauseOffset+failurecause)
    void EnumeratePhoneCompleted(TInt aStatus);
    void MessageWriteCompleted(TInt aStatus, const CSmsMessage* aSmsMessage = NULL);
    void MessageDeleteCompleted(TInt aStatus);
    void ReadSmsParamsCompleted(TInt aStatus,CMobilePhoneSmspList* aSmspList);
    void WriteSmsParamsCompleted(TInt aStatus);
    virtual TBool ClientConfirmsMessage()const;
    virtual TBool SmsAddrIsDuplicate(const MSmsMessageObserver* aObserver,const TSmsAddr& aAddr)const;
    TInt ExternalizeEnumeratedMessagesL(TInt& aCount);
    TInt ExternalizeMessageL(const CSmsMessage& aSmsMessage,TBool aAppend);

protected:
    CSmsProvider(CSmsProtocol& aProtocol);
    virtual TInt SecurityCheck(MProvdSecurityChecker* aSecurityChecker);
    void ConstructL();

private:
    TInt DoMessageReceivedL(const CSmsMessage& aSmsMessage);
    inline static TInt NumSegments(TInt aSize);
    void DoWriteL(const TDesC8& aData,TUint aOptions);
    TInt ExternalizeEnumeratedMesagesL(TInt& aCount);
    CSmsMessage* InternalizeMessageL();
    TInt ExternalizeParametersL(const CMobilePhoneSmspList& aMobilePhoneSmspList);
    CMobilePhoneSmspList* InternalizeParametersL();
    
    inline TBool ObserverAddedToProtocol() const;
    inline void SetObserverAddedToProtocol(TBool aAdded);
    inline TBool IoctlOutstanding() const;
    inline void SetIoctlOutstanding(TBool aOutstanding);
    CSmsMessage* InternalizeMessageL(	CBufSeg* aBufSeg);
    void PopulateBufferWithPDUSlotsL(const CSmsMessage& aSmsMessage);
    
private:
    ///< Enumerated flag values for current of the provider
    enum TSmsProviderFlags
        {
        ESmsProviderObserverAddedToProtocol=0x01,   ///< Registered as observer of the protocol
        ESmsProviderIoctlOutstanding=0x02           ///< Special SMS ioctl is outstanding
        };

private:
    CSmsProtocol& iProtocol;                    ///< Reference to the SMS Protocol object
    TSmsAddr iLocalAddress;                     ///< The local address of the access point
    TInt iFlags;                                ///< Flags indicating current state of the provider
    CArrayPtr<CBufSeg>* iRecvBufSegArray;       ///< Array of serialized SMS messages to be read from the socket
    TInt iSegmentIndex;                         ///< Index of next segment of current SMS message to read
    CBufSeg* iSendBufSeg;                       ///< SMS message segments written to the socket
    TUint iName;                                ///< The integer "name" of the current ioctl request
    TBool iEnumSocket;                          ///< this is enumeration client
    TInt iNumOfEnumeratedMessages;              ///< number of enumerated segments
    MProvdSecurityChecker* iSecurityChecker;    ///< ESOCK security checker object
    };

#include "smsprot.inl"

/**
 *  @internalComponent
 */
class CSmsProtocolBootTimer : public CTimer
	{
public:
    static CSmsProtocolBootTimer* NewL(CSmsProtocol& aSmsProtocol);
    virtual ~CSmsProtocolBootTimer();

    void Start(const TTimeIntervalMicroSeconds32& aTimeIntervalMicroSeconds32);

private:
    CSmsProtocolBootTimer(CSmsProtocol& aSmsProtocol);
    virtual void RunL();

protected:
    CSmsProtocol& iSmsProtocol;         ///< reference to owner CSmsProtocol object
    };

#endif // SMSPROT_H
