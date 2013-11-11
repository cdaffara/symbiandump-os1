// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.symbian.commdb.data;


/**
 * This is an exception thrown by a member method of the
 * com.symbian.commdb.data.* class in case of an error
 * during the processing of the database
 */
public class DatabaseException extends java.lang.Exception
{
    
    /**
     * Creates a new instance of DatabaseException
     *
     * @param methodName The name of the method that threw this exception
     * @param operationStage The stage during an operation performed on the
     *                       database when this exception was thrown
     * @param errorMessage The error message
     */
    public DatabaseException(String methodName, String operationStage, String errorMessage)
    {
        super();
        m_methodName     = new String(methodName);
        m_operationStage = new String(operationStage);
        m_errorMessage   = new String(errorMessage);
    }
    
    // Access Methods
    
    /**
     * Get the name of the method which generated the error
     *
     * @return The name of the method which generate the error
     */
    public String GetMethodName()
    {
        return new String(m_methodName);
    }
    
    /**
     * Get the stage during some operation of the database that generated
     * this exception
     *
     * @return The stage at which the error was generated
     */
    public String GetOperationStage()
    {
        return new String(m_operationStage);
    }
    
    /**
     * Get the error message
     *
     * @return The error message
     */
    public String GetErrorMessage()
    {
        return new String(m_methodName + ":\n" + m_errorMessage);
    }
    
    /**
     * Output to the console a summary of the generated error
     */
    public void OutputErrorSummary()
    {
        System.out.println("Member method : " + m_methodName);
        System.out.println("Stage         : " + m_operationStage);
        System.out.println("Error message : " + m_errorMessage);
    }
    
    // Data Members
    
    /**
     * The name of the method that threw this exception
     */
    private String m_methodName;
    
    /**
     * The stage of the database construction process when this
     * exception was thrown
     */
    private String m_operationStage;
    
    /**
     * The error message
     */
    private String m_errorMessage;
}
