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

#ifndef FEATURECONTROL_H
#define FEATURECONTROL_H

#include <e32std.h>
#include <e32svr.h>
#include <featmgr/featurecmn.h>

NONSHARABLE_CLASS(RFeatureControl)
    {
    public: 
        IMPORT_C RFeatureControl();
        IMPORT_C TInt Connect();
        IMPORT_C TInt Open();
        IMPORT_C void Close();
        IMPORT_C TInt FeatureSupported( TUid aFeature );
        IMPORT_C TInt FeatureSupported( TFeatureEntry& aFeature );
        IMPORT_C TInt FeaturesSupported( RFeatureArray& aFeatures );
        IMPORT_C TInt EnableFeature( TUid aFeature );
        IMPORT_C TInt DisableFeature( TUid aFeature );
        IMPORT_C TInt SetFeature( TUid aFeature, TBool aEnabled, TUint32 aData );
        IMPORT_C TInt SetFeature( TUid aFeature, TUint32 aData );
        IMPORT_C TInt AddFeature( TFeatureEntry& aFeature );
        IMPORT_C TInt ListSupportedFeatures( RFeatureUidArray& aSupportedFeatures );      
    };

#endif      // FEATURECONTROL_H
