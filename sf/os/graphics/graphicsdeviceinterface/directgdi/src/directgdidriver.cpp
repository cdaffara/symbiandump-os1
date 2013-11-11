// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "directgdipaniccodes.h" // Included first to resolve dependency

#include <graphics/sgimage.h>
#include <graphics/directgdiimagetarget.h>
#include <graphics/directgdidrawablesource.h>
#include <graphics/directgdipanics.h>
#include <graphics/directgdidriverinternal.h>
#include <e32svr.h>
#include <u32hal.h>

#include "directgdidriver.h"
#include "directgditypes.h"
#include "directgdidriverinternallibrary.inl"

typedef TInt (*TDriverCreateFunction)(CDirectGdiDriverInternal*&, RLibrary);

/**
Returns the singleton instance of CDirectGdiDriver for the calling thread. 

@return Pointer to an opened CDirectGdiDriver object else NULL if a CDirectGdiDriver had not been opened yet.
*/
EXPORT_C CDirectGdiDriver* CDirectGdiDriver::Static()
	{
	return static_cast<CDirectGdiDriver*>(Dll::Tls());						
	}

/**
Creates a new CDirectGdiDriver object if one has not already been created in this thread.
Performs any adaptation initialisation that is needed to allow the calling thread to use 
functionality provided by the new DirectGDI interfaces. Calling this method multiple times 
from the same client thread has no effect after the first call, other than to increase the 
open count. 

@see CDirectGdiDriver::Close()

@pre None.
@post The object reference counter is incremented. Adaptation resources for the 
calling thread are created and initialised. The new DirectGDI interfaces are ready 
to be used from the calling thread.  

@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
EXPORT_C TInt CDirectGdiDriver::Open()
	{
	TInt err = KErrNone;
	CDirectGdiDriver* self = Static(); 
		
	// Check we're not already opened
	if (!self)
		{				
		self = new CDirectGdiDriver();
		if (!self)
			{
			err = KErrNoMemory;
			return err;
			}

#ifdef __WINS__
		// Dynamically load DirectGDI adapter library.
		// Under WINS two dlls will be available - directgdiadapter_sw.dll and directgdiadapter_vg.dll
		// Need to select which dll to load.  The value of the keyword SYMBIAN_GRAPHICS_DIRECTGDI_USE
		// in the epoc.ini file will determine the dll to load.
		// If this keyword does not appear in the epoc.ini file, the software version of
		// directgdiadapter will be loaded by default.
		_LIT(KVgAdapterDllName, "directgdiadapter_vg.dll");
		_LIT(KSwAdapterDllName, "directgdiadapter_sw.dll");
		TBool useOpenVg = EFalse;
		UserSvr::HalFunction(EHalGroupEmulator, EEmulatorHalBoolProperty,  (TAny*)"symbian_graphics_directgdi_use_openvg",  &useOpenVg);
		TPtrC libraryName;
		if(useOpenVg)
			{
			libraryName.Set(KVgAdapterDllName);
			}
		else
			{
			libraryName.Set(KSwAdapterDllName);
			}
		RLibrary lib;
		err = lib.Load(libraryName);
		if (err != KErrNone)
			{
			delete self;
			return (err==KErrNotFound ? KErrNotSupported : err);
			}
		
		// Create internal driver
		err = self->CreateInternalDriver(lib);
		if (err != KErrNone)
			{
			delete self;
			lib.Close();
			return err;
			}
#else
		// Under armv5, the appropriate dll (software or vg) is determined at rom build time.
		err = self->CreateInternalDriver();
		if (err != KErrNone)
			{
			delete self;
			return err;
			}
#endif

		// Store a pointer to the new driver instance in thread local storage:
		err = Dll::SetTls(self);
		if (err != KErrNone)	
			{
			delete self;
			}				
		}
	
	if (err == KErrNone)
		{
		++(self->iOpenCount);
		}
	
	return err;
	}

/**
Create the adaptation/internal object for this driver.

@pre The adaptation/internal driver object has not been created yet.
@post The adaptation/internal object has been created.

@return KErrNone if successful, otherwise one of the system-wide error codes.
 */
