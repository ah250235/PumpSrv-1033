#if !defined(___OLA_PARAM_H___)
#define ___OLA_PARAM_H___


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <utility>
#include <stdarg.h>
#include <string>
#include <vector>
#include "CStrFormat.h"
#include "StringHelpers.h"
#include "FixedPointValue.h"


//
//  Object for constructing OLA parameters
//	using stream, the client code don't need to keep track of the 
//  index/offset into the buffer.
//  The object auto terminate the buffer with null at destruction time
//  MemoryStream::length() was added to find the offset location if necessary
//		when the code must be mixed with the existing functions
//	Destructor checks and logs buffer overflow
//
//  Ex:
//		char* pStartingAddr = buffer + offset;
//		size_t BufferSize   = sizeof(buffer) - offset;
//		OLAParamWriter  stream(bpStartingAddr, BufferSize);
//
//		// Use as C++ ostream, end user is responsible for adding ,
//		stream << "Data=12,";
//
//		// Add data
//		Stream.Add("Data", 12);
//
//		Stream.AddPrice("Data", 12551);
//
//		// perform FieldName formatting
//		std::string FieldName = Stream.Format("Data%d[F]", index);
//		Stream.Add(FieldName, 12);
//
//		// Perform Direct formatting to Add
//		Stream.Add("FieldName", "%d.%03d", Price/1000, Price%1000);
//
//		// Format both FieldName and DataValue
//		std::string FieldName = Stream.Format("Data%d[F]", index);
//		std::string DataValue = Stream.Format("%d.%03d", Price/1000, Price%1000);
//		Stream.Add(FieldName, DataValue);
//
//		// calculate new offset location when going back to character indexed method
//		offset = offset + Stream.length();
//	
class OLAParamWriter : public MemoryStream {

public:
	// default constrctor construction not allowed
	OLAParamWriter() = delete;

	explicit OLAParamWriter(char* pBuffer, size_t size) :
		MemoryStream(pBuffer, size)
	{

	}

	virtual ~OLAParamWriter();

	// C-Style formatting
	std::string Format(const char* pFormat,...);

	// Add using 2 const strings
	void Add(const char* lVal, const char* rVal, bool bEndWithComma = true);
	
	// output lval=rval, rval is string
	void Add(const std::string& lVal, const std::string& rVal, bool bEndWithComma = true);

	// output lval=rval, rval is long integer
	void Add(const std::string& lVal, long rVal, bool bEndWithComma = true);

	// output lval=rVal where rval is Dollar.Cents, cents is forced to 3 digits
	void AddPrice(const std::string& lVal, long lPrice, bool bEndWithComma = true);

	// catch all formatter
	// output lval=rVal, catach all as you can pass format and variable parameters
	void Add(const std::string& lVal, char* pFormat, ...);

	// add only if data is not empty
	void AddIfData(const char* lVal, const char* rVal, bool bEndWithComma = true);
	
	void AddIfData(const std::string& lVal, const std::string& rVal, bool bEndWithComma = true);
};

// OLA_Param is Name,Value pair
class OLA_Param {
	std::string Name_;
	std::string Value_;

public:
	OLA_Param(const std::string& sName, const std::string& sValue) :
		Name_(sName), Value_(sValue) {
		Name_ = string_trim(Name_);
	}
	// copy constrct
	OLA_Param(const OLA_Param& Other) {
		Name_ = Other.Name_;
		Value_ = Other.Value_;
	}

	// move
	OLA_Param(OLA_Param&& Other) {
		Name_ = std::move(Other.Name_);
		Value_ = std::move(Other.Value_);
	}

	void SetName(const string& sName) {
		Name_ = sName;
	}

	void SetValue(const string& sValue) {
		Value_ = sValue;
	}

	const std::string& GetName() const {
		return Name_;
	}

	const std::string& GetValue() const {
		return Value_;
	}

	std::string Get() const {
		string Result;
		if (!Name_.empty()) {
			// pre-reserve string to prevent re-growing of the string
			Result.reserve(Name_.length() + Value_.length() + 4);
			Result = Name_ + "=" + Value_ + ",";
		}
		else {
			;
		}
		return Result;
	}

	friend ostream& operator << (ostream& os, const OLA_Param& Data);
};


inline ostream& operator << (ostream& os, const OLA_Param& Data) {
	os << Data.GetName() << "=" << Data.GetValue() << ",";
	return os;
}

typedef vector<OLA_Param> OLAParameterList;

//
//  Converts charactor array of OLA Parameters into Vector for
//	read operation
//
class OLAParamReader {
	// copy of the OLA parameter string in Heap memory
	// the source buffer is likely character array, non-null terminated
	//std::string   mParameters;

	OLAParameterList  ParameterList_;

public:
	OLAParamReader(char* pParamBuffer, size_t size) {

		// make a copy of the incoming buffer
		std::string   ParameterStr(pParamBuffer, pParamBuffer + size);
		int nSearchPos = 0;

		// find first ,
		size_t Found = ParameterStr.find(',', nSearchPos);
		while (Found != std::string::npos) {
			size_t len = Found - nSearchPos;

			// find the Parameter entry
			string sParameter = ParameterStr.substr(nSearchPos, len);
			if (!sParameter.empty()) {
				// extract, and put into the data structure
				ProcessParameter(sParameter);
			}

			// keep finding next ,
			nSearchPos = Found + 1;
			Found = ParameterStr.find(',', nSearchPos);
		}

		// if not found, then the data can be either empty or has single Parameter
		if (Found == std::string::npos) {
			size_t len = ParameterStr.length() - nSearchPos;
			string sParameter = ParameterStr.substr(nSearchPos, len);
			if (!sParameter.empty()) {
				ProcessParameter(sParameter);
			}
		}
	}

