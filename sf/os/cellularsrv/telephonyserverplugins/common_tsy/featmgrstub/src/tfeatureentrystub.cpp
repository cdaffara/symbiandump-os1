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
// tfeatureentry.cpp
// This file contains a stubbed implementation of TFeatureEntry.
// 
//

#include <featmgr/featurecmn.h>

EXPORT_C TFeatureEntry::TFeatureEntry( TUid /*aFeature*/ )
	{
	}

EXPORT_C TFeatureEntry::TFeatureEntry( TUid /*aFeature*/, TBitFlags32 /*aFlags*/, TUint32 /*aData */)
	{
	}

EXPORT_C TUid TFeatureEntry::FeatureUid() const
	{
	return TUid();
	}


EXPORT_C TBitFlags32 TFeatureEntry::FeatureFlags() const
	{
	return 0;
	}


EXPORT_C TUint32 TFeatureEntry::FeatureData() const
	{
	return 0;
	}


EXPORT_C TFeatureEntry::TFeatureEntry()
	{
	}

