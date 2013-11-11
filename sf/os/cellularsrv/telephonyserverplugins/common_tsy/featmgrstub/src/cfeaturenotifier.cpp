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
// This file contains a stubbed implementation of TFeatureEntry.
// 
//

#include <featmgr/featurenotifier.h>

EXPORT_C  CFeatureNotifier* CFeatureNotifier::NewL( MFeatureObserver& /*aObserver */)
	{
	return NULL;
	}

EXPORT_C CFeatureNotifier::~CFeatureNotifier()
	{
	}

EXPORT_C TInt CFeatureNotifier::NotifyRequest( TUid /*aFeature*/ )
	{
	return KErrNone;
	}

EXPORT_C TInt CFeatureNotifier::NotifyRequest( RFeatureUidArray& /*aFeatures*/ )
	{
	return KErrNone;
	}

EXPORT_C TInt CFeatureNotifier::NotifyCancel( TUid /*aFeature*/ )
	{
	return KErrNone;
	}

EXPORT_C TInt CFeatureNotifier::NotifyCancelAll()
	{
	return KErrNone;
	}

void CFeatureNotifier::RunL()
	{
	}

TInt CFeatureNotifier::RunError( TInt /*aError*/ )
	{
	return KErrNone;
	}

void CFeatureNotifier::DoCancel()
	{
	}
	
