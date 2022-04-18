#include "stdafx.h"
#include "TreeFlowChart.h"

#define PAYMENT_CARD "Payment"
#define CARWASH "CarWash"
#define PRE_AUTHORIZE_SCREEN "PreAuthorizeScreen"
#define AUTHOIRZE_PAYMENT    "AuthorizePayment"
#define ALT_ID_NUMBER		"AltId"
#define METHOD_OF_PAYMENT	"MethodOfPayment"
#define DEFERRAL			"Deferral"

CTreeFlowChart::CTreeFlowChart(void)
{
	BOOL bRetCode = TRUE;
	char sTmpStr[100];
	_Module.m_server.m_cParam.LoadParam("FlowControl", "DynamicPapFlowXmlPath", (char *)sTmpStr, sizeof(sTmpStr), "", TRUE);	//4.0.20.10

	
	m_sXmlPathAndName = sTmpStr; 
	m_sXmlPathAndName +=  "\\DynamicPapFlow.xml";

	bRetCode = m_XmlFlowObject.InitDoc();

	if (bRetCode == TRUE)
	{
		bRetCode = m_XmlFlowObject.LoadFile(m_sXmlPathAndName.GetBuffer(0));

		if (!bRetCode)
		{
			if (DEFAULT_LOG_LEVEL)
			{
				CString sMsg;
				sMsg.Format("CTreeFlowChart::CTreeFlowChart Failed to load xml from file %s", m_sXmlPathAndName);
				_LOGMSG.LogMsg(sMsg);
			}
		}
	}
}


CTreeFlowChart::~CTreeFlowChart(void)
{
	m_CurrentNodePtr.Clear();
	m_XmlFlowObject.CloseDoc();
}

short CTreeFlowChart::GetNextState(long lPumpNumber, long lLastInput, TERMINAL_CONFIGURATION & cTerminalConfiguration)
{
	CXMLPointers pChildNode, pCurrentElement;
	short OlaState = 0;
	long lCurrentAltId, lCurrentPayment, lNextPayment = 0, lNextAltId = 0, lMethodOfPayment = 0, lDeferral = 0; // EyalP: RFUEL-1087
	BOOL bMethodOfPayment = FALSE;
	GetCurrentCardsCounters(lLastInput, lCurrentAltId, lCurrentPayment);

	int iNumberOfLoops = 1;
	while (iNumberOfLoops > 0)
	{
		m_XmlFlowObject.GetChildNodesList(&m_CurrentNodePtr, &pChildNode);

		if (pChildNode.m_pMSListOfItems)
		{
			while ((m_XmlFlowObject.GetNextItem(pChildNode, pCurrentElement)))
			{
				lNextAltId = m_XmlFlowObject.GetAttribute(ALT_ID_NUMBER, &pCurrentElement);

				lNextPayment = m_XmlFlowObject.GetAttribute(PAYMENT_CARD, &pCurrentElement);

				lMethodOfPayment = m_XmlFlowObject.GetAttribute(METHOD_OF_PAYMENT, &pCurrentElement);

				if (lCurrentAltId == lNextAltId && lCurrentPayment == lNextPayment)
				{
					m_CurrentNodePtr = pCurrentElement;

					break;
				}
			}
		}

		iNumberOfLoops--;
		if (iNumberOfLoops > 0)
			GetCurrentCardsCounters(lLastInput, lCurrentAltId, lCurrentPayment);
	}

	if (lMethodOfPayment)
	{
		OlaState = OLA_LOYALTY_METHOD_OF_PAYMENT;
	}
	else if (lNextAltId == 1 && lNextPayment == 0)
	{
		OlaState = OLA_LOYALTY_ALTERNATE_ID;
	}
	else if (lNextAltId == 0 && lNextPayment == 1)
	{
		OlaState = OLA_LOYALTY_SWIPE_PAYMENT;
	}
	else
	{
		OlaState = OLA_LOYALTY_START_FUELING;
	}

	GetCurrentTerminalConfiguration(cTerminalConfiguration);

	CString strMsg;
	strMsg.Format( "CTreeFlowChart::GetNextState lMethodOfPayment = %ld , lNextAltId = %ld , lNextPayment = %ld , OlaState = %s , lLastInput = %ld", lMethodOfPayment, lNextAltId, lNextPayment, _LOGMSG.GetStateString(OlaState), lLastInput);
	_LOGMSG.LogMsg(strMsg);

	return OlaState;
}

