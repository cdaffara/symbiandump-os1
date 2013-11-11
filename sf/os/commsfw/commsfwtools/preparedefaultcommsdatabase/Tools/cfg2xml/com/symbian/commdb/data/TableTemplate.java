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

import java.io.File;
import java.io.IOException;
import java.util.Vector;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.xml.sax.SAXException;

import com.symbian.commdb.xml.XMLUtility;

/**
 * This class describes a particular CommDB table and it contains all the
 * parameters of the table.
 */
public class TableTemplate
{
    /**
     * Default constructor. Creates a new instance of TableTemplate
     * without any parameters
     */
    public TableTemplate()
    {
        m_tableTemplateName = new String("");
        m_maxEntriesAllowed = 0;
        
        // Initialise the vectors
        
        m_tableParameters       = new Vector();
        m_mustExist             = new Vector();
        m_isTableEntryLink      = new Vector();
        m_parameterTypes        = new Vector();
        m_parameterEnumerations = new Vector();
    }
    
    // Access Methods
    
    /**
     * Get the name of this table template
     *
     * @return The name of this table template
     */
    public String GetTableTemplateName()
    {
        return new String(m_tableTemplateName);
    }
    
    /**
     * Get the number of parameters this table has
     *
     * @return The number of parameters this table has
     */
    public int GetNumberOfParameters()
    {
        return m_tableParameters.size();
    }
    
    /**
     * Get the number of enumerations stored in this table template
     *
     * @return The number of enumerations stored in this table template
     */
    public int GetNumberOfEnumerationTypes()
    {
        return m_parameterEnumerations.size();
    }
    
    /**
     * Get the maximum number of table entries allowed by this table template
     *
     * @return The maximum number of table entries allowed
     */
    public int GetMaxNumberOfEntriesAllowed()
    {
        return m_maxEntriesAllowed;
    }
    
    /**
     * Get a Vector with boolean values which indicate whether a particular
     * parameter is of some enumeration type (true) or of a simple type (false)
     *
     * @return The Vector with boolean values which identify the enumeration
     *         types of the table template's parameters
     */
    public Vector GetEnumerationTypeFlags()
    {
        Vector flags = new Vector(m_parameterTypes.size());
        
        for(int i=0; i<m_parameterTypes.size(); i++)
        {
            String type = (String)m_parameterTypes.elementAt(i);
            int index = GetEnumerationTypeIndex(type);
            
            if(index == -1)
                flags.add(new Boolean(false));
            else
                flags.add(new Boolean(true));
        }
        
        return flags;
    }
    
    /**
     * Get a table parameter given its index
     *
     * @param index The index of the parameter to return
     * @return The parameter string
     */
    public String GetParameter(int index)
    {
        return new String((String)m_tableParameters.elementAt(index));
    }
    
    /**
     * Get the existence flag for a parameter given its index
     *
     * @param index The index of the parameter flag to return
     * @return The flag
     */
    public boolean GetParameterMustExist(int index)
    {
        return ((Boolean)m_mustExist.elementAt(index)).booleanValue();
    }
    
    /**
     * Get the flag which specifies whether the value of a parameter
     * specified by its index represents a table entry link or not
     *
     * @param index The index of the parameter flag to return
     * @return The flag
     */
    public boolean GetIsTableEntryLink(int index)
    {
        return ((Boolean)m_isTableEntryLink.elementAt(index)).booleanValue();
    }
    
    /**
     * Get the existence flag for a parameter given the name of the
     * parameter
     *
     * @param name The name of the parameter flag to return
     * @return The flag
     */
    public boolean GetParameterMustExist(String name)
    {
        return GetParameterMustExist(GetParameterIndex(name));
    }
    
    /**
     * Get the flag which specifies whether the value of a parameter
     * specified by its name represents a table entry link or not
     *
     * @param name The name of the parameter flag to return
     * @return The flag
     */
    public boolean GetIsTableEntryLink(String name)
    {
        return GetIsTableEntryLink(GetParameterIndex(name));
    }
    
    /**
     * Get a parameter type given its index
     *
     * @param index The index of the parameter type to return
     * @return The parameter type string
     */
    public String GetParameterType(int index)
    {
        return new String((String)m_parameterTypes.elementAt(index));
    }
    
