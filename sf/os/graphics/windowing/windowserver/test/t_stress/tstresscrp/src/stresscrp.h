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

#ifndef STRESSCRP_H
#define STRESSCRP_H

class MWsGc;
class TRect;

class StressCrpNgaContext
	{
	public:
		static void Draw(MWsGc& aGc, const TRect& aRect);
	};

class StressCrpNonNgaContext
	{
	public:
		static void Draw(MWsGc& aGc, const TRect& aRect);
	};

#endif  // STRESSCRP_H

