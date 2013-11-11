/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


#if	(!defined __T_DATA_RAVDP_H__ )
#define	__T_DATA_RAVDP__H__

//	EPOC Includes
#include <test/datawrapper.h>
#include <gavdp.h>
#include <bluetoothav.h>

// User includes
#include "DataWrapperBase.h"


/**
 * Test	Active Notification	class
 *
 */
class CT_DataRGavdp :	public CDataWrapperBase, 
						public MGavdpUser
	{
public:
	/**
	* Two phase	constructor
	*/
	static CT_DataRGavdp*	NewL();

	/**
	* Public destructor
	*/
	~CT_DataRGavdp();

	/**
	* Process a	command	read from the ini file
	*
	* @param aDataWrapper	test step requiring	command	to be processed
	* @param aCommand	the	command	to process
	* @param aSection		the	entry in the ini file requiring	the	command	to be processed
	*
	* @return ETrue	if the command is processed
	*/
	virtual	TBool	DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);

	/**
	* Return a pointer to the object that the data wraps
	*
	* @return	pointer	to the object that the data	wraps
	*/
	virtual	TAny*	GetObject()	{ return iRGavdp; }

	/**
	* Set the object that the data wraps
	*
	* @param	aObject	object that	the	wrapper	is testing
	*
	*/
	virtual	void	SetObjectL(TAny* aAny);

	/**
	* The object will no longer be owned by this
	*
	* @leave	KErrNotSupported if the the function is not supported
	*/
	virtual void	DisownObjectL();

	inline virtual TCleanupOperation CleanupOperation();
	
	//	MGavdpUser implemention
	virtual void GAVDP_ConnectConfirm(const TBTDevAddr& aDeviceAddr);
	virtual void GAVDP_SEPDiscovered(const TAvdtpSEPInfo& aSEP);
	virtual void GAVDP_SEPDiscoveryComplete();	
	virtual void GAVDP_SEPCapability(TAvdtpServiceCapability* aCapability);
	virtual void GAVDP_SEPCapabilityComplete();		
	virtual void GAVDP_AbortStreamConfirm();	
	virtual void GAVDP_StartStreamsConfirm();
	virtual void GAVDP_SuspendStreamsConfirm();	
	virtual void GAVDP_SecurityControlConfirm(const TDesC8& aResponseData);	
	virtual void GAVDP_ConfigurationConfirm();
	virtual void GAVDP_ConfigurationStartIndication(TSEID aLocalSEID, TSEID aRemoteSEID);
	virtual TInt GAVDP_ConfigurationIndication(TAvdtpServiceCapability* aCapability);
	virtual TInt GAVDP_ConfigurationEndIndication();
	virtual TInt GAVDP_StartIndication(TSEID aSEID);
	virtual TInt GAVDP_SuspendIndication(TSEID aSEID);
	virtual TInt GAVDP_SecurityControlIndication(TSEID aSEID, TDes8& aSecurityDataInOut);
	virtual void GAVDP_AbortIndication(TSEID aSEID);
	virtual void GAVDP_ReleaseIndication(TSEID aSEID);
	virtual void GAVDP_BearerReady(RSocket aSocket, const TAvdtpSockAddr& aAddress);
	virtual void GAVDP_Error(TInt aError, const TDesC8& aErrorData);	
	
protected:
	/**
	* Protected	constructor. First phase construction
	*/
	CT_DataRGavdp();

	/**
	* Second phase construction
	*/
	void	ConstructL();

	void	RunL(CActive* aActive, TInt aIndex);
	
private:

	static void CleanupOperation(TAny* aAny);

    enum 
    {
    	EGAVDP_NoneEvent,
        EGAVDP_ConnectConfirm,
        EGAVDP_SEPDiscovered,
        EGAVDP_SEPDiscoveryComplete,
        EGAVDP_SEPCapability,
        EGAVDP_SEPCapabilityComplete,
        EGAVDP_AbortStreamConfirm,
        EGAVDP_StartStreamsConfirm,
        EGAVDP_SuspendStreamsConfirm,
        EGAVDP_SecurityControlConfirm,
        EGAVDP_ConfigurationConfirm,
        EGAVDP_ConfigurationStartIndication,
        EGAVDP_ConfigurationIndication,
        EGAVDP_ConfigurationEndIndication,
        EGAVDP_StartIndication,
        EGAVDP_SuspendIndication,
        EGAVDP_SecurityControlIndication,
        EGAVDP_AbortIndication,
        EGAVDP_ReleaseIndication,
        EGAVDP_BearerReady,
        EGAVDP_Error,
    };  
    TBool	ReadExpectedEventsFromConfig(const TTEFSectionName& aSection);
	TBool	ConvertEvents(TDesC &aEventStr, RArray<TUint>& aEvent);
	void	SetEventOutstanding();
	void	CheckEvent(TUint aEvent);
	void	CheckLocalSEID(TSEID aSEID);

	/**
	* Helper methods
	*/
	
	void		DestroyData();	
	void 		DoCmdConstructor();
	void 		DoCmdOpen(const TTEFSectionName& aSection);
	void		DoCmdListen(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdClose();
	void		DoCmdCancel();
	void		DoCmdConnect(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdDiscoverRemoteSEPs(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdGetRemoteSEPCapabilities(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdRegisterSEP(const TTEFSectionName& aSection);
	void		DoCmdBeginConfiguringRemoteSEP(const TTEFSectionName& aSection);
	void		DoCmdBeginConfiguringLocalSEP(const TTEFSectionName& aSection);
	void		DoCmdAddSEPCapability(const TTEFSectionName& aSection);
	void		DoCmdCommitSEPConfiguration(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdStartStream(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdSuspendStream(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdAbortStream(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdSendSecurityControl(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdCreateBearerSockets(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	void		DoCmdMaxSecurityControlLength(const TTEFSectionName& aSection);
	void		DoCmdShutdown();
	void		DoCmdDestructor();

	void		Read(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void		Send(const TDesC& aSection, const TInt aAsyncErrorIndex);
	void		CloseBearerSockets();
	void		IncreaseOutstanding(const TTEFSectionName& aSection, const TInt aAsyncErrorIndex);
	
	TBool		ConvertSEIDs(TDesC &aSEIDsStr, RArray<TUint>& aSEIDs);
	TBool		isExistSEID(TUint aSEID);
	
private:
	RGavdp*		iRGavdp;
	TBTDevAddr* iRemoteAddr;
	RArray<TUint> iExpectedEvent;
	TInt		iExpectedEventCount;
	TInt		iEventCount;
	RArray<TUint> iExistRemoteSEIDs;
	TInt		iExpectedSEPsCount;
	TInt		iDiscoverdSEPsCount;
	RPointerArray<TAvdtpSEPInfo> iRemoteSEPs;
	TAvdtpServiceCategory iSrvCategory;
	RPointerArray<TAvdtpServiceCapability> iRemoteCapabilities;
	TFixedArray<RSocket, 3> iSockets;
	HBufC8*		iReadBuffer;
	TPtr8		iReadBufferDes;
	HBufC8*		iReadExpectBuffer;
	HBufC8*		iSendBuffer;
	HBufC8*		iExpectedResponseSecurityData;
	TSEID		iLocalSEID;
	TSEID		iRemoteSEID;
	
	CActiveCallback*	iActive;
	TInt				iCmdIndex;
	
	TInt		iConfigReturnCode;
	TInt		iConfigEndReturnCode;
	
	};

#endif /* __T_DATA_RGAVDP_H__*/
