#ifndef __STATIC_STRING_TEMPLATE_H_
#define __STATIC_STRING_TEMPLATE_H_

//////////////////////////////////////////////////////////////////////////
//TStaticString is a template class, encapsulating static strings
//for easy use.  All allocations here are static, which means that
//when the class is cleared from the call stack, all the members
//are cleared as well.
//Note that each time you declare a template class with a 
//different size, you are effectively declaring a different data type. 
/////////////////////////////////////////////////////////////////////////

#define STATIC_STRING_8            8
#define STATIC_STRING_16           16
#define STATIC_STRING_32           32
#define STATIC_STRING_64           64
#define STATIC_STRING_128          128
#define STATIC_STRING_256          256
#define STATIC_STRING_512          512
#define STATIC_STRING_1024         1024
#define STATIC_STRING_2048         2048

template<int iSize> class TStaticString
{
public:
	TStaticString() { memset((char*)m_sStaticStr , 0x00 , iSize); }
	~TStaticString() {}

	unsigned int Size()	{ return sizeof(m_sStaticStr); }

	void Clear() { memset((char*)m_sStaticStr , 0x00 , iSize); }

	operator char*() { return (char*)m_sStaticStr; }

	void operator=(char * sStrToCopy) 
	{
		memset((char*)m_sStaticStr , 0x00 , iSize);
		memcpy((char*)m_sStaticStr , sStrToCopy , min((iSize - 1) , strlen(sStrToCopy))); 
	}

private:
	char m_sStaticStr[iSize];
};

#endif