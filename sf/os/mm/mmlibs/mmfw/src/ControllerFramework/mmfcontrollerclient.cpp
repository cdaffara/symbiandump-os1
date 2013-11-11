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

#include "mmfcontroller.h"
#include "mmfcontrollerpluginresolver.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfcontrollerextendeddata.h>
#include <mmf/common/mmfcustomcommandparsermanager.h>
#endif

/**
Constructor.

@since 7.0s
*/
EXPORT_C RMMFController::RMMFController() :
	iDestinationPckg(TMMFMessageDestination(KUidInterfaceMMFController, KMMFObjectHandleController))
	{
	}

/**
Loads a controller plugin by specifying the UID of the plugin to load.

Creates a new thread and loads the specified controller into the new thread.  Use the classes
CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters
to find the uid of the controller to load.

@param  aControllerUid
        The UID of the controller plugin to be loaded.
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.
@param	aUseSharedHeap
		If this value is EFalse each controller is created with its own heap. The alternative,
		if the value is ETrue, is that controllers share a special heap with other controllers
		created the same way. Each heap uses a chunk, so this avoids situations where the
		number of chunks per process is limited. The default behaviour is generally to be
		preferred, and should give lower overall memory usage. However, if many controllers are
		to be created for a particular thread, then ETrue should be provided to prevent running
		out of heaps or chunks.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see    CMMFControllerPluginSelectionParamters
@since  7.0s

*/
EXPORT_C TInt RMMFController::Open(TUid aControllerUid, const TMMFPrioritySettings& aPrioritySettings, TBool aUseSharedHeap)
	{
	// Load Controller
	TInt err = iControllerProxy.LoadController(aControllerUid, aUseSharedHeap);
	if (!err)
		{
		err = SetPrioritySettings(aPrioritySettings);
		}
	return err;
	}

/**
Loads a controller plugin by specifying a CMMFControllerImplementationInformation object.

CMMFControllerImplementationInformation is passed as a parameter to allow
the controller thread's heap size to be determined without a further query to ECOM.

The function creates a new thread and loads the specified controller into the new thread.  The classes
CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters are used
to find the UID of the controller to load.

@param  aControllerInfo
        A reference to a CMMFControllerImplementationInformation object
        used for determining the controller's UID & heap size
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.
@param	aUseSharedHeap
		If this value is EFalse each controller is created with its own heap. The alternative,
		if the value is ETrue, is that controllers share a special heap with other controllers
		created the same way. Each heap uses a chunk, so this avoids situations where the
		number of chunks per process is limited. The default behaviour is generally to be
		preferred, and should give lower overall memory usage. However, if many controllers are
		to be created for a particular thread, then ETrue should be provided to prevent running
		out of heaps or chunks.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see CMMFControllerPluginSelectionParamters
@since 7.0s

*/
EXPORT_C TInt RMMFController::Open(
	const CMMFControllerImplementationInformation& aControllerInfo,
	const TMMFPrioritySettings& aPrioritySettings,
	TBool aUseSharedHeap)
	{
	// Load Controller
	TInt err = iControllerProxy.LoadController(aControllerInfo, aUseSharedHeap);
	if (!err)
		err = SetPrioritySettings(aPrioritySettings);
	return err;
	}

/**
Loads a controller plugin by specifying the UID of the plugin to load.

Creates a new thread and loads the specified controller into the new thread.  Use the classes
CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters
to find the uid of the controller to load.
This version of Open() will give each controller its own heap which will consume one chunk. Use the
overloaded version of Open() to create controllers that share a single heap.

@param  aControllerUid
        The UID of the controller plugin to be loaded.
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see    CMMFControllerPluginSelectionParamters
@since  7.0s

*/
EXPORT_C TInt RMMFController::Open(TUid aControllerUid, const TMMFPrioritySettings& aPrioritySettings)
	{
	// Load Controller
	return Open( aControllerUid, aPrioritySettings, EFalse ); // give controllers their own heaps
	}

/**
Loads a controller plugin by specifying a CMMFControllerImplementationInformation object.

CMMFControllerImplementationInformation is passed as a parameter to allow
the controller thread's heap size to be determined without a further query to ECOM.

The function creates a new thread and loads the specified controller into the new thread.  The classes
CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters are used
to find the UID of the controller to load.
This version of Open() will give each controller its own heap which will consume one chunk. Use the
overloaded version of Open() to create controllers that share a single heap.

@param  aControllerInfo
        A reference to a CMMFControllerImplementationInformation object
        used for determining the controller's UID & heap size
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see CMMFControllerPluginSelectionParamters
@since 7.0s

*/
EXPORT_C TInt RMMFController::Open(
		const CMMFControllerImplementationInformation& aControllerInfo,
		const TMMFPrioritySettings& aPrioritySettings)
	{
		// Load Controller
		return Open( aControllerInfo, aPrioritySettings, EFalse ); // give controllers their own heaps
	}

/**
Same as RMMFController::Open(TUid, const TMMFPrioritySettings&) which loads a controller plugin by 
specifying the UID of the plugin to load, except that this is expected to be called by user thread 
without DRM Capability.

Creates a new thread through DRM Plugin Server and loads the specified controller into the new thread.  
Use the classes CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters
to find the uid of the controller to load.
This version of Open() will give each controller its own heap which will consume one chunk. Use the
overloaded version of Open() to create controllers that share a single heap.

@param  aControllerUid
        The UID of the controller plugin to be loaded.
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see    CMMFControllerPluginSelectionParamters

*/
EXPORT_C TInt RMMFController::OpenInSecureDRMProcess(TUid aControllerUid, const TMMFPrioritySettings& aPrioritySettings)
	{
	return OpenInSecureDRMProcess(aControllerUid, aPrioritySettings, EFalse);
	}

