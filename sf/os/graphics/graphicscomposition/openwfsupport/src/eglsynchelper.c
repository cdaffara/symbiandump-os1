// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// eglsynchelper.c
//

#include "eglsynchelper.h"
#include "EGL/egltypes.h"

#ifndef EGL_EGLEXT_PROTOTYPES

#define EGLSYNC_TYPE 0xE5

/* sync object data type for POSIX thread implementation*/

typedef struct CondVarSync_* NativeSyncType;

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------*//*!
 * \brief   Create a sync object for the specified display.
 * \ingroup api
 * \param   dpy     Identifier of the display which will own the sync object
 * \param   type    Type of the sync object. EGL_SYNC_REUSABLE_KHR is only supported
 * \param   attrib_list Attribute-value list specifying attributes of the sync 
 * object, terminated by an attribute entry EGL_NONE
 * \return  Handle for the created sync object if successful, EGL_NO_SYNC_KHR otherwise
 * \error   EGL_BAD_DISPLAY if <dpy> is not a name of a valid EGLDisplay;
 * EGL_NOT_INITIALIZED if the display object associated with the <dpy> has not been initialized;
 * EGL_BAD_ATTRIBUTE if <attrib_list> is neither NULL nor empty (containing only EGL_NONE) or 
 * if <type> is not a supported type of sync object;
 * EGL_BAD_ALLOC if the memory allocation related to sync object is not successful
 * \note    If <type> is EGL_SYNC_REUSABLE_KHR, a reusable sync object is created. 
 * In this case <attrib_list> must be NULL or empty (containing only EGL_NONE).
 *  *//*-------------------------------------------------------------------*/
EGLSyncKHR eglCreateSyncKHR( EGLDisplay dpy,
                       EGLenum condition,
                       const EGLint *attrib_list )
{
    PFNEGLCREATESYNCKHRPROC createPointer = (PFNEGLCREATESYNCKHRPROC) eglGetProcAddress("eglCreateSyncKHR");

	if (createPointer == NULL)
		{
		return EGL_NO_SYNC_KHR;
		}
	else
		{
		return createPointer(dpy, condition, attrib_list);
		}
}


/*-------------------------------------------------------------------*//*!
 * \brief   Destroy a sync object and free memory associated with it
 * \ingroup api
 * \param   dpy     Identifier of the display which owns the sync object
 * \param   sync    Sync object handle. 
 * \return  EGL_TRUE if deletion was successful and EGL_FALSE otherwise
 * \error   EGL_BAD_DISPLAY if <dpy> is not a name of a valid EGLDisplay;
 * EGL_NOT_INITIALIZED if the display object associated with the <dpy> has not been initialized;
 * EGL_BAD_PARAMETER if <sync> is not a valid sync object for <dpy>
 * \note    If any eglClientWaitSyncKHR commands are blocking on <sync> when 
 * eglDestroySyncKHR is called, they will be woken up, as if <sync> were signaled. 
 * If no errors are generated, <sync> will no longer be the handle of a valid sync object.
 *//*-------------------------------------------------------------------*/
EGLBoolean eglDestroySyncKHR( EGLDisplay dpy, EGLSyncKHR sync )
{
    PFNEGLDESTROYSYNCKHRPROC destroyPointer = (PFNEGLDESTROYSYNCKHRPROC) eglGetProcAddress("eglDestroySyncKHR");

	if (destroyPointer == NULL)
		{
		return EGL_FALSE;
		}
	else
		{
		return destroyPointer(dpy, sync);
		}
}

