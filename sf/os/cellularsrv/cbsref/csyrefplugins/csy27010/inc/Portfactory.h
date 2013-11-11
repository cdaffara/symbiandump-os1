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
// * Description:  This file is the root of the module. This file contains the 
// *               declarations for the CPortFactory class and is derived from 
// *               C32's CSerial.
//

// PortFactory.h

/** @file PortFactory.h
 *
 */

#ifndef _PORTFACTORY_H__
#define _PORTFACTORY_H__

#include <e32std.h>
#include <e32hal.h>
#include <c32comm.h>
#include <d32comm.h>
#include <cs_port.h>
#include <cdblen.h>
#include "CsyGlobals.h"

class CPortC32Interface;
class CPortC32InterfaceBase;
class CChannelMgrBase;
class CChannelMgrCtrl;
class CChannelMgrCmdData;
class CMux0710Protocol;
class CCommFrameWriterAo;
class CCommFrameReaderAo;

/** @class CPortFactory PortFactory.h "PortFactory.h"
 *  @brief  This class provides the factory object which is used to
 *  construct port objects in the CSY. There is only one instance of
 *  this class in the CSY. CPortFactory is derived from C32's CSerial
 *  base class. CPortFactory is also responsible for allocating the
 *  CSY's multiplexer objects and LDD interface objects.
 */
class CPortFactory : public CSerial  
	{
public:
	enum TC32ClientType
		{
		EC32ClientUndefined,
		EC32ClientTsy,
		EC32ClientNif,
		EC32ClientIpNif
		};

	struct TC32PortInfo
		{
		TC32ClientType iClientType;
		TInt           iPortNumber;
		};

public:
	static CPortFactory* NewL();

	~CPortFactory();

	static void CloseObject(TAny* aObject);

	inline TBool IsMultiplexerCreated();

	TC32ClientType GetClientType(TInt aPortNum);

	inline CCommFrameWriterAo* GetCommWriterAo() const;
	inline CCommFrameReaderAo* GetCommReaderAo() const;
	
	RBusDevComm* DTEPort();  // LDD

	void ConnectControlChannel();
	void DisconnectControlChannel();
	void ChannelCtrlDoCancel();
	
	CPortC32InterfaceBase* FindPortC32Interface(const TUint8 aDlcNum);
	CChannelMgrCmdData* FindChannelMgrByDlcNum(const TUint8 aDlcNum);

	inline CMux0710Protocol* GetMux0710Protocol() const;

	inline CChannelMgrCtrl* GetControlChannel() const;

	void RemoveC32Port(CPortC32InterfaceBase* aPort);

	void ConnectIpNifPort(const TUint8 aDlcNum);

	TSecurityPolicy PortPlatSecCapability(TUint aPort) const;

	TBool FindDlcToEnable();
	void FindActiveDataDlcToStop();
	void StopAnyDlc();
	TBool DecrementNumOfOpenPorts();
	


private:
	CPortFactory();	
	void ConstructL();

	void CreateCsyObjectsL();

	// from CSerial
	virtual CPort* NewPortL(const TUint aUnit);
	virtual void Info(TSerialInfo& aSerialInfo);

	// utility methods
	CChannelMgrCmdData* FindChannelMgr(const TUint aC32PortNum);

	TInt ConfigurePhysicalCommPortL();

private:
	// attributes
	TBool							iMuxObjectsCreated;

	// C32 interface ports
	TSglQue<CPortC32InterfaceBase>		iPortC32InterfaceList;
	TSglQueIter<CPortC32InterfaceBase>	iPortC32InterfaceIter;	

	// multiplexer framer
 	CMux0710Protocol*				iMux0710Protocol;

	// multiplexer Control channel
	CChannelMgrCtrl*				iChannelCtrl;

	// multiplexer Command channels
	TSglQue<CChannelMgrCmdData>		iDataChannelList;
	TSglQueIter<CChannelMgrCmdData>	iDataChannelIter;

	// interface to the serial port logical device driver
	RBusDevComm						iCommPort;
	CCommFrameWriterAo*				iCommWriterAo;
	CCommFrameReaderAo*				iCommReaderAo;

	TBool iDisconnectInProgress;

	TUint iLastDlcNum;
	TUint iNumOfOpenPorts;
	
	TBool iOpenPortFailed;
	};


// Inline Methods
inline CCommFrameWriterAo* CPortFactory::GetCommWriterAo() const
/**
 * Return a pointer to the CSY's comm writer object.
 * @param void
 * @return Pointer to the CSY's comm writer object
 */
	{ 
 	return iCommWriterAo; 
	}

inline CCommFrameReaderAo* CPortFactory::GetCommReaderAo() const
/**
 * Return a pointer to the CSY's comm reader object.
 * @param void
 * @return Pointer to the CSY's comm reader object
 */
	{ 
 	return iCommReaderAo; 
	}

inline RBusDevComm* CPortFactory::DTEPort()
/**
 * Return a pointer to the CSY's LDD port interface object.
 * @param void
 * @return Pointer to the CSY's LDD port interface object
 */
	 { 
 	return (&iCommPort);
	 }

inline CMux0710Protocol* CPortFactory::GetMux0710Protocol() const
/**
 * Return a pointer to the CSY's mux protocol object.
 * @param void
 * @return Pointer to the CSY's mux protocol object
 */
	{
	return iMux0710Protocol;
	}

inline CChannelMgrCtrl* CPortFactory::GetControlChannel() const
/**
 * Return a pointer to the CSY's control channel object.
 * @param void
 * @return Pointer to the CSY's control channel object
 */
	{
	return iChannelCtrl;
	}

#endif  // _PORTFACTORY_H__