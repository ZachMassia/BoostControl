#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>


double barToPSI(double bar)
{
    return bar / 0.0689475728;
}


double psiToBar(double psi)
{
    return psi * 0.0689475728;
}


#endif /* UTILS_H  */
