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

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfutilities.h>
#include <mmf/server/mmfclip.h>
#include <mmf/server/mmffile.h>
#include <mmf/server/mmfformat.h>

//static const TUid KUidMmfPluginInterfaceFormatDecode = {KMmfUidPluginInterfaceFormatDecode};
//static const TUid KUidMmfPluginInterfaceFormatEncode = {KMmfUidPluginInterfaceFormatEncode};

const TInt KMmfHeaderBufferSize = 512 ;		//512 bytes read to parse the header

CMMFFormatDecode* CMMFFormatDecode::CreateFormatL(TUid aUid, MDataSource* aSource)
	{
	CMMFFormatDecode* s = REINTERPRET_CAST( CMMFFormatDecode*,
				REComSession::CreateImplementationL( aUid, _FOFF( CMMFFormatDecode, iDtor_ID_Key),
				STATIC_CAST( TAny*, aSource ) ) ) ;
	s->iImplementationUid = aUid;
	return s;
	}

/**
Allocates and constructs an ECom format decode object.

This is used to explicitly instantiate a format decoder where the UID is known. This method might
be used by controllers that only support one type of format such that the UID of the format is
already known. For example, if an mp3 controller has been instantiated, there is no need for any
further format recognition as this has already been performed in the controller instantiation, thus
the controller can instantiate an mp3 format directly from it’s UID.

@param  aUid 
        The implementation UID. This is used by Com to create the plugin.
@param  aSource 
        The controller MDataSource and is the source of the data for the format decode plugin. The 
        format decode plugin is the sink of data for the MDataSource.

@return If successful, returns the address of the format decode plugin object created. If not successful, 
        leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatDecode* CMMFFormatDecode::NewL( TUid aUid, MDataSource* aSource )
	{
	return CreateFormatL(aUid, aSource);
	}



/**
Attempt to locate and instantiate a FormatDecode using a filename or an extension.

Only the extension is used.  If no extension  is supplied (no dot is present) the whole of the 
filename will be treated as the extension.

If the file already exists, the file header is scanned to find a match in the opaque_data field of 
the resource file.

@param  aFileName
        The file name of target file.  May be extension only or may include full path.
@param  aSource
        The controller's MDataSource. This is the source of the data for the format decode plugin. This must
        be a CMMFFile source when instantiating a CMMFFormatDecode using a file name.
@param  aPreferredSupplier
        If this is provided the list of matching plugins will be further searched for the latest version of a
        plugin supplied by supplier named.

@return If successful returns an instantiated CMMFFormatDecode from a plugin. If not successful, leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatDecode* CMMFFormatDecode::NewL( const TDesC16& aFileName, MDataSource* aSource, const TDesC& aPreferredSupplier )
	{
	CMMFFormatDecodePluginSelectionParameters* pluginSelector = CMMFFormatDecodePluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParams = CMMFFormatSelectionParameters::NewLC();
	formatParams->SetMatchToFileNameL(aFileName);
	pluginSelector->SetRequiredFormatSupportL(*formatParams);
	if (aPreferredSupplier.Length() > 0)
		pluginSelector->SetPreferredSupplierL(aPreferredSupplier, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	// Instantiate this format
	CMMFFormatDecode* theChosenOne = 
		MMFFormatEcomUtilities::SelectFormatDecodePluginL(*pluginSelector, aSource);

	// Now clean up.
	CleanupStack::PopAndDestroy(2);//formatParams, pluginSelector

	return theChosenOne;
	}

/**
Attempt to locate and instantiate a FormatDecode using a filename or an extension.

Only the extension is used.  If no extension  is supplied (no dot is present) the whole of the 
filename will be treated as the extension.

If the file already exists, the file header is scanned to find a match in the opaque_data field of 
the resource file.

@param  aFileName
        The file name of target file.  May be extension only or may include full path.
@param  aSource
        The controller's MDataSource. This is the source of the data for the format decode plugin. This must
        be a CMMFFile source when instantiating a CMMFFormatDecode using a file name.
@param  aPreferredSupplier
        If this is provided the list of matching plugins will be further searched for the latest version of a
        plugin supplied by supplier named.
@param  aSupportsCustomInterfaces
		Indicates whether the instantiated FormatDecode supports custom interfaces.

@return If successful returns an instantiated CMMFFormatDecode from a plugin. If not successful, leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatDecode* CMMFFormatDecode::NewL( const TDesC16& aFileName, MDataSource* aSource, const TDesC& aPreferredSupplier, TBool& aSupportsCustomInterfaces )
	{
	CMMFFormatDecodePluginSelectionParameters* pluginSelector = CMMFFormatDecodePluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParams = CMMFFormatSelectionParameters::NewLC();
	formatParams->SetMatchToFileNameL(aFileName);
	pluginSelector->SetRequiredFormatSupportL(*formatParams);
	if (aPreferredSupplier.Length() > 0)
		pluginSelector->SetPreferredSupplierL(aPreferredSupplier, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	// Instantiate this format
	CMMFFormatDecode* theChosenOne = 
		MMFFormatEcomUtilities::SelectFormatDecodePluginL(*pluginSelector, aSource, aSupportsCustomInterfaces);

	// Now clean up.
	CleanupStack::PopAndDestroy(2);//formatParams, pluginSelector

	return theChosenOne;
	}


/**
Attempts to locate and instantiate a CMMFFormatDecode using data in a buffer.  The buffer is expected to contain 
header data (from a file, stream or descriptor).

Signatures (supplied by the plugin registry information) are sought in aSourceHeader.

This function uses the match string as a resolver parameter. The format base class uses the match string
to find a match to the match string defined in the opaque_data field of the resource file. The
match string would typically be a signature for a particular format usually defined in the format header.

@param  aSourceHeader
        The data which is searched for matching signatures.
@param  aSource
        The controller's MDataSource and the source of the data for the format decode plugin.
@param  aPreferredSupplier
        If this is provided the list of matching plugins will be further searched for the latest version of a
        plugin supplied by the specified supplier.

@return If successful returns an instantiated CMMFFormatDecode from a plugin.  If not successful, leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatDecode* CMMFFormatDecode::NewL( const TDesC8& aSourceHeader, MDataSource* aSource, const TDesC& aPreferredSupplier )
	{
	CMMFFormatDecodePluginSelectionParameters* pluginSelector = CMMFFormatDecodePluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParams = CMMFFormatSelectionParameters::NewLC();
	formatParams->SetMatchToHeaderDataL(aSourceHeader);
	pluginSelector->SetRequiredFormatSupportL(*formatParams);
	if (aPreferredSupplier.Length() > 0)
		pluginSelector->SetPreferredSupplierL(aPreferredSupplier, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	// Instantiate this format
	CMMFFormatDecode* theChosenOne = 
		MMFFormatEcomUtilities::SelectFormatDecodePluginL(*pluginSelector, aSource);

	// Now clean up.
	CleanupStack::PopAndDestroy(2);//formatParams, pluginSelector

	return theChosenOne;
	}

/**
Attempts to locate and instantiate a CMMFFormatDecode using data in a buffer.  The buffer is expected to contain 
header data (from a file, stream or descriptor).

Signatures (supplied by the plugin registry information) are sought in aSourceHeader.

This function uses the match string as a resolver parameter. The format base class uses the match string
to find a match to the match string defined in the opaque_data field of the resource file. The
match string would typically be a signature for a particular format usually defined in the format header.

@param  aSourceHeader
        The data which is searched for matching signatures.
@param  aSource
        The controller's MDataSource and the source of the data for the format decode plugin.
@param  aPreferredSupplier
        If this is provided the list of matching plugins will be further searched for the latest version of a
        plugin supplied by the specified supplier.
@param  aSupportsCustomInterfaces
		Indicates whether the instantiated FormatDecode supports custom interfaces.

@return If successful returns an instantiated CMMFFormatDecode from a plugin.  If not successful, leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatDecode* CMMFFormatDecode::NewL( const TDesC8& aSourceHeader, MDataSource* aSource, const TDesC& aPreferredSupplier, TBool& aSupportsCustomInterfaces )
	{
	CMMFFormatDecodePluginSelectionParameters* pluginSelector = CMMFFormatDecodePluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParams = CMMFFormatSelectionParameters::NewLC();
	formatParams->SetMatchToHeaderDataL(aSourceHeader);
	pluginSelector->SetRequiredFormatSupportL(*formatParams);
	if (aPreferredSupplier.Length() > 0)
		pluginSelector->SetPreferredSupplierL(aPreferredSupplier, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	// Instantiate this format
	CMMFFormatDecode* theChosenOne = 
		MMFFormatEcomUtilities::SelectFormatDecodePluginL(*pluginSelector, aSource, aSupportsCustomInterfaces);

	// Now clean up.
	CleanupStack::PopAndDestroy(2);//formatParams, pluginSelector

	return theChosenOne;
	}

/**
Attempts to locate and instantiate a CMMFFormatDecode using data from MDataSource.  

The data is expected to contain header data (from a file, stream or descriptor). Signatures 
(supplied by the plugin registry information) are sought in aSource.

@param  aSource
        Header data. Must be derived from CMMFClip.
@param  aPreferredSupplier
        If this is provided, the list of matching plugins will be further searched for the latest version of a
        plugin supplied by the specified supplier.

@return If successful returns an instantiated CMMFFormatDecode from a plugin. If not successful, leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatDecode* CMMFFormatDecode::NewL( MDataSource* aSource, const TDesC& aPreferredSupplier )
	{
	// Read header data from aSource.  Call source header version

	if ( !( (aSource->DataSourceType() == KUidMmfFileSource ) || ( aSource->DataSourceType() == KUidMmfDescriptorSource) ) )
		User::Leave( KErrNotSupported ) ;

	CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(KMmfHeaderBufferSize) ;
	CleanupStack::PushL( buffer ) ;

	aSource->SourcePrimeL();
	TCleanupItem srcCleanupItem(DoDataSourceStop, aSource);
	CleanupStack::PushL(srcCleanupItem);

	STATIC_CAST( CMMFClip*, aSource )->ReadBufferL( buffer, 0 ) ;

	CleanupStack::Pop();
	aSource->SourceStopL();

	// attempt to instantiate the format by header data
	// if this fails, try using the file extension
	CMMFFormatDecode* ret = NULL;
	TInt err, errFile;
	TRAP(err, ret = NewL( buffer->Data(), aSource, aPreferredSupplier ));
	if (err != KErrNone && aSource->DataSourceType() == KUidMmfFileSource)
		{
		CMMFFile* mmfFile = static_cast<CMMFFile*> (aSource);
		TRAP(errFile, ret = NewL( mmfFile->FullName(), aSource, aPreferredSupplier ));
		if (errFile == KErrNone)
			err = errFile;
		}
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy() ; // buffer
	return ret ;
	
	}

/**
Attempts to locate and instantiate a CMMFFormatDecode using data from MDataSource.  

The data is expected to contain header data (from a file, stream or descriptor). Signatures 
(supplied by the plugin registry information) are sought in aSource.

@param  aSource
        Header data. Must be derived from CMMFClip.
@param  aPreferredSupplier
        If this is provided, the list of matching plugins will be further searched for the latest version of a
        plugin supplied by the specified supplier.
@param  aSupportsCustomInterfaces
		Indicates whether the instantiated FormatDecode supports custom interfaces.

@return If successful returns an instantiated CMMFFormatDecode from a plugin. If not successful, leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatDecode* CMMFFormatDecode::NewL( MDataSource* aSource, const TDesC& aPreferredSupplier, TBool& aSupportsCustomInterfaces )
	{
	// Read header data from aSource.  Call source header version

	if ( !( (aSource->DataSourceType() == KUidMmfFileSource ) || ( aSource->DataSourceType() == KUidMmfDescriptorSource) ) )
		User::Leave( KErrNotSupported ) ;

	CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(KMmfHeaderBufferSize) ;
	CleanupStack::PushL( buffer ) ;

	aSource->SourcePrimeL();
	TCleanupItem srcCleanupItem(DoDataSourceStop, aSource);
	CleanupStack::PushL(srcCleanupItem);

	STATIC_CAST( CMMFClip*, aSource )->ReadBufferL( buffer, 0 ) ;

	CleanupStack::Pop();
	aSource->SourceStopL();

	// attempt to instantiate the format by header data
	// if this fails, try using the file extension
	CMMFFormatDecode* ret = NULL;
	TInt err, errFile;
	TRAP(err, ret = NewL( buffer->Data(), aSource, aPreferredSupplier, aSupportsCustomInterfaces ));
	if (err != KErrNone && aSource->DataSourceType() == KUidMmfFileSource)
		{
		CMMFFile* mmfFile = static_cast<CMMFFile*> (aSource);
		TRAP(errFile, ret = NewL( mmfFile->FullName(), aSource, aPreferredSupplier, aSupportsCustomInterfaces ));
		if (errFile == KErrNone)
			err = errFile;
		}
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy() ; // buffer
	return ret ;
	
	}

CMMFFormatEncode* CMMFFormatEncode::CreateFormatL(TUid aUid, MDataSink* aSink)
	{
	CMMFFormatEncode* s = REINTERPRET_CAST( CMMFFormatEncode*,
									REComSession::CreateImplementationL( aUid, _FOFF( CMMFFormatEncode, iDtor_ID_Key ),
									STATIC_CAST( TAny*, aSink ) ) ) ;
	s->iImplementationUid = aUid;
	return s;
	}

/**
Allocates and constructs an ECom format encode object.

@param  aUid
        The implementation UID.
@param  aSink
        The data sink.

@return If successful, returns the address of the format decode plugin object created. If not successful,
        leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatEncode* CMMFFormatEncode::NewL( TUid aUid, MDataSink* aSink )
	{
	return CreateFormatL(aUid, aSink);
	}

/**
Attempts to locate and instantiate a CMMFFormatEncode using a filename or an extension.

Only the extension of the supplied file name is used. If no extension is supplied (ie. no dot is present)
the whole of the filename will be treated as the extension.

@param  aFileName
        File name of target file. May be extension only or may include the full path.
@param  aSink
		The data source.
@param  aPreferredSupplier
        If this is provided, the list of matching plugins will be further searched for the latest version of a
        plugin supplied by supplier named.

@return If successful, returns the address of the format decode plugin object created. If not successful,
        leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatEncode* CMMFFormatEncode::NewL( const TDesC16& aFileName, MDataSink* aSink, const TDesC& aPreferredSupplier )
	{
	CMMFFormatEncodePluginSelectionParameters* pluginSelector = CMMFFormatEncodePluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParams = CMMFFormatSelectionParameters::NewLC();
	formatParams->SetMatchToFileNameL(aFileName);
	pluginSelector->SetRequiredFormatSupportL(*formatParams);
	if (aPreferredSupplier.Length() > 0)
		pluginSelector->SetPreferredSupplierL(aPreferredSupplier, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	TUid chosenUid = MMFFormatEcomUtilities::SelectFormatPluginL(*pluginSelector);

	// Instantiate this format
	CMMFFormatEncode* theChosenOne = CreateFormatL(chosenUid, aSink);

	// Now clean up.
	CleanupStack::PopAndDestroy(2);//formatParams, pluginSelector

	return theChosenOne;
	}

/**
Attempts to locate and instantiate a CMMFFormatEncode using data in the specified buffer.

The buffer is expected to contain header data (from a file, stream or descriptor).
Signatures (supplied by the plugin registry information) are sought in aSourceHeader.

@param  aSourceHeader
        The data which is searched for matching signatures.
@param  aSink
        The data sink.
@param  aPreferredSupplier
        If this is provided the list of matching plugins will be further searched for the latest version of a
        plugin supplied by supplier named.

@return If successful, returns the address of the format decode plugin object created. If not successful,
        leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatEncode* CMMFFormatEncode::NewL( const TDesC8& aSourceHeader, MDataSink* aSink,  const TDesC& aPreferredSupplier )
	{
	CMMFFormatEncodePluginSelectionParameters* pluginSelector = CMMFFormatEncodePluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* formatParams = CMMFFormatSelectionParameters::NewLC();
	formatParams->SetMatchToHeaderDataL(aSourceHeader);
	pluginSelector->SetRequiredFormatSupportL(*formatParams);
	if (aPreferredSupplier.Length() > 0)
		pluginSelector->SetPreferredSupplierL(aPreferredSupplier, CMMFPluginSelectionParameters::EOnlyPreferredSupplierPluginsReturned);

	TUid chosenUid = MMFFormatEcomUtilities::SelectFormatPluginL(*pluginSelector);

	// Instantiate this format
	CMMFFormatEncode* theChosenOne = CreateFormatL(chosenUid, aSink);

	// Now clean up.
	CleanupStack::PopAndDestroy(2);//formatParams, pluginSelector

	return theChosenOne;
	}

/**
Attempts to locate and instantiate a CMMFFormatEncode using data from aSink.

The data is expected to contain header data (from a file, stream or descriptor).
Signatures (supplied by the plugin registry information) are sought in the source header.

@param  aSink
        The header data. Must be derived from CMMFClip.
@param  aPreferredSupplier
        If this is provided, the list of matching plugins will be further searched for the latest version of a
        plugin supplied by supplier specified.

@return If successful, returns the address of the format decode plugin object created. If not successful,
        leaves with KErrNotFound.
*/
EXPORT_C CMMFFormatEncode* CMMFFormatEncode::NewL( MDataSink* aSink, const TDesC& aPreferredSupplier )
	{
	// Read header data from aSource.  Call source header version if there is header data, file name version otherwise.

	if ( !( (aSink->DataSinkType() == KUidMmfFileSink ) || ( aSink->DataSinkType() == KUidMmfDescriptorSink) ) )
		User::Leave( KErrNotSupported ) ;

	CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(KMmfHeaderBufferSize) ;
	CleanupStack::PushL( buffer ) ;

	aSink->SinkPrimeL();
	TCleanupItem sinkCleanupItem(DoDataSinkStop, aSink);
	CleanupStack::PushL(sinkCleanupItem);

	STATIC_CAST( CMMFClip*, aSink )->ReadBufferL( buffer, 0 ) ;

	CleanupStack::Pop();
	aSink->SinkStopL();

	CMMFFormatEncode* ret = NULL ;  // set to null to avoid compiler warning.
	// Check for data in the buffer
	if (  buffer->BufferSize() != 0 )
		ret = NewL( buffer->Data(), aSink, aPreferredSupplier ) ;
	else if ( aSink->DataSinkType() == KUidMmfFileSink )
		ret = NewL( STATIC_CAST(CMMFFile*, aSink)->Extension(), aSink, aPreferredSupplier ) ;
	else
		User::Leave( KErrNotSupported ) ;

	CleanupStack::PopAndDestroy() ; // buffer
	return ret ;
	}