BOOL CTreeFlowChart::IsValidInput(long lInput)
{
	CString attributeToSerach = ConvertInputToAttributeName(lInput);

	long lResult = m_XmlFlowObject.GetAttribute(attributeToSerach.GetBuffer(0),&m_CurrentNodePtr);

	if(lResult > 0)
		return TRUE;

	return FALSE;
}

void CTreeFlowChart::Init()
{
	//m_CurrentNodePtr.Clear();
	bool rootElemntFound = m_XmlFlowObject.GetRootToElement(&m_CurrentNodePtr);
	if (!rootElemntFound)
		_LOGMSG.LogMsg("CTreeFlowChart::Init GetRootToElement failed", LOG_LEVEL_1); // EyalP: RFUEL-1087
}

void CTreeFlowChart::GetCurrentTerminalConfiguration(TERMINAL_CONFIGURATION & cTerminalConfiguration)
{
	cTerminalConfiguration.sMessage = m_XmlFlowObject.GetAttribute("Message",&m_CurrentNodePtr);

	cTerminalConfiguration.nSettings = 0;
	if(m_XmlFlowObject.GetAttribute("CardReader",&m_CurrentNodePtr))
		cTerminalConfiguration.nSettings |= ONMP_OpenCardReader;
	if(m_XmlFlowObject.GetAttribute("ScannerMessage",&m_CurrentNodePtr))
		cTerminalConfiguration.nSettings |= ONMP_OpenScanner;
	if(m_XmlFlowObject.GetAttribute("NumericKeyboard",&m_CurrentNodePtr))
		cTerminalConfiguration.nSettings |= ONMP_OpenNumericKeys;
	if(m_XmlFlowObject.GetAttribute("YesNo",&m_CurrentNodePtr))
		cTerminalConfiguration.nSettings |= ONMP_OpenYesNoKeys;
	if(m_XmlFlowObject.GetAttribute("EnterCancel",&m_CurrentNodePtr))
		cTerminalConfiguration.nSettings |= ONMP_OpenEnterCancelKeys;
	if(m_XmlFlowObject.GetAttribute("PayInside",&m_CurrentNodePtr))
		cTerminalConfiguration.nSettings |= ONMP_OpenPayInsideKey;
	if (m_XmlFlowObject.GetAttribute("Contactless", &m_CurrentNodePtr))  // TD 460484 [7/28/2016 MaximB]
		cTerminalConfiguration.nSettings |= ONMP_OpenContaless;


	cTerminalConfiguration.lScreenNumber  =m_XmlFlowObject.GetAttribute("ScreenNumber",&m_CurrentNodePtr);

}

CString CTreeFlowChart::ConvertInputToAttributeName( long lInput )
{
	if (lInput == CARD_TYPE_LOYALTY)
	{
		return "AltId";
	}

	else if (_Module.m_server.m_cCommonFunction.IsPaymentCard(lInput))				// 4.0.32.??? TD 405435
	{
		return "Payment";
	}

	return "null";
}

void CTreeFlowChart::GetCurrentCardsCounters(long lLastInput, long & lCurrentAltId, long &lCurrentPayment)
{
	lCurrentAltId = m_XmlFlowObject.GetAttribute(ALT_ID_NUMBER, &m_CurrentNodePtr);

	lCurrentPayment = m_XmlFlowObject.GetAttribute(PAYMENT_CARD, &m_CurrentNodePtr);

	CString sLastInputAttribute = ConvertInputToAttributeName(lLastInput);

	if (0 == sLastInputAttribute.Compare(ALT_ID_NUMBER) && lCurrentAltId > 0)// && lCurrentPPT > 0)
		lCurrentAltId--;

	else if (0 == sLastInputAttribute.Compare(PAYMENT_CARD) && lCurrentPayment > 0)// && lCurrentPayment > 0)
		lCurrentPayment--;
}

short CTreeFlowChart::MoveStatesAfterPaymentCardSwipe(long lPumpNumber)
{
	long lPumpIndex = lPumpNumber - 1;
	short OlaState;
	CXMLPointers pChildNode, pCurrentElement;

	m_XmlFlowObject.GetChildNodesList(&m_CurrentNodePtr, &pChildNode);

	m_XmlFlowObject.GetNextItem(pChildNode, pCurrentElement);

	variant_t bAuthoirzePayment = m_XmlFlowObject.GetAttribute(AUTHOIRZE_PAYMENT, &pCurrentElement);

	if (bAuthoirzePayment)
	{
		OlaState = OLA_LOYALTY_AUTHORIZE_CARD;
	}
	else
	{
		OlaState = OLA_LOYALTY_START_FUELING;
	}

	m_CurrentNodePtr = pCurrentElement;

	return OlaState;
}