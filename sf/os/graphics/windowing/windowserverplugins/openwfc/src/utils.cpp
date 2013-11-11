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

#include <gdi.h>
#include <pixelformats.h>
#include "panic.h"

void Panic(TStdPluginPanic aPanic)
	{
	_LIT(KStdPanicCategory, "WSERV-PLUGIN");
	User::Panic(KStdPanicCategory, aPanic);
	}

GLDEF_C TDisplayMode ParseDisplayMode(const TDesC& aModeName)
	{
	// Not using _LIT because we only want the string temporarily, not permanently on the heap.
	if (!aModeName.CompareF(_L("Color16MAP")))
		return EColor16MAP;
	else if (!aModeName.CompareF(_L("Color16MA")))
		return EColor16MA;
	else if (!aModeName.CompareF(_L("Color16MU")))
		return EColor16MU;
	else if (!aModeName.CompareF(_L("Color64K")))
		return EColor64K;
	else if (!aModeName.CompareF(_L("Color4K")))
		return EColor4K;
	else if (!aModeName.CompareF(_L("Color256")))
		return EColor256;
	else if (!aModeName.CompareF(_L("Color16")))
		return EColor16;
	else if (!aModeName.CompareF(_L("Gray256")))
		return EGray256;
	else if (!aModeName.CompareF(_L("Gray16")))
		return EGray16;
	else if (!aModeName.CompareF(_L("Gray4")))
		return EGray4;
	else if (!aModeName.CompareF(_L("Gray2")))
		return EGray2;
	else
		return ENone;
	}