TUid MMFFormatEcomUtilities::SelectFormatPluginL(const CMMFFormatPluginSelectionParameters& aSelectParams)
	{
	RMMFFormatImplInfoArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);
	aSelectParams.ListImplementationsL(pluginArray);

	// Just leave if no implementations were found
	if (pluginArray.Count() == 0)
		User::Leave(KErrNotSupported);

	// Return the uid of the first plugin in the list
	TUid ret = pluginArray[0]->Uid();
	CleanupStack::PopAndDestroy();//pluginArray
	return ret;
	}

/*
 * instantiate each format decode plugin in turn until we find one that works
 */
CMMFFormatDecode* MMFFormatEcomUtilities::SelectFormatDecodePluginL(const CMMFFormatPluginSelectionParameters& aSelectParams, MDataSource* aSource)
	{
	RMMFFormatImplInfoArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);
	aSelectParams.ListImplementationsL(pluginArray);

	TInt pluginCount = pluginArray.Count();
	CMMFFormatDecode* theChosenOne = NULL;

	TInt err = KErrNotSupported;
	for (TInt n=0; n<pluginCount; n++)
		{
		// Try to instantiate this format
		TRAP(err, theChosenOne = CMMFFormatDecode::NewL(pluginArray[n]->Uid(), aSource));
		// Ensure OOM or any unexpected error is caught immediately 
		// i.e. don't try the next plugin
		if (err != KErrNotSupported && err != KErrCorrupt && err != KErrArgument)
			break;
		}
	User::LeaveIfError(err);

	CleanupStack::PopAndDestroy(&pluginArray);

	return theChosenOne;
	}
	
