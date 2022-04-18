//class the implement the insert and read xml from QDX
//4.0.9.503
#ifndef _CONVERT_XML_H
#define _CONVERT_XML_H


class CConvertXML
{
public:
	CConvertXML();
	long InsertXMLReceptIntoOlaTempQDX(BSTR & bstrInXML,CPumpTransact & trs,long lPumpNumber);
	long ReadXMLFromQDX(CComBSTR &  bstrOutXML,CPumpTransact & trs,long lPumpNumber); //4.0.12.501

};


#endif //_CCONVERT_XML_H
