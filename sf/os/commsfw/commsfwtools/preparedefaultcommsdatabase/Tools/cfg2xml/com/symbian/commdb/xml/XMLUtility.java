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

package com.symbian.commdb.xml;

import java.util.Vector;

import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * This class is used as a container for a number of static utility functions
 * used for the manipulation of XML elements parsed by a DOM parser
 */
public class XMLUtility
{
    /**
     * It sends to the console output stream a node and all its children.
     * This method should be used for testing/debugging purposes only.
     *
     * @param node The node to output
     * @param indent The identation string to use for the output
     */
    public static void OutputXMLNodes(Node node, String indent)
    {
        // Get the name and type of the node
        
        String nodeName = node.getNodeName();
        short type      = node.getNodeType();
        String typeName = new String();
        
        switch(type)
        {
            case Node.ATTRIBUTE_NODE:
                typeName = "ATTRIBUTE_NODE";
                break;
            case Node.CDATA_SECTION_NODE:
                typeName = "CDATA_SECTION_NODE";
                break;
            case Node.COMMENT_NODE:
                typeName = "COMMENT_NODE";
                break;
            case Node.DOCUMENT_FRAGMENT_NODE:
                typeName = "DOCUMENT_FRAGMENT_NODE";
                break;
            case Node.DOCUMENT_NODE:
                typeName = "DOCUMENT_NODE";
                break;
            case Node.DOCUMENT_TYPE_NODE:
                typeName = "DOCUMENT_TYPE_NODE";
                break;
            case Node.ELEMENT_NODE:
                typeName = "ELEMENT_NODE";
                break;
            case Node.ENTITY_NODE:
                typeName = "ENTITY_NODE";
                break;
            case Node.ENTITY_REFERENCE_NODE:
                typeName = "ENTITY_REFERENCE_NODE";
                break;
            case Node.NOTATION_NODE:
                typeName = "NOTATION_NODE";
                break;
            case Node.PROCESSING_INSTRUCTION_NODE:
                typeName = "PROCESSING_INSTRUCTION_NODE";
                break;
            case Node.TEXT_NODE:
                typeName = "TEXT_NODE";
                break;
            default:
                typeName = "UNKNOWN_NODE";
        }
        
        System.out.println(indent + nodeName + " Node, type is "
        + typeName + ";");
        System.out.println(indent + " " + node);
        
        // Get the attributes of the node, if any
        
        if(node instanceof Element && node.hasAttributes())
        {
            System.out.println(indent + "Attributes of " + nodeName + " are:");
            NamedNodeMap attribs = node.getAttributes();
            for(int i=0; i<attribs.getLength(); i++)
            {
                Node at = attribs.item(i);
                System.out.println(at.getNodeName() +
                " is an attribute with value " +
                at.getNodeValue());
            }
        }
        
        // Get the children of the node, if any
        
        NodeList list = node.getChildNodes();
        if(list.getLength() > 0)
        {
            System.out.println(indent + "Child Nodes of " + nodeName + " are:");
            for(int i=0; i<list.getLength(); i++)
            {
                OutputXMLNodes(list.item(i),indent + " ");
            }
        }
    }
    
    /**
     * It creates a vector with contents all the children elements of a node
     * (without the node itself)
     *
     * @param node The node to process
     * @param elements The vector to use for storing the elements
     */
    public static void VectoriseChildrenNodes(Node node, Vector elements)
    {
        NodeList list = node.getChildNodes();
        if(list.getLength() > 0)
            for(int i=0; i<list.getLength(); i++)
            {
                elements.add(list.item(i));
                VectoriseChildrenNodes(list.item(i),elements);
            }
    }
    
    /**
     * It creates a vector with contents all the first children of a node
     * (without the node itself)
     */
    public static void VectoriseFirstChildrenNodes(Node node, Vector elements)
    {
        NodeList list = node.getChildNodes();
        if(list.getLength() > 0)
            for(int i=0; i<list.getLength(); i++)
                elements.add(list.item(i));
    }
    
    /**
     * It locates all child element of a node with a given attribute name
     *
     * @param node The node to process
     * @param attributeName The name of the attribute to locate
     * @param nodes The located nodes
     */
    public static void LocateNodesFromAttribute(Node node, String attributeName,
    Vector nodes)
    {
        Vector children = new Vector();
        VectoriseChildrenNodes(node,children);
        
        for(int i=0; i<children.size(); i++)
        {
            Node child = (Node)children.elementAt(i);
            if(child instanceof Element && child.hasAttributes())
            {
                NamedNodeMap attribs = child.getAttributes();
                for(int k=0; k<attribs.getLength(); k++)
                {
                    Node at = attribs.item(k);
                    String name = at.getNodeName();
                    if(name.equals(attributeName))
                    {
                        nodes.add(child);
                        break;
                    }
                }
            }
        }
    }
    
    /**
     * It locates all child elements of a node with a given attribute name
     * and a specific attribute value
     *
     * @param node The node to process
     * @param attributeName The name of the attribute to locate
     * @param nodes The located nodes
     */
    public static void LocateNodesFromAttribute(Node node, String attributeName,
    String attributeValue, Vector nodes)
    {
        Vector children = new Vector();
        VectoriseChildrenNodes(node,children);
        
        if(nodes == null)
            nodes = new Vector();
        
        for(int i=0; i<children.size(); i++)
        {
            Node child = (Node)children.elementAt(i);
            if(child instanceof Element && child.hasAttributes())
            {
                NamedNodeMap attribs = child.getAttributes();
                for(int k=0; k<attribs.getLength(); k++)
                {
                    Node at = attribs.item(k);
                    String name = at.getNodeName();
                    if(name.equals(attributeName))
                    {
                        if(at.getNodeValue().equals(attributeValue))
                        {
                            nodes.add(child);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    /**
     * It locates all child elements of a node with a specific node name
     *
     * @param node The node to process
     * @param name The name of the nodes to locate
     * @param nodes The located nodes
     */
    public static void LocateNodesFromName(Node node, String name, Vector nodes)
    {
        Vector children = new Vector();
        VectoriseChildrenNodes(node,children);
        
        if(nodes == null)
            nodes = new Vector();
        
        for(int i=0; i<children.size(); i++)
        {
            Node child = (Node)children.elementAt(i);
            if(child instanceof Element)
                if(child.getNodeName().equals(name))
                    nodes.add(child);
        }
    }
    
    /**
     * It locates the attribute of a node given its name
     *
     * @param node The node to process
     * @param name The name of the attribute to locate
     * @return The located attribute
     */
    public static Node LocateNodeAttribute(Node node, String name)
    {
        if(node.hasAttributes())
        {
            NamedNodeMap attribs = node.getAttributes();
            return attribs.getNamedItem(name);
        }
        else
            return null;
    }
}
