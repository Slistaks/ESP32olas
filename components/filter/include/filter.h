#ifndef SAMPLEFILTER_H_
#define SAMPLEFILTER_H_

/*

MODIFIQUE EL ORIGINAL, LO DE ABAJO es lo de los coef comentados.
los coef nuevos tienen: window type: blackman, sampling rate: 400hz, Cutoff frequency: 25hz, Transition bandwidth: 20hz, coef: 93

FIR filter designed with
 http://t-filter.appspot.com

sampling frequency: 400 Hz

* 0 Hz - 25 Hz
  gain = 1
  desired ripple = 5 dB
  actual ripple = 0.06273922515930783 dB

* 50 Hz - 200 Hz
  gain = 0
  desired attenuation = -40 dB
  actual attenuation = -76.29928791309017 dB

*/

//#define SAMPLEFILTER_TAP_NUM 57

#define SAMPLEFILTER_TAP_NUM 93

typedef struct {
  float history[SAMPLEFILTER_TAP_NUM];
  unsigned int last_index;
} SampleFilter;

void SampleFilter_init(SampleFilter* f);
void SampleFilter_put(SampleFilter* f, float input);
double SampleFilter_get(SampleFilter* f);

#endif
