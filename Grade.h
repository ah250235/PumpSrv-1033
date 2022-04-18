//
//4.0.9.506 //amram
#ifndef _GRADE_
#define _GRADE_

#define GRADE_NAME 30




class CGrade 
{
public:
	CGrade();
	~CGrade();

	inline void SetGradeNumber(long lGradeNum) {m_lGradeNumber = lGradeNum;};
	inline void SetGradeName(char * szGradeName) {strcpy_s(m_sGradeName,szGradeName);};
	inline void SetGradeValid(BOOL bValid){m_bIsValid = bValid;};

	inline long	  GetGradeNamber(){return m_lGradeNumber;};
	inline BOOL   IfGradeValid() {return m_bIsValid;};
	inline char * GetGradeName(){return m_sGradeName;};
	

private:
	long	m_lGradeNumber;
	BOOL	m_bIsValid;
	char    m_sGradeName[GRADE_NAME];



};
//class that hold for each pump all the grades that belongs to the pump
/*class PumpGrade
{
	

private:
	CGrade	m_GradeInfo[MAX_POSITIONS_PER_PUMP];
	long	m_lPumpNumber;



};
*/


#endif //_GRADE_