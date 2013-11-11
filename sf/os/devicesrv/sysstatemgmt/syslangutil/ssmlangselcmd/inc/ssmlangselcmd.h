/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Declaration of CSsmLangSelCmd class.
 *
 */

#ifndef SSMLANGSELCMD_H
#define SSMLANGSELCMD_H

#include "ssmcommonlocale.h"
#include <e32base.h>
#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmadaptationcli.h>
#include <e32hashtab.h> 

class CSsmMapperUtility;

/**
 *  Each command is implemented in a class derived from MSsmCustomCommand.
 *  It is also possible to implement multiple similar command in the same
 *  class and differentiate between them based on constructor parameters
 *  from the factory method.
 */
NONSHARABLE_CLASS( CSsmLangSelCmd ) : public CActive, public MSsmCustomCommand
    {

public:

    /**
     * Two-phased constructor
     */
	static CSsmLangSelCmd* NewL();

    /**
     * Destructor
     */
    virtual ~CSsmLangSelCmd();
    
private: // From CActive

    void RunL();
    void DoCancel();

private: // From MSsmCustomCommand

    /**
     * @see MSsmCustomCommand
     */
    TInt Initialize( CSsmCustomCommandEnv* aCmdEnv );

    /**
     * @see MSsmCustomCommand
     */
	void Execute( const TDesC8& aParams, TRequestStatus& aRequest );

    /**
     * @see MSsmCustomCommand
     */
	void ExecuteCancel();

    /**
     * @see MSsmCustomCommand
     */
	void Close();

    /**
     * @see MSsmCustomCommand
     */
	void Release();
	
private:
    
    CSsmLangSelCmd();
    
    void ConstructL();
    
    void InitializeL();

    /**
     * Read language code selected by user from Central Repository and return it.
     * Return 0 (automatic) in case of an error.
     */
    TInt GetLastSelectedLang();

    /**
     * Check if the language code given as parameter can be found in the
     * iValidLanguages array.
     *
     * @param aLanguage The language code to check.
     * @return ETrue if the language code was found in the list, EFalse otherwise.
     */
    TBool IsValid(const TInt aLanguage) const;

    /**
     * Prepare next preferred language list for fetching.
     */
    void PrepareNextList();

    /**
     * Fetch the prepared language list.
     */
    void FetchLanguageListL();

    /**
     * Handle language list received from StartupAdaptation.
     *
     * @return Language code that was selected or KErrNotFound.
     */
    TInt InterpretResult();

    /**
     * Check all alternatives for the language given as parameter, and return
     * first language id that is valid.
     *
     * @param aLanguage The language code to check.
     * @return The language code given as parameter if it is a valid language.
     * Otherwise the first valid alternative found, or KErrNotFound if there are
     * none.
     */
    TInt ValidAlternative(const TInt aLanguage) const;

    /**
     * Fetch next preferred list if there are any left. Otherwise check if
     * the last selected language can be used. If not, use the default
     * language.
     */
    void TryNextList();

    /**
     * Store a language code to Central Repository.
     * @param aLanguage The language code to store.
     */
    void StoreLanguageToCentRep(const TInt aLanguage);

    /**
     * Get a language code from Central Repository.
     *
     * @return Language code from CentRep or iDefaultLanguage in case of failure.
     */
    TInt GetLanguageFromCentRep();

    /**
     * Take the language into use and signal client that language selection is
     * complete.
     *
     * @param aLang The language to use or a system-wide error code.
     */
    void UseLanguage(const TInt aLang);

    /**
    * Initialize the language region mapping
    * 
    */
    void InitializeRegionMappingL();

    /**
    * Used to check whether the region is valid or not
    * 
    * @param aRegion Region to be validated
    * 
    * @return ETrue if the region code was found in the list, EFalse otherwise.
    */
    TBool IsRegionValidL(const TInt aRegion) const;
    
    /**
    * Get the stored Region and Collation code from Central Repository
    * 
    * @param aRegion On return, will store the Region code fetched from CR
    * @param aCollation On return, will store the Collation code fetched from CR
    * 
    */
    void GetIndividualSettingsFromCentRepL(TInt& aRegion, TInt& aCollation);

    /**
    * Store the given Region and Collation code to Central Repository
    * 
    * @param aRegion Region code to be stored in CR
    * @param aCollation Collation code to be stored in CR
    * 
    * @return Success code, Contains any of the error code in case of any error.
    */
    TInt SetIndividualSettingsToCentRep(const TInt aRegion, const TInt aCollation);

    /**
    * Gets the selected Region and Collation codes. If not set will get the asscoiated codes 
    * for the language
    * 
    * @param aLastSelectedLang Contains the seleted Language code
    * @param aLastSelectedRegion On success contains the Region code
    * @param aLastSelectedCollation On success contains the Collation code
    * 
    * @return ETrue If the Region and Collation codes are valid or found the mapped Region 
    * incase of auto selection, EFalse otherwise.
    */
    TBool ValidateAndGetSettings(const TInt aLastSelectedLang, TInt& aLastSelectedRegion, TInt& aLastSelectedCollation);

    /**
    * Gets the selected Region and Collation codes. If not set will get the asscoiated codes 
    * for the language
    * 
    * @param aLastSelectedLang Contains the seleted Language code
    * @param aLastSelectedRegion On success contains the Region code
    * @param aLastSelectedCollation On success contains the Collation code
    * 
    * @return ETrue If the Region and Collation codes are valid or found the mapped Region 
    * incase of auto selection, EFalse otherwise. Leaves with any of error code incase of any error
    */
    TBool ValidateAndGetSettingsL(const TInt aLastSelectedLang, TInt& aLastSelectedRegion, TInt& aLastSelectedCollation);

    /**
    * Loads the default language setting if it is valid
    * 
    * @return KErrNone if the default language settings are valid and loaded successfully,
    * any of the error code otherwise.
    */
    TInt ValidateAndUseDefaultLanguage();

    /**
    * Loads the corresponsing locale codes
    * 
    * @param aLang Language code to be loaded
    * @param aRegion Region code to be loaded
    * @param aCollation Collation code to be loaded
    * 
    * @return Success code, Contains any of the error code in case of any error.
    */
    TInt UseLocale( const TInt aLang, const TInt aRegion, const TInt aCollation );

    /**
    * Gets the region mapped with the given language after validating
    * 
    * @param aLanguage Language code for getting associated Region code
    * @param aRegion On return contains the Region code associated with 
    * the given language code
    * 
    * @return ETrue if the region code was found in the list, EFalse otherwise.
    */

    TBool ValidateAndGetMappedRegion( const TInt aLanguage, TInt& aRegion );

private: // data

    /** Custom command environment. Not owned. Set in Initialise. */
    CSsmCustomCommandEnv* iEnv;

    /**
     * Array of valid languages (languages that can be used). Own. Not NULL.
     */
    CArrayFixFlat<TInt>* iValidLanguages;

    /** Adaptation controller for querying SIM preferred language list. */
    RSsmMiscAdaptation iAdaptation;

    /** Request status to complete when language selection is ready. */
    TRequestStatus* iClientStatus;

    /**
     * The language to use if any of the SIM languages or the last used
     * language is not valid.
     */
    TInt iDefaultLanguage;

    /** Which language list has been queried last: 0 = the most preferred. */
    TInt iLangListPref;

    /** Will receive the required language list size. */
    TPckgBuf<TInt> iLangListSize;

    /** Will receive the language list contents. */
    CArrayFix<TInt32>* iLangList;
    
    /** Internal state of the object. */
    enum TState
        {
        EQueryListNone = 0,
        EQueryListSize = 1,
        EQueryListContent = 2
        };
    TState iState;
    
    // The mapper utility instance, owned.
    CSsmMapperUtility* iMapperUtility;

    /** Array of  language and region mappings. */
    RHashSet<TLanguageRegion>  iLangRegionMappingHashSet;
    
    /** Array of  valid regions. */
    RArray<TLanguageRegion>  iRegionsArray;
    };

#endif // SSMLANGSELCMD_H
