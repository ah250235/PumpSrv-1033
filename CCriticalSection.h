#ifndef INC_CCRITSEC_H
#define INC_CCRITSEC_H

class CCritSec
{
public:
    CCritSec()
        { ::InitializeCriticalSection(&m_rep); }
    ~CCritSec()
        { ::DeleteCriticalSection(&m_rep); }
    
    void Enter()
        { ::EnterCriticalSection(&m_rep); }
	BOOL TryEnter()
		{ return  ::TryEnterCriticalSection(&m_rep); }
    void Leave()
        { ::LeaveCriticalSection(&m_rep); }
    
private:
    // copy ops are private to prevent copying
    CCritSec(const CCritSec&);
    CCritSec& operator=(const CCritSec&);
    
    CRITICAL_SECTION m_rep;
};

class CCSLock
{
public:
    CCSLock(CCritSec& a_section)
        : m_section(a_section) { m_section.Enter(); }
    ~CCSLock()
        { m_section.Leave(); }
    
private:
    // copy ops are private to prevent copying
    CCSLock(const CCSLock&);
    CCSLock& operator=(const CCSLock&);
    
    CCritSec& m_section;
};

class CSmartCSLock
{
public:
    CSmartCSLock(CCritSec& a_section, BOOL& bResult)
        : m_bInsideCS(FALSE), m_section(a_section) { bResult = m_bInsideCS = m_section.TryEnter(); }
    ~CSmartCSLock()
	{ 
		if (m_bInsideCS) 
			m_section.Leave(); 
	}
    
private:
    // copy ops are private to prevent copying
    CSmartCSLock(const CSmartCSLock&);
    CSmartCSLock& operator=(const CSmartCSLock&);
    
    CCritSec& m_section;
	BOOL	  m_bInsideCS;
};

#endif //INC_CCRITSEC_H