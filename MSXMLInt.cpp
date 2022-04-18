#include "stdafx.h"
#include "MSXMLInt.h" 
#include <stdio.h>

CMSInterface::CMSInterface()
{
	m_pDocument=NULL; /*Document Pointer*/
	m_pDocRoot = NULL;
	m_pCurrentDocRoot = NULL;

}

CMSInterface::~CMSInterface()
{
	CloseDoc();
}

BOOL CMSInterface::InitDoc()
{
	BOOL bRetCode = TRUE;
	HRESULT hr=0;
	try
	{
		hr = m_pDocument.CreateInstance(__uuidof(DOMDocument));
		if (FAILED(hr))
		{
			bRetCode =FALSE;
			 _com_error er(hr);
		}
	}
	catch(...)
	{
		bRetCode = FALSE;
	}
	return bRetCode;
}

BOOL CMSInterface::LoadFile(LPCSTR cFileName)
{
	BOOL bRetCode = TRUE;
	variant_t vResult(cFileName);

	try
	{	
		VARIANT_BOOL vbIsSuccessful = VARIANT_FALSE;
		
		vResult = m_pDocument->load(vResult, &vbIsSuccessful);

		bRetCode = (vbIsSuccessful == VARIANT_TRUE);

		if (bRetCode == TRUE)
		{
			IXMLDOMElement* pIXMLDOMElement = NULL;

			m_pDocument->get_documentElement(&pIXMLDOMElement);
			m_pDocRoot = pIXMLDOMElement;
			m_pCurrentDocRoot = m_pDocRoot;
		}
	}
	catch(...)
	{
		vResult.Clear();
		bRetCode = FALSE;
	}

	return bRetCode;
}


long CMSInterface::GetLastErr(char * bstrStr,long lSize)
{
	long lRetCode=0;
	
	try
	{
		IXMLDOMParseError* pError = NULL;
		m_pDocument->get_parseError(&pError);

		pError->get_errorCode(&lRetCode);
		if (lRetCode)
		{
			BSTR pBSTRReason = SysAllocString(L"Temp Value Len Only For Allocating ");
			pError->get_reason(&pBSTRReason);
			_bstr_t ShowString(pBSTRReason);
			if (bstrStr)
				memcpy(bstrStr,(const char *)ShowString,lSize);					//4.0.25.80 TD 142857
			SysFreeString (pBSTRReason);
		}
	}
	catch(...)
	{
	
	}

	return lRetCode;
}

BOOL CMSInterface::LoadXmlString(char * bstrStr)
{
	BOOL bRetCode = TRUE;
	CComBSTR bstr(bstrStr);
	try
	{
		VARIANT_BOOL vbIsSuccessful = VARIANT_FALSE;
		m_pDocument->loadXML(bstr.m_str, &vbIsSuccessful);
		bRetCode = (vbIsSuccessful == VARIANT_TRUE);

		if (bRetCode == TRUE)
		{
			IXMLDOMElement* pIXMLDOMElement = NULL;

			m_pDocument->get_documentElement(&pIXMLDOMElement);

			m_pDocRoot = pIXMLDOMElement;			
			m_pCurrentDocRoot = m_pDocRoot;
		}
	}
	catch(...)
	{
		bRetCode = FALSE;
	}
	bstr.Empty();//4.0.15.285

	return bRetCode;
}

BOOL CMSInterface::GetXmlString(BSTR*  bstrStr)
{
	BOOL bRetCode = TRUE;

	try
	{
		if (m_pDocument->get_xml(bstrStr) != S_OK)
		{
			bRetCode = FALSE;
		}
	}
	catch(...)
	{
		bRetCode = FALSE;
	}

	return bRetCode;
}
	
void CMSInterface::CloseDoc()
{
	if (m_pDocument)
	{
		m_pDocument.Release();
		m_pDocument.Detach();
		m_pDocument = NULL;
	}
}

variant_t CMSInterface::GetAttribute(IXMLDOMElementPtr pElem,char *sAttribName)
{
	variant_t cRetCode;
	char * str = new char[strlen(sAttribName)+2];
	sprintf(str,"@%s",sAttribName);

	IXMLDOMAttributePtr cCheckIfAttributeExist;
	cCheckIfAttributeExist = SelectSingleAttribNode(pElem,str);
	if (cCheckIfAttributeExist)
	{
		try
		{
			CComBSTR cwsAttribName(sAttribName);
			pElem->getAttribute(cwsAttribName, &cRetCode);
		}
		catch(...)
		{
			cRetCode.Clear();
		}
	}
	else
	{
		cRetCode.Clear();
	}
	delete []str;
	return cRetCode;
}
	
