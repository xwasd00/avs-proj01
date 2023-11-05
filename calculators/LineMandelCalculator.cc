/**
 * @file LineMandelCalculator.cc
 * @author FULL NAME <xsovam00@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over lines
 * @date DATE
 */
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>


#include "LineMandelCalculator.h"


LineMandelCalculator::LineMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
	BaseMandelCalculator(matrixBaseSize, limit, "LineMandelCalculator")
{
	// @TODO allocate & prefill memory
	data = (int *) _mm_malloc(height * width * sizeof(int), 64);
	
	x = (float *) _mm_malloc(width * sizeof(float), 64);

	zReal = (float *) _mm_malloc(width * sizeof(float), 64);
	zImag = (float *) _mm_malloc(width * sizeof(float), 64);

	for (int i = 0; i < width; i++){
		x[i] = x_start + i * dx;
		zReal[i] = 0;
		zImag[i] = 0;
	}
	for (int i = 0; i < height * width; i++){
		data[i] = 0;
	}
}

LineMandelCalculator::~LineMandelCalculator() {
	// @TODO cleanup the memory
	_mm_free(data);
	_mm_free(x);
	_mm_free(zReal);
	_mm_free(zImag);
	data = NULL;
	x = NULL;
	zReal = NULL;
	zImag = NULL;
}

int * LineMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers
	const int half = height / 2;
	const float *px = x;
	int *pdata = data;
	
	#pragma omp simd
	for (int i = 0; i < half; ++i) {
		const float y = y_start + i * dy;
		float *pzReal = zReal;
		float *pzImag = zImag;

		for (int k = 0; k < limit; ++k) {
			bool over = false;
			#pragma omp simd reduction(&:over) aligned(pdata:64,px:64,pzReal:64,pzImag:64) simdlen(64)
			for (int j = 0; j < width; ++j) {
				pzReal[j] = k ? pzReal[j] : px[j];
				pzImag[j] = k ? pzImag[j] : y;

				float r2 = pzReal[j] * pzReal[j];
				float i2 = pzImag[j] * pzImag[j];

				over &= (r2 + i2 > 4.0f);
				if (r2 + i2 <= 4.0f){
					pdata[i * width + j] += 1;
					pdata[(height - i - 1) * width + j] += 1;
					pzImag[j] = 2.0f * pzReal[j] * pzImag[j] + y;
					pzReal[j] = r2 - i2 + px[j];
				}
			}
			if (over)
				break;
		}
	}
	return data;

}