#ifdef __WINS__
TInt CDirectGdiDriver::CreateInternalDriver(const RLibrary& aLibrary)
	{
	// Function at ordinal 1 creates new CDirectGdiDriverInternal
	TDriverCreateFunction create = reinterpret_cast<TDriverCreateFunction>(aLibrary.Lookup(1));
	return create(iDriverInternal, aLibrary);
	}
#else
TInt CDirectGdiDriver::CreateInternalDriver()
	{
	return CDirectGdiDriverInternal::New(iDriverInternal, RLibrary());
	}
#endif

/**
Performs adaptation termination which is needed to release any internal resources 
allocated by the calling thread. It will also perform an implicit Finish() by submitting
all outstanding requests from the calling thread. A DirectGDI client's thread that 
uses the new DirectGDI interfaces must call this method at the end of its lifetime.

@see CDirectGdiDriver::Open()

@pre The driver is open from a call to Open().
@post The object open count is decremented. If the open count reaches zero, any 
adaptation resources allocated to the calling thread must be released, and the CDirectGdiDriver 
object for the calling thread is destroyed.

@panic DGDI 14, if Close() is called on a CDirectGdiDriver object that is not open.
*/
EXPORT_C void CDirectGdiDriver::Close()
	{	
	GRAPHICS_ASSERT_ALWAYS(iOpenCount > 0, EDirectGdiPanicDriverOpenCountError);
		
	if (--iOpenCount == 0)
		{
		// We call Finish() instead of Flush() because it will decrease the reference count
		// of images used in DrawResource() (if any) and is guaranteed that it will complete.
		// Flush() cannot guarantee completion so we can't decrement image reference counts.
		Finish();

		// Get the handle to the DLL used by iDriverInternal so that we can close
		// it once iDriverInternal has been deleted.
		RLibrary lib = iDriverInternal->Library();
		delete this;
		lib.Close();
			
		// Free the space we saved for the driver singleton pointer in thead local storage
		Dll::FreeTls();		
		}		
	}

/**
Ensures all outstanding requests on the calling thread are submitted for processing.
The method immediately returns and does not wait until all outstanding requests are
completely processed. Clients can continue issuing rendering requests after calling
this method.

@see    CDirectGdiDriver::Finish()

@pre 	CDirectGdiDriver object has been initialised for the calling thread.
@post 	All outstanding requests have been submitted for processing.
*/
EXPORT_C void CDirectGdiDriver::Flush()
	{
	iDriverInternal->Flush();
	}

/**
Ensures all outstanding rendering requests from the calling thread are submitted
and processed. This method will wait until all the outstanding rendering requests
have been completely processed.

@see    CDirectGdiDriver::Flush()

@pre 	CDirectGdiDriver object has been initialised for the calling thread.
@post 	All outstanding requests have been completely processed.
*/
EXPORT_C void CDirectGdiDriver::Finish()
	{
	iDriverInternal->Finish();
	}

/**
Returns the first error code (as the result of calling any DirectGDI API), if any,
since the last call to this function or, if it has not previously been called, since
the CDirectGdiDriver was initialised. Calling this function clears the error code.

@see    CDirectGdiDriver::SetError(TInt)

@pre 	CDirectGdiDriver object has been initialised for the calling thread.
@post 	The error code has been reset after being read.

@return The first error code, if any, since the last call to this function or, 
		if it has not previously been called, since the CDirectGdiDriver was initialised. 
		KErrNone will indicate that no such error has occurred.
*/
EXPORT_C TInt CDirectGdiDriver::GetError()
	{
	return iDriverInternal->GetError();
	}


