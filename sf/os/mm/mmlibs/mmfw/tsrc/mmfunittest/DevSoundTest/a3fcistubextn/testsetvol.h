
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

#ifndef TESTSETVOL_H
#define TESTSETVOL_H

const TUid KUidTestSetVolIf = {0x102866B0};

class MTestSetVolIf
	{
public:
	// Backdoor to set volume - support via CIClient/ServerExtension
	virtual TInt SetVol(TInt aVolume) = 0;
	// Backdoor to volume - actually the gain value from the context scaled to 0..aMaxVol
	virtual TInt Vol(TInt aMaxVol) = 0;
	};


#endif