    /**
     * Get a parameter enumeration type given its index
     *
     * @param index The index of the parameter enumeration to return
     * @return The enumeration type
     */
    public EnumerationType GetEnumerationType(int index)
    {
        return (EnumerationType)m_parameterEnumerations.elementAt(index);
    }
    
    /**
     * Get a parameter enumeration type given the name of the enumeration
     *
     * @param enumName The name of the enumeration
     * @return The enumeration type
     */
    public EnumerationType GetEnumerationType(String enumName)
    {
        EnumerationType type = null;
        
        for(int i=0; i<m_parameterEnumerations.size(); i++)
        {
            EnumerationType check = (EnumerationType)m_parameterEnumerations.elementAt(i);
            String name = check.GetEnumerationName();
            if(name.equals(enumName))
            {
                type = check;
                break;
            }
        }
        return type;
    }
    /**
     * Get the index of a table parameter given its name
     *
     * @param parameter The parameter to find its index
     * @return The index of the parameter. If not found, return -1
     */
    public int GetParameterIndex(String parameter)
    {
        int size = m_tableParameters.size();
        for(int i=0; i<size; i++)
        {
            String stored = (String)m_tableParameters.elementAt(i);
            if(stored.equals(parameter))
                return i;
        }
        return -1;
    }
    
    /**
     * Get the index of an enumeration type given its name
     *
     * @param enumerationName The name of the enumeration type to find its index
     * @return The index of the enumeration type. If not found, return -1
     */
    public int GetEnumerationTypeIndex(String enumerationName)
    {
        int size = m_parameterEnumerations.size();
        for(int i=0; i<size; i++)
        {
            EnumerationType stored = (EnumerationType)m_parameterEnumerations.elementAt(i);
            String typeName = stored.GetEnumerationName();
            if(typeName.equals(enumerationName))
                return i;
        }
        return -1;
    }
    
    // Set Methods
    
    /**
     * Set the name of this table template
     *
     * @param name The name of this table template
     */
    public void SetTableTemplateName(String name)
    {
        m_tableTemplateName = new String(name);
    }
    
    /**
     * Add a new parameter to this table
     *
     * @param parameter The parameter name to add to this table
     * @param existFlag The existence flag of the parameter to add
     * @param type The type of the parameter to add
     */
    public void AddParameter(String parameter, boolean existFlag, String type)
    {
        m_tableParameters.add(new String(parameter));
        m_mustExist.add(new Boolean(existFlag));
        m_parameterTypes.add(new String(type));
    }
    
    /**
     * Remove a parameter from this table given its index
     *
     * @param index The index of the parameter to remove
     * @return True if operation was successful and false if it failed
     */
    public boolean RemoveParameter(int index)
    {
        try
        {
            m_tableParameters.removeElementAt(index);
            m_mustExist.removeElementAt(index);
            m_parameterTypes.removeElementAt(index);
        }
        catch(ArrayIndexOutOfBoundsException e)
        {
            return false;
        }
        return true;
    }
    
    /**
     * Remove a parameter from this table given the parameter itself
     *
     * @param parameter The parameter to remove from this table
     * @return True if the operation was successful and false if it failed
     */
    public boolean RemoveParameter(String parameter)
    {
        for(int i=0; i<m_tableParameters.size(); i++)
        {
            String stored = (String)m_tableParameters.elementAt(i);
            if(stored.equals(parameter))
            {
                m_tableParameters.removeElementAt(i);
                m_mustExist.removeElementAt(i);
                m_parameterTypes.removeElementAt(i);
                return true;
            }
        }
        return false;
    }
    
    /**
     * Remove all the parameters of this table
     */
    public void RemoveAllParameters()
    {
        m_tableParameters.clear();
        m_mustExist.clear();
        m_isTableEntryLink.clear();
        m_parameterTypes.clear();
        m_parameterEnumerations.clear();
    }
    
