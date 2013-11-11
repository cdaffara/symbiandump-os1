// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Flow / Binder
//  (data plane)
//
//
//  This is a 3-plane comms layer implementation example, which has been customised to be a test layer which gobbles and releases ESOCK MBUFs.
//  The MBuf gobbling functionality can be disabled by undefining the macro SYMBIAN_COMMSFW_MBUF_GOBBLER which is specified in mbufgobblerproviders.mmp.
//  When SYMBIAN_COMMSFW_MBUF_GOBBLER is undefined, the source code specified by mbufgobblerproviders.mmp becomes a pass through layer i.e. it passes the data
//  through to the layer above or below without altering it. This makes it useful as a starting point for implementing your own layers / providers;
//  useful documentation on how to customise your own passthrough layer can be found in ..\docs\MbufGobblerLayer.doc
//

/**
 @file
 @internalComponent
*/

#ifndef MBUFGOBBLERFLOW_H
#define MBUFGOBBLERFLOW_H

#include <elements/nm_signatures.h>
#include <comms-infras/ss_subconnflow.h>
#include <comms-infras/ss_flowbinders.h>
#include <platform/es_mbuf.h>
#include <comms-infras/mbufchain.h>
#include "mbufgobblertestflags.h"
#include <e32property.h>

class CMbufGobblerBinder;
class CMBufGobbler;

class CMbufGobblerFlow : public ESock::CSubConnectionFlowBase, public ESock::MFlowBinderControl
	{
	friend class CMbufGobblerFlowFactory;

protected:
	CMbufGobblerFlow(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
    void ConstructL();
public:
	static CMbufGobblerFlow* NewL(ESock::CSubConnectionFlowFactoryBase& aFactory, const Messages::TNodeId& aSubConnId, ESock::CProtocolIntfBase* aProtocolIntf);
	virtual ~CMbufGobblerFlow();
	
	// from CSubConnectionFlowBase:
	virtual ESock::MFlowBinderControl* DoGetBinderControlL();
	virtual void ReceivedL(
		const Messages::TRuntimeCtxId& aSender,
		const Messages::TNodeId& aRecipient,
		Messages::TSignatureBase& aMessage
		);

	// from MFlowBinderControl:
	virtual ESock::MLowerControl* GetControlL(const TDesC8& aProtocol);
	virtual ESock::MLowerDataSender* BindL(const TDesC8& aProtocol, ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl);
	virtual void Unbind( ESock::MUpperDataReceiver* aReceiver, ESock::MUpperControl* aControl);
	virtual ESock::CSubConnectionFlowBase* Flow();

	// my helper methods:
	CMbufGobblerBinder* FindOrCreateBinderL(const TDesC8& aProtocol);
	
private:
	RPointerArray<CMbufGobblerBinder> iBinders;
	
#ifdef SYMBIAN_COMMSFW_MBUF_GOBBLER	
    CMBufGobbler* iMBufGobbler;
#endif    
	};

class CMbufGobblerBinder :	public CBase,
			public ESock::MLowerControl, // I receive control requests from above
			public ESock::MLowerDataSender, // I receive data from above (outgoing)
			public ESock::MUpperControl, // I receive control signals from below
			public ESock::MUpperDataReceiver // I receive data from below (incoming)
	{
	public:
	// My constructor
		static CMbufGobblerBinder* NewL(const TDesC8& aProtocolName);
		void ConstructL();
	protected:
		CMbufGobblerBinder(const TDesC8& aProtocolName);		
	public:
		virtual ~CMbufGobblerBinder();
		
		// from ESock::MLowerControl:
		virtual TInt GetName(TDes& aName);
		virtual TInt BlockFlow(TBlockOption aOption);
		virtual TInt GetConfig(TBinderConfig& aConfig);
		virtual TInt Control(TUint aLevel, TUint aName, TDes8& aOption);
		
		// from ESock::MLowerDataSender:
		virtual ESock::MLowerDataSender::TSendResult Send(RMBufChain& aData);
		
		// from ESock::MUpperControl:
		virtual void StartSending();
		virtual void Error(TInt anError);
		
		// from ESock::MUpperDataReceiver:
		virtual void Process(RMBufChain& aData);
		

		// my own methods
		TBool InUse() const {return iLowerControl || iLowerDataSender || iUpperControl || iUpperDataReceiver ; }
		void BindToUpperL(ESock::MUpperDataReceiver& aUpperDataReceiver, ESock::MUpperControl& aUpperControl);
		TBool UnbindFromUpper(ESock::MUpperDataReceiver& aUpperDataReceiver, ESock::MUpperControl& aUpperControl);
		void BindToLowerFlowL(ESock::MFlowBinderControl& aLowerControl);
		void UnbindFromLowerFlow(); // NEW UNBIND COMMAND
		const TDesC8& ProtocolName() const;
		
	private:
		ESock::MFlowBinderControl* iBinderControl;
		ESock::MLowerControl* iLowerControl; 		// .. so I can send controls down
		ESock::MLowerDataSender* iLowerDataSender;		// .. so I can send data down (outgoing)
		ESock::MUpperControl* iUpperControl;		// .. so I can send controls up (err/startsending)
		ESock::MUpperDataReceiver* iUpperDataReceiver;	// .. so I can send data up (incoming)
		TBuf8<10> iProtocolName; // long enough for an acronym one would hope	
		MBUFGOBBLER_TEST_DATA	
	};

#ifdef SYMBIAN_COMMSFW_MBUF_GOBBLER

class CMBufGobbler :   public CActive
    {
public:
    static CMBufGobbler* NewL();
    void ConstructL();
protected:
    CMBufGobbler();
public:
    virtual ~CMBufGobbler();

    // from CActive:
    virtual void DoCancel();
    virtual void RunL();

    RMBufChain iChain;
    RProperty iProperty;
    };
#endif

#endif // MBUFGOBBLERFLOW_H
