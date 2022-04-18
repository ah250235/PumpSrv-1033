#include "stdafx.h"
#include "OLAPumpServiceMode.h"
#include "PumpClass.h"

OLAPumpServiceMode::OLAPumpServiceMode(const CPumpStatus& oStatus) {
	ServiceMode_ = oStatus.GetServiceMode();
	SetOLAServiceMode(ServiceMode_);
}
