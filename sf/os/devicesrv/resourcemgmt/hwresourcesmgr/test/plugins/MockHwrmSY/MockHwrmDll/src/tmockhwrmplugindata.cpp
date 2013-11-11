// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tmockhwrmplugindata.h"
#include <s32mem.h>


EXPORT_C TMockHwrmPluginData::TMockHwrmPluginData()
	: iCheckTansId(EFalse), iCheckCommandData(EFalse)
	{
	}

EXPORT_C TMockHwrmPluginData::TMockHwrmPluginData(TUint8 aTransId)
	: iTransId(aTransId), iCheckTansId(ETrue), iCheckCommandData(EFalse)
	{
	}
	
EXPORT_C TMockHwrmPluginData::TMockHwrmPluginData(const TDesC8& aCommandData)
	: iCheckTansId(EFalse), iCheckCommandData(ETrue)
	{
	iCommandData.Create(aCommandData);
    iTransId = 0xFF;
	}

EXPORT_C TMockHwrmPluginData::TMockHwrmPluginData(TUint8 aTransId, const TDesC8& aCommandData)
	: iTransId(aTransId), iCheckTansId(ETrue), iCheckCommandData(ETrue)
	{
	iCommandData.Create(aCommandData);
	}

EXPORT_C TMockHwrmPluginData::~TMockHwrmPluginData()
	{
	iCommandData.Close();
	}


EXPORT_C void TMockHwrmPluginData::SerialiseL(RBuf8& aPackedData)
	{
	aPackedData.CreateL(sizeof(TUint8)+sizeof(TInt8)+iCommandData.Length()+sizeof(TInt)+sizeof(TInt8));
	aPackedData.Zero();
	RDesWriteStream stream;
	stream.Open(aPackedData);
	stream << iTransId;
	stream << static_cast<TInt8>(iCheckTansId);
	stream << iCommandData;
	stream << static_cast<TInt8>(iCheckCommandData);
	stream.Close();
	}
	
EXPORT_C void TMockHwrmPluginData::DeserialiseL(const TDesC8& aPackedData)
	{
	RDesReadStream stream;
	TInt8 flag;
	iCommandData.CreateL(aPackedData.Length());
	stream.Open(aPackedData);
	stream >> iTransId;
	stream >> flag;
	iCheckTansId = flag;
	stream >> iCommandData;
	stream >> flag;
	iCheckCommandData = flag;
	stream.Close();
	}

EXPORT_C TBool TMockHwrmPluginData::operator == (const TDesC8& aPackedData) const
	{
	// Deserialise the aPackedData
	TMockHwrmPluginData data;
	TRAPD(err,data.DeserialiseL(aPackedData));
	if (err != KErrNone)
		return EFalse;
	TBool result = ETrue;
	if (iCheckTansId && data.iCheckTansId)
		{
		result &= (iTransId == data.iTransId);
		}
	if (iCheckCommandData && data.iCheckCommandData)
		{
		result &= (iCommandData == data.iCommandData);
		}
	return result;
	}
