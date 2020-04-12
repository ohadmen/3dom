#pragma once
#define paramsStaticParameter(paramName, paramType,paramDefaultValue) \
static paramType paramName(paramType vin = paramType(-1)) \
    {                               \
    static paramType vs(paramDefaultValue);               \
    if (vin != -1)                   \
        vs = vin;                   \
    return vs;                       \
    }                               \


class Params
{
public:
	
    paramsStaticParameter(nStatusLines, int, 3);
    paramsStaticParameter(camZnear, float, 0.1f);
    paramsStaticParameter(camZfar, float, 1e2f);
    paramsStaticParameter(camFOV, float, 60.0f);
    paramsStaticParameter(whiteBackground, int, 0);
	paramsStaticParameter(pointSize, float, 5);
    
    
private:
    Params() {}                    // namespace

};
