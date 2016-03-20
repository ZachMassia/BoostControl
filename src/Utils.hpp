#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include "Globals.h"


double barToPSI(double bar);

double psiToBar(double psi);

double mapDouble(double x, double in_min, double in_max, double out_min, double out_max);

double mapVoltageToPSIA(int sensorRating, double voltage);

#endif /* UTILS_H  */
