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
// Graphics Resource - kernel extension inline functions
//

#ifndef SGEXTENSION_INL
#define SGEXTENSION_INL

inline TInt DSgResource::Compare(const TUint64* aId, const DSgResource& aResource)
	{
	if (*aId > aResource.Id())
		{
		return 1;
		}
	if (*aId < aResource.Id())
		{
		return -1;
		}
	return 0;
	}

inline TInt DSgResource::Compare(const DSgResource& aResource1, const DSgResource& aResource2)
	{
	if (aResource1.Id() > aResource2.Id())
		{
		return 1;
		}
	if (aResource1.Id() < aResource2.Id())
		{
		return -1;
		}
	return 0;
	}

#endif // SGEXTENSION_INL