/*-------------------------------------------------------------------*//*!
 * \brief   Blocks the calling thread until the specified sync object 
 * is signaled, or until <timeout> nanoseconds have passed.  
 * \ingroup api
 * \param   dpy     Identifier of the display which owns the sync object.
 * \param   sync    Sync object handle. 
 * \param   flags   If the EGL_FLUSH_COMMANDS_BIT_KHR bit is set in <flags>
 * and <sync> is unsignaled when the function is called, then the equivalent
 * of Flush() will be performed for the current API context.   
 * \param   timeout The thread will be unblocked when <timeout> is expired.
 * If the <timeout> is to zero, the function just test the current status 
 * of the sync object. If the <timeout> is set to EGL_FOREVER_KHR, then the 
 * function does not time out. For all other values, <timeout> is adjusted to 
 * the closest value which may be substantially longer than one nanosecond. 
 * \return  EGL_CONDITION_SATISFIED if <sync> was signaled before
 * the timeout expired, which includes the case when <sync> was already 
 * signaled when eglClientWaitSyncKHR was called; EGL_TIMEOUT_EXPIRED_KHR if the 
 * specified timeout period expired before <sync> was signaled; 
 * EGL_FALSE if an error occurs.
 * \error   EGL_BAD_DISPLAY if <dpy> is not a name of a valid EGLDisplay;
 * EGL_NOT_INITIALIZED if the display object associated with the <dpy> has not been initialized;
 * EGL_BAD_PARAMETER if <sync> is not a valid sync object for <dpy> or 
 * if <flags> does not equal to 0 or EGL_SYNC_FLUSH_COMMAND_BIT_KHR
 * Note\    More than one eglClientWaitSyncKHR may be outstanding on the same <sync> at any given time. 
 * When there are multiple threads blocked on the same <sync> and the sync object is signaled, 
 * all such threads are released, but the order in which they are released is not defined.
 * If a sync object is destroyed while an eglClientWaitSyncKHR is blocking on that object, 
 * eglClientWaitSyncKHR will unblock and return immediately, just as if the sync object 
 * had been signaled prior to being destroyed.
 *//*-------------------------------------------------------------------*/
EGLint eglClientWaitSyncKHR( EGLDisplay dpy,
                             EGLSyncKHR sync,
                             EGLint flags,
                             EGLTimeKHR timeout )
{
    PFNEGLCLIENTWAITSYNCKHRPROC clientWaitPointer = (PFNEGLCLIENTWAITSYNCKHRPROC) eglGetProcAddress("eglClientWaitSyncKHR");

	if (clientWaitPointer == NULL)
		{
		return EGL_FALSE;
		}
	else
		{
		return clientWaitPointer(dpy, sync, flags, timeout);
		}
}

/*-------------------------------------------------------------------*//*!
 * \brief   Signals or unsignals the reusable sync object.  
 * \ingroup api
 * \param   dpy     Identifier of the display which owns the sync object.
 * \param   sync    Sync object handle. 
 * \param   mode    Status of the sync object. There are two possible states: 
 *  EGL_SIGNALED_KHR and EGL_UNSIGNALED_KHR. 
 * \return  EGL_TRUE if an operation was successful and EGL_FALSE otherwise.
 * \note    The error code returned from eglSignalSyncImpl() will be generated
 *//*-------------------------------------------------------------------*/
EGLBoolean eglSignalSyncKHR( EGLDisplay dpy,
                             EGLSyncKHR sync,
                             EGLenum mode )
{
    PFNEGLSIGNALSYNCKHRPROC signalPointer = (PFNEGLSIGNALSYNCKHRPROC) eglGetProcAddress("eglSignalSyncKHR");
	
   if (signalPointer == NULL)
	   {
	   return EGL_FALSE;
       }
   else
	   {
	   return signalPointer(dpy, sync, mode);
	   }
}


/*-------------------------------------------------------------------*//*!
 * \brief   Query an attribute of the sync object  
 * \ingroup api
 * \param   dpy     Identifier of the display which owns the sync object
 * \param   sync    Sync object handle. 
 * \param   attribute   An attribute to be retrieved. 
 * \param   value   Pointer to the value for the requested attribute which will be filled on function return. 
 * \return  EGL_TRUE if an operation was successful and EGL_FALSE otherwise
 * \error   EGL_BAD_DISPLAY if <dpy> is not a name of a valid EGLDisplay;
 * EGL_NOT_INITIALIZED if the display object associated with the <dpy> has not been initialized;
 * EGL_BAD_PARAMETER if <sync> is not a valid sync object for <dpy> or if <value> is not 
 * a valid pointer; EGL_BAD_ATTRIBUTE if <attribute>  does not lie within expected range;
 * EGL_BAD_MATCH if <attribute> is not supported for the type of sync object passed in <sync>
 *//*-------------------------------------------------------------------*/
EGLBoolean eglGetSyncAttribKHR( EGLDisplay dpy,
                                EGLSyncKHR sync,
                                EGLint attribute,
                                EGLint *value )
{
    PFNEGLGETSYNCATTRIBKHRPROC getAttribPointer = (PFNEGLGETSYNCATTRIBKHRPROC) eglGetProcAddress("eglGetSyncAttribKHR");

	if (getAttribPointer == NULL)
		{
		return EGL_FALSE;
		}
	else
		{
		return getAttribPointer(dpy, sync, attribute, value);
		}
}



#ifdef __cplusplus
}
#endif
#endif
