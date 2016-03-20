#include "Utils.hpp"


double barToPSI(double bar)
{
    return bar / 0.0689475728;
}


double psiToBar(double psi)
{
    return psi * 0.0689475728;
}

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double mapVoltageToPSIA(int sensorRating, double voltage)
{
    switch (sensorRating)
    {
    case 2:
        return mapDouble(voltage, 0.0, 1023.0, TWO_BAR_MIN_PSIA,   TWO_BAR_MAX_PSIA);
    case 3:
        return mapDouble(voltage, 0.0, 1023.0, THREE_BAR_MIN_PSIA, THREE_BAR_MAX_PSIA);
    default:
        return 0.0;
    }
}