/**
Sets the error code on the driver. If the error code is already set to a value other
than KErrNone, the error code will not be modified.

@see    CDirectGdiDriver::GetError()

@param  aErr The error code to set.

@pre 	CDirectGdiDriver object has been initialised for the calling thread.
@post 	The error code has been set.
*/
EXPORT_C void CDirectGdiDriver::SetError(TInt aErr)
	{
	iDriverInternal->SetError(aErr);
	}

/**
Retrieves a pointer to an instance of the requested extension interface implementation, if provided.

@param	aInterfaceId The globally unique identifier of the requested interface.	
@param	aInterface On return, holds the specified interface, or NULL if the interface cannot be found.

@pre	CDirectGdiDriver object has been initialised for the calling thread.

@return KErrNone If the interface is supported, KErrExtensionNotSupported otherwise.
 */
EXPORT_C TInt CDirectGdiDriver::GetInterface(TUid aInterfaceId, TAny*& aInterface)
	{
	return iDriverInternal->GetInterface(aInterfaceId, aInterface);
	}

/**
CDirectGdiDriver private constructor as this class is singleton.

@pre 	None.
@post 	Initialises the member reference counter to zero.
*/
CDirectGdiDriver::CDirectGdiDriver()
	{	
	}

/**
CDirectGdiDriver default destructor.

@pre None.
@post None.

@panic DGDI 15, if the resource count is not zero.
*/
CDirectGdiDriver::~CDirectGdiDriver()
	{
	GRAPHICS_ASSERT_ALWAYS(iOpenCount == 0, EDirectGdiPanicDriverDestructorOpenCountError);
	delete iDriverInternal;		
	}

/**
Delegates the call to the CDirectGdiDriverInternal object, which creates a DirectGDI 
adaptation-specific resource from the given drawable resource so it can be drawn 
using the DirectGDI rendering API.

@see CloseDrawableSource()

@param 	aRDirectGdiDrawableSource The RDirectGdiDrawableSource object to be created
@param 	aRSgDrawable The RSgDrawable object to use when creating aRDirectGdiDrawableSource

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
@post 	The DirectGDI adaptation-specific resource that is bound to the given 
drawable resource is created and this handle is now associated with it. The reference 
counter on the drawable resource is incremented. The CDirectGdiDriver for this thread 
is now aware of and owns the adaptation specific resource.

@return KErrNone if successful, KErrNotSupported if the drawable resource is not 
created with the correct usage (ESgUsageDirectGdiSource must be set), otherwise one of the system-wide error codes.

@panic DGDI 19, if this handle is already associated with a DirectGDI adaptation-specific drawable resource.
@panic DGDI 20, if the drawable resource is not valid.
*/
TInt CDirectGdiDriver::CreateDrawableSource(RDirectGdiDrawableSource& aRDirectGdiDrawableSource, const RSgDrawable& aRSgDrawable)
	{				
	GRAPHICS_ASSERT_ALWAYS(aRDirectGdiDrawableSource.Handle() == KNullHandle, EDirectGdiPanicDrawableSourceAlreadyExists);
    
	if(aRSgDrawable.DrawableType() == KSgImageTypeUid)
		{// make sure that drawable was created with the flag ESgUsageDirectGdiSource
		RSgImage *image = (RSgImage*) &aRSgDrawable;
		TSgImageInfo info;
		TInt err = image ->GetInfo (info);
		GRAPHICS_ASSERT_ALWAYS(err == KErrNone, EDirectGdiPanicImageSourceInfoError);
	
		if (!(info.iUsage & ESgUsageDirectGdiSource))
			{
			return KErrNotSupported;
			}
		}
	return iDriverInternal->CreateDrawableSource(aRDirectGdiDrawableSource.iHandle, aRSgDrawable);
	}

