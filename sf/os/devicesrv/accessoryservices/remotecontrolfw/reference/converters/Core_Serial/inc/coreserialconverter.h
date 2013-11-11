// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CORESERIALCONVERTER_H
#define CORESERIALCONVERTER_H

#include <e32base.h>
#include <remcon/remconconverterplugin.h>
#include <remcon/remconconverterinterface.h>

/**
Converter for the Core API and the serial bearer.
*/
NONSHARABLE_CLASS(CCoreSerialConverter) : public CRemConConverterPlugin, 
										public MRemConConverterInterface
	{
public:
	static CCoreSerialConverter* NewL();
	~CCoreSerialConverter();

private: // from CRemConConverterPlugin
	TAny* GetInterface(TUid aUid);

private: // from MRemConConverterInterface
	TInt InterfaceToBearer(TUid aInterfaceUid, 
		TUint aOperationId,
		const TDesC8& aData,
		TRemConMessageType aMsgType, 
		TDes8& aBearerData) const;
	
	TInt BearerToInterface(const TDesC8& aBearerData,
		TUid& aInterfaceUid, 
		TUint& aOperationId,
		TRemConMessageType& aMsgType, 
		TDes8& aData) const;

	TBool SupportedUids(const TUid& aInterfaceUid, const TUid& aBearerUid) const;
	TBool SupportedInterface(const TDesC8& aInterfaceData, const TUid& aBearerUid) const;

private:
	CCoreSerialConverter();

private: // owned
	static const TUint KRemConSerialBearerMessageLength = 30;
	};

#endif // CORESERIALCONVERTER_H
