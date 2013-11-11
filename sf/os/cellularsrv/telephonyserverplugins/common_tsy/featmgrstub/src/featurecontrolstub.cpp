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
// This file contains a stubbed implementation of RFeatureControl.
// 
//

#include <featmgr/featurecontrol.h>

EXPORT_C RFeatureControl::RFeatureControl()
	{
	}

EXPORT_C TInt RFeatureControl::Connect()
	{
	return KErrNone;
	}

EXPORT_C TInt RFeatureControl::Open()
	{
	return KErrNone;
	}

EXPORT_C void RFeatureControl::Close()
	{
	}

EXPORT_C TInt RFeatureControl::FeatureSupported( TUid /*aFeature */)
	{
	return KFeatureSupported;
	}

EXPORT_C TInt RFeatureControl::FeatureSupported( TFeatureEntry& /*aFeature */)
	{
	return KFeatureSupported;
	}

EXPORT_C TInt RFeatureControl::FeaturesSupported( RFeatureArray& /*aFeatures*/ )
	{
	return KFeatureSupported;
	}

EXPORT_C TInt RFeatureControl::EnableFeature( TUid /*aFeature*/ )
	{
	return KErrNone;
	}

EXPORT_C TInt RFeatureControl::DisableFeature( TUid /*aFeature*/ )
	{
	return KErrNone;
	}

EXPORT_C TInt RFeatureControl::SetFeature( TUid /*aFeature*/, TBool /*aEnabled*/, TUint32 /*aData */)
	{
	return KErrNone;
	}

EXPORT_C TInt RFeatureControl::SetFeature( TUid /*aFeature*/, TUint32 /*aData */)
	{
	return KErrNone;
	}

EXPORT_C TInt RFeatureControl::AddFeature( TFeatureEntry& /*aFeature*/ )
	{
	return KErrNone;
	}

EXPORT_C TInt RFeatureControl::ListSupportedFeatures( RFeatureUidArray& /*aSupportedFeatures*/ )
	{
	return KErrNone;
	}
