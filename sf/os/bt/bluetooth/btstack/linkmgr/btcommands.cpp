// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bttypes.h>
#include "internaltypes.h"

enum TBluetoothHciCommandsPanic
	{
	ERangeTooLarge,
	};

void CommandsPanic(TBluetoothHciCommandsPanic aPanic)
	{
	_LIT(KPanicText, "BluetoothHciCommands");
	User::Panic(KPanicText, aPanic);
	}

TBluetoothHciCommands::TBluetoothHciCommands()
	{
	Init(KNullDesC8);
	}


TBluetoothHciCommands::TBluetoothHciCommands(const TDesC8& aFeatures)
	{
	Init(aFeatures);
	}


void TBluetoothHciCommands::Init(const TDesC8& aFeatures)
	{
	iCommands.Copy(aFeatures);
	iCommands.SetMax();
	}


TBool TBluetoothHciCommands::operator[](TInt aOffset) const
	{
	if (aOffset > KBluetoothHciCommandsBytes * 8)
		{
		CommandsPanic(ERangeTooLarge);
		}
	TInt byte = aOffset / 8;
	TInt offset = aOffset - (byte * 8);
	TUint8 byteval = iCommands[byte];
	return TBool(iCommands[byte] & (1<<offset));
	}
