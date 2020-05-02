#pragma once
#include <QtGui/QVector3D>


#define paramsStaticParameter(paramName, paramType,paramDefaultValue,paramUnknownValue) \
static paramType paramName(paramType vin = paramUnknownValue) \
    {                               \
    static paramType vs(paramDefaultValue);               \
    if (vin != paramUnknownValue)                   \
        vs = vin;                   \
    return vs;                       \
    }                               \


class Params
{
public:

    paramsStaticParameter(drawablesTexture, int, 1,-1);
	paramsStaticParameter(viewGrid, int, 1,-1);
    paramsStaticParameter(viewAxes, int, 1,-1);
    paramsStaticParameter(trackBallRadius, float, 0.5f,-1);
    paramsStaticParameter(nStatusLines, int, 3,-1);
    paramsStaticParameter(camZnear, float, 0.1f,-1);
    paramsStaticParameter(camZfar, float, 1e2f,-1);
    paramsStaticParameter(camFOV, float, 60.0f,-1);
    paramsStaticParameter(whiteBackground, int, 0,-1);
	paramsStaticParameter(pointSize, float, 3,-1);
    paramsStaticParameter(lightDir, QVector3D,QVector3D(0.4, 0.48, 0.51),QVector3D(-1, -1, -1) );
    
    
private:
    Params() {}                    // namespace

};
 