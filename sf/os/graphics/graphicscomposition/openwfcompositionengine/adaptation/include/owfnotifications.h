/* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * 
 * Description:
 * Symbian notifications
 */


#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  \brief Initialises the Symbian specific device data, including
 *  the SUS observers
 *
 *  \param source the source to be initialised
 *  \return OWF_TRUE id successfull
 */
OWF_API_CALL OWFboolean
owfSymDeviceInitialise(WFC_IMAGE_PROVIDER* source);

/*!
 *  \brief Destroys the Symbian specific device data, including
 *  the SUS observers
 *
 *  \param device the device to be destroyed
 *  \param source the source to be initialised
 *  \param position in the device list
 */
OWF_API_CALL void
owfSymDeviceDestroy(WFC_DEVICE* device, WFC_IMAGE_PROVIDER* source, WFCint position);

/*!
 *  \brief Notifies the stream associated with element that the read buffer has been aquired
 *
 *  Used to solve the initial availabilty request
 *
 *  \param context The context corresponding to the ongoing composition
 *  \param element The element that composition has just started for
 *
 *  \return OWF_TRUE id successfull
 */
OWF_API_CALL OWFboolean
owfSymElementNotifications(WFC_CONTEXT* context, WFC_ELEMENT* element);

/*!
 *  \brief Resets the visibility state associated with each source
 *
 *  Used after each commit
 *
 *  \param context The context
 */
OWF_API_CALL void
owfSymDeviceResetVisibilityState(WFC_CONTEXT* context);

/*!
 *  \brief Processes all notifications for a context immediately after the context has been composed 
 *
 *  \param context The context coresponding to the caller composer
 */
OWF_API_CALL WFCint
owfSymProcessAllNotifications(WFC_CONTEXT* context);

#ifdef __cplusplus
}
#endif