/**
Same as Open(const CMMFControllerImplementationInformation&, const TMMFPrioritySettings&) which loads a 
controller plugin by specifying a CMMFControllerImplementationInformation object.

CMMFControllerImplementationInformation is passed as a parameter to allow
the controller thread's heap size to be determined without a further query to ECOM.

The function creates a new thread through DRM Plugin server and loads the specified controller into 
the new thread.  The classes CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters are used
to find the UID of the controller to load.
This version of Open() will give each controller its own heap which will consume one chunk. Use the
overloaded version of Open() to create controllers that share a single heap.

@param  aControllerInfo
        A reference to a CMMFControllerImplementationInformation object
        used for determining the controller's UID & heap size
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see CMMFControllerPluginSelectionParamters

*/	
EXPORT_C TInt RMMFController::OpenInSecureDRMProcess(const CMMFControllerImplementationInformation& aControllerInfo, const TMMFPrioritySettings& aPrioritySettings)
	{
	return OpenInSecureDRMProcess(aControllerInfo, aPrioritySettings, EFalse);
	}

/**
Same as RMMFController::Open(TUid, const TMMFPrioritySettings&, TBool) which loads a controller 
plugin by specifying the UID of the plugin to load, except that this is expected to be called by user thread 
without DRM Capability.

Creates a new thread through DRM Plugin server and loads the specified controller into the new thread.  
Use the classes CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters
to find the uid of the controller to load.

@param  aControllerUid
        The UID of the controller plugin to be loaded.
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.
@param	aUseSharedHeap
		If this value is EFalse each controller is created with its own heap. The alternative,
		if the value is ETrue, is that controllers share a special heap with other controllers
		created the same way. Each heap uses a chunk, so this avoids situations where the
		number of chunks per process is limited. The default behaviour is generally to be
		preferred, and should give lower overall memory usage. However, if many controllers are
		to be created for a particular thread, then ETrue should be provided to prevent running
		out of heaps or chunks.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see    CMMFControllerPluginSelectionParamters

*/
EXPORT_C TInt RMMFController::OpenInSecureDRMProcess(TUid aControllerUid, const TMMFPrioritySettings& aPrioritySettings, TBool aUseSharedHeap )
	{
	// Load Controller
	TInt err = iControllerProxy.LoadControllerInSecureDRMProcess(aControllerUid, aUseSharedHeap);
	if (!err)
		{
		err = SetPrioritySettings(aPrioritySettings);
		}
	return err;
	}

/**
Same as Open(const CMMFControllerImplementationInformation&, const TMMFPrioritySettings&, TBool) which 
loads a controller plugin by specifying a CMMFControllerImplementationInformation object.

CMMFControllerImplementationInformation is passed as a parameter to allow
the controller thread's heap size to be determined without a further query to ECOM.

The function creates a new thread through DRM Plugin Server and loads the specified controller into 
the new thread.  The classes CMMFControllerPluginSelectionParameters and CMMFFormatSelectionParamters are used
to find the UID of the controller to load.

@param  aControllerInfo
        A reference to a CMMFControllerImplementationInformation object
        used for determining the controller's UID & heap size
@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.
@param	aUseSharedHeap
		If this value is EFalse each controller is created with its own heap. The alternative,
		if the value is ETrue, is that controllers share a special heap with other controllers
		created the same way. Each heap uses a chunk, so this avoids situations where the
		number of chunks per process is limited. The default behaviour is generally to be
		preferred, and should give lower overall memory usage. However, if many controllers are
		to be created for a particular thread, then ETrue should be provided to prevent running
		out of heaps or chunks.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.

@see CMMFControllerPluginSelectionParamters

*/
EXPORT_C TInt RMMFController::OpenInSecureDRMProcess(const CMMFControllerImplementationInformation& aControllerInfo, const TMMFPrioritySettings& aPrioritySettings, TBool aUseSharedHeap )
	{
	// Load Controller
	TInt err = iControllerProxy.LoadControllerInSecureDRMProcess(aControllerInfo, aUseSharedHeap);
	if (!err)
		err = SetPrioritySettings(aPrioritySettings);
	return err;
	
	}
	
/**
Closes the controller plugin.

Calling this method will unload the controller plugin and close down the controller thread.

@since	7.0s
*/
EXPORT_C void RMMFController::Close()
	{
	iControllerProxy.Close();
	}

/**
Sets the priority settings for this controller.

@param  aPrioritySettings
        The priority settings for this plugin, used by the policy
        component to arbitrate between different controllers that are
        attempting to use the same hardware resource simultaneously.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since	7.0s

*/
EXPORT_C TInt RMMFController::SetPrioritySettings(const TMMFPrioritySettings& aPrioritySettings) const
	{
	TMMFPrioritySettingsPckg pckg(aPrioritySettings);
	return iControllerProxy.SendSync(iDestinationPckg, 
									 EMMFControllerSetPrioritySettings, 
									 pckg, 
									 KNullDesC8);
	}

/**
Adds a data source to the controller.

A typical data source would be a file, descriptor, audio input (microphone) or camera.
A controller plugin may require multiple data sources to be added (for example a video
recorder controller would require two); the exact number is plugin-specific.
Data sources are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param  aSourceUid
        The uid of the data source to be added. For more information,
        see the documentation for the data source you wish to add.
@param  aSourceInitData
        The data used to initialise the data source.  The exact contents
        of this data are dependent on the type of data source. For more
        information, see the documentation for the data source you wish
        to add.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since	7.0s
*/
EXPORT_C TInt RMMFController::AddDataSource(TUid aSourceUid, const TDesC8& aSourceInitData)
	{
	TMMFMessageDestination handleInfo;
	return AddDataSource(aSourceUid, aSourceInitData, handleInfo);
	}

