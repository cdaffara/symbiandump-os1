// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <mmf/server/mmfcodec.h>
#include "MmfUtilities.h"
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <ecom/ecom.h>
#include <mm/mmpluginutils.h>

_LIT8( KEmptyFourCCString, "    ,    " ) ;
_LIT( KNullString, "" ) ;

static const TUid KUidMmfPluginInterfaceCodec = {KMmfUidPluginInterfaceCodec};

/**
Creates a CMMFCodec object with match parameter in addition to the source and
destination fourCC codes (for instance a manufacturer's name).

Will attempt match without extra match string if no match.

Will Leave if no match on 4CC codes (KErrNotFound).

Used where there may be multiple codecs that perform the same conversion to ensure the controller
uses the codec specified by aPreferredSupplier.

@param  aSrcDataType
        The source data type.
@param  aDstDataType
        The destination data type.
@param  aPreferredSupplier
        Additional resolution criteria when searching for plug in codec. If this is provided, the 
        list of matching plugins will be further searched for the latest version of a plugin 
        supplied by supplier named. Note that the display name field is parsed for a match.

@return An instantiated CMMFCodec derived obeject from an ECOM plugin DLL.
*/
EXPORT_C CMMFCodec* CMMFCodec::NewL(const TFourCC& aSrcDataType, const TFourCC& aDstDataType,  const TDesC& aPreferredSupplier)
	{

	// Create a match string using the two FourCC codes.
	TBufC8<9> fourCCString( KEmptyFourCCString ) ;
	TPtr8 fourCCPtr = fourCCString.Des() ;
	TPtr8 fourCCPtr1( &fourCCPtr[0], 4 ) ;
	TPtr8 fourCCPtr2( &fourCCPtr[5], 4 ) ;
	aSrcDataType.FourCC( &fourCCPtr1 ) ;
	aDstDataType.FourCC( &fourCCPtr2 ) ;

	// Do a list implementations here.

	RImplInfoPtrArray plugInArray; // Array to return matching decoders in
	CleanupResetAndDestroyPushL(plugInArray);

	MmPluginUtils::FindImplementationsL(KUidMmfPluginInterfaceCodec, plugInArray, fourCCPtr);

	if (plugInArray.Count() == 0)
		{
		User::Leave(KErrNotSupported);
		}

	TUid chosenUid = {0};

	if ( plugInArray.Count() == 1 )
		{
		chosenUid = plugInArray[0]->ImplementationUid() ;
		}
	else
		{
		// Use the preferred supplier to select a codec.
		SelectByPreference( plugInArray, aPreferredSupplier ) ;
		for ( TInt ii = 0 ; ii < plugInArray.Count() ; ii++ )
			{
			if ( !(plugInArray[ii]->Disabled()) )
				{
				// we've found our plugin...
				chosenUid = plugInArray[ii]->ImplementationUid() ;
				break ;
				}
			}
		}

	//Instantiate the chosen one
	CMMFCodec* theChosenOne = REINTERPRET_CAST( CMMFCodec*,
						REComSession::CreateImplementationL( chosenUid, _FOFF( CMMFCodec, iDtor_ID_Key ) ) ) ;

	CleanupStack::PopAndDestroy() ;  // pluginArray

	return theChosenOne ;
	}

/**
Creates a CMMFCodec object with known fourCC codes for source and destination.
The first matching plug-in will be used.

The FourCC codes are the same codes as those specified in the resource file and are used to identify
the datatype. ECom scans the registry to find a codec that is registered in its resource file as
being able to convert between the source data type and the destination data type as specified by
their FourCC codes. If a match is found then an instantiation of the appropriate CMMFCodec is made.
If a match is not found this function leaves with KErrNotFound. If more than one codec is present
with the same fourCC match codes then the one that is instantiated is indeterminate. If there is
likely to be any ambiguity due to more than one codec that performs the same conversion being
present, then a preferred supplier should be specified.

@param  aSrcDataType
        The source data type.
@param  aDstDataType
        The destination data type.

@return An instantiated CMMFCodec derived obeject from an ECOM plugin DLL.
*/
EXPORT_C CMMFCodec* CMMFCodec::NewL(const TFourCC& aSrcDataType, const TFourCC& aDstDataType )
	{
	// Create a match string using the two FourCC codes.
	return NewL( aSrcDataType, aDstDataType, KNullString ) ;

	}