/*
 * instantiate each format decode plugin in turn until we find one that works
 */
CMMFFormatDecode* MMFFormatEcomUtilities::SelectFormatDecodePluginL(const CMMFFormatPluginSelectionParameters& aSelectParams, MDataSource* aSource, TBool& aSupportsCustomInterfaces)
	{
	RMMFFormatImplInfoArray pluginArray;
	CleanupResetAndDestroyPushL(pluginArray);
	aSelectParams.ListImplementationsL(pluginArray);

	TInt pluginCount = pluginArray.Count();
	CMMFFormatDecode* theChosenOne = NULL;

	TInt err = KErrNotSupported;
	CMMFFormatImplementationInformation* implInfo = NULL;
	for (TInt n=0; n<pluginCount; n++)
		{
		implInfo = pluginArray[n];
		// Try to instantiate this format
		TRAP(err, theChosenOne = CMMFFormatDecode::NewL(implInfo->Uid(), aSource));
		// Ensure OOM or any unexpected error is caught immediately 
		// i.e. don't try the next plugin
		if (err != KErrNotSupported && err != KErrCorrupt && err != KErrArgument)
			break;
		}
	User::LeaveIfError(err);
	aSupportsCustomInterfaces = implInfo->SupportsCustomInterfaces();

	CleanupStack::PopAndDestroy(&pluginArray);

	return theChosenOne;
	}