    /**
     * Fill the table template with parameters imported from an XML Schema
     * file which contains the grammar of a particular CommDB table
     *
     * @param fileName The name of the XML Schema file to read
     * @param tableTemplateName The name of the table template to locate
     * @throws DatabseException
     */
    public void ImportParameters(String fileName, String tableTemplateName) throws DatabaseException
    {
        SetTableTemplateName(tableTemplateName);
        
        // Create a DOM XML parser instance
        
        DocumentBuilderFactory builderFactory = DocumentBuilderFactory.newInstance();
        builderFactory.setNamespaceAware(true);
        builderFactory.setValidating(false);
        builderFactory.setIgnoringComments(true);
        DocumentBuilder builder = null;
        try
        {
            builder = builderFactory.newDocumentBuilder();
        }
        catch(ParserConfigurationException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportParameters()",
            "Creation of an XML DOM parser",
            e.getMessage());
        }
        
        // Open the specified XML Schema file and parse it
        
        File xmlFile = new File(fileName);
        Document xmlDoc = null;
        try
        {
            xmlDoc = builder.parse(xmlFile);
        }
        catch(SAXException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportParameters()",
            "Parser exception during parsing file " + fileName,
            e.getMessage());
        }
        catch(IOException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportParameters()",
            "I/O exception during parsing file " + fileName,
            e.getMessage());
        }
        
        // Locate the node which refers to the table entries
        
        Vector nodes = new Vector();
        XMLUtility.LocateNodesFromAttribute(xmlDoc.getDocumentElement(),
        "ref",tableTemplateName,nodes);
        
        if(nodes.size() != 1)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportParameters()",
            "Locating node which refers to " + tableTemplateName + " after parsing file " + fileName,
            nodes.size() + " found when only one was expected");
        }
        
        Node refNode = (Node)nodes.elementAt(0);
        Node attrib  = refNode.getAttributes().getNamedItem("maxOccurs");
        
        if(attrib != null)
        {
            String maxOccurs = attrib.getNodeValue();
            if(maxOccurs.equals("unbounded"))
                m_maxEntriesAllowed = Integer.MAX_VALUE;
            else
                m_maxEntriesAllowed = Integer.parseInt(maxOccurs);
        }
        else
            m_maxEntriesAllowed = Integer.MAX_VALUE;
        
        // Locate the node with the name supplied by the user, if it exists
        
        nodes = new Vector();
        XMLUtility.LocateNodesFromAttribute(xmlDoc.getDocumentElement(),
        "name",tableTemplateName,nodes);
        if(nodes.size() != 1)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportParameters()",
            "Locating node " + tableTemplateName + " after parsing file " + fileName,
            nodes.size() + " found when only one was expected");
        }
        
        // Find all the children of this node
        
        Vector children = new Vector();
        XMLUtility.VectoriseChildrenNodes((Node)nodes.elementAt(0),children);
        
        // Pick the "name" attributes of each "xsd:element" children node and
        // set them as parameters to this table template. Note that all existing
        // parameters are removed. Do the same for the "minOccurs" attribute
        // as well as the "type" attribute for setting the parameter existence
        // and type arrays respectively
        
        RemoveAllParameters();
        
        for(int i=0; i<children.size(); i++)
        {
            Node child = (Node)children.elementAt(i);
            if(child.getNodeName().equals("xsd:element"))
            {
                if(child.hasAttributes())
                {
                    NamedNodeMap attribs = child.getAttributes();
                    boolean minOccursSpecified = false;
                    
                    for(int k=0; k<attribs.getLength(); k++)
                    {
                        Node at = attribs.item(k);
                        String attribName = at.getNodeName();
                        
                        if(attribName.equals("name"))
                        {
                            m_tableParameters.add(new String(at.getNodeValue()));
                        }
                        else if(attribName.equals("minOccurs"))
                        {
                            minOccursSpecified = true;
                            
                            if(at.getNodeValue().equals("1"))
                                m_mustExist.add(new Boolean(true));
                            else
                                m_mustExist.add(new Boolean(false));
                        }
                        else if(attribName.equals("type"))
                        {
                            String typeName = new String(at.getNodeValue());
                            m_parameterTypes.add(typeName);
                            
                            if(typeName.equals("TableEntryNameType"))
                                m_isTableEntryLink.add(new Boolean(true));
                            else
                                m_isTableEntryLink.add(new Boolean(false));
                        }
                    }
                    
                    if(!minOccursSpecified)
                        m_mustExist.add(new Boolean(false));
                }
            }
        }
        
        if(m_tableParameters.size() == 0 || m_parameterTypes.size() == 0)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportParameters()",
            "After processing the parsed file " + fileName,
            "No table parameters and/or types were found");
        }
    }
    
    /**
     * Import the enumeration types employed by the parameters of this table
     *
     * @param fileName The name of the XML Schema file which contains the
     *                 definitions of the enumeration types used by this
     *                 table template
     * @throws DatabaseException
     */
    public void ImportEnumerations(String fileName) throws DatabaseException
    {
        // Create a DOM XML parser instance
        
        DocumentBuilderFactory builderFactory = DocumentBuilderFactory.newInstance();
        builderFactory.setNamespaceAware(true);
        builderFactory.setValidating(false);
        builderFactory.setIgnoringComments(true);
        DocumentBuilder builder = null;
        try
        {
            builder = builderFactory.newDocumentBuilder();
        }
        catch(ParserConfigurationException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportEnumerations()",
            "Creation of an XML DOM parser",
            e.getMessage());
        }
        
        // Open the specified XML Schema file and parse it
        
        File xmlFile = new File(fileName);
        Document xmlDoc = null;
        try
        {
            xmlDoc = builder.parse(xmlFile);
        }
        catch(SAXException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportEnumerations()",
            "Parser exception during parsing file " + fileName,
            e.getMessage());
        }
        catch(IOException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.TableTemplate.ImportEnumerations()",
            "I/O exception during parsing file " + fileName,
            e.getMessage());
        }
        
        // Vectorise all the children nodes of the main element
        
        Vector nodes = new Vector();
        XMLUtility.VectoriseChildrenNodes(xmlDoc.getDocumentElement(),nodes);
        
        // Locate all the enumeration definitions and store them
        
        for(int i=0; i<nodes.size(); i++)
        {
            Node child = (Node)nodes.elementAt(i);
            if(child.getNodeName().equals("xsd:simpleType"))
            {
                String enumerationName = child.getAttributes().getNamedItem("name").getNodeValue();
                Node next = (Node)nodes.elementAt(i+2);
                
                if(next.getNodeName().equals("xsd:restriction"))
                {
                    int numberChildren = next.getChildNodes().getLength();
                    EnumerationType type = new EnumerationType(enumerationName);
                    
                    for(int k=i+3; k<i+numberChildren+2; k++)
                    {
                        next = (Node)nodes.elementAt(k);
                        if(next.getNodeName().equals("xsd:enumeration"))
                        {
                            String value = next.getAttributes().getNamedItem("value").getNodeValue();
                            type.AddEnumeration(value);
                        }
                    }
                    
                    if(type.GetNumberEnumerations() > 0)
                        m_parameterEnumerations.add(type);
                }
            }
        }
        
        // Add some Symbian-specific enumerations
        
        EnumerationType type = new EnumerationType("TBool");
        type.AddEnumeration("TRUE");
        type.AddEnumeration("FALSE");
        m_parameterEnumerations.add(type);
    }
    
    /**
     * Create a Vector with a set of JComboBoxes, one for each enumeration type
     * stored in this table template
     *
     * @return A Vector with a set of JComboBoxes, one for each enumeration type
     *         stored in this table template
     */
    public Vector CreateJComboBoxes()
    {
        Vector boxes = new Vector();
        
        for(int i=0; i<m_parameterEnumerations.size(); i++)
        {
            EnumerationType type = (EnumerationType)m_parameterEnumerations.elementAt(i);
            boxes.add(type.CreateJComboBox());
        }
        
        return boxes;
    }
    
    // Data Members
    
    /**
     * The name of this table template
     */
    private String m_tableTemplateName;
    
    /**
     * The array of all the parameters forming this table
     */
    private Vector m_tableParameters;
    
    /**
     * An array of flags which specify whether a particular parameter
     * must have a value assigned to it (true) or not (false), in which
     * case a default value can be used
     */
    private Vector m_mustExist;
    
    /**
     * An array of flags which specify whether the value of a parameter
     * represents a link to some table entry (true) or not (false)
     */
    private Vector m_isTableEntryLink;
    
    /**
     * The array which contains the type of each parameter of
     * this table template
     */
    private Vector m_parameterTypes;
    
    /**
     * The array which contains the enumeration types for the parameters
     * of this table template
     */
    private Vector m_parameterEnumerations;
    
    /**
     * The maximum number of table entries allowed
     */
    private int m_maxEntriesAllowed;
}
