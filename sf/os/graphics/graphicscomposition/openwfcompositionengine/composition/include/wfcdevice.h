/* Copyright (c) 2009 The Khronos Group Inc.
 * Portions copyright (c) 2009-2010  Nokia Corporation and/or its subsidiary(-ies)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */
/*! \ingroup wfc
 *  \file wfcdevice.h
 *
 *  \brief Device handling interface
 */
#ifndef WFCDEVICE_H_
#define WFCDEVICE_H_

#include "WF/wfc.h"
#include "wfcstructs.h"
#include "wfcimageprovider.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  Get list of available devices.
 *
 *  \param idList Pointer to list where device IDs should be written
 *  \param listCapacity Maximum number of IDs to write
 *  \param filterList User provided array
 *
 *  \return Number of IDs written.
 */
OWF_API_CALL WFCint
WFC_Devices_GetIds(WFCint* idList,
                   WFCint listCapacity,
                   const WFCint* filterList);

/*!
 *  \brief Create instance of a device whose ID
 *  matches deviceId
 *
 *  \param deviceId ID of the device to create. Must be WFC_DEFAULT_DEVICE_ID
 *  or some other valid device ID (returned by Devices_GetIds)
 *
 *  \return Handle to created device or WFC_INVALID_HANDLE
 *
 */
OWF_API_CALL WFCint
WFC_Device_Create(WFCint deviceId);

/*!
 *  \brief Destroy device, or rather queue it for destruction.
 *
 *  \param device Device
 */
OWF_API_CALL void
WFC_Device_Destroy(WFC_DEVICE* device);

/*!
 *  \brief Set error code for device.
 *
 *  Obs! In case the previous
 *  error code hasn't been read from the device, this function
 *  does nothing; the new error is set only if "current" error
 *  is none.
 *
 *  \param dev Device object
 *  \param code Error to set
 *
 */
OWF_API_CALL void OWF_APIENTRY
WFC_Device_SetError(WFCDevice dev,
                    WFCErrorCode code);

/*!
 *  \brief Read and reset last error code from device.
 *
 *  \param device Device object
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Device_GetError(WFC_DEVICE* device);

/*!
 *  \brief Find device object by handle
 *
 *  \param dev Device handle
 *
 *  \return Mathing device object or NULL
 */
OWF_API_CALL WFC_DEVICE*
WFC_Device_FindByHandle(WFCDevice dev);

/*!
 *  \brief Get device attribute
 *
 *  \param device Device
 *  \param attrib Attribute name
 *  \param value Pointer to where the value should be saved
 *
 *  \return WFCErrorCode
 */
OWF_API_CALL WFCErrorCode
WFC_Device_GetAttribi(WFC_DEVICE* device,
                      WFCDeviceAttrib attrib,
                      WFCint* value);

/*!
 *  \brief Create context on device
 *
 *  \param device Device
 *  \param stream Target stream for context
 *  \param type Context type
 *
 *  \return New context
 */
OWF_API_CALL WFC_CONTEXT*
WFC_Device_CreateContext(WFC_DEVICE* device,
                         OWFNativeStreamType stream,
                         WFCContextType type,
                         WFCint screenNum);

/*!
 *  \brief Destroy context from device
 */
OWF_API_CALL WFCErrorCode
WFC_Device_DestroyContext(WFC_DEVICE* device,
                          WFCContext context);

/*!
 *  \brief Destroy all device's contexts
 *
 *  \param device Device object
 */
OWF_API_CALL void
WFC_Device_DestroyContexts(WFC_DEVICE* device);

/*!
 *  \brief Find context context object by handle
 *
 *  \param device Device
 *  \param context Context handle
 *
 *  \return Corresponding context object or NULL
 *  if handle is invalid.
 */
OWF_API_CALL WFC_CONTEXT*
WFC_Device_FindContext(WFC_DEVICE* device,
                       WFCContext context);

/*!
 *  \brief Create new element
 *
 *  \param device
 *  \param context
 *
 *  \return New element or NULL
 */
OWF_API_CALL WFC_ELEMENT*
WFC_Device_CreateElement(WFC_DEVICE* device, WFC_CONTEXT* context);

/*!
 *  \brief Destroy element
 *
 *  \param device
 *  \param element
 */
OWF_API_CALL WFCErrorCode
WFC_Device_DestroyElement(WFC_DEVICE* device,
                          WFCElement element);

/*!
 *  \brief Destroy all elements from device
 *
 *  \param device Device
 */