// local function to disable items which do not match the preferred supplier.
// Note that at least one enabled item is returned (if there was an enabled item to begin with) which
// may not match the preferred supplier.
/**
Selects a codec according to the specified preference.

@param  aPlugInArray
        On return, array of plugins.
@param  aPreferredSupplier
        Search criteria, a supplier's name for example.
*/
void CMMFCodec::SelectByPreference( RImplInfoPtrArray& aPlugInArray, const TDesC& aPreferredSupplier )
	{	
	// Use the Disabled flag to eliminated all currently enabled matches that
	// do not match the preferred supplier.
	TInt firstEnabled = -1 ; // to ensure that we return something valid
	TInt matchCount = 0 ;
	for ( TInt ii = 0 ; ii < aPlugInArray.Count() ; ii++ )
		{
		if ( !( aPlugInArray[ii]->Disabled() ) )
			{
			if ( firstEnabled == -1 )
				firstEnabled = ii ;
			if ( aPlugInArray[ii]->DisplayName().FindF( aPreferredSupplier ) == KErrNotFound )
				aPlugInArray[ii]->SetDisabled( ETrue ) ;
			else
				matchCount++ ;
			}
		}

	// If there are no matches then re-enable the first enabled
	if ( matchCount == 0 )
		aPlugInArray[firstEnabled]->SetDisabled( EFalse ) ;
	else if ( matchCount > 1 )
		{
		// find the latest version from more than one match
		TInt highestVersionIndex = -1 ;
		for ( TInt ii = 0 ; ii < aPlugInArray.Count() ; ii++ )
			{
			if ( !( aPlugInArray[ii]->Disabled() ) )  // only interested in enabled elements
				{
				if ( highestVersionIndex == -1 )
					{ // first match.  Store this.  Keep it enabled
					highestVersionIndex = ii ;
					}
				else if ( aPlugInArray[ii]->Version() > aPlugInArray[highestVersionIndex]->Version() )
					{ // a new leader.  Disable the previous leader.  Keep this one.
					aPlugInArray[highestVersionIndex]->SetDisabled( ETrue ) ;
					highestVersionIndex = ii ;
					}
				else  // we already have a higher version.
					aPlugInArray[ii]->SetDisabled( ETrue ) ;
				}
			}
		}	
	}


/**
Creates a CMMFCodec object with a known UID.

Will Leave if the plug in is not found (KErrNotFound).

@param aUid
       The UID of a plugin implementation.

@return An instantiated CMMFCodec derived obeject from an ECOM plugin DLL.
*/
EXPORT_C CMMFCodec* CMMFCodec::NewL(TUid aUid)
	{
	return REINTERPRET_CAST(CMMFCodec*, REComSession::CreateImplementationL(aUid,
												_FOFF(CMMFCodec,iDtor_ID_Key)));
	}

/**
@internalComponent

Gets a pointer to the extension specified by an identifier. The extension can be either an
interface or function. If the extension is not supported NULL value is given and returns
KErrExtensionNotSupported.

@param	aExtensionId
		Extension identifier.
@param	aExtPtr
		Pointer to get the extension.
@return	If successful returns KErrNone otherwise KErrExtensionNotSupported.		
*/
TInt CMMFCodec::ExtensionInterface(TUint aExtensionId, TAny*& aExtPtr)
	{
	return Extension_(aExtensionId, aExtPtr, NULL);	
	}