/**
Adds a data sink to the controller.

A typical data sink would be a file, descriptor, audio output (speaker) or display.
A controller plugin may require multiple data sinks to be added (for example a video
playing controller would require two); the exact number is plugin-specific.
Data sinks are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param  aSinkUid
        The UID of the data sink to be added. For more information,
        see the documentation for the data sink you wish to add.
@param  aSinkInitData
        The data used to initialise the data sink.  The exact contents
        of this data are dependent on the type of data sink. For more
        information, see the documentation for the data sink you wish
        to add.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::AddDataSink(TUid aSinkUid, const TDesC8& aSinkInitData)
	{
	TMMFMessageDestination handleInfo;
	return AddDataSink(aSinkUid, aSinkInitData, handleInfo);
	}

/**
Adds a data source to the controller, and receive a handle to allow removal and direct
communication with that data source.

A typical data source would be a file, descriptor, audio input (microphone) or camera.
A controller plugin may require multiple data sources to be added (for example a video
recorder controller would require two); the exact number is plugin-specific.
Data sources are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param  aSourceUid
        The uid of the data source to be added. For more information,
        see the documentation for the data source you wish to add.
@param  aSourceInitData
        Data used to initialise the data source.  The exact contents
        of this data is dependent on the type of data source. For more
        information, see the documentation for the data source you wish
        to add.
@param  aHandleInfo
        This object is filled in by the controller framework, and identifies
        the data source inside the controller framework.  This allows
        the caller to send custom commands directly to the data source, and
        to also remove the data source from the controller.  Note that
        not all data sources support custom commands, and not all
        controller plugins support the removal of a data source.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::AddDataSource(TUid aSourceUid, const TDesC8& aSourceInitData,
											TMMFMessageDestination& aHandleInfo)
	{
	TMMFMessageDestinationPckg pckg;
	TMMFUidPckg uidPckg(aSourceUid);
	TInt error = iControllerProxy.SendSync(iDestinationPckg,
										   EMMFControllerAddDataSource,
										   uidPckg,
										   aSourceInitData,
										   pckg);
	if (!error)
		{
		aHandleInfo = pckg();
		}
	return error;
	}

/**
Add a file handle data source, together with its source info, to the controller asynchronously, 
and receive a handle to allow removal and directcommunication with that data source.

Note: only one call to this method can be outstanding at any time.

The data source would be a file for this API.
A controller plugin may require multiple data sources to be added (for example a video
recorder controller would require two); the exact number is plugin-specific.
Data sources are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param "aFile"				"The shared session file handle source to be added."
@param "aSourceInitData"	"Data used to initialise the data source.  The reference must remain valid 
							until the command has been completed or cancelled. The exact contents
							of this data is dependent on the type of data source. For more
							information, see the documentation for the data source you wish
							to add."
@param "aHandleInfoPckg"	"This object is filled in by the controller framework, and identifies
							the data source inside the controller framework.  This allows
							the caller to send custom commands directly to the data source, and
							to also remove the data source from the controller.  Note that
							not all data sources support custom commands, and not all
							controller plugins support the removal of a data source."
@param "aStatus"			"TRequestStatus of the active object to be signalled on completion
							of this request."

@return An error code indicating if the function call was successful. 
		KErrBadHandle if the file handle is not shared through the call RFs::ShareProtected().
		KErrNone on success, otherwise another of the system-wide error codes.
*/
EXPORT_C void RMMFController::AddFileHandleDataSource(const RFile& aFile, const TDesC8& aSourceInitData, 
													  TMMFMessageDestinationPckg& aHandleInfoPckg, 
													  TRequestStatus& aStatus)
	{
	TInt error = iControllerProxy.SendSync(iDestinationPckg,
										   EMMFControllerSourceSinkInitDataPreload,
										   aSourceInitData,
										   KNullDesC8);
	TIpcArgs ipcArgs(&iDestinationPckg, NULL, NULL, &aHandleInfoPckg);
	if (error == KErrNone)
		{
		error = aFile.TransferToServer(ipcArgs, 1, 2);
		}
		
	if (error == KErrNone)
		{
		iControllerProxy.SendAsync(EMMFControllerAddFileHandleDataSourceWithInitData,
								   ipcArgs,
								   aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, error);
		}
	}
	
/**
Add a file handle data source to the controller asynchronously, and receive a handle to allow 
removal and directcommunication with that data source.

Note: only one call to this method can be outstanding at any time.

The data source would be a file for this API.
A controller plugin may require multiple data sources to be added (for example a video
recorder controller would require two); the exact number is plugin-specific.
Data sources are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param "aFile"				"The shared session file handle source to be added."
@param "aHandleInfoPckg"	"This object is filled in by the controller framework, and identifies
							the data source inside the controller framework.  This allows
							the caller to send custom commands directly to the data source, and
							to also remove the data source from the controller.  Note that
							not all data sources support custom commands, and not all
							controller plugins support the removal of a data source."
@param "aStatus"			"TRequestStatus of the active object to be signalled on completion
							of this request."

@return An error code indicating if the function call was successful. 
		KErrBadHandle if the file handle is not shared through the call RFs::ShareProtected().
		KErrNone on success, otherwise another of the system-wide error codes.
*/
EXPORT_C void RMMFController::AddFileHandleDataSource(const RFile& aFile,
											TMMFMessageDestinationPckg& aHandleInfo, TRequestStatus& aStatus)
	{
	TIpcArgs ipcArgs(&iDestinationPckg, NULL, NULL, &aHandleInfo);
	TInt error = aFile.TransferToServer(ipcArgs, 1, 2);
	
	if (!error)
		{
		iControllerProxy.SendAsync(EMMFControllerAddFileHandleDataSource,
											   ipcArgs,
											   aStatus);		
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, error);
		}

	}

/**
Adds a file handle data sink, together with its source info, to the controller, and receives 
a handle to allow removal and direct communication with that data sink.

Note: only one call to this method can be outstanding at any time.

The data sink would be a file for this API.
A controller plugin may require multiple data sinks to be added (for example a video
playing controller would require two); the exact number is plugin-specific.
Data sinks are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param  aFile
        The shared session file handle sink to be added.
@param  aSinkInitData
        Data used to initialise the data sink.  The exact contents
        of this data are dependent on the type of data sink. For more
        information, see the documentation for the data sink you wish
        to add.
@param  aHandleInfo
        This object is filled in by the controller framework, and identifies
        the data sink inside the controller framework.  This allows
        the caller to send custom commands directly to the data sink, and
        to also remove the data sink from the controller.  Note that
        not all data sinks support custom commands, and not all
        controller plugins support the removal of a data sink.
@param 	aStatus
		TRequestStatus of the active object to be signalled on completion
		of this request.

@return An error code indicating if the function call was successful. 
		KErrBadHandle if the file handle is not shared through the call RFs::ShareProtected().
		KErrNone on success, otherwise another of the system-wide error codes.
*/
EXPORT_C void RMMFController::AddFileHandleDataSink(const RFile& aFile, const TDesC8& aSinkInitData, 
													TMMFMessageDestinationPckg& aHandleInfoPckg, 
													TRequestStatus& aStatus)
	{
	TInt error = iControllerProxy.SendSync(iDestinationPckg,
										   EMMFControllerSourceSinkInitDataPreload,
										   aSinkInitData,
										   KNullDesC8);
	TIpcArgs ipcArgs(&iDestinationPckg, NULL, NULL, &aHandleInfoPckg);
	if (!error)
		{
		error = aFile.TransferToServer(ipcArgs, 1, 2);
		}
 	
	if (!error)
		{
		iControllerProxy.SendAsync(EMMFControllerAddFileHandleDataSinkWithInitData,
								   ipcArgs,
								   aStatus);
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, error);
		}
	}

