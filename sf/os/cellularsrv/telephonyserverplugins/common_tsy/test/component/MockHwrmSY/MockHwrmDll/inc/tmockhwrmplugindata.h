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

#ifndef TMOCKHWRMPLUGINDATA_H
#define TMOCKHWRMPLUGINDATA_H

#include <e32base.h>
#include "cmocksyengine.h"

class TMockHwrmPluginData : public MMockSyEngineData
	{
public:
	IMPORT_C TMockHwrmPluginData();
	IMPORT_C TMockHwrmPluginData(TUint8 aTransId);
	IMPORT_C TMockHwrmPluginData(const TDesC8& aCommandData);
	IMPORT_C TMockHwrmPluginData(TUint8 aTransId, const TDesC8& aCommandData);	
	IMPORT_C ~TMockHwrmPluginData();

public:
	IMPORT_C void SerialiseL(RBuf8& aPackedData);
	IMPORT_C void DeserialiseL(const TDesC8& aPackedData);

public:	
	IMPORT_C TBool operator == (const TDesC8& aPackedData) const;
	inline TUint8 TransId() const;
	inline const TDesC8& CommandData() const;
	inline TBool HasTransId() const;
		
protected:
	TUint8 	iTransId;
	TBool 	iCheckTansId;
	RBuf8	iCommandData;
	TBool 	iCheckCommandData;
	};

inline TUint8 TMockHwrmPluginData::TransId() const
	{
	return iTransId;
	}
	
inline const TDesC8& TMockHwrmPluginData::CommandData() const
	{
	return iCommandData;
	}

inline TBool TMockHwrmPluginData::HasTransId() const
	{
	return iCheckTansId;
	}

#endif
