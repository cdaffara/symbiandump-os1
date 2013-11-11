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


#ifndef __T_DATA_COMMS_BASE_H__
#define __T_DATA_COMMS_BASE_H__

// User includes
#include "DataWrapperBase.h"

// Epoc includes
#include <es_enum.h>

const	TInt	KMaxConnectDataLength		= 255;
const	TInt	KMaxOptionNameLength		= 128;

typedef TBuf<KMaxTestExecuteCommandLength>	TTempBuf;

class CT_DataCommsBase : public CDataWrapperBase
	{
public:
	/**
	* Public destructor
	*/
	~CT_DataCommsBase();

protected:
	/**
	* Protected constructor.
	*/
	CT_DataCommsBase();

private:
	//	MActiveCallback implementation
	virtual void					RunL(CActive* aActive, TInt aIndex);
	virtual void					DoCancel(CActive* aActive, TInt aIndex);

protected:
	void							MissingParamError( const TDesC& aParamName );
	TConnInterfaceState				InterfaceNotificationState( const TDesC& aState );
	TConnectionType					ConnectionType( const TDesC& aConnectionTypeName );
	RConnection::TConnStopType		ConnectionStopType( const TDesC& aConnectionStopType );
	RConnection::TConnAttachType	ConnectionAttachType( const TDesC& aConnectionAttachType );
	TUint							ProgressNotificationExpectedStage( const TDesC& aStage );
	TBool							GetSockType( const TDesC& aSockTypeStr, TUint& aSockType );
	TBool							GetProtocol( const TDesC& aProtocolStr, TUint& aProtocol );
	TBool							GetAddrFamily( const TDesC& aAddrFamilyStr, TUint& aAddrFamily );
	TBool							GetCapability( const TDesC& aCapabilityStr, TCapability& aCapability );
	TBool							GetIntValue(const TDesC& aConstantName, TInt& aValue);
	TUint32							GetLinkStateNotifierBits(const TDesC& aConstantName);
	TBool							GetTShutdown(const TDesC& aConstantName, RSocket::TShutdown& aShutdown);
	TInt							GetTBTPacketType(const TDesC& aConstantName);
	TUint32							GetLowPowerMode(const TDesC& aConstantName);
	TBool 							SubConnectionType( const TDesC& aSubConnTypeStr, RSubConnection::TSubConnType& aSubConnType);
	TBool 							GetEventGroupUid(const TDesC& aEventGroupUidStr, TInt32& aEventGroupUid);
	TBool							GetEventMaskBits( const TDesC& aConstantName, TUint32& aEventMask );
	TBool							ParameterSetType( const TDesC& aParameterSetTypeStr, CSubConParameterFamily::TParameterSetType& aParameterSetType );
	void							ConvertWhiteSpace( const TDesC& inputData, TDes& convertedData );
	void 							PrintProtocol( const TDesC&, const TProtocolDesc& aProtocol, TBool aShowServiceInfoFlags = EFalse );
	TBool							CompareProtocolInfo( TProtocolDesc& aLhs, TProtocolDesc& aRhs );
	TInt 							GetStringAsInt( const TDesC& aEntry, const TDesC& aStrParameter );
	
	};

#endif /* __T_DATA_COMMS_BASE_H__ */