/**
Adds a file handle data sink, together with its source info, to the controller, and receives 
a handle to allow removal and direct communication with that data sink.

Note: only one call to this method can be outstanding at any time.

The data sink would be a file for this API.
A controller plugin may require multiple data sinks to be added (for example a video
playing controller would require two); the exact number is plugin-specific.
Data sinks are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param  aFile
        The shared session file handle sink to be added.
@param  aHandleInfo
        This object is filled in by the controller framework, and identifies
        the data sink inside the controller framework.  This allows
        the caller to send custom commands directly to the data sink, and
        to also remove the data sink from the controller.  Note that
        not all data sinks support custom commands, and not all
        controller plugins support the removal of a data sink.
@param 	aStatus
		TRequestStatus of the active object to be signalled on completion
		of this request.

@return An error code indicating if the function call was successful. 
		KErrBadHandle if the file handle is not shared through the call RFs::ShareProtected().
		KErrNone on success, otherwise another of the system-wide error codes.
*/
EXPORT_C void RMMFController::AddFileHandleDataSink(const RFile& aFile,
											TMMFMessageDestinationPckg& aHandleInfo, TRequestStatus& aStatus)
	{
	TIpcArgs ipcArgs(&iDestinationPckg, NULL, NULL, &aHandleInfo);
	TInt error = aFile.TransferToServer(ipcArgs, 1, 2);
	
	if (!error)
		{
		iControllerProxy.SendAsync(EMMFControllerAddFileHandleDataSink,
											   ipcArgs,
											   aStatus);		
		}
	else
		{
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, error);
		}

	}


/**
Adds a data sink to the controller, and receives a handle to allow removal and direct
communication with that data sink.

A typical data sink would be a file, descriptor, audio output (speaker) or display.
A controller plugin may require multiple data sinks to be added (for example a video
playing controller would require two); the exact number is plugin-specific.
Data sinks are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param  aSinkUid
        The UID of the data sink to be added. For more information,
        see the documentation for the data sink you wish to add.
@param  aSinkInitData
        Data used to initialise the data sink.  The exact contents
        of this data are dependent on the type of data sink. For more
        information, see the documentation for the data sink you wish
        to add.
@param  aHandleInfo
        This object is filled in by the controller framework, and identifies
        the data sink inside the controller framework.  This allows
        the caller to send custom commands directly to the data sink, and
        to also remove the data sink from the controller.  Note that
        not all data sinks support custom commands, and not all
        controller plugins support the removal of a data sink.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::AddDataSink(TUid aSinkUid, const TDesC8& aSinkInitData,
										  TMMFMessageDestination& aHandleInfo)
	{
	TMMFMessageDestinationPckg pckg;
	TMMFUidPckg uidPckg(aSinkUid);
	TInt error = iControllerProxy.SendSync(iDestinationPckg,
										   EMMFControllerAddDataSink,
										   uidPckg,
										   aSinkInitData,
										   pckg);
	if (!error)
		{
		aHandleInfo = pckg();
		}
	return error;
	}


/**
Add a data source to the controller asynchronously, and receive a handle to allow removal and direct
communication with that data source.

Note: only one call to this method can be outstanding at any time.

A typical data source would be a file, descriptor, audio input (microphone) or camera.
A controller plugin may require multiple data sources to be added (for example a video
recorder controller would require two); the exact number is plugin-specific.
Data sources are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param "aSourceUid"			"The uid of the data source to be added, packaged for async data transfer. 
							The reference must remain valid until the command has been completed or 
							cancelled."
@param "aSourceInitData"	"Data used to initialise the data source.  The reference must remain valid 
							until the command has been completed or cancelled. The exact contents
							of this data is dependent on the type of data source. For more
							information, see the documentation for the data source you wish
							to add."
@param "aHandleInfoPckg"	"This object is filled in by the controller framework, and identifies
							the data source inside the controller framework.  This allows
							the caller to send custom commands directly to the data source, and
							to also remove the data source from the controller.  Note that
							not all data sources support custom commands, and not all
							controller plugins support the removal of a data source."
@param "aStatus"			"TRequestStatus of the active object to be signalled on completion
							of this request."
@since	8.0
*/
EXPORT_C void RMMFController::AddDataSource(const TMMFUidPckg& aSourceUid, const TDesC8& aSourceInitData, 
											TMMFMessageDestinationPckg& aHandleInfoPckg, 
											TRequestStatus& aStatus)
	{
	iControllerProxy.SendAsync(iDestinationPckg,
							   EMMFControllerAddDataSource,
							   aSourceUid,
							   aSourceInitData,
							   aHandleInfoPckg,
							   aStatus);
	}

/**
Cancels an outstanding call to the asynchronous version of AddDataSource().
@since	8.0
*/
EXPORT_C void RMMFController::CancelAddDataSource()
	{
	iControllerProxy.SendSync(iDestinationPckg,
							  EMMFControllerCancelAddDataSource,
							  KNullDesC8,
							  KNullDesC8);
	}