OWF_API_CALL void
WFC_Device_DestroyElements(WFC_DEVICE* device);

/*!
 *  \brief Find element by handle
 *
 *  \param device Device
 *  \param el Element handle
 *
 *  \return Element object
 */
OWF_API_CALL WFC_ELEMENT*
WFC_Device_FindElement(WFC_DEVICE* device,
                       WFCElement el);

/*!
 *  \brief Set element integer vector attribute
 *
 *  \param device Device
 *  \param element Element handle
 *  \param attrib Attribute name
 *  \param count Attribute value vector length (1 for scalar attribute)
 *  \param values Pointer to values
 *
 *  \return WFCErrorCode: WFC_ERROR_BAD_ATTRIBUTE if attribute name is invalid;
 *  WFC_ERROR_INVALID_ARGUMENT if values is NULL or if the count doesn't match
 *  the attribute's size; WFC_ERROR_BAD_HANDLE if element handle is invalid.
 */
OWF_API_CALL WFCErrorCode
WFC_Device_SetElementAttribiv(WFC_DEVICE* device,
                              WFCElement element,
                              WFCElementAttrib attrib,
                              WFCint count,
                              const WFCint* values);

/*!
 *  \brief Set element attribute
 */
OWF_API_CALL WFCErrorCode
WFC_Device_SetElementAttribfv(WFC_DEVICE* device,
                              WFCElement element,
                              WFCElementAttrib attrib,
                              WFCint count,
                              const WFCfloat* values);
/*!
 *  \brief Get element attribute
 */
OWF_API_CALL WFCErrorCode
WFC_Device_GetElementAttribiv(WFC_DEVICE* device,
                              WFCElement element,
                              WFCElementAttrib attrib,
                              WFCint count,
                              WFCint* values);
/*!
 *  \brief Get element attribute
 */
OWF_API_CALL WFCErrorCode
WFC_Device_GetElementAttribfv(WFC_DEVICE* device,
                              WFCElement element,
                              WFCElementAttrib attrib,
                              WFCint count,
                              WFCfloat* values);


/*!
 *  \brief Destroy all image providers from device
 *
 *  \param device Device
 */
OWF_API_CALL void
WFC_Device_DestroyImageProviders(WFC_DEVICE* device);

OWF_API_CALL WFC_IMAGE_PROVIDER*
WFC_Device_FindImageProvider(WFC_DEVICE* device,
                             WFCHandle handle,
                             WFC_IMAGE_PROVIDER_TYPE type);

OWF_API_CALL WFC_IMAGE_PROVIDER*
WFC_Device_CreateSource(WFC_DEVICE* device,
                        WFC_CONTEXT* context,
                        WFCNativeStreamType stream);

OWF_API_CALL WFC_IMAGE_PROVIDER*
WFC_Device_CreateMask(WFC_DEVICE* device,
                      WFC_CONTEXT* context,
                      WFCNativeStreamType stream);

OWF_API_CALL WFCErrorCode
WFC_Device_DestroySource(WFC_DEVICE* device, WFCSource source);

OWF_API_CALL WFCErrorCode
WFC_Device_DestroyMask(WFC_DEVICE* device, WFCMask mask);

OWF_API_CALL WFC_IMAGE_PROVIDER*
WFC_Device_FindSource(WFC_DEVICE* device, WFCSource source);

OWF_API_CALL WFC_IMAGE_PROVIDER*
WFC_Device_FindMask(WFC_DEVICE* device, WFCMask mask);

OWF_API_CALL void
WFC_Device_EnableContentNotifications(WFC_DEVICE* device,
                                      WFC_CONTEXT* context,
                                      WFCboolean enable);

OWF_API_CALL WFCboolean
WFC_Device_StreamIsTarget(WFC_DEVICE* device,
                          WFCNativeStreamType stream);

OWF_API_CALL WFC_CONTEXT*
WFC_Device_FindContextByScreen(WFC_DEVICE* device,
                               WFCint screenNumber);

OWF_API_CALL void
WFC_Device_DestroyContextElements(WFC_DEVICE* device,
                                  WFC_CONTEXT* context);

OWF_API_CALL void
WFC_Device_DestroyContextImageProviders(WFC_DEVICE* device,
                                        WFC_CONTEXT* context);

OWF_API_CALL WFCboolean
WFC_Device_FindScreenNumber(WFCint screenNumber);

#ifdef __cplusplus
}
#endif

#endif /*WFCDEVICE_H_*/
