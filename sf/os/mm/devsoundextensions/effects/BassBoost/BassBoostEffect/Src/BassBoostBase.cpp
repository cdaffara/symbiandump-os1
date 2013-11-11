/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Implementation of the bassboost effect class
*
*/




// INCLUDE FILES

#ifdef _DEBUG
#include <e32svr.h>
#endif

#include <BassBoostBase.h>
#include <CustomInterfaceUtility.h>
#include "BassBoostProxy.h"
#include <DrmAudioSamplePlayer.h>
#include <mdaaudioinputstream.h>
#include <mdaaudiooutputstream.h>
#include <mdaaudiotoneplayer.h>
#include <mmf/server/sounddevice.h>
#include <videoplayer.h>

#ifdef _DEBUG
#define DEBPRN0         RDebug::Printf( "%s", __PRETTY_FUNCTION__);
#define DEBPRN1(str)    RDebug::Printf( "%s %s", __PRETTY_FUNCTION__, str );
#else
#define DEBPRN0
#define DEBPRN1(str)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBassBoost::CBassBoost
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost::CBassBoost()
    : 	iBassBoostData(),
    	iDataPckgTo(iBassBoostData),
    	iDataPckgFrom(iBassBoostData)
    {
    }

// Destructor
EXPORT_C CBassBoost::~CBassBoost()
    {
    }


// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CMdaAudioInputStream& aUtility, TBool aEnable)
	{
    
    DEBPRN0;
    CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)aUtility.CustomInterface(KUidBassBoostEffect);
	
	if (bassboostProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
		
		if(aEnable)
	    {
		    bassboostProxy->EnableL();
	    }
	    
    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CMdaAudioOutputStream& aUtility , TBool aEnable)
	{
    
    DEBPRN0;
    CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)aUtility.CustomInterface(KUidBassBoostEffect);
	
	if (bassboostProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
		
		if(aEnable)
	    {
		    bassboostProxy->EnableL();
	    }
	    
    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CMdaAudioConvertUtility& aUtility , TBool aEnable)
	{

    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)customInterface->CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	{
		bassboostProxy->EnableL();
	}
	
    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CMdaAudioPlayerUtility& aUtility , TBool aEnable)
	{
	    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)customInterface->CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	{
		bassboostProxy->EnableL();
	}

    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CMdaAudioRecorderUtility& aUtility,
	TBool aRecordStream , TBool aEnable)
	{
	    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility, aRecordStream);
    CleanupStack::PushL(customInterface);

	CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)customInterface->CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	{
		bassboostProxy->EnableL();
	}
	
    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CMdaAudioToneUtility& aUtility, TBool aEnable)
	{
    
    DEBPRN0;
    CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)aUtility.CustomInterface(KUidBassBoostEffect);
	
	if (bassboostProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
		
		if(aEnable)
	    {
		    bassboostProxy->EnableL();
	    }
	    
    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CMMFDevSound& aDevSound, TBool aEnable )
	{
    
    DEBPRN0;
    CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)aDevSound.CustomInterface(KUidBassBoostEffect);
	
	if (bassboostProxy == NULL)
		{
        DEBPRN1("No Adaptation Support - leaving");    
    	User::Leave(KErrNotSupported);
		}
		
		if(aEnable)
	    {
		    bassboostProxy->EnableL();
	    }
	    
    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	CCustomCommandUtility* aUtility, TBool aEnable )
	{
	    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)customInterface->CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	{
		bassboostProxy->EnableL();
	}
	
    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
	MCustomInterface& aCustomInterface , TBool aEnable)
	{
	    
    DEBPRN0;
	CBassBoost* bassboostProxy = (CBassBoost*)aCustomInterface.CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}
	
	if(aEnable)
	{
		bassboostProxy->EnableL();
	}

    return bassboostProxy;
	}

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL( 
    CMidiClientUtility& aUtility, TBool aEnable )
 	{
    
    DEBPRN0;
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)customInterface->CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving"); 
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
    	{
		bassboostProxy->EnableL();
	    }
	
    return bassboostProxy;
	}   
    
// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
    CDrmPlayerUtility& aUtility, TBool aEnable)
 	{

    DEBPRN0; 
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)customInterface->CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	    {
		bassboostProxy->EnableL();
	    }
	
    return bassboostProxy;
	}   

// -----------------------------------------------------------------------------
// CBassBoost::NewL
// Static function for creating an instance of the BassBoost object.
// -----------------------------------------------------------------------------
//
EXPORT_C CBassBoost* CBassBoost::NewL(
    CVideoPlayerUtility& aUtility, TBool aEnable)
 	{

    DEBPRN0; 
   	CCustomInterfaceUtility* customInterface = CCustomInterfaceUtility::NewL(aUtility);
    CleanupStack::PushL(customInterface);

	CBassBoostProxy* bassboostProxy = (CBassBoostProxy*)customInterface->CustomInterface(KUidBassBoostEffect);

	if ( !bassboostProxy )
		{
		DEBPRN1("No Adaptation Support - leaving");
		User::Leave(KErrNotSupported);
		}

	CleanupStack::Pop(customInterface);

	if(aEnable)
	    {
		bassboostProxy->EnableL();
	    }
	
    return bassboostProxy;
	}   
        
// -----------------------------------------------------------------------------
// CBassBoost::DoEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& CBassBoost::DoEffectData()
	{
    DEBPRN0;
	iDataPckgTo = iBassBoostData;
	return iDataPckgTo;
	}

// -----------------------------------------------------------------------------
// CBassBoost::SetEffectData
// -----------------------------------------------------------------------------
//
EXPORT_C void CBassBoost::SetEffectData(
	const TDesC8& aEffectDataBuffer )
	{
    DEBPRN0;
	TEfBassBoostDataPckg dataPckg;
	dataPckg.Copy(aEffectDataBuffer);
	iBassBoostData = dataPckg();
	iEnabled = iBassBoostData.iEnabled;
	iEnforced = iBassBoostData.iEnforced;
	iHaveUpdateRights = iBassBoostData.iHaveUpdateRights;
	}


// -----------------------------------------------------------------------------
// CBassBoost::Uid
// -----------------------------------------------------------------------------
//
EXPORT_C TUid CBassBoost::Uid() const
	{
	return KUidBassBoostEffect;
	}


// ========================== OTHER EXPORTED FUNCTIONS =========================


