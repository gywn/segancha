#pragma once

namespace color {
/* ICC's PCS illuminant */
const double PCS_X = 31595. / 32768.;
const double PCS_Y = 1;
const double PCS_Z = 27030. / 32768.;

/* Macbook's LCD color profile for XYZ -> RGB */
const double A11 = 3.98875596;
const double A12 = -2.41598659;
const double A13 = -0.52021109;
const double A21 = -1.40052984;
const double A22 = 2.37728799;
const double A23 = -0.03294208;
const double A31 = -0.03269638;
const double A32 = -0.18259425;
const double A33 = 1.4716528;

static const double r2 = 0.2; // cyan's absorbance of green
static const double r3 = 0.1; // cyan's absorbance of blue
static const double g1 = 0.0; // magenta's absorbance of red
static const double g3 = 0.1; // magenta's absorbance of blue
static const double b1 = 0;   // yellow's absorbance of red
static const double b2 = 0.1; // yellow's absorbance of green

/* (1 - RGB) -> CMY */
const double B11 = 1 - b2 * g3;
const double B12 = -g1 + b1 * g3;
const double B13 = -b1 + b2 * g1;
const double B21 = -r2 + b2 * r3;
const double B22 = 1 - b1 * r3;
const double B23 = -b2 + b1 * r2;
const double B31 = g3 * r2 - r3;
const double B32 = -g3 + g1 * r3;
const double B33 = 1 - g1 * r2;
} // namespace color