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



#ifndef EPOS_CPOSMODULESSETTINGS_H_
#define EPOS_CPOSMODULESSETTINGS_H_

#include <e32base.h>
#include <lbscommon.h>

#include <lbs/epos_mposmodulesobserver.h>

class CPosModuleIdList;

NONSHARABLE_CLASS( CPosModulesSettings ) : public CBase
	{
	private:
		class TPosModuleAttributesBase
			{
			public:
				inline TPosModuleAttributesBase()
					: iAvailable( EFalse ),
				      iCost ( TPositionQuality::ECostUnknown ) {};

				inline TPosModuleAttributesBase( const TPosModuleAttributesBase& aBase )
				    : iAvailable( aBase.iAvailable ),
				      iCost ( aBase.iCost ) {};

			public:
				TBool iAvailable;
				TPositionQuality::TCostIndicator iCost;
			};

		class TPosModuleListItem : public TPosModuleAttributesBase
			{
			public:
				inline TPosModuleListItem()
				    : TPosModuleAttributesBase(), iUid( TUid::Uid( 0 ) ) {};

				inline TPosModuleListItem( TUid aUid )
				    : TPosModuleAttributesBase(), iUid( aUid ) {};

				inline TPosModuleListItem( const TPosModuleAttributesBase& aBase )
				    : TPosModuleAttributesBase( aBase ), iUid( TUid::Uid( 0 ) ) {};

				inline TPosModuleListItem( const TPosModuleAttributesBase& aBase, TUid aUid )
				    : TPosModuleAttributesBase( aBase ), iUid( aUid ) {};

			public:
				TPositionModuleId iUid;
			};

	public:
		class TPosModuleAttributes : public TPosModuleAttributesBase
			{
			public:
				inline TPosModuleAttributes()
				    : TPosModuleAttributesBase(), iVisible( EFalse ) {};

				inline TPosModuleAttributes( const TPosModuleAttributesBase& aBase )
				    : TPosModuleAttributesBase( aBase ), iVisible( EFalse ) {};

				inline TPosModuleAttributes( const TPosModuleAttributesBase& aBase, TBool aVisible )
				    : TPosModuleAttributesBase( aBase ), iVisible( aVisible ) {};

			public:
				TBool iVisible;
			};

    public:

        /** Two-phased constructor. */
        static CPosModulesSettings* NewL(
        	const TDesC& aPriorityKey,
        	const TDesC& aInvisibleKey,
        	const TDesC& aVersionKey );

        /** Two-phased copy constructor. */
        static CPosModulesSettings* NewL( 
            const CPosModulesSettings& aSettings );

        /** Destructor. */
        virtual ~CPosModulesSettings();

	public:
    
        /** Returns attributes of a module.
        * @param[in] aModuleId UID of the module of interest
        * @param[out] aAttributes Current attributes of the module
        * @return KErrNotFound if module not found, KErrNone otherwise.
        */
		TInt GetAttributes(
			TPositionModuleId aModuleId,
			TPosModuleAttributes& aAttributes ) const;

        /** Sets attributes of a module.
         * @param[in] aModuleId UID of the module of interest
         * @param[in] aAttributes New attributes for the module
         * @leave KErrNotFound if module not found.
         */
		void SetAttributesL(
			TPositionModuleId aModuleId,
			const TPosModuleAttributes& aAttributes );
		
        /** Copies module attributes (cost and status) from other
         *  settings, for all modules. */
		void CopyAttributesL( const CPosModulesSettings& aSource );

		CPosModuleIdList* GetPriorityListLC() const;
		CPosModuleIdList* GetInstalledListLC() const;

		TBool IsVisible( TPositionModuleId aModuleId ) const;

		void CompareL(
			const CPosModulesSettings& aSettings,
			RArray<TPosModulesEvent>& aEvents ) const;

		void SetPrioritiesL( const CPosModuleIdList& aList );

		/** Verifies if a module can be found in priority list
		 *  or in invisibility list */
		TBool ModuleExists( TPositionModuleId aModuleId ) const;

		void AddL( TPositionModuleId aModuleId, const TPosModuleAttributes& aAttributes );
		void Remove( TPositionModuleId aModuleId );

		void FormatL( TPtr& aPriorityKey, TPtr& aInvisibleKey ) const;
		
        /** Returns version string of these settings */
		const TPtrC Version() const;
        /** Sets version string for these settings */
		void SetVersionL( const TDesC& aVersion );

    private:

        CPosModulesSettings();
        void ConstructL(
        	const TDesC& aPriorityKey,
        	const TDesC& aInvisibleKey,
        	const TDesC& aVersionKey );
        void ConstructL( const CPosModulesSettings& aSettings );

        // Prohibit copy constructor
        CPosModulesSettings( const CPosModulesSettings& );
        // Prohibit assigment operator
        CPosModulesSettings& operator= ( const CPosModulesSettings& );

    private:

		typedef RArray<TPosModuleListItem> RModuleList;

    	void ParseAttributedKeyL( const TDesC& aString, RModuleList& aList ) const;
    	HBufC* FormatAttributedKeyLC( const RModuleList& aList ) const;

		void SearchNewOldOrChangedL(
			const CPosModulesSettings& aOldSettings,
			RArray<TPosModulesEvent>& aEvents ) const;

		void SearchPriorityChangeL(
			const CPosModulesSettings& aOldSettings,
			RArray<TPosModulesEvent>& aEvents ) const;

		TInt FindVisible( const TPositionModuleId& aModuleId ) const;
		TInt FindInvisible( const TPositionModuleId& aModuleId ) const;

		static TBool CompareListItemsByUid(
		    const TPosModuleListItem& aOne,
		    const TPosModuleListItem& aAnother );

		void AddEventL(
			TPositionModuleId aModuleId,
			TPosModulesEventType aEventType,
			RArray<TPosModulesEvent>& aEvents ) const;

		void ComparePropertiesL(
        	TPositionModuleId aModuleId,
			TPosModuleAttributes aOne,
			TPosModuleAttributes aAnother,
			RArray<TPosModulesEvent>& aEvents ) const;

    private:
    	RModuleList iPriorityList;
    	RModuleList iInvisibleList;
    	HBufC*		iVersion;
	};

#endif /*EPOS_CPOSMODULESSETTINGS_H_*/