/**
Add a data sink to the controller asynchronously, and receive a handle to allow removal and direct
communication with that data sink.

A typical data sink would be a file, descriptor, audio output (speaker) or display.
A controller plugin may require multiple data sinks to be added (for example a video
playing controller would require two); the exact number is plugin-specific.
Data sinks are plugins themselves, and are loaded by the controller framework
inside the controller thread.

@param "aSinkUid"			"The uid of the data sink to be added, packaged for async data transfer. 
							The reference must remain valid until the command has been completed or 
							cancelled."
@param "aSinkInitData"		"Data used to initialise the data sink.  The reference must remain valid 
							until the command has been completed or cancelled. The exact contents
							of this data are dependent on the type of data sink. For more
							information, see the documentation for the data sink you wish
							to add."
@param "aHandleInfoPckg"	"This object is filled in by the controller framework, and identifies
							the data sink inside the controller framework.  This allows
							the caller to send custom commands directly to the data sink, and
							to also remove the data sink from the controller.  Note that
							not all data sinks support custom commands, and not all
							controller plugins support the removal of a data sink."
@param "aStatus"			"TRequestStatus of the active object to be signalled on completion
							of this request."
@since	8.0
*/
EXPORT_C void RMMFController::AddDataSink(const TMMFUidPckg& aSinkUid, const TDesC8& aSinkInitData, 
										  TMMFMessageDestinationPckg& aHandleInfoPckg, 
										  TRequestStatus& aStatus)
	{
	iControllerProxy.SendAsync(iDestinationPckg,
							   EMMFControllerAddDataSink,
							   aSinkUid,
							   aSinkInitData,
							   aHandleInfoPckg,
							   aStatus);
	}


/**
Cancels an outstanding call to the asynchronous version of AddDataSink().
@since	8.0
*/
EXPORT_C void RMMFController::CancelAddDataSink()
	{
	iControllerProxy.SendSync(iDestinationPckg,
							  EMMFControllerCancelAddDataSink,
							  KNullDesC8,
							  KNullDesC8);
	}

/**
Removes a data source from the controller.

In certain situations, it may be necessary to remove a data source from a controller, for
example when you need to play a different file of the same format as the current one.

It should be noted that not all controller plugins will support the removal of a data source.

@param  aHandleInfo
        The handle object returned by the controller framework when the
        data source was added.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::RemoveDataSource(const TMMFMessageDestination& aHandleInfo)
	{
	TMMFMessageDestinationPckg pckg(aHandleInfo);
	TInt error = iControllerProxy.SendSync(iDestinationPckg,
										   EMMFControllerRemoveDataSource,
										   pckg,
										   KNullDesC8);
	return error;
	}

/**
Removes a data sink from the controller.

In certain situations, it may be necessary to remove a data sink from a controller, for
example when you need change output devices on the fly.

It should be noted that not all controller plugins will support the removal of a data sink.

@param  aHandleInfo
        The handle object returned by the controller framework when the
        data sink was added.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::RemoveDataSink(const TMMFMessageDestination& aHandleInfo)
	{
	TMMFMessageDestinationPckg pckg(aHandleInfo);
	TInt error = iControllerProxy.SendSync(iDestinationPckg,
										   EMMFControllerRemoveDataSink,
										   pckg,
										   KNullDesC8);
	return error;
	}

/**
Reverts the controller plugin back to the state it was in just after it had been Opened.

Note: All sources and sinks will be removed from the controller.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::Reset()
	{
	return iControllerProxy.SendSync(iDestinationPckg,
									 EMMFControllerReset,
									 KNullDesC8,
									 KNullDesC8);
	}

/**
Prepares the controller to start playing.

The controller should initialise its sources, sinks and buffers. This moves the controller
from the STOPPED to the PRIMED state.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::Prime()
	{
	return iControllerProxy.SendSync(iDestinationPckg,
									 EMMFControllerPrime,
									 KNullDesC8,
									 KNullDesC8);
	}

/**
Starts the controller playing.
The controller will begin transferring data from its data source(s) to its data sink(s).

This moves the controller from the PRIMED to the PLAYING state.

Play() means "Start Playing" - i.e. this method will return as soon as
playback has begun.

If the data transfer comes to an end due to an internal event (e.g. source runs out of data),
the caller will be notified via the ReceiveEvents() interface.

Note: 
Prime() must have been called prior to calling Play().

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::Play()
	{
	return iControllerProxy.SendSync(iDestinationPckg,
									 EMMFControllerPlay,
									 KNullDesC8,
									 KNullDesC8);
	}

/**
Pauses the controller.

The controller will cease transferring data from its data source(s) to its data sink(s).
A subsequent call to Play() will result in the data transfer resuming from the
same place as when the Pause() was called.

This moves the controller from the PLAYING back to the PRIMED state.

Note: 
Play() must have been called prior to calling Pause().

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::Pause()
	{
	return iControllerProxy.SendSync(iDestinationPckg,
									 EMMFControllerPause,
									 KNullDesC8,
									 KNullDesC8);
	}

/**
Stops the controller.

The controller will cease transferring data from its data source(s) to its data sink(s), reset
its position and delete any allocated buffers.
In effect, calling Stop() undoes any actions performed by the controller
during the call to Prime().

This moves the controller from the PRIMED back to the STOPPED state.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since	7.0s
*/
EXPORT_C TInt RMMFController::Stop()
	{
	return iControllerProxy.SendSync(iDestinationPckg,
									 EMMFControllerStop,
									 KNullDesC8,
									 KNullDesC8);
	}
	
/**
Gets the current position microseconds.

Note: The controller must be in the PRIMED or PLAYING state before this can be called.

@param  aPosition
        The current position in microseconds, filled in by the controller framework.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since	7.0s
*/
EXPORT_C TInt RMMFController::GetPosition(TTimeIntervalMicroSeconds& aPosition) const
	{
	TMMFTimeIntervalMicroSecondsPckg pckg;
	TInt err = iControllerProxy.SendSync(iDestinationPckg,
										 EMMFControllerGetPosition,
										 KNullDesC8,
										 KNullDesC8,
										 pckg);
	if (!err)
		aPosition = pckg();
	return err;
	}

/**
Sets the current position microseconds.

Note: The controller must be in the PRIMED or PLAYING state before this can be called.

@param  aPosition
        The new transfer position in microseconds.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::SetPosition(const TTimeIntervalMicroSeconds& aPosition) const
	{
	TMMFTimeIntervalMicroSecondsPckg pckg(aPosition);
	return iControllerProxy.SendSync(iDestinationPckg,
									 EMMFControllerSetPosition,
									 pckg,
									 KNullDesC8);
	}

/**
Gets the duration of the clip in microseconds.

Note: The controller must be in the PRIMED or PLAYING state before this can be called.

@param  aDuration
        The duration of the clip in microseconds, filled in by the
        controller framework.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since	7.0s
*/
EXPORT_C TInt RMMFController::GetDuration(TTimeIntervalMicroSeconds& aDuration) const
	{
	TMMFTimeIntervalMicroSecondsPckg pckg;
	TInt err = iControllerProxy.SendSync(iDestinationPckg,
										 EMMFControllerGetDuration,
										 KNullDesC8,
										 KNullDesC8,
										 pckg);
	if (!err)
		aDuration = pckg();
	return err;
	}

