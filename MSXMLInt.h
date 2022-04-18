#ifndef _MSXMLInterface
#define _MSXMLInterface

#import "c:\windows\system32\msxml4.dll" raw_interfaces_only 

#define IXMLDOMElementPtr		MSXML2::IXMLDOMElementPtr
#define IXMLDOMAttributePtr		MSXML2::IXMLDOMAttributePtr
#define IXMLDOMNodeList			MSXML2::IXMLDOMNodeList


class CMSInterface
{
public:
	CMSInterface();
	~CMSInterface();

	BOOL InitDoc();
	BOOL LoadFile(LPCSTR cFileName);
	BOOL LoadXmlString(char* bstrStr);
	void CloseDoc();
	BOOL GetXmlString(BSTR*  bstrStr);
	BOOL SaveToFile(char * sFileName);
	BOOL SaveToFile(char * sFileName,_com_error & eError);
	
	long GetLastErr(char * bstrStr=NULL,long lSize=0);
	
	variant_t GetAttribute(IXMLDOMElementPtr pElem,char *sAttribName);
	BOOL SetAttribute(char *sAttributeName,variant_t vAttributeValue,IXMLDOMElementPtr pElem);

	BOOL AppendChild(IXMLDOMElementPtr SonElem,IXMLDOMElementPtr FatherElem);
	BOOL AppendRootElement(IXMLDOMElementPtr pElemPointer);


	IXMLDOMElementPtr CreateElement(char *sAttribName);
	IXMLDOMNodeListPtr selectNodes(IXMLDOMElementPtr ElemPtr,char *NodeName);

	IXMLDOMElementPtr SelectSingleElementNode(IXMLDOMElementPtr pElemPtr,char *sNodeName);
	IXMLDOMAttributePtr SelectSingleAttribNode(IXMLDOMElementPtr pElemPtr,char *sNodeName);
	
	IXMLDOMElementPtr GetRootPtr();
	IXMLDOMElementPtr GetRootToElement(); 
	BOOL SetCurrentRootPtr(char *sRootName,long lRootNumber, BOOL bConditionAttribute,char *sConditionAttributeName);
	
	IXMLDOMNodePtr CreateNode(char *sName);
	IXMLDOMNodePtr CreateChild( char *sChildName,  char* sValue );
	BOOL GetName(IXMLDOMElementPtr pNodePointer ,char *sName);
	variant_t GetChildNodeValue (IXMLDOMElementPtr pNodePointer);

private:
	IXMLDOMDocumentPtr	   m_pDocument; /*Document Pointer*/
	IXMLDOMElementPtr      m_pDocRoot,m_pCurrentDocRoot;

};




#endif