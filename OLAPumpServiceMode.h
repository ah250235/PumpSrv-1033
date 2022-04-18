#pragma once

class CPumpStatus;

// ServiceMode abstracton
//
// ServiceMode code is either FULL_SERVICE_MODE or SELF_SERVICE_MODE
// OLA ServiceMode is either 1 or 2, both being the long data type many logical
// errors are implemented due to misusing the code.
// This class abstracts the serice code, and provides interface 
// so that these codes cannot be directly used for comparison,
// instead, the client code must call member functions.  Also create type safety by
// passing a class object instead of long
//
class OLAPumpServiceMode {
private:
	long ServiceMode_;
	long OLAServiceModeCode_;

public:
	OLAPumpServiceMode() : ServiceMode_(-1) {
		SetOLAServiceMode(ServiceMode_);
	}
	explicit OLAPumpServiceMode(long lServiceMode) : ServiceMode_(lServiceMode) {
		SetOLAServiceMode(ServiceMode_);
		;
	}

	OLAPumpServiceMode(const OLAPumpServiceMode& Other) {
		ServiceMode_ = Other.ServiceMode_;
		OLAServiceModeCode_ = Other.OLAServiceModeCode_;
	}
	OLAPumpServiceMode(const CPumpStatus& oStatus);


	virtual ~OLAPumpServiceMode() {
		;
	}

	void SetServiceMode(long nCode) {
		ServiceMode_ = nCode;
		SetOLAServiceMode(ServiceMode_);
	}

	long GetServiceMode() const {
		return ServiceMode_;
	}

	virtual bool IsFullService() const {
		return (ServiceMode_ == FULL_SERVICE_MODE);
	}

	virtual bool IsSelfService() const {
		return (ServiceMode_ == SELF_SERVICE_MODE);
	}

	long GetOLAServiceMode() const {
		return OLAServiceModeCode_;
	}

	friend ostream& operator << (ostream& os, const OLAPumpServiceMode& Other);

protected:
	void SetOLAServiceMode(long lServiceMode) {
		switch (lServiceMode) {
		case FULL_SERVICE_MODE:
			OLAServiceModeCode_ = 1;
			break;

		case SELF_SERVICE_MODE:
			OLAServiceModeCode_ = 2;
			break;

		default:
			OLAServiceModeCode_ = -1;
			break;
		}
	}
};

inline ostream& operator << (ostream& os, const OLAPumpServiceMode& oPumpServiceMode) {
	os <<  " PumpServiceMode=" << (oPumpServiceMode.IsFullService() ? "FULL SERVICE" : "SELF SERVICE")
		<< " OLAServiceMode=" << oPumpServiceMode.OLAServiceModeCode_;
	return os;
}