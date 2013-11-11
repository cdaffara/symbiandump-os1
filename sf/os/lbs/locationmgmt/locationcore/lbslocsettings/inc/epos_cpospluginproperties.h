/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/



#ifndef EPOS_CPOSPLUGINPROPERTIES_H_
#define EPOS_CPOSPLUGINPROPERTIES_H_

#include <e32base.h>
#include <f32file.h>
#include <lbscommon.h>
#include <lbs/positioningpluginfwinternalcrkeys.h>

class CImplementationInformation;

NONSHARABLE_CLASS( CPosPluginProperties ) : public CBase
{
    public:

        /** Two-phased constructor. */
        static CPosPluginProperties* NewL(
        	CImplementationInformation& aEcomInfo,
            RFs& aFsSession );

        /** Destructor. */
        virtual ~CPosPluginProperties();

    public:

        enum TModulePriority
            {
            EModulePriorityAuto,
            EModulePriorityFirst,
            EModulePriorityLast
            };

        /** Returns implementation ID of the plugin. */
        TPositionModuleId ModuleId() const;

        /** Returns version of the implementation */
        TVersion Version() const;

        /** Returns ECom property of whether the implementation is ROM based or not */
        TBool IsRomBased() const;

        /** Returns default visibility of the module from its cenrep file */
        TBool IsVisible() const;

        /** Returns preferred priority of the module from its cenrep file */
        TModulePriority PreferredPriority() const;

        /**
        * Fills parameter with all information about PSY.
        * @param aInfo target container
        */
        void GetModuleInfo( TPositionModuleInfo& aInfo ) const;

    private:

        CPosPluginProperties( RFs& aFsSession );
        void ConstructL( CImplementationInformation& aEcomInfo );

        // Prohibit copy constructor
        CPosPluginProperties( const CPosPluginProperties& );
        // Prohibit assigment operator
        CPosPluginProperties& operator= ( const CPosPluginProperties& );

    private:
        void LoadCenRepInfoL( CRepository& aRepository );

        /** Finds resource file with PSYs name, localized or RSC
         *  It searches only system and Z drive */
        TBool FindPsyNameResourceFileL(
            RFs& aFsSession,
            const TDesC& aRscFile,
            TFileName& aFileName );

        /** Finds a file on given drive.
         *  @param[in] aFile file name without drive
         *  @param[out] aFileName full file name if found
         *  @return ETrue if file is found */
        TBool FindFile(
            RFs& aFsSession,
            TInt aDrive,
            const TDesC& aFile,
            TFileName& aFileName );

        /** Generates full name of the file on given drive */
        void FileNameOnDrive(
            RFs& aFsSession,
            TInt aDrive,
            const TDesC& aFile,
            TFileName& aFileName );

        /** Reads localized PSY name from resource file
         * @param[in] aFsSession Open RFs session
         * @param[in] aResourceFileName Filename of resource file
         */
        HBufC* GetPsyNameFromResourceFileLC(
            RFs& aFsSession,
            const TDesC& aResourceFileName );

    private:
        RFs& iFs;

    	/* UID of the module */
    	TPositionModuleId iModuleId;

		/* CenRep-based properties */
	    TInt iInterfaceVersion;
	    TInt iDefaultPriority;
	    TBool iDefaultStatus;
	    TInt64 iTimeToFirstFix;
	    TInt64 iTimeToNextFix;
	    TReal iHorizontalAccuracy;
	    TReal iVerticalAccuracy;
	    TInt iCostIndicator;
	    TInt iPowerConsumption;
	    TInt iTechnologyType;
	    TInt iDeviceLocation;
	    TInt iCapabilities;
	    TInt iClassesSupported;
	    TBool iDefaultVisibility;
	    TBuf<KPSYNameResourceMaxLength> iLocalizedNameResource;
    	TBuf<KPositionMaxModuleName> iLocalizedName;

    	/* ECom-based properties */
    	TBuf<KPositionMaxModuleName> iEcomName;
    	TInt iVersion;
    	TBool iRomBased;
    };

#endif /*EPOS_CPOSPLUGINPROPERTIES_H_*/
