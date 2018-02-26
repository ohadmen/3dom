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
    paramsStaticParameter(trackBallCircleStep     , int  , 64 );
    paramsStaticParameter(trackBallRadius        ,float, 0.4f);
    paramsStaticParameter(trackBallLineWidthStill , float, 0.9f);
    paramsStaticParameter(trackBallLineWidthMoving, float, 3.8f);
    paramsStaticParameter(camZnear, float, 0.0001f);
    paramsStaticParameter(camZfar, float, 99.0f);
    paramsStaticParameter(camFOV, float, 45.0F);
    
private:
    Params() {}                    // namespace

};
