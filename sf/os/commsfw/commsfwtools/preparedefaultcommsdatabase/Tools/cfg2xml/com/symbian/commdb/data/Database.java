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

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
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
 * This class contains all the CommDB tables. The structure of these tables
 * is determined by the processing of a set of XML Schema files which specify
 * the CommDB database. These files should all reside in the same directory
 * or subdirectories of the same directory.
 */
public class Database
{
    /**
     * Creates a new instance of Database
     */
    public Database(String schemaDirectory)
    {
        // Initialisation
        
        m_tables               = new Vector();
        m_fileNames            = new Vector();
        m_tableNames           = new Vector();
        m_tableTemplateNames   = new Vector();
        m_enumerationFilenames = new Vector();
        m_commdbType           = new String("Unspecified");
        m_schemaDirectory      = new String(schemaDirectory);
    }
    
    // Access Methods
    
    /**
     * Get the name of the XML Schema directory used by the database
     *
     * @return The name of the XML Schema direcoty used by the database
     */
    public String GetXMLSchemaDirectory()
    {
        return new String(m_schemaDirectory);
    }
    
    /**
     * Get the type of this CommDB database
     *
     * @return The type of this CommDB database
     */
    public String GetDatabaseType()
    {
        return new String(m_commdbType);
    }
    
    /**
     * Get the number of table templates in the database. Note that this
     * number is the same to the number of XML Schema files supplied to
     * the database
     *
     * @return The number of table templates in the database
     */
    public int GetNumberOfTableTemplates()
    {
        return m_tableTemplateNames.size();
    }
    
    /**
     * Get the number of tables in existence within the database
     *
     * @return The number of tables in existence
     */
    public int GetNumberOfTables()
    {
        return m_tables.size();
    }
    
    /**
     * Get the number of table entries for all tables in the database
     *
     * @return The total number of table entries
     */
    public int GetNumberOfTableEntries()
    {
        int num = 0;
        for(int i=0; i<m_tables.size(); i++)
        {
            Table tab = (Table)m_tables.elementAt(i);
            num += tab.GetNumberOfTableEntries();
        }
        return num;
    }
    
    /**
     * Get the name of an XML Schema file in the database given its index
     *
     * @param index The index of the XML Schema file
     * @return The name of the XML Schema file
     */
    public String GetXMLSchemaFilename(int index)
    {
        return new String((String)m_fileNames.elementAt(index));
    }
    
    /**
     * Get the name of an XML Schema file used for defining the
     * enumerated types of a table template given its index
     *
     * @param index The index of the XML Schema file
     * @return The name of the XML Schema file
     */
    public String GetEnumerationFilename(int index)
    {
        return new String((String)m_enumerationFilenames.elementAt(index));
    }
    
    /**
     * Get the name of a table template given its index
     *
     * @param index The index of the table template
     * @return The name of the table template
     */
    public String GetTableTemplateName(int index)
    {
        return new String((String)m_tableTemplateNames.elementAt(index));
    }
    
    /**
     * Get the name of a table given its index
     *
     * @param index The index of the table
     * @return The name of the table
     */
    public String GetTableName(int index)
    {
        return new String((String)m_tableNames.elementAt(index));
    }
    
    /**
     * Get a table from this database given its index
     *
     * @param index The index of the table
     * @return The table
     */
    public Table GetTable(int index)
    {
        return (Table)m_tables.elementAt(index);
    }
    
    /**
     * Get a table from this database given its name
     *
     * @param name The name of the table
     * @return The table, if it exists, else returns null
     */
    public Table GetTable(String name)
    {
        for(int i=0; i<m_tables.size(); i++)
        {
            Table tab = (Table)m_tables.elementAt(i);
            if(tab.GetTableName().equals(name))
                return tab;
        }
        return null;
    }
    
    /**
     * Get the name of the XML Schema file which defines a specific
     * table template in this database. If that does not exist then
     * an empty string is returned
     *
     * @param tableTemplateName The name of the table template
     * @return The name of the XML Schema file or an empty string if not found
     */
    public String GetXMLSchemaFilename(String tableTemplateName)
    {
        int index = m_tableTemplateNames.indexOf(tableTemplateName);
        if(index == -1)
            return new String("");
        else
            return GetXMLSchemaFilename(index);
    }
    
    /**
     * Get the name of the XML Schema file which defines the enumerated
     * types of a table template in this database. If that does not exist then
     * an empty string is returned
     *
     * @param tableTemplateName The name of the table template
     * @return The name of the XML Schema file or an empty string if not found
     */
    public String GetEnumerationFilename(String tableTemplateName)
    {
        int index = m_tableTemplateNames.indexOf(tableTemplateName);
        if(index == -1)
            return new String("");
        else
            return GetEnumerationFilename(index);
    }
    