	~OLAParamReader() {}

	bool Exists(const std::string& sParameterName) const {
		bool bExists = false;

		// stl search with lamda for name comparison
		auto  itr = std::find_if(ParameterList_.begin(), ParameterList_.end(),
			[&sParameterName](const OLA_Param& Entry)
		{ return Entry.GetName() == sParameterName; });

		if (itr != ParameterList_.end()) {
			bExists = true;
		}

		return bExists;
	}

	friend ostream& operator << (ostream& os, const OLAParamReader& Reader);

	const OLAParameterList& GetParameterList() const {
		return ParameterList_;
	}
protected:
	// Processes single Parameter=Value pair
	bool ProcessParameter(std::string& sParameter) {
		bool bProcessed = false;

		// find position of =
		size_t EqualPos = sParameter.find('=', 0);
		if (EqualPos != std::string::npos) {

			// extract Parameter name
			size_t NameLen = EqualPos;
			string NameField = sParameter.substr(0, NameLen);
			NameField = string_trim(NameField);

			// Extract Value
			size_t ValueLen = sParameter.length() - NameField.length() - 1;
			string ValueField = sParameter.substr(EqualPos + 1, NameLen);

			// add to the vector if valid
			if (!NameField.empty()) {
				OLA_Param Parm(NameField, ValueField);
				ParameterList_.push_back(Parm);

				bProcessed = true;
			}
		}
		else {
			// search for = failed
			// extract the entire string as the field name
			string NameField = string_trim(sParameter);
			string ValueField;
			if (!NameField.empty()) {
				OLA_Param Parm(NameField, ValueField);
				ParameterList_.push_back(Parm);

				bProcessed = true;
			}

		}
		return bProcessed;
	}
};


inline ostream& operator << (ostream& os, const OLAParamReader& Reader) {
	const OLAParameterList& List = Reader.ParameterList_;
	std::for_each(List.begin(), List.end(), [&os](const OLA_Param& Data) {
		os << Data;
	});
	return os;
}



//
//   Builder for OLA parameters
//	 keeps internal list of OLA_Param
//   Provides basic Add/AddPrice methods like OLA Writer
//	 Override << operator to be able to dump entire built object to
//	 character array
//
class OLAParamBuilder {	

	OLAParameterList  ParameterList_;

public:
	OLAParamBuilder() {
		;
	}

	OLAParamBuilder(char* pParamBuffer, size_t size) {
		// if we have existing data, read and build from passed data
		if (pParamBuffer != nullptr) {
			OLAParamReader Reader(pParamBuffer, size);

			const OLAParameterList& List = Reader.GetParameterList();
			for (OLA_Param Item : List) {
				// add a copy
				ParameterList_.push_back(Item);
			}
		}
	}

	~OLAParamBuilder() {
		;
	}

	// C-Style formatting
	std::string Format(const char* pFormat, ...);

	void Add(const OLA_Param& Data);

	void Add(const char* lVal, const char* rVal);
	void Add(const std::string& lVal, const std::string& rVal);
	void Add(const std::string& lVal, long rVal);

	// output lval=rVal where rval is Dollar.Cents, cents is forced to 3 digits
	void AddPrice(const std::string& lVal, long lPrice);

	void AddPrice(const std::string& lVal, const mathext::Fixed_Point_Value& Price);
	
	// catch all formatter
	// output lval=rVal, catach all as you can pass format and variable parameters
	void Add(const std::string& lVal, char* pFormat, ...);

	// returns string version of the parameter list
	std::string Get() const {
		std::string output;
		output.reserve(4096);

		// turn all Parameters into string, merge into Output
		const OLAParameterList& List = ParameterList_;
		std::for_each(List.begin(), List.end(), [&output](const OLA_Param& Data) {
			output = output + Data.Get();
		});

		return output;
	}

	friend ostream& operator << (ostream& os, const OLAParamBuilder& Builder);
};


inline ostream& operator << (ostream& os, const OLAParamBuilder& Builder) {
	const OLAParameterList& List = Builder.ParameterList_;
	std::for_each(List.begin(), List.end(), [&os](const OLA_Param& Data) {
		os << Data;
	});
	return os;
}


inline 	std::string OLAParamBuilder::Format(const char* pFormat, ...) {
	char pBuffer[512] = { 0 };
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


inline
void OLAParamBuilder::Add(const OLA_Param& Data) {
	ParameterList_.push_back(Data);
}

inline void OLAParamBuilder::Add(const char* lVal, const char* rVal) {
	string FieldName = lVal;
	string Value = rVal;
	OLA_Param Data(FieldName, Value);
	
	Add(Data);
}


inline void OLAParamBuilder::Add(const std::string& lVal, const std::string& rVal) {
	Add(lVal.c_str(), rVal.c_str());
}

inline void OLAParamBuilder::Add(const std::string& lVal, long rVal) {
	std::string srVal = std::to_string(rVal);
	Add(lVal, srVal);
}



inline void OLAParamBuilder::Add(const std::string& lVal, char* pFormat, ...) {
	char Buffer[512] = { 0 };
	constexpr size_t CurrentBufferSize = sizeof(Buffer);
	int Count = 0;
	va_list args;
	va_start(args, pFormat);

	// if you get buffer too small assertion getting hit, increase buffer size
	//Count = vsprintf_s(pBuffer, CurrentBufferSize, Format, args);
	Count = vsnprintf(Buffer, CurrentBufferSize, pFormat, args);

	// if data is larger than the buffer size, data truncated
	if ((size_t)Count > CurrentBufferSize) {
		assert(false);
	}
	va_end(args);

	std::string rVal = Buffer;

	Add(lVal, rVal);
}

#endif //___OLA_PARAM_H___

