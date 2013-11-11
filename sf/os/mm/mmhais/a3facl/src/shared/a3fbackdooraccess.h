
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef A3FBACKDOORACCESS_H
#define A3FBACKDOORACCESS_H

/*
This class is to give access to various a3f interfaces from the CustomInterface()
call. It is intended for testing purposes, and is not for general use.
*/

const TUid KA3FBackdoorAccessIfUid = {0x10286696};

// forward decs
class MAudioContext;
class MAudioStream;
class MAudioProcessingUnit;

class MA3FBackdoorAccessIf
	{
public:
	virtual MAudioContext* AudioContext() = 0;
	virtual MAudioStream* AudioStream() = 0;
	virtual MAudioProcessingUnit* ProcessingUnit(TUid aType) = 0;
	};

#endif // A3FBACKDOORACCESS_H