    /**
     * Get the name of the table template which is defined by a specific
     * XML Schema file in this database. If that does not exist then
     * an empty string is returned
     *
     * @param schemaFilename The name of the XML Schema file
     * @return The name of the table template or an empty string if not found
     */
    public String GetTableTemplateName(String schemaFilename)
    {
        int index = m_fileNames.indexOf(schemaFilename);
        if(index == -1)
            return new String("");
        else
            return GetTableTemplateName(index);
    }
    
    // Set Methods
    
    /**
     * Set the XML Schema directory
     *
     * @param directory The name of the XML Schema directory to use
     */
    public void SetXMLSchemaDirectory(String directory)
    {
        m_schemaDirectory = new String(directory);
    }
    
    /**
     * Add an XML Schema file name in the database list. This name may contain
     * the subdirectory of the XML Schema directory the file resides i.e.
     *
     * [m_schemaDirectory]\[subdirectory\filename]
     *                     { --- fileName ----   }
     *
     * At the same time, the name of the table template corresponding to this
     * file must be supplied e.g.
     *
     * fileName           = "IAPTable.xsd"
     * tableNTemplateName = "IAP"
     * tableName          = "IAPTable"
     *
     * @param fileName The name of the XML Schema file to add
     * @param tableTemplateName The name of the table template corresponding to this XML Schema file
     * @param tableName The name of the table corresponding to this XML Schema file
     * @param enumerationFileName The name of the XML Schema file with the definition of the enumerations
     * @return True if the operation was successful, false if not
     */
    public boolean AddXMLSchemaFileName(String fileName,
    String tableTemplateName,
    String tableName,
    String enumerationFileName)
    {
        if(fileName.equals("") ||
        tableTemplateName.equals("") ||
        tableName.equals("") ||
        enumerationFileName.equals(""))
        {
            return false;
        }
        else
        {
            m_fileNames.add(new String(fileName));
            m_tableTemplateNames.add(new String(tableTemplateName));
            m_tableNames.add(new String(tableName));
            m_enumerationFilenames.add(new String(enumerationFileName));
            return true;
        }
    }
    
    /**
     * It builds the table templates of the CommDB database by processing
     * each of the specified XML Schema files
     *
     * @param progressBar The progress bar object which displays the progress
     *                    of this operation
     * @throws DatabaseException
     */
    public void BuildTableTemplates(javax.swing.JProgressBar progressBar) throws DatabaseException
    {
        int prog     = 0;
        int numFiles = 2*m_fileNames.size();
        
        progressBar.setMinimum(0);
        progressBar.setMaximum(numFiles);
        progressBar.setValue(0);
        progressBar.repaint();
        
        for(int i=0; i<m_fileNames.size(); i++)
        {
            TableTemplate template     = new TableTemplate();
            String fullName            = m_schemaDirectory + (String)m_fileNames.elementAt(i);
            String templateName        = (String)m_tableTemplateNames.elementAt(i);
            String enumerationFileName = m_schemaDirectory + (String)m_enumerationFilenames.elementAt(i);
            
            try
            {
                template.ImportParameters(fullName,templateName);
                progressBar.setValue(++prog);
                template.ImportEnumerations(enumerationFileName);
                progressBar.setValue(++prog);
            }
            catch(DatabaseException e)
            {
                m_tables.clear();
                throw e;
            }
            
            Table tab = new Table(template);
            tab.SetTableName((String)m_tableNames.elementAt(i));
            m_tables.add(tab);
        }
    }
    