/**
Delegates call to the CDirectGdiDriverInternal object, which destroys the DirectGDI
adaptation-specific resource associated with this handle. Calling this method on a
handle that is not associated with any DirectGDI adaptation specific resource will 
do nothing. Once Close() is called, this handle can be reused.

@see CreateDrawableSource()

@param 	aSource The RDirectGdiDrawableSource object.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
@post 	The DirectGDI specific resource associated with this handle will be 
destroyed (at any time preferred by the adaptation). This handle is no longer 
associated with a DirectGDI specific resource. The reference counter of the 
underlying non-image resource is decremented. 
*/
void CDirectGdiDriver::CloseDrawableSource(RDirectGdiDrawableSource& aSource)
	{
	iDriverInternal->CloseDrawableSource(aSource.iHandle);
	}
	
/**
Delegates call to the CDirectGdiDriverInternal object, which creates a DirectGDI 
adaptation-specific resource from the given image resource so it can be used as a 
target of DirectGDI rendering.

@see CloseImageTarget()

@param 	aTarget The RDirectGdiImageTarget object.
@param 	aImage The RSgImage object.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
The image resource has been fully constructed and created with the correct usage 
that allows it to be used as a DirectGDI target.

@post 	The DirectGDI adaptation-specific resource that is bound to the given image 
resource is created and this handle is now associated with it. The reference counter
on the image resource is incremented. 

@return KErrNone if successful, KErrNotSupported if the image resource is not 
created with the correct usage, otherwise one of the system-wide error codes.

@panic DGDI 17, if this handle is already associated with a DirectGDI adaptation-specific resource.
@panic DGDI 21, if the image resource is not valid.
*/
TInt CDirectGdiDriver::CreateImageTarget(RDirectGdiImageTarget& aTarget, const RSgImage& aImage)
	{		
	GRAPHICS_ASSERT_ALWAYS(aTarget.Handle() == KNullHandle, EDirectGdiPanicImageTargetAlreadyExists);
    TSgImageInfo info;
	TInt err = aImage.GetInfo(info);
	GRAPHICS_ASSERT_ALWAYS(err == KErrNone, EDirectGdiPanicImageTargetInfoError);
		
	if (!(info.iUsage & ESgUsageDirectGdiTarget))
		{
		return KErrNotSupported;
		}
	return iDriverInternal->CreateImageTarget(aTarget.iHandle, aImage);
	}
	
/**
Delegates call to the CDirectGdiDriverInternal object, which destroys the DirectGDI 
adaptation specific resource associated with this handle. Calling this method on a 
handle that is not associated with any DirectGDI adaptation specific resource will 
do nothing. Once Close() is called, this handle can be reused.

@see CreateImageTarget()

@param 	aTarget The RDirectGdiImageTarget object.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.

@post 	The DirectGDI specific resource associated with this handle will be destroyed
(at any time preferred by the adaptation). This handle is no longer associated with 
a DirectGDI specific resource. The reference counter of the underlying image 
resource is decremented. 
*/
void CDirectGdiDriver::CloseImageTarget(RDirectGdiImageTarget& aTarget)
	{
	iDriverInternal->CloseImageTarget(aTarget.iHandle);
	}
		
/**
Delegates call to the CDirectGdiDriverInternal object, which creates a
MDirectGdiEngine object.

@param 	aEngine The MDirectGdiEngine object.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
@post   None.

@return KErrNone if successful, otherwise one of the system-wide error codes.
*/
TInt CDirectGdiDriver::CreateEngine(MDirectGdiEngine*& aEngine)
	{
	return iDriverInternal->CreateEngine(aEngine);
	}
	
/**
Delegates call to the CDirectGdiDriverInternal object, which destroys a
MDirectGdiEngine object.

@param 	aEngine The MDirectGdiEngine object.

@pre 	CDirectGdiDriver object has been initialised from the calling thread.
@post   None.
*/
void CDirectGdiDriver::DestroyEngine(MDirectGdiEngine* aEngine)
	{
	iDriverInternal->DestroyEngine(aEngine);
	}

