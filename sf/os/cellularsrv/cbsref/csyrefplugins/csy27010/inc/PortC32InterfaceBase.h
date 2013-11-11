//
// * Copyright 2004 Neusoft America Inc.
// * All rights reserved.
// * This component and the accompanying materials are made available
// * under the terms of the Eclipse Public License v1.0
// * which accompanies this distribution, and is available
// * at the URL "http://www.eclipse.org/legal/epl-v10.html".
// *
// * Contributors:
// * Keith Collins (Neusoft America Inc.)  original software development and additional code and modifications.
// * Thomas Gahagen (Neusoft America Inc.)  additional code and modifications.
// * Zhen Yuan (Neusoft America Inc.)  additional code and modifications.
// *
// * Description:  This file defines the CPortC32Interface class.  This class
// *               is equivalent to client RComm subsessions. This is derived 
// *               from the C32 base class for ports CPort.
//

// CPortC32InterfaceBase.h
/** @file
 *
 * PortC32InterfaceBase:
 * - Equivalent to client RComm subsessions. This is base class 
 *   derived from the C32 base class for ports CPort.
 */

#ifndef _PORTC32INTERFACEBASE_H__
#define _PORTC32INTERFACEBASE_H__

#include <cs_port.h>
#include "CsyGlobals.h"
#include "Portfactory.h"

class CChannelMgrCmdData;
class CCsyMsgBufBpFrame;

class CPortC32InterfaceBase : public CPort
	{
public:
	virtual ~CPortC32InterfaceBase();

	// from CPort
	virtual void Destruct();
	virtual void StartRead(const TAny* aClientBuffer, TInt aLength);
	virtual void ReadCancel();
	virtual void NotifySignalChange(TUint aSignalMask);
	virtual void NotifySignalChangeCancel();
	virtual void StartWrite(const TAny* aClientBuffer, TInt aLength);
	virtual TInt SetConfig(const TDesC8& aPackage);

	// message passing
	void CompleteWriteRequest(TInt aStatus);
	virtual void SendFrameToClient(CCsyMsgBufBpFrame* aFrame) = 0;


	// utility methods
	void SetMuxChannel(CChannelMgrCmdData* aMuxChannel);
	CChannelMgrCmdData* GetMuxChannel() const;
	TInt GetPortNumber() const;
	CPortFactory::TC32ClientType GetClientType() const;

	void ModemAndCsyToClientFlowCtrl(const TFlowControl aFlowControl);
	void ReceivedV24Signals(const TUint8 aV24Signals);
	TInt SetV24Signals(const TUint aRs232Signals);

	void IncRefCount();
	void CompleteOutstandingRequest();

	// link for parent CPortFactory
	TSglQueLink iLink;

protected:
	CPortC32InterfaceBase(CPortFactory& aPortFactory,
		CPortFactory::TC32PortInfo& aPortInfo);
	void ConstructL();
	void SetDataAvailable();

	void CompleteReadRequest(TInt aStatus);
	virtual TBool ReadFromBufOrQueue() = 0;

	// utility
	void RemoveWaitingAllFrames(TBool aAddToFreeList = ETrue);
	void Shutdown();
	

	// from CPort, but not all are supported by the CSY
	virtual TInt QueryReceiveBuffer(TInt& aLength) const = 0;
	virtual void ResetBuffers(TUint aFlags) = 0;
	virtual void Break(TInt aTime);
	virtual void BreakCancel();
	virtual void WriteCancel();
	virtual TInt GetConfig(TDes8& aPackage) const;
	virtual TInt SetServerConfig(const TDesC8& aPackage);
	virtual TInt GetServerConfig(TDes8& aPackage);
	virtual TInt GetCaps(TDes8& aPackage);
	virtual TInt GetSignals(TUint& aSignals);
	virtual TInt SetSignalsToMark(TUint aSignals);
	virtual TInt SetSignalsToSpace(TUint aSignals);
	virtual TInt GetReceiveBufferLength(TInt& aLength) const;
	virtual TInt SetReceiveBufferLength(TInt aLength);
	virtual void FreeMemory(); 
	virtual void NotifyDataAvailable();
	virtual void NotifyDataAvailableCancel();
	virtual void NotifyConfigChange();
	virtual void NotifyConfigChangeCancel();
	virtual void NotifyFlowControlChange();
	virtual void NotifyFlowControlChangeCancel();
	virtual void NotifyBreak();
	virtual void NotifyBreakCancel();
	virtual void NotifyOutputEmpty();
	virtual void NotifyOutputEmptyCancel();
	virtual TInt GetFlowControlStatus(TFlowControl& aFlowControl);
	virtual TInt GetRole(TCommRole& aRole);
	virtual TInt SetRole(TCommRole aRole);

protected:

	// List of frames waiting to be read by the client
	TSglQue<CCsyMsgBufBpFrame>		iFramesWaitingToBeReadList;
	TSglQueIter<CCsyMsgBufBpFrame>	iFramesWaitingToBeReadIter;

	// attributes
	TCommRole						iRole;
	TCommConfigV01					iConfig;
	CPortFactory&					iPortFactory;

	CPortFactory::TC32PortInfo		iPortInfo;
	CChannelMgrCmdData*				iMuxChannel;

	// Pointer to client's read buffer when there is a read outstanding.
	// (This is the way C32 transfers messages from clients to CSYs.)
	TAny*							iClientBuffer;
	TInt							iClientLength;
	TInt							iPos;
	TBool							iOneOrMore;

	TFlowControl					iCsyAllowedToSendFrames;
	TBool							iFlowControlNotifyPending;

	TUint							iSignals;
	TUint							iSentSignals;
	TUint							iSignalMask;
	TBool							iSignalChangeNotifyPending;

	TBool							iDataAvailableNotifyPending;

	TBool							iIsWriteInProgress;
	TBool							iIsReadInProgress;
	};


// Inline methods
inline CChannelMgrCmdData* CPortC32InterfaceBase::GetMuxChannel() const
/**
 * Return a pointer to the mux channel associated with this C32 port.
 *
 * @return Pointer to the mux channel associated with this C32 port
 */
	{
	return iMuxChannel;
	}

inline CPortFactory::TC32ClientType CPortC32InterfaceBase::GetClientType() const
/**
 * Return the client type (command or data) for this C32 port.
 *
 * @return Client type command or data
 */
	{
	return iPortInfo.iClientType;
	}

inline TInt CPortC32InterfaceBase::GetPortNumber() const
/**
 * Return the port number for this C32 port.
 *
 * @return Port number
 */
	{
	return iPortInfo.iPortNumber;
	}

inline void CPortC32InterfaceBase::IncRefCount()
/**
 * Increment the reference count for this object.
 */
	{
	Inc();
	};

#endif  // _PORTC32INTERFACEBASE_H__
