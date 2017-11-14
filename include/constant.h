#ifndef ICC_CONSTANT_H_
#define ICC_CONSTANT_H_

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
} // namespace color

#endif /* ICC_CONSTANT_H_ */