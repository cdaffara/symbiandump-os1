// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TFBSOOGM_H
#define TFBSOOGM_H
#include <EGL/egl.h>

#include <e32cmn.h>
#include <EGL/eglext.h>
#include <VG/openvg.h>
#include <sgresource/sgresource.h>
#include <sgresource/sgimage.h>
#include <graphics/fbsglyphdataiterator.h>
#include "FbsMessage.h"

#include "test/TGraphicsHarness.h"


/**
Test class for the GPU 'Out of Graphics Memory' (OoGM) plug-in

Hardware only as the GoomMonitor framework does not operate under WINSCW
*/
class CTFbsOogm : public CTGraphicsBase
    {
public:
    CTFbsOogm( CTestStep* aStep );
    ~CTFbsOogm();
protected:
    //from  CTGraphicsBase
    virtual void RunTestCaseL( TInt aCurTestCase );
    void ConstructL();

private:
    // Test Cases
    void CacheClearanceAndLimitAdjustments();

private:
    void UseGpuL();
    TInt FillGraphicsMemoryWithImages( const TSize& aSize, RArray<RSgImage>& aImages );

private:
    CFbsFont* iFont;
    CFbsTypefaceStore* iTs;
    };



class CTFbsOogmStep : public CTGraphicsStep
    {
public:
    CTFbsOogmStep();

protected:
    //from CTGraphicsStep
    virtual CTGraphicsBase* CreateTestL();
    };

_LIT( KTFbsOogmStep,"TFbsOogm" );



#endif // TFBSOOGM_H
