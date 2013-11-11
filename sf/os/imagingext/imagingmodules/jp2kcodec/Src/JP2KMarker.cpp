/*
* Copyright (c) 2003, 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Collection of structs used to gather the various
*                marker related information in the JP2 image file.
*
*/


// INCLUDE FILES
#include "JP2KMarker.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TSizMarker::TSizMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TSizMarker::TSizMarker() : 
    iXsiz(1),
    iYsiz(1),
    iXOsiz(0),
    iYOsiz(0),
    iXTsiz(1),
    iYTsiz(1),
    iXTOsiz(0),
    iYTOsiz(0),
    iRsiz(0),
    iCsiz(1),
    iSsiz(3), 
    iXRsiz(3), 
    iYRsiz(3)
    {
    }

// Destructor
TSizMarker::~TSizMarker()
    {
    iSsiz.Close();
    iXRsiz.Close();
    iYRsiz.Close();
    }

// -----------------------------------------------------------------------------
// TCODMarker::TCODMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TCODMarker::TCODMarker() : 
    iScod(0),
    iProgressionOrder(0),
    iColorTransformation(0),
    iNumOfLevels(0),
    iCodeBlockStyle(0),
    iWaveletTransformation(0),
    iNumOfLayers(1),
    iCodeBlockSiz(32,32),
    iPrecinctSiz(0)
    {
    }

// Destructor
TCODMarker::~TCODMarker()
    {
    delete iPrecinctSiz;
    iPrecinctSiz = 0;
    }

// -----------------------------------------------------------------------------
// TCOCMarker::TCOCMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TCOCMarker::TCOCMarker() : 
    iCcoc(0),
    iScoc(0),
    iNumOfLevels(0),
    iCodeBlockStyle(0),
    iWaveletTransformation(0),
    iCodeBlockSiz(32,32),
    iPrecinctSiz(0)
    {
    }

// Destructor
TCOCMarker::~TCOCMarker()
    {
    delete iPrecinctSiz;
    iPrecinctSiz = 0;
    }

// -----------------------------------------------------------------------------
// TQCDMarker::TQCDMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TQCDMarker::TQCDMarker(): 
    iSqcd(0),
    iExponent(0), 
    iMantissa(0)
    {
    }

// Destructor
TQCDMarker::~TQCDMarker()
    {
    delete iExponent;
    iExponent = 0;
    delete iMantissa;
    iMantissa = 0;
    }

// -----------------------------------------------------------------------------
// TQCCMarker::TQCCMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TQCCMarker::TQCCMarker() : 
    iCqcc(0),
    iSqcc(0),
    iExponent(0), 
    iMantissa(0)
    {
    }

// Destructor
TQCCMarker::~TQCCMarker()
    {
    delete iExponent;
    iExponent = 0;
    delete iMantissa;
    iMantissa = 0;
    }

// -----------------------------------------------------------------------------
// TPOCMarker::TPOCMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPOCMarker::TPOCMarker(): 
    iRSpoc(1), 
    iCSpoc(1),
    iLYEpoc(1), 
    iREpoc(1),
    iCEpoc(1), 
    iPpoc(1)
    {
    }

// Destructor
TPOCMarker::~TPOCMarker()
    {
    iRSpoc.Close();
    iCSpoc.Close();
    iLYEpoc.Close();
    iREpoc.Close();
    iCEpoc.Close();
    iPpoc.Close();
    }

// -----------------------------------------------------------------------------
// TPPMMarker::TPPMMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPPMMarker::TPPMMarker(): 
    iZppm(0),
    iRemainder(0),
    iNppm(1),
    iIppm(0)
    {
    }

// Destructor
TPPMMarker::~TPPMMarker()
    {
    delete iIppm;
    iIppm = 0;
    }

// -----------------------------------------------------------------------------
// TTLMMarker::TTLMMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TTLMMarker::TTLMMarker() : 
    iZtlm(0),
    iStlm(0),
    iTtlm(1), 
    iPtlm(1)
    {
    }

// Destructor
TTLMMarker::~TTLMMarker()
    {
    iTtlm.Close();
    iPtlm.Close();
    }

// -----------------------------------------------------------------------------
// TPLMMarker::TPLMMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPLMMarker::TPLMMarker() : 
    iZplm(0),
    iNplm(1),
    iIplm(0)
    {
    }

// Destructor
TPLMMarker::~TPLMMarker()
    {
    delete iIplm;
    iIplm = 0;
    }

// -----------------------------------------------------------------------------
// TPPTMarker::TPPTMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPPTMarker::TPPTMarker(): 
    iZppt(0),
    iIppt(0)
    {
    }

// Destructor
TPPTMarker::~TPPTMarker()
    {
    delete iIppt;
    iIppt = 0;
    }

// -----------------------------------------------------------------------------
// TPLTMarker::TPLTMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPLTMarker::TPLTMarker() : 
    iZplt(0),
    iIplt(1)
    {
    }

// Destructor
TPLTMarker::~TPLTMarker()
    {
    iIplt.Close();
    }

// -----------------------------------------------------------------------------
// TCRGMarker::TCRGMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TCRGMarker::TCRGMarker() :
    iXYcrg(1)
    {
    }

// Destructor
TCRGMarker::~TCRGMarker()
    {
    iXYcrg.Close();
    }

// -----------------------------------------------------------------------------
// TCOMMarker::TCOMMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TCOMMarker::TCOMMarker() : 
    iRemainder(0),
    iRcom(0),
    iCcom(0)
    {
    }

// Destructor
TCOMMarker::~TCOMMarker()
    {
    delete iCcom;
    iCcom = 0;
    }

// -----------------------------------------------------------------------------
// TMainMarker::TMainMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TMainMarker::TMainMarker(): 
    iCod(),
    iQcd(),
    iCoc(3), 
    iQcc(3), 
    iRgn(1), 
    iPoc(0),
    iPpm(1), 
    iTlm(1), 
    iPlm(1), 
    iCrg(0), 
    iCom(1)
    {
    }

// Destructor
TMainMarker::~TMainMarker()
    {
    iCoc.ResetAndDestroy();
    iQcc.ResetAndDestroy();
    iRgn.ResetAndDestroy();

    delete iPoc;
    iPoc = 0;

    iPpm.ResetAndDestroy();
    iTlm.ResetAndDestroy();
    iPlm.ResetAndDestroy();

    delete iCrg;
    iCrg = 0;

    iCom.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// TTileMarker::TTileMarker
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TTileMarker::TTileMarker(): 
    iCod(0), 
    iQcd(0), 
    iCoc(3), 
    iQcc(3), 
    iRgn(1), 
    iPoc(0),
    iPpt(1), 
    iPlt(1), 
    iCom(1)
    {
    }

// Destructor
TTileMarker::~TTileMarker()
    {
    delete iCod;
    iCod = 0;

    delete iQcd;
    iQcd = 0;

    iCoc.ResetAndDestroy();
    iQcc.ResetAndDestroy();
    iRgn.ResetAndDestroy();
    
    delete iPoc;
    iPoc =0;

    iPpt.ResetAndDestroy();
    iPlt.ResetAndDestroy();
    iCom.ResetAndDestroy();
    }