/**
Sends a custom command synchronously to the controller plugin.

Custom commands allow controller plugins to extend the standard API.

Note: This method will not return until the controller plugin has serviced the command.

@param  aDestination
        The destination of the custom command, consisting of the UID of
        the interface of this custom command and a special handle ID,
        KMMFObjectHandleController to indicate that the custom
        command is to be handled by the controller plugin.
@param  aFunction
        The function number to indicate which function is to be called
        on the controller's custom command interface.
@param  aDataTo1
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller.  Use a value of KNullDesC8 if you have no data to send.
@param  aDataTo2
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller.  Use a value of KNullDesC8 if you have no data to send.
@param  aDataFrom
        A reference to an area of memory to which the controller plugin will
        write any data to be passed back to the client.

@return The result of the custom command. Exact range of values is dependent on the custom command interface.
@since  7.0s
*/
EXPORT_C TInt RMMFController::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom)
	{
	return iControllerProxy.SendSync(aDestination,
									 aFunction,
									 aDataTo1,
									 aDataTo2,
									 aDataFrom);
	}

/**
Sends a custom command synchronously to the controller plugin.

Custom commands allow controller plugins to extend the standard API.

Note: This method will not return until the controller plugin has serviced the command.

@param  aDestination
        The destination of the custom command, consisting of the UID of
        the interface of this custom command and a special handle ID,
        KMMFObjectHandleController to indicate that the custom
        command is to be handled by the controller plugin.
@param  aFunction
        The function number to indicate which function is to be called
        on the controller's custom command interface.
@param  aDataTo1
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller. Use a value of KNullDesC8 if you have no data to send.
@param  aDataTo2
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller. Use a value of KNullDesC8 if you have no data to send.

@return The result of the custom command. Exact range of values is dependent on the custom command interface.
@since  7.0s
*/
EXPORT_C TInt RMMFController::CustomCommandSync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2)
	{
	return iControllerProxy.SendSync(aDestination,
									 aFunction,
									 aDataTo1,
									 aDataTo2);
	}

/**
Sends a custom command asynchronously to the controller plugin.

Custom commands allow controller plugins to extend the standard API.

Note: This method will return immediately.  The RunL of the active object owning the aStatus
parameter will be called when the command is completed by the controller plugin.

@param  aDestination
        The destination of the custom command, consisting of the UID of
        the interface of this custom command and a special handle ID,
        KMMFObjectHandleController to indicate that the custom
        command is to be handled by the controller plugin.
@param  aFunction
        The function number to indicate which function is to be called
        on the controller's custom command interface.
@param  aDataTo1
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller. Use a value of KNullDesC8 if you have no data to send.
@param  aDataTo2
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller. Use a value of KNullDesC8 if you have no data to send.
@param  aDataFrom
        A reference to an area of memory to which the controller plugin will
        write any data to be passed back to the client.
@param  aStatus
        The TRequestStatus of an active object. This will  contain the
        result of the custom command on completion. The exact range of
        result values is dependent on the custom command interface.

@since  7.0s
*/
EXPORT_C void RMMFController::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TDes8& aDataFrom, TRequestStatus& aStatus)
	{
	iControllerProxy.SendAsync(aDestination,
							   aFunction,
							   aDataTo1,
							   aDataTo2,
							   aDataFrom,
							   aStatus);
	}

/**
Sends a custom command asynchronously to the controller plugin.

Custom commands allow controller plugins to extend the standard API.

Note: This method will return immediately.  The RunL() of the active object owning the aStatus
parameter will be called when the command is completed by the controller plugin.

@param  aDestination
        The destination of the custom command, consisting of the UID of
        the interface of this custom command and a special handle ID,
        KMMFObjectHandleController to indicate that the custom
        command is to be handled by the controller plugin.
@param  aFunction
        The function number to indicate which function is to be called
        on the controller's custom command interface.
@param  aDataTo1
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller. Use a value of KNullDesC8 if you have no data to send.
@param  aDataTo2
        A reference to data to be copied to the controller plugin. The exact
        contents of the data are dependent on the custom command interface
        of the controller. Use a value of KNullDesC8 if you have no data to send.
@param  aStatus
        The TRequestStatus of an active object. This will  contain the
        result of the custom command on completion. The exact range of
        result values is dependent on the custom command interface.

@since	7.0s
*/
EXPORT_C void RMMFController::CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination, TInt aFunction, const TDesC8& aDataTo1, const TDesC8& aDataTo2, TRequestStatus& aStatus)
	{
	iControllerProxy.SendAsync(aDestination,
							   aFunction,
							   aDataTo1,
							   aDataTo2,
							   aStatus);
	}

/**
Registers to receive events from the controller plugin.

Events can be generated at any time, and are generally associated with things that occur
due to something happening internally within the controller. For example, an event will
be generated if the controller stops playing due to reaching the end of a file.

@param  aEventPckg
        A reference to a TMMFEventPckg object that must be member data
        of the active object calling this method.  The details of the event
        will be copied into this object when an event occurs.
@param  aStatus
        The TRequestStatus of the active object calling this method.

@see    TMMFEvent
@see    CMMFControllerEventMonitor
@since  7.0s
*/
EXPORT_C void RMMFController::ReceiveEvents(TMMFEventPckg& aEventPckg, TRequestStatus& aStatus)
	{
	iControllerProxy.ReceiveEvents(aEventPckg, aStatus);
	}

/**
Cancels a previous registration to receive events from the controller plugin.

This must be called from the DoCancel() method of the active object using the
ReceiveEvents() API function.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@see    TMMFEvent
@since  7.0s
*/
EXPORT_C TInt RMMFController::CancelReceiveEvents()
	{
	return iControllerProxy.CancelReceiveEvents();
	}

