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
    paramsStaticParameter(trackBallCircleStep     , int  , 64 );
    paramsStaticParameter(trackBallRadius        ,float, 0.4f);
    paramsStaticParameter(trackBallLineWidthStill , float, 0.9f);
    paramsStaticParameter(trackBallLineWidthMoving, float, 3.8f);
    paramsStaticParameter(camZnear, float, 0.5f);
    paramsStaticParameter(camZfar, float, 1e2f);
    paramsStaticParameter(camFOV, float, 60.0f);
    
private:
    Params() {}                    // namespace

};
