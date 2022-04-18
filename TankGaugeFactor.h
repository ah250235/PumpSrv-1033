// TankGaugeFactor.h: interface for the CTankGaugeFactor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TANKGAUGEFACTOR_H__70327C08_B0B2_4D62_8A32_94161F391B73__INCLUDED_)
#define AFX_TANKGAUGEFACTOR_H__70327C08_B0B2_4D62_8A32_94161F391B73__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CTankGaugeFactor  
{
public:
	void SetFuelLevelFactor(long lFactor);
	long GetFuelLevelFactor();
	void SetTemperatureFactor(long lFactor);
	long GetTemperatureFactor();
	void SetVolumeFactor(long lFactor);
	long GetVolumeFactor();
	CTankGaugeFactor();
	virtual ~CTankGaugeFactor();
private:
	long m_lVolume;   
	long m_lTemperature;
	long m_lFuelLevel; 
};

#endif // !defined(AFX_TANKGAUGEFACTOR_H__70327C08_B0B2_4D62_8A32_94161F391B73__INCLUDED_)