/**
Gets the number a meta data entries in the clip.

A clip may contain zero or more entries of meta data, e.g Author, Copyright etc.

@param  aNumberOfEntries
        The number of meta data entries in the clip, filled in by the controller framework.

@return An error code indicating if the function call was successful. KErrNone on success, otherwise
        another of the system-wide error codes.
@since  7.0s
*/
EXPORT_C TInt RMMFController::GetNumberOfMetaDataEntries(TInt& aNumberOfEntries) const
	{
	TPckgBuf<TInt> pckg;
	TInt error = iControllerProxy.SendSync(iDestinationPckg,
										   EMMFControllerGetNumberOfMetaDataEntries,
										   KNullDesC8,
										   KNullDesC8,
										   pckg);
	if (!error)
		aNumberOfEntries = pckg();
	return error;
	}

/**
Returns a particular meta data entry from the clip.

All meta data entries can be retrieved by multiple calls to this method, iterating
aIndex from 0 to the value returned by GetNumberOfMetaDataEntries().

@param  aIndex
        The index of the meta data entry to retrieve.

@return The meta data entry retrieved from the controller plugin. Ownership of the entry is
		passed to the caller.

@see    CMMFMetaDataEntry
@since  7.0s
*/
EXPORT_C CMMFMetaDataEntry* RMMFController::GetMetaDataEntryL(TInt aIndex) const
	{
	// Get the size of the entry
	TPckgBuf<TInt> indexPckg(aIndex);
	TPckgBuf<TInt> sizePckg;
	User::LeaveIfError(iControllerProxy.SendSync(iDestinationPckg,
												 EMMFControllerGetSizeOfMetaDataEntry,
												 indexPckg,
												 KNullDesC8,
												 sizePckg));
	// Create a buffer of appropriate size and get the entry data
	HBufC8* buf = HBufC8::NewLC(sizePckg());
	TPtr8 ptr = buf->Des();
	User::LeaveIfError(iControllerProxy.SendSync(iDestinationPckg,
												 EMMFControllerGetMetaDataEntry,
												 KNullDesC8,
												 KNullDesC8,
												 ptr));
	// Create the entry and internalize the data
	CMMFMetaDataEntry* entry = CMMFMetaDataEntry::NewL();
	CleanupStack::PushL(entry);
	RDesReadStream stream(ptr);
	CleanupClosePushL(stream);

	entry->InternalizeL(stream);

	CleanupStack::PopAndDestroy();//stream
	CleanupStack::Pop(entry);//entry
	CleanupStack::PopAndDestroy(buf);//buf

	return entry;	
	}

/**
Set the priority of the controller's sub thread.

This can be used to increase the responsiveness of the audio plugin to minimise
any lag in processing. This function should be used with care as it may have knock-on
effects elsewhere in the system.

@param	aPriority
		The TThreadPriority that the thread should run under.  The default is EPriorityNormal.
@return	TInt
		A standard error code: KErrNone if successful, KErrNotReady if the thread does not have a
		valid handle.
*/
EXPORT_C TInt RMMFController::SetThreadPriority(const TThreadPriority& aPriority) const
	{
	return iControllerProxy.SetThreadPriority(aPriority);
	}

/**
Create a new Controller Event Monitor.

@param aObserver "The observer to notify whenever an event is received.
@param aMMFController "A reference to the controller that is to be monitored for events.
@return "A pointer to the object created."
*/
EXPORT_C CMMFControllerEventMonitor* CMMFControllerEventMonitor::NewL(MMMFControllerEventMonitorObserver& aObserver, 
															 RMMFController& aMMFController)
	{
	return (new(ELeave) CMMFControllerEventMonitor(aObserver, aMMFController));
	}

CMMFControllerEventMonitor::CMMFControllerEventMonitor(MMMFControllerEventMonitorObserver& aObserver, 
													   RMMFController& aMMFController) :
	CActive(EPriorityStandard),
	iObserver(aObserver), 
	iMMFController(aMMFController)
	{
	CActiveScheduler::Add(this);
	}

CMMFControllerEventMonitor::~CMMFControllerEventMonitor()
	{
	Cancel();
	}


/**
Start receiving events from the controller.
This can only be called once the controller is open.
*/
EXPORT_C void CMMFControllerEventMonitor::Start()
	{
	iMMFController.ReceiveEvents(iEventPckg, iStatus);
	SetActive();
	}

void CMMFControllerEventMonitor::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		// Save the package buf as calling Start() will overwrite the contents of iEventPckg
		TMMFEventPckg packageBuf = iEventPckg;
		Start();
		iObserver.HandleEvent(packageBuf());
		}
	else
		{
		//something's gone wrong with trying to receive events (e.g. server died etc)
		TMMFEvent event(KMMFErrorCategoryControllerGeneralError, iStatus.Int());
		iObserver.HandleEvent(event);
		//we don't want to receive events again here...
		}
	}

void CMMFControllerEventMonitor::DoCancel()
	{
	iMMFController.CancelReceiveEvents();
	}

/**
Constructs a CMMFAddDataSourceSinkAsync object.

@param aObs A reference to the observer of the active object.  The observer will be
						notified when the AddDataSource/Sink command is complete.
@return	 A pointer to the new object.
@leave	This method can leave with one of the standard system-wide error codes.
@since 7.0s
*/
EXPORT_C CMMFAddDataSourceSinkAsync* CMMFAddDataSourceSinkAsync::NewL(MMMFAddDataSourceSinkAsyncObserver& aObs)
	{
	return new(ELeave) CMMFAddDataSourceSinkAsync(aObs);
	}

CMMFAddDataSourceSinkAsync::CMMFAddDataSourceSinkAsync(MMMFAddDataSourceSinkAsyncObserver& aObserver) :
	CActive(EPriorityNormal),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	iCurrentAction = EIdle;
	}
/**
Destructor.
*/
CMMFAddDataSourceSinkAsync::~CMMFAddDataSourceSinkAsync()
	{
	Cancel();
	delete iSourceSinkInitData;
	}
