// TankXML.h: interface for the CTankXML class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TANKXML_H__9E74B12C_A4A7_4676_8BBD_EC35662DC9D8__INCLUDED_)
#define AFX_TANKXML_H__9E74B12C_A4A7_4676_8BBD_EC35662DC9D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//4.0.13.503 


class CTankXML  
{
public:
	CTankXML();
	virtual ~CTankXML();

	void TankReadingInfo2Xml(TANK_READ_INFO* info, BSTR* pRec);
	void Xml2TankReadingInfo(BSTR* pRec, TANK_READ_INFO* info);

	//4.0.13.504
	void Xml2DeliveryInfo(BSTR* pRec, DELIVERY_INFO* info);
	void DeliveryInfo2Xml(DELIVERY_INFO* info, BSTR* pRec);

	//4.0.21.500 TD 66935
	void TankReadingInfoDeliveryXml(CTank* info, BSTR* pRec, BOOL bAddReadings, BOOL bAddDeliveryIndication);
	long GetDensityFromXML(BSTR & bstrXml);
	void TankStateXml(CTank* info, BSTR * pRec);		//	4.0.25.1130 
	void AllTanksStateXml(BSTR * pRec);				//	4.0.25.1130 
private:
	long SetReadingType(short nFlags);//CR_475695 
};

#endif // !defined(AFX_TANKXML_H__9E74B12C_A4A7_4676_8BBD_EC35662DC9D8__INCLUDED_)