    /**
     * Import a CommDB XML settings file and populate the contents of the
     * database with the read information. Note that all existing database
     * elements will be be erased
     *
     * @throws DatabaseException
     */
    public void ImportDatabase(String fileName)
    throws DatabaseException
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
            throw new DatabaseException("com.symbian.commdb.data.Database.ImportDatabase()",
            "Creation of an XML DOM parser",
            e.getMessage());
        }
        
        // Open the specified XML file with the CommDB settings and parse it
        
        File xmlFile = new File(fileName);
        Document xmlDoc = null;
        try
        {
            xmlDoc = builder.parse(xmlFile);
        }
        catch(SAXException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.Database.ImportDatabase()",
            "Parser exception during parsing file " + fileName,
            e.getMessage());
        }
        catch(IOException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.Database.ImportDatabase()",
            "I/O exception during parsing file " + fileName,
            e.getMessage());
        }
        
        // Remove all existing table entries, if any
        
        for(int i=0; i<m_tables.size(); i++)
        {
            Table tab = (Table)m_tables.elementAt(i);
            tab.RemoveAllTableEntries();
        }
        
        // Process the DOM structure and populate the CommDB tables of this database
        
        Vector nodes = new Vector();
        XMLUtility.VectoriseChildrenNodes(xmlDoc.getDocumentElement(), nodes);
        
        for(int nodeIndex=0; nodeIndex<nodes.size(); nodeIndex++)
        {
            Node aNode = (Node)nodes.elementAt(nodeIndex);
            
            // Find a node of type "element" which may be a CommDB table
            
            if(aNode.getNodeType() != Node.ELEMENT_NODE)
                continue;
            
            String nodeName = aNode.getNodeName();
            
            // Look for the name of this node in the list of valid CommDB
            // table names
            
            for(int k=0; k<m_tableTemplateNames.size(); k++)
            {
                String tableName = (String)m_tableTemplateNames.elementAt(k);
                
                // If a table has been located then create a new table entry
                // in the database
                
                if(tableName.equals(nodeName))
                {
                    Table tab = (Table)m_tables.elementAt(k);
                    TableEntry entry = tab.CreateTableEntry();
                    
                    // Get the "operation" attribute, if it exists
                    
                    if(aNode.hasAttributes())
                    {
                        NamedNodeMap attribs = aNode.getAttributes();
                        for(int m=0; m<attribs.getLength(); m++)
                        {
                            Node at = attribs.item(m);
                            if(at.getNodeName().equals("operation"))
                            {
                                entry.SetOperation(at.getNodeValue());
                                break;
                            }
                        }
                    }
                    
                    // Process the parameters of the newly created entry
                    
                    Vector paramNodes = new Vector();
                    XMLUtility.VectoriseChildrenNodes(aNode,paramNodes);
                    
                    for(int childIndex=0; childIndex<paramNodes.size(); childIndex++)
                    {
                        Node param = (Node)paramNodes.elementAt(childIndex);
                        
                        // For each child node of type element check to see if
                        // it's name is one of the names of the parameters for
                        // this table
                        
                        if(param.getNodeType() != Node.ELEMENT_NODE)
                            continue;
                        
                        String paramName = param.getNodeName();
                        
                        int numParams = entry.GetNumberOfParameters();
                        
                        for(int paramIndex=0; paramIndex<numParams; paramIndex++)
                        {
                            String name = entry.GetParameterName(paramIndex);
                            if(name.equals(paramName))
                            {
                                Node next = (Node)paramNodes.elementAt(childIndex+1);
                                String paramValue = next.getNodeValue();
                                entry.SetParameterValue(paramIndex,paramValue);
                                break;
                            }
                        }
                    }
                    
                    break;
                }
            }
        }
        
        // Check the table link references
        
        Vector brokenParams = new Vector();
        Vector brokenLinks  = new Vector();
        boolean ok = FindBrokenTableEntryLinks(brokenParams,brokenLinks);
        
        if(!ok)
            throw new DatabaseException("com.symbian.commdb.data.Database.ImportDatabase()",
            "Resolving table entry references in file " + fileName,
            "Unresolved table link references");
    }
    
    /**
     * Import a CommDB settings file in the pre-XML format and populate the
     * contents of the database with the read information. Note that all
     * existing database elements will be be erased.
     *
     * IMPORTANT: This routine can only convert Zephyr CommDB databases
     *            to the new Zephyr CommDB format!
     *
     * @throws DatabaseException
     */
    public void ImportOldDatabase(String fileName) throws DatabaseException
    {
        // Define tables which allow the conversion of CommDB_IDs employed
        // in the pre-XML format to table references in the XML format
        // Note that these tables are invetibaly specific to the latest
        // version of CommDB (Zephyr) which supports the pre-XML format.
        // This information is stored in vectors with the following
        // specification:
        //
        // tabName   - The name of the table
        // idParam   - The name of the parameter which contains the id
        //             of the table entry reference
        // typeParam - The name of the table which contains the table
        //             entry reference. If sign "+" is appended to the
        //             name then the value of this parameter specifies
        //             the table name, else if sign "-" is appended
        //             then typeParam is the name of the table.
        // newParam  - The name of the Zephyr CommDB parameter which
        //             stores the table entry reference in the new
        //             XML-based representation
        
        Vector tabName   = new Vector();
        Vector idParam   = new Vector();
        Vector typeParam = new Vector();
        Vector newParam  = new Vector();
        
        tabName.add("IAP");
        idParam.add("IAPService"); typeParam.add("+IAPServiceType"); newParam.add("IAPService");
        
        tabName.add("IAP");
        idParam.add("IAPBearer"); typeParam.add("+IAPBearerType"); newParam.add("IAPBearer");
        
        tabName.add("IAP");
        idParam.add("IAPNetwork"); typeParam.add("-Network"); newParam.add("IAPNetwork");
        
        tabName.add("IAP");
        idParam.add("Location"); typeParam.add("-Location"); newParam.add("LocationRef");
        
        tabName.add("IAP");
        idParam.add("Chargecard"); typeParam.add("-Chargecard"); newParam.add("ChargecardRef");
        
        tabName.add("Proxy");
        idParam.add("ISP"); typeParam.add("+ProxyServiceType"); newParam.add("ISPRef");
        
        tabName.add("WAPIPBearer");
        idParam.add("AccessPointId"); typeParam.add("-WAPAccessPoint"); newParam.add("AccessPoint");
        
        tabName.add("WAPIPBearer");
        idParam.add("IAP"); typeParam.add("-IAP"); newParam.add("IAPRef");
        
        tabName.add("WAPSMSBearer");
        idParam.add("AccessPointId"); typeParam.add("-WAPAccessPoint"); newParam.add("AccessPoint");
        
        tabName.add("VpnService");
        idParam.add("HomeIAP"); typeParam.add("-IAP"); newParam.add("HomeIAP");
        
        tabName.add("VpnService");
        idParam.add("HomeNetwork"); typeParam.add("-Network"); newParam.add("HomeNetwork");
        
        tabName.add("ConnectionPreferences");
        idParam.add("IAP"); typeParam.add("-IAP"); newParam.add("IAPRef");

        tabName.add("GlobalSettings");
        idParam.add("WAPAccessPoint"); typeParam.add("-WAPAccessPoint"); newParam.add("WAPAccess");
        
        tabName.add("GlobalSettings");
        idParam.add("DefaultNetwork"); typeParam.add("-Network"); newParam.add("DefaultNetwork");

        tabName.add("GlobalSettings");
        idParam.add("ModemForDataAndFax"); typeParam.add("*ModemBearer"); newParam.add("ModemForDataAndFax");
        
        tabName.add("GlobalSettings");
        idParam.add("ModemForPhoneServicesAndSMS"); typeParam.add("*ModemBearer"); newParam.add("ModemForPhoneServicesAndSMS");
        
        tabName.add("GlobalSettings");
        idParam.add("LocationForDataAndFax"); typeParam.add("-Location"); newParam.add("LocationForDataAndFax");

        tabName.add("GlobalSettings");
        idParam.add("LocationForPhoneServicesAndSMS"); typeParam.add("-Location"); newParam.add("LocationForPhoneServicesAndSMS");

        File oldFile = new File(fileName);
        
        // Start reading the file line by line and process the contents
        
        try
        {
            // Create a file reader for the CommDB settings file
            
            FileReader fileReader = new FileReader(oldFile);
            BufferedReader bufReader = new BufferedReader(fileReader);
            
            main_loop: while(true)
            {
                // Read a line
                
                String lineContext = null;
                lineContext = bufReader.readLine();
                
                // Termination criterion = no more text to read
                
                if(lineContext == null)
                    break main_loop;
                
                // Avoid empty lines
                
                if(lineContext.length() == 0)
                    continue main_loop;
                
                // Avoid comments
                
                if(lineContext.charAt(0) == '#')
                    continue main_loop;
                
                // Examine the read line to see if we have a table to read
                // which has the format "[TableName]"
                
                if(lineContext.charAt(0) == '[')
                {
                    int endIndex = lineContext.indexOf(']');
                    String tableNameRead = null;
                    
                    try
                    {
                        tableNameRead = lineContext.substring(1,endIndex);                        
                    }
                    catch(IndexOutOfBoundsException e)
                    {
                        continue main_loop;
                    }
                    
                    try
                    {
                        // There is an anomaly with the naming of some
                        // tables and a renaming is required
                        
                        if(tableNameRead.equals("Proxies"))
                        {
                            tableNameRead = "Proxy";
                        }
 						else if (tableNameRead.equals("DefaultCDMA2000SettingsTable"))
                       	{
                         	tableNameRead = "DefaultCDMA2000Table";
						}
						else
                        {
                            int len = tableNameRead.length();
                            if(tableNameRead.substring(len-5,len).equals("Table"))
                            {
                                tableNameRead = tableNameRead.substring(0,len-5);
                            }
                        }
                    }
                    catch(IndexOutOfBoundsException e)
                    { }
                    
                    // Look for the name of this table in the list of valid CommDB
                    // table names
                    
                    table_search_loop: for(int k=0; k < m_tableTemplateNames.size(); k++)
                    {
                        String tableName = (String)m_tableTemplateNames.elementAt(k);
                        
                        // If a table has been located then create a new table entry
                        // in the database
                        
                        if(tableName.equals(tableNameRead))
                        {
                            Table tab = (Table)m_tables.elementAt(k);
                            
                            // Next we look for table entries within the table
                            
                            int entry_id = 0;
                            TableEntry entry = null;
                            
                            // Next we read the table entry parameters
                            
                            while(true)
                            {
                                bufReader.mark(80);
                                lineContext = bufReader.readLine();
                                
                                // Terminate parsing if reached end of file
                                
                                if(lineContext == null)
                                    break main_loop;
                                
                                // Avoid empty lines
                                
                                if(lineContext.length() == 0)
                                    continue;
                                
                                // Avoid comment lines
                                
                                if(lineContext.charAt(0) == '#')
                                    continue;
                                
                                // Termination criterion is the encounter of the
                                // definition of a new table
                                
                                if(lineContext.charAt(0) == '[')
                                {
                                    bufReader.reset();
                                    continue main_loop;
                                }
                                
                                // If a new table entry is enountered then
                                // a new table entry object is created
                                
                                String check = null;
                                
                                try
                                {
                                    check = lineContext.substring(0,4);
                                }
                                catch(IndexOutOfBoundsException e)
                                {
                                    check = "";
                                }
                                
                                if(check.equals("ADD_"))
                                {
                                    entry_id++;
                                    entry = tab.CreateTableEntry();
                                    entry.SetID(entry_id);
                                    
                                    try
                                    {
                                        if(lineContext.substring(0,11).equals("ADD_SECTION"))
                                            entry.SetOperation("add");
                                        else if(lineContext.substring(0,12).equals("ADD_TEMPLATE"))
                                            entry.SetOperation("template");
                                    }
                                    catch(IndexOutOfBoundsException e)
                                    { }
                                    
                                    continue;
                                }
                                
                                // Avoid lines which do not define parameters
                                
                                int signEq = lineContext.indexOf('=');
                                if(signEq == -1)
                                    continue;
                                
                                // Avoid table parameter counter markers
                                
                                try
                                {
                                    check = lineContext.substring(1,7);
                                }
                                catch(IndexOutOfBoundsException e)
                                {
                                    check = "";
                                }
                                
                                if(check.equals("FIELD_"))
                                {
                                    continue;
                                }
                                
                                // Obtain the name of the parameter and its value
                                
                                String paramRead = null;
                                String valueRead = null;
                                
                                try
                                {
                                    paramRead = lineContext.substring(1,signEq);
                                    valueRead = lineContext.substring(signEq+1);
                                }
                                catch(IndexOutOfBoundsException e)
                                {
                                    continue;
                                }
                                
                                // Check for the "Name" parameter
                                
                                if(paramRead.equals("Name"))
                                {
                                    entry.SetParameterValue("Name",valueRead);
                                    continue;
                                }
                                
                                // Check to see whether this parameter specifes
                                // the id or the type of some table entry reference
                                // Table references are temporarily stored using
                                // the format:
                                //
                                // [TableName].[ID]
                                //
                                // and the proper table entry qualified names are
                                // supplied in place of the [ID] parameter when
                                // the parsing of the file is completed
                                
                                boolean tabRef = false;
                                
                                for(int m=0; m < tabName.size(); m++)
                                {
                                    String tabl = (String)tabName.elementAt(m);
                                    
                                    // Table name checks out
                                    
                                    if(tableName.equals(tabl))
                                    {
                                        // Did we read an ID ?
                                        
                                        String id = (String)idParam.elementAt(m);
                                        
                                        if(paramRead.equals(id))
                                        {
                                            // Our IDs start from 0 while in the pre-XML file
                                            // start with 1 so we need to reduce its value by
                                            // 1.
                                            
                                            int valueId = Integer.parseInt(valueRead);
                                            valueId--;
                                            valueRead = (new Integer(valueId)).toString();
                                            
                                            String paramName = (String)newParam.elementAt(m);
                                            String typ = (String)typeParam.elementAt(m);
                                            char sign = typ.charAt(0);
                                            
                                            if(sign == '-')
                                            {
                                                String val = typ.substring(1) + "." + valueRead;
                                                entry.SetParameterValue(paramName,val);
                                                
                                            }
                                            else if( sign == '*' )
                                            {
                                                String numVal = valueRead;
                                                entry.SetParameterValue(paramName,numVal);
                                            }
                                            else
                                            {                                               
                                            	String qualName  = entry.GetParameterValue(paramName) + valueRead;
                                            	entry.SetParameterValue(paramName,qualName);
                                            }
                                            
                                            tabRef = true;
                                            break;
                                        }
                                        
                                        // Did we read a type?
                                        
                                        String typ = (String)typeParam.elementAt(m);
                                        typ = typ.substring(1);
                                                                                
                                        if(paramRead.equals(typ))
                                        {
                                            try
                                            {
                                                // There is an anomaly with the naming of some
                                                // tables and a renaming is required
                                         		if(valueRead.equals("Proxies"))
                                            	{
                                                	valueRead = "Proxy"; 
                                                }
                                            	else if (tableNameRead.equals("DefaultCDMA2000SettingsTable"))
                         						{
                         							tableNameRead = "DefaultCDMA2000Table";
 												}
                                          		else 
                                          		{
 	                                              	int len = valueRead.length();
 	                                              	if(valueRead.substring(len-5,len).equals("Table"))
 	                                                	valueRead = valueRead.substring(0,len-5);
                                          		}                        
                                            }
                                            catch(IndexOutOfBoundsException e)
                                            { }
                                            
                                            String paramName = (String)newParam.elementAt(m);
                                            String qualName  = valueRead + "." + entry.GetParameterValue(paramName);
                                            entry.SetParameterValue(paramName,qualName);
                                            tabRef = true;
                                            break;
                                        }
                                    }
                                }
                                
                                // We are dealing with a regular parameter. Find the appropriate
                                // parameter in the table entry and update its value
                                
                                if(!tabRef)
                                {
                                    int numParams = tab.GetTableTemplate().GetNumberOfParameters();
                                    for(int m=0; m < numParams; m++)
                                    {
                                        String paramName = entry.GetParameterName(m);
                                        if(paramName.equals(paramRead))
                                        {
                                            entry.SetParameterValue(m,valueRead);
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                        
                        continue table_search_loop;
                    }
                }
            }
        }
        catch(IOException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.Database.ImportOldDatabase()",
            "I/O exception during parsing file " + fileName,
            e.getMessage());
        }
        
        // We require all table entries to have a "Name" parameter and if this is
        // not set by the pre-XML file we need to create an artificial one. This
        // is generated by using the following specification:
        //
        // [TableName][ID]
        //
        // where:
        //
        // TableName - is the name of the table which contains the table entry
        // ID        - is the id of the table entry
        
        for(int i=0; i < m_tables.size(); i++)
        {
            Table table = (Table)m_tables.elementAt(i);
            
            for(int j=0; j < table.GetNumberOfTableEntries(); j++)
            {
                TableEntry entry = table.GetTableEntry(j);
                Integer entryId  = new Integer(entry.GetID());
                String entryName = entry.GetParameterValue("Name");
                
                if(entryName.equals(""))
                {
                    entryName = table.GetTableName() + entryId.toString();
                    entry.SetParameterValue("Name",entryName);
                }
            }
        }
        
        // Resolve the table entry references by replacing the parameter values of
        // type:
        //
        // [TableName].[ID]
        //
        // with
        //
        // [TableName].[TableEntryName]
        
        for(int i=0; i < m_tables.size(); i++)
        {
            Table table = (Table)m_tables.elementAt(i);
            TableTemplate templ = table.GetTableTemplate();
            
            for(int j=0; j < table.GetNumberOfTableEntries(); j++)
            {
                TableEntry entry = table.GetTableEntry(j);
                
                for(int k=0; k < entry.GetNumberOfParameters(); k++)
                {
                    if(templ.GetIsTableEntryLink(k))
                    {
                        String linkName = entry.GetParameterValue(k);
                        
                        // Resolve the table entry reference
                        
                        int dotIndex = linkName.indexOf(".");
                        
                        if(dotIndex > 0)
                        {
                            String tabNm = linkName.substring(0,dotIndex) + "Table";
                            int tabId;
                            
                            try
                            {
                                tabId = Integer.parseInt(linkName.substring(dotIndex+1));
                            }
                            catch(NumberFormatException e)
                            {
                                continue;
                            }
                            
                            Table locTable = GetTable(tabNm);
                            
                            if(locTable != null && tabId >= 0)
                            {
                                TableEntry ref = locTable.GetTableEntry(tabId);
                                if(ref != null)
                                {
                                    String entryName = ref.GetQualifiedName();
                                    entry.SetParameterValue(k,entryName);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    /**
     * Exports this CommDB database to a file in XML format
     *
     * @throws FileNotFoundException
     * @throws IOException
     */
    public void ExportDatabase(File exportedFile) throws FileNotFoundException,
    IOException
    {
        // Open the file to write the XML elements representing the database
        
        FileOutputStream output;
        
        try
        {
            output = new FileOutputStream(exportedFile);
        }
        catch(FileNotFoundException e)
        {
            throw e;
        }
        
        // Write the initial XML elements common to all files
        
        String initial = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        initial +=       "<!--\n";
        initial +=       "        CommDB Settings File\n";
        initial +=       "        Copyright (c) 2003-2004, Symbian Ltd.\n";
        initial +=       "        All Rights Reserved\n";
        initial +=       "-->\n";
        initial +=       "<CommDB:Config xmlns=\"SymbianOS.Generic.Comms-Infras.CommDB\"\n";
        initial +=       "               xmlns:CommDB=\"SymbianOS.Generic.Comms-Infras.CommDB\"\n";
        initial +=       "               xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n";
        initial +=       "               xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"\n";
        initial +=       "               xsi:schemaLocation=\"SymbianOS.Generic.Comms-Infras.CommDB CommDB.xsd\">\n\n";
        
        try
        {
            output.write(initial.getBytes());
        }
        catch(IOException e)
        {
            throw e;
        }
        
        for(int i=0; i<m_tables.size(); i++)
        {
            Table tab = (Table)m_tables.elementAt(i);
            String xmlDoc = tab.toXML();
            output.write(xmlDoc.getBytes());
        }
        
        output.write((new String("\n</CommDB:Config>\n")).getBytes());
        output.close();
    }
    
    /**
     * Output this CommDB database to the console in XML format
     * This method is mainly used for debugging purposes
     */
    public void OutputDatabaseContents()
    {
        System.out.println("Database Structure");
        System.out.println("==================");
        
        System.out.println("Number of tables = " + m_tables.size());
        System.out.println("The database is:\n");
        
        for(int i=0; i<m_tables.size(); i++)
        {
            Table tab = (Table)m_tables.elementAt(i);
            System.out.print(tab.toXML());
        }
    }
    
    /**
     * Erase the database. This removes all database elements
     */
    public void EraseDatabase()
    {
        m_tables.clear();
        m_fileNames.clear();
        m_tableNames.clear();
        m_tableTemplateNames.clear();
        m_enumerationFilenames.clear();
        m_schemaDirectory = "";
    }
    
    /**
     * Erase the table entries only.
     */
    public void EraseTableEntries()
    {
        for(int i=0; i<m_tables.size(); i++)
        {
            Table tab = (Table)m_tables.elementAt(i);
            tab.RemoveAllTableEntries();
        }
    }
    
    /**
     * Parse the "Contents.xml" file which defines the files to use for
     * importing the definition of the CommDB database
     *
     * @throws DatabaseException
     */
    public void ReadContents() throws DatabaseException
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
            throw new DatabaseException("com.symbian.commdb.data.Settings.ReadContents()",
            "Creation of an XML DOM parser",
            e.getMessage());
        }
        
        // Open the specified XML file with the CommDB information and parse it
        
        File xmlFile = new File(m_schemaDirectory + "Contents.xml");
        Document xmlDoc = null;
        try
        {
            xmlDoc = builder.parse(xmlFile);
        }
        catch(SAXException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.Settings.ReadContents()",
            "Parser exception during parsing file Contents.xml",
            e.getMessage());
        }
        catch(IOException e)
        {
            throw new DatabaseException("com.symbian.commdb.data.Settings.ReadContents()",
            "I/O exception during parsing file Contents.xml",
            e.getMessage());
        }
        
        // Vectorise the nodes
        
        Vector nodes = new Vector();
        XMLUtility.VectoriseChildrenNodes(xmlDoc.getDocumentElement(), nodes);
        
        // Find the name of this database
        
        for(int nodeIndex=0; nodeIndex<nodes.size(); nodeIndex++)
        {
            Node aNode = (Node)nodes.elementAt(nodeIndex);
            if(aNode.getNodeName().equals("commdb"))
            {
                m_commdbType = XMLUtility.LocateNodeAttribute(aNode,"type").getNodeValue();
                break;
            }
        }
        
        // Extract the table information
        
        String m_commdbSchemaFilename;
        String m_commdbTableName;
        String m_commdbTableTemplateName;
        String m_commdbEnumerationFilename;
        
        for(int nodeIndex=0; nodeIndex<nodes.size(); nodeIndex++)
        {
            Node aNode = (Node)nodes.elementAt(nodeIndex);
            
            // Find a node of type "element" with at least one attribute,
            // which may be a CommDB table
            
            if(aNode.getNodeName().equals("table"))
            {
                NamedNodeMap attribs = aNode.getAttributes();
                Node file   = attribs.getNamedItem("file");
                Node name   = attribs.getNamedItem("name");
                Node entry  = attribs.getNamedItem("entry");
                Node enumer = attribs.getNamedItem("enum");
                
                if(name == null || file == null || entry == null || enumer == null)
                    throw new DatabaseException("com.symbian.commdb.data.Settings.ReadContents()",
                    "Contents.xml not properly formatted",
                    "One or more attributes for a table not specified");
                
                m_commdbSchemaFilename = new String(file.getNodeValue());
                m_commdbTableName = new String(name.getNodeValue());
                m_commdbTableTemplateName = new String(entry.getNodeValue());
                m_commdbEnumerationFilename = new String(enumer.getNodeValue());
                
                AddXMLSchemaFileName(m_commdbSchemaFilename,
                m_commdbTableTemplateName,
                m_commdbTableName,
                m_commdbEnumerationFilename);
            }
        }
    }
    
    /**
     * This method creates a vector with the qualified names of the table entries
     * for all tables in the database and a corresponding vector with the table entries.
     *
     * @param qualNames The returned vector with the qualified names of all table entries.
     * @param tableEntries The returned vector with all the table entries.
     */
    public void TableEntryQualifiedNames(Vector qualNames, Vector tableEntries)
    {
        // Create a vector with the qualified names of the table
        // entries for all tables in the database
        
        qualNames.clear();
        tableEntries.clear();
        
        for(int i=0; i < m_tables.size(); i++)
        {
            Table table = (Table)m_tables.elementAt(i);
            
            for(int j=0; j < table.GetNumberOfTableEntries(); j++)
            {
                TableEntry entry = table.GetTableEntry(j);
                String qname     = entry.GetQualifiedName();
                
                qualNames.add(qname);
                tableEntries.add(entry);
            }
        }
    }
    
    /**
     * This method examines the table entry parameters which expect as a value
     * a table entry link, for all tables in the database, and finds all broken
     * references.
     *
     * @param brokenParams A vector with all the qualified names of all parameters
     * with invalid table entry references
     * @param brokenLinks A vector with all the qualified names of the invalid
     * table entries
     * @return True if the table entry references are all valid, false
     * in the opposite case.
     */
    public boolean FindBrokenTableEntryLinks(Vector brokenParams, Vector brokenLinks)
    {
        boolean ok = true;brokenParams.clear();
        brokenLinks.clear();
        
        // Create a vector with the qualified names of the table
        // entries for all tables in the database
        
        Vector qualNames    = new Vector();
        Vector tableEntries = new Vector();
        
        TableEntryQualifiedNames(qualNames,tableEntries);
        
        // Find invalid table entry links in the database
        
        for(int i=0; i < m_tables.size(); i++)
        {
            Table table = (Table)m_tables.elementAt(i);
            TableTemplate templ = table.GetTableTemplate();
            
            for(int j=0; j < table.GetNumberOfTableEntries(); j++)
            {
                TableEntry entry = table.GetTableEntry(j);
                
                for(int k=0; k < entry.GetNumberOfParameters(); k++)
                {
                    if(templ.GetIsTableEntryLink(k))
                    {
                        String linkName = entry.GetParameterValue(k);
                        
                        // If a table entry link is not supplied then there
                        // is nothing to do.
                        
                        if(linkName.indexOf(".") == -1)
                            continue;
                        
                        // If an unspecified table entry link is supplied
                        // and the table entry is not marked as a template
                        // then this represents a broken table entry link
                        // else it is ignored
                        
                        if(linkName.endsWith(".-1"))
                        {
                            if(!(entry.GetOperation().equals("template")))
                            {
                                brokenParams.add(entry.GetQualifiedParameterName(k));
                                brokenLinks.add(linkName);
                                ok = false;
                            }
                            continue;
                        }
                        
                        // Try to locate the table entry name in the database
                        
                        int loc = qualNames.indexOf(linkName);
                        
                        // The table entry reference cannot be resolved
                        
                        if(loc == -1)
                        {
                            brokenParams.add(entry.GetQualifiedParameterName(k));
                            brokenLinks.add(linkName);
                            ok = false;
                        }
                    }
                }
            }
        }
        
        return ok;
    }
    
    // Data Members
    
    /**
     * The array of CommDB tables forming this database
     */
    private Vector m_tables;
    
    /**
     * The directory which contains the XML Schema files for CommDB
     */
    protected String m_schemaDirectory;
    
    /**
     * The names of the XML Schema files for each table template in CommDB
     */
    private Vector m_fileNames;
    
    /**
     * The name of each table template as they are defined in CommDB
     */
    private Vector m_tableTemplateNames;
    
    /**
     * The name of each table in CommDB
     */
    private Vector m_tableNames;
    
    /**
     * The names of the files which contain the definition of the enumerated
     * types for each table template
     */
    private Vector m_enumerationFilenames;
    
    /**
     * The type of this CommDB database
     */
    private String m_commdbType;
}


