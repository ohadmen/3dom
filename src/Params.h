#pragma once
#define paramsStaticParameter(paramName, paramType,paramDefaultValue) \
static paramType paramName(paramType vin = paramType(-1)) \
	{							   \
	static paramType vs(paramDefaultValue);			   \
	if (vin != -1)				   \
		vs = vin;				   \
	return vs;					   \
	}							   \


class Params
{
public:
	paramsStaticParameter(trackBallCircleStep     , int  , 64 );
	paramsStaticParameter(trackBallRadius		,float, 0.5);
	paramsStaticParameter(trackBallLineWidthStill , float, 0.9f);
	paramsStaticParameter(trackBallLineWidthMoving, float, 1.8f);
	paramsStaticParameter(camZnear, float, 0.0001f);
	paramsStaticParameter(camZfar, float, 99.0f);
	paramsStaticParameter(camFOV, float, 90.0f);
	
private:
	Params() {}                    // namespace

};