/**
Add a data source to the controller.  The caller will be signalled on completion via the
MMMFAddDataSourceSinkAsyncObserver interface.

Only one method call may be outstanding on this object at any one time.

@param aMMFController		A reference to the client controller interface class.
@param aSourceUid			The uid of the data source to be added. For more information, 
							see the documentation for the data source you wish to add.
@param aSourceInitData	    Data used to initialise the data source.  The exact contents
							of this data are dependent on the type of data source. For more
							information, see the documentation for the data source you wish
							to add.
@since	7.0s
*/
EXPORT_C void CMMFAddDataSourceSinkAsync::AddDataSource(RMMFController& aMMFController, TUid aSourceUid, const TDesC8& aSourceInitData)
	{
	ASSERT(iCurrentAction == EIdle);

	iMMFController = &aMMFController;
	iSourceSinkUidPckg() = aSourceUid;
	delete iSourceSinkInitData;
	iSourceSinkInitData = NULL;
	iSourceSinkInitData = aSourceInitData.Alloc();
	if (!iSourceSinkInitData)
		{
		iObserver.MadssaoAddDataSourceSinkAsyncComplete(KErrNoMemory, iHandleInfoPckg());
		}
	else
		{
		iMMFController->AddDataSource(iSourceSinkUidPckg, *iSourceSinkInitData, iHandleInfoPckg, iStatus);
		SetActive();
		iCurrentAction = EAddingDataSource;
		}
	}
	
/**
Add a data sink to the controller.  The caller will be signalled on completion via the
MMMFAddDataSourceSinkAsyncObserver interface.

Only one method call may be outstanding on this object at any one time.

@param aMMFController		A reference to the client controller interface class.
@param aSinkUid			    The uid of the data sink to be added. For more information, 
							see the documentation for the data sink you wish to add.
@param aSinkInitData		Data used to initialise the data sink.  The exact contents
							of this data are dependent on the type of data sink. For more
							information, see the documentation for the data sink you wish
							to add.
@since	7.0s
*/
EXPORT_C void CMMFAddDataSourceSinkAsync::AddDataSink(RMMFController& aMMFController, TUid aSinkUid, const TDesC8& aSinkInitData)
	{
	ASSERT(iCurrentAction == EIdle);

	iMMFController = &aMMFController;
	iSourceSinkUidPckg() = aSinkUid;
	delete iSourceSinkInitData;
	iSourceSinkInitData = NULL;
	iSourceSinkInitData = aSinkInitData.Alloc();
	if (!iSourceSinkInitData)
		{
		iObserver.MadssaoAddDataSourceSinkAsyncComplete(KErrNoMemory, iHandleInfoPckg());
		}
	else
		{
		iMMFController->AddDataSink(iSourceSinkUidPckg, *iSourceSinkInitData, iHandleInfoPckg, iStatus);
		SetActive();
		iCurrentAction = EAddingDataSink;
		}
	}
	

void CMMFAddDataSourceSinkAsync::RunL()
	{
	iCurrentAction = EIdle;
	iMMFController = NULL;
	delete iSourceSinkInitData;
	iSourceSinkInitData = NULL;

	iObserver.MadssaoAddDataSourceSinkAsyncComplete(iStatus.Int(), iHandleInfoPckg());
	}

void CMMFAddDataSourceSinkAsync::DoCancel()
	{
	// although the server does nothing with the cancel message,
	// by calling it we can at least be assured that by the time it completes, 
	// the async AddSource/Sink message will have been completed too
	// so we don't get any stray events.
	if (iCurrentAction == EAddingDataSource)
		iMMFController->CancelAddDataSource();
	else if (iCurrentAction == EAddingDataSink)
		iMMFController->CancelAddDataSink();

	iCurrentAction = EIdle;
	iMMFController = NULL;
	delete iSourceSinkInitData;
	iSourceSinkInitData = NULL;
	}
	

EXPORT_C void CMMFAddDataSourceSinkAsync::AddFileHandleDataSource(RMMFController& aMMFController, const RFile& aFile)
	{
	ASSERT(iCurrentAction == EIdle);

	iMMFController = &aMMFController;
	iMMFController->AddFileHandleDataSource(aFile,iHandleInfoPckg, iStatus);
	SetActive();
	iCurrentAction = EAddingDataSource;
	}

EXPORT_C void CMMFAddDataSourceSinkAsync::AddFileHandleDataSource(RMMFController& aController, const RFile& aFile, 
																  const TDesC8& aSourceInitData)
	{
	if (aSourceInitData.Length() == 0)
		{
		AddFileHandleDataSource(aController, aFile);
		}
	else
		{
		ASSERT(iCurrentAction == EIdle);

		iMMFController = &aController;
		delete iSourceSinkInitData;
		iSourceSinkInitData = NULL;
		iSourceSinkInitData = aSourceInitData.Alloc();
		if (!iSourceSinkInitData)
			{
			iObserver.MadssaoAddDataSourceSinkAsyncComplete(KErrNoMemory, iHandleInfoPckg());
			}
		else
			{
			iMMFController->AddFileHandleDataSource(aFile, *iSourceSinkInitData, iHandleInfoPckg, iStatus);
			SetActive();
			iCurrentAction = EAddingDataSource;
			}
		}
	}
	
EXPORT_C void CMMFAddDataSourceSinkAsync::AddFileHandleDataSink(RMMFController& aMMFController, const RFile& aFile)
	{
	ASSERT(iCurrentAction == EIdle);

	iMMFController = &aMMFController;
	iMMFController->AddFileHandleDataSink(aFile,iHandleInfoPckg, iStatus);
	SetActive();
	iCurrentAction = EAddingDataSink;
	}

EXPORT_C void CMMFAddDataSourceSinkAsync::AddFileHandleDataSink(RMMFController& aController, const RFile& aFile, 
																const TDesC8& aSinkInitData)
	{
	if (aSinkInitData.Length() == 0)
		{
		AddFileHandleDataSink(aController, aFile);
		}
	else
		{
		ASSERT(iCurrentAction == EIdle);

		iMMFController = &aController;
		delete iSourceSinkInitData;
		iSourceSinkInitData = NULL;
		iSourceSinkInitData = aSinkInitData.Alloc();
		if (!iSourceSinkInitData)
			{
			iObserver.MadssaoAddDataSourceSinkAsyncComplete(KErrNoMemory, iHandleInfoPckg());
			}
		else
			{
			iMMFController->AddFileHandleDataSink(aFile, *iSourceSinkInitData, iHandleInfoPckg, iStatus);
			SetActive();
			iCurrentAction = EAddingDataSink;
			}	
		}
	}

