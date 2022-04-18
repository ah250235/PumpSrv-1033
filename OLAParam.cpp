#include "stdafx.h"
#include <stdarg.h>
#include "CStrFormat.h"
#include "FixedPointValue.h"
#include "OLAParam.h"

#define SZ_BUFFER_SIZE 512


OLAParamWriter::~OLAParamWriter() {
	// prevent the thread from getting out of the destructor using exception
	try {
		// null terminate the stream
		*this << std::ends;

		if (this->bad()) {
			LogStream LogMsg;
			LogMsg << __FUNCTION__
				<< " - ERROR : Buffer Overflow"
				<< ends;

			// Potentially show stopper
			_LOGMSG.LogMsg(LogMsg, LOG_LEVEL_0);
		}
	}
	catch (...) {
		;	// purposely left empty as code inside here can also throw an exception 
			//and cause a problem we are trying to avoid
	}
}


std::string OLAParamWriter::Format(const char* pFormat, ...)
{		
	char pBuffer[SZ_BUFFER_SIZE] = {0};
	constexpr size_t CurrentBufferSize = sizeof(pBuffer);
	int Count = 0;
	va_list args;
	va_start(args, pFormat);

	// if you get buffer too small assertion getting hit, increase buffer size
	//Count = vsprintf_s(pBuffer, CurrentBufferSize, Format, args);
	Count = vsnprintf(pBuffer, CurrentBufferSize, pFormat, args);

	// if data is larger than the buffer size, data truncated
	if ((size_t)Count > CurrentBufferSize) {
		assert(false);
	}
	va_end(args);

	std::string rVal = pBuffer;

	return rVal;
}

void OLAParamWriter::Add(const char* lVal, const char* rVal, bool bEndWithComma) {
	*this << lVal << "=" << rVal;
	if (bEndWithComma) {
		*this << ",";
	}
}

void OLAParamWriter::Add(const std::string& lVal,const std::string& rVal, bool bEndWithComma)
{
	Add(lVal.c_str(), rVal.c_str(), bEndWithComma);
}

void OLAParamWriter::Add(const std::string& lVal, long rVal, bool bEndWithComma)
{
	*this << lVal << "=" << rVal;
	if (bEndWithComma) {
		*this << ",";
	}
}

void OLAParamWriter::AddPrice(const  std::string& lVal, long lPrice, bool bEndWithComma)
{
	const long Dollar = lPrice / 1000;
	const long Cents = lPrice % 1000;

	// LVal=xxx.ccc
	// ccc is 3 digits 0 prefixed
	*this << lVal << "=" << Dollar << "." << std::setfill('0') << std::setw(3) << Cents;
	if (bEndWithComma) {
		*this << ",";
	}
}

void OLAParamWriter::Add(const  std::string& lVal, char* Format, ...)  {

	char Buffer[SZ_BUFFER_SIZE] = { 0 };
	constexpr size_t CurrentBufferSize = sizeof(Buffer);
	int Count = 0;
	va_list args;
	va_start(args, Format);

	// if you get buffer too small assertion getting hit, increase buffer size
	//Count = vsprintf_s(pBuffer, CurrentBufferSize, Format, args);
	Count = vsnprintf(Buffer, CurrentBufferSize, Format, args);

	// if data is larger than the buffer size, data truncated
	if ((size_t)Count > CurrentBufferSize) {
		assert(false);
	}
	va_end(args);

	//std::string rVal = Buffer;

	*this << lVal << "=" << Buffer << ",";
}


void OLAParamWriter::AddIfData(const char* lVal, const char* rVal, bool bEndWithComma) {
	if ((lVal[0] != 0) && (rVal[0] != 0))
		Add(lVal, rVal, bEndWithComma);
}

void OLAParamWriter::AddIfData(const std::string& lVal, const std::string& rVal, bool bEndWithComma) {
	if (lVal.length() > 0 && rVal.length() > 0) {
		Add(lVal, rVal, bEndWithComma);
	}
}






// put into C++ to remove foreign object reference
// so that I don't have to include the Fixed_Point_value header file
void OLAParamBuilder::AddPrice(const std::string& lVal, long lPrice) {
	mathext::Fixed_Point_Value Price(lPrice);

	char FormatBuffer[SZ_BUFFER_SIZE] = { 0 };
	sprintf_s(FormatBuffer, _countof(FormatBuffer), "%ld.%03ld",
		Price.GetDollars(),
		Price.GetCents());
	std::string srVal = FormatBuffer;
	Add(lVal, srVal);
}


void OLAParamBuilder::AddPrice(const std::string& lVal, const mathext::Fixed_Point_Value& Price) {
	char FormatBuffer[SZ_BUFFER_SIZE] = { 0 };
	sprintf_s(FormatBuffer, _countof(FormatBuffer), "%ld.%03ld",
		Price.GetDollars(),
		Price.GetCents());
	std::string srVal = FormatBuffer;
	Add(lVal, srVal);
}