IXMLDOMElementPtr CMSInterface::SelectSingleElementNode(IXMLDOMElementPtr pElemPtr,char *sNodeName)
{
	IXMLDOMElementPtr Element= NULL;
	try
	{
		CComBSTR cwsNodeName(sNodeName);

		MSXML2::IXMLDOMNode* pIXMLDOMNode = NULL;
		pElemPtr->selectSingleNode(cwsNodeName.m_str, &pIXMLDOMNode);
		
		Element = pIXMLDOMNode;
	}
	catch (...)
	{
		Element = NULL;
	}
	return Element;
}


IXMLDOMAttributePtr CMSInterface::SelectSingleAttribNode(IXMLDOMElementPtr pElemPtr,char *sNodeName)
{
	IXMLDOMAttributePtr Attrib= NULL;

	try
	{
		CComBSTR cwsNodeName(sNodeName);

		MSXML2::IXMLDOMNode* pIXMLDOMNode = NULL;
		pElemPtr->selectSingleNode(cwsNodeName.m_str, &pIXMLDOMNode);
		
		Attrib = pIXMLDOMNode;
	}
	catch (...)
	{
		Attrib = NULL;
	}
	return Attrib;
}

BOOL CMSInterface::SetCurrentRootPtr(char *sRootName,long lRootNumber, BOOL bConditionAttribute,char *sConditionAttributeName)
{
	BOOL bRetCode = FALSE; 
	IXMLDOMElementPtr Element= NULL;
	char cRootNumber[50];
	BOOL bGetOut=FALSE;
	do 
	{
		sprintf(cRootNumber,"%s[@Id =\"%ld\"]",sRootName,lRootNumber);
		Element= SelectSingleElementNode(m_pDocRoot,cRootNumber);
		variant_t vTempVar = GetAttribute(Element,sConditionAttributeName);
		// in order to defend from cases where the attribute not exist.
		// the returned value is EMPTY therefore we cann't ask on the returned value.
		BOOL bTmpCondition;
		if (vTempVar.vt != VT_EMPTY)
		{
			// the values are read as char and not as BOOL value
			bTmpCondition = (*vTempVar.pbVal) - '0'; 
		}
		else
		  bTmpCondition = FALSE;

		if ((bTmpCondition) && (!bConditionAttribute))
		{
			lRootNumber++;
			bGetOut = FALSE;
		}
		else
		{
			bGetOut = TRUE;
		}

	} while(!bGetOut);
	
	if (Element)
	{
		m_pCurrentDocRoot = Element;
		bRetCode = TRUE;
	}
	return bRetCode;
}

BOOL CMSInterface::SetAttribute(char *sAttributeName,variant_t vAttributeValue,IXMLDOMElementPtr pElem)
{
	BOOL bRetCode = TRUE;
	try
	{
		CComBSTR cwsAttributeName(sAttributeName);
		if (pElem->setAttribute(cwsAttributeName.m_str, vAttributeValue) != S_OK)
		{
			bRetCode = FALSE;
		} 
	}
	catch(...)
	{
		bRetCode = FALSE;
	}
	return bRetCode;
}


BOOL CMSInterface::AppendChild(IXMLDOMElementPtr SonElem,IXMLDOMElementPtr FatherElem)
{
	BOOL bRetCode = TRUE;
	MSXML2::IXMLDOMNode* pTmp = NULL; 
	try
	{
		FatherElem->appendChild(SonElem, &pTmp); 
			
		SonElem.Release();//4.0.15.285

		if (pTmp)
		{
			bRetCode = TRUE;
		}
		else
		{
			bRetCode = FALSE;
		}
	}
	catch(...)
	{
		bRetCode = FALSE;
	}
	return bRetCode;
}

BOOL CMSInterface::AppendRootElement(IXMLDOMElementPtr pElemPointer)
{
	BOOL bRetCode = TRUE;
	IXMLDOMNode* pTmp = NULL; 

	try
	{
		MSXML2::IXMLDOMNode* pIXMLDOMNode = pElemPointer; 
		m_pDocument->appendChild((IXMLDOMNode*)pIXMLDOMNode, &pTmp); 

		pElemPointer.Release();//4.0.15.285

		if (pTmp)
		{
			bRetCode = TRUE;
		}
		else
		{
			bRetCode = FALSE;
		}
	}
	catch(...)
	{
		bRetCode = FALSE;
	}
	return bRetCode;
}


IXMLDOMElementPtr CMSInterface::CreateElement(char *sAttribName)
{
	IXMLDOMElementPtr Element= NULL;

	try
	{
		CComBSTR			cwsAttribName(sAttribName);
		IXMLDOMElement*		pElement= NULL;

		m_pDocument->createElement(cwsAttribName.m_str, &pElement);
		Element = pElement;
	}
	catch(...)
	{
		Element=NULL;
	}
	
	return Element;
}

IXMLDOMNodeListPtr CMSInterface::selectNodes(IXMLDOMElementPtr ElemPtr,char *NodeName)
{
	IXMLDOMNodeListPtr pIXMLDOMList =NULL;
	try
	{
		CComBSTR	cwsNodeName(NodeName);
		IXMLDOMNodeList* pIXMLDOMListRes = NULL;

		ElemPtr->selectNodes(cwsNodeName.m_str, &pIXMLDOMListRes);

		long lLength = 0;
		pIXMLDOMListRes->get_length(&lLength);

		if (lLength > 0)
			pIXMLDOMList = pIXMLDOMListRes;
	}
	catch (...)
	{
		pIXMLDOMList = NULL;
	}
	return pIXMLDOMList;
}

BOOL CMSInterface::SaveToFile(char * sFileName)
{
	BOOL bRetCode =TRUE;
	try
	{
		_bstr_t xmlString;
		m_pDocument->get_xml(&xmlString.GetBSTR());
		variant_t vsFileName(sFileName);

		m_pDocument->save(vsFileName);
	}
	catch (...)
	{
		bRetCode = FALSE;
	}
	return bRetCode;

}
/******************************************************************************
 Description:	 Create Node	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			4/12/2003   11:49		Start
******************************************************************************/

IXMLDOMNodePtr CMSInterface::CreateNode(char *sName)
{
	IXMLDOMNodePtr Node= NULL;

	try
	{
		variant_t vsType("element");
		CComBSTR cwsFileName(sName);
		CComBSTR cwsEmpty(L"");
		IXMLDOMNode* pNodeRes = NULL;

		Node = m_pDocument->createNode(vsType, cwsFileName.m_str, cwsEmpty, &pNodeRes);
	}
	catch(...)
	{
		Node = NULL;
	}
	
	return Node;
}

/******************************************************************************
 Description:	 Create Child	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			4/12/2003   11:49		Start
******************************************************************************/

IXMLDOMNodePtr CMSInterface::CreateChild(char *sChildName, char* sValue )
{
	IXMLDOMNodePtr pChild = NULL;
	_bstr_t bstrT(sValue);

	try
	{
		pChild = CreateNode(sChildName);
		pChild->put_text(bstrT);    
	}
	catch(...)
	{
		pChild=NULL;
	}
	
	return pChild;
}


IXMLDOMElementPtr CMSInterface::GetRootToElement()
{
	IXMLDOMElement* pIXMLDOMElement = NULL;
	m_pDocument->get_documentElement(&pIXMLDOMElement);

	return pIXMLDOMElement;
}

/******************************************************************************
 Description:		Retrieve a node name.	
 Returns:			BOOL bRetCode - Success FALSE - FAILED
 Parameters:   		sName - the name of the node (out)
					pNodePointer - pointer to the node that we want to retrieve its name. 
					
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ariel		20/10/2005   17:40		Start
******************************************************************************/
BOOL CMSInterface::GetName(IXMLDOMElementPtr pNodePointer ,char *sName)
{	
	BOOL bRetCode = TRUE;
	try
	{
		CComBSTR cwsNodeName(L"");

		if (pNodePointer == NULL)
		{			
			m_pCurrentDocRoot->get_nodeName(&cwsNodeName.m_str);
		}
		else
			pNodePointer->get_nodeName(&cwsNodeName.m_str);

		Bstr2Buffer(cwsNodeName.m_str, sName, cwsNodeName.Length());
	}
	catch(...)
	{
		memcpy (sName,"no_name",strlen ("no_name"));
		bRetCode = FALSE;
	}
	return bRetCode;
}

/******************************************************************************
 Description:		Retrieve a node value.	
 Returns:			variant_t cRetCode - the value of the node.
 Parameters:   		pNodePointer - pointer to the node that we want to retrieve its value. 
					
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Ariel		8/11/2005   17:40			Start
******************************************************************************/
variant_t CMSInterface::GetChildNodeValue (IXMLDOMElementPtr pNodePointer)
{
	variant_t cRetCode;
	IXMLDOMElementPtr Element= NULL;	
	try
	{
		CComBSTR cwsNodeName(L"");
		cRetCode = pNodePointer->get_text(&cwsNodeName.m_str);

		cRetCode = cwsNodeName.Copy();
	}
	catch(...)
	{
		cRetCode.Clear();
	}
		
	return cRetCode;
}

IXMLDOMElementPtr CMSInterface::GetRootPtr()
{
	return m_pCurrentDocRoot;
}

/******************************************************************************
 Description:	 SaveToFile with returning the com exception	
 Returns:      	
 Parameters:   	
 
 WHO			WHEN                    WHAT
-------------------------------------------------------------------------------
 Amit H			20/07/2008   11:49		Start
******************************************************************************/

BOOL CMSInterface::SaveToFile(char * sFileName,_com_error & eError)
{
		
	BOOL bRetCode =TRUE;
	try
	{
		_bstr_t xmlString;
		m_pDocument->get_xml(&xmlString.GetBSTR());
		
		variant_t vsFileName(sFileName);

		m_pDocument->save(vsFileName);
	}
	catch(_com_error & e )
	{
		eError = e;
		bRetCode = FALSE;
	}
	catch (...)
	{
		bRetCode = FALSE;
	}
	return bRetCode;

}