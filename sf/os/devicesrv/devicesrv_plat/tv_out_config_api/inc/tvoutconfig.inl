/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file provides inline functions required for 
*                TVOutConfig Dummy Implementation.
*
*/


//- Member Functions --------------------------------------------------------

//---------------------------------------------------------------------------
/**
    Constructor of TTvSettings class
*/
TTvSettings::TTvSettings():
        iTvSystem( EUndefSystem ),
        iRatio( EUndefRatio ),
        iScanMode( EUndefScan ),
        iOutput( EUndefOutput ),
        iFlickerFilter( EUndefFlicker ),
        iAutoDetection( EUndefAutoDetect ),
        iHorizontalOverScan( 0 ),
        iVerticalOverScan( 0 )
    {
    }

//---------------------------------------------------------------------------
/**
    Constructor of TStandByFigure class
*/
TStandByFigure::TStandByFigure():
        iTable( NULL ),
        iFigureType( EStandByFigure ),
        iColumns( 0 ),
        iRows( 0 ),
        iScaleNumeratorWidth( 1 ),//no scaling (1/1)
        iScaleDenominatorWidth( 1 ),
        iScaleNumeratorHeight( 1 ),//no scaling (1/1)
        iScaleDenominatorHeight( 1 ),
        iLeftTopColumn( 0 ),
        iLeftTopRow( 0 ),
        iBackGroundColor( 0 )
    {
    }

// End of File

