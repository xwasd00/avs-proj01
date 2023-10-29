/**
 * @file LineMandelCalculator.cc
 * @author FULL NAME <xlogin00@stud.fit.vutbr.cz>
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
	data = (int *)(calloc(height * width, sizeof(int)));
	
	x = (float *)(calloc(width, sizeof(float)));
	y = (float *)(calloc(height, sizeof(float)));

	zReal = (float *)(calloc(width * height, sizeof(float)));
	zImag = (float *)(calloc(width * height, sizeof(float)));

}

LineMandelCalculator::~LineMandelCalculator() {
	// @TODO cleanup the memory
	free(data);
	free(x);
	free(y);
	free(zReal);
	free(zImag);
	data = NULL;
	x = NULL;
	y = NULL;
	zReal = NULL;
	zImag = NULL;
}


int * LineMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers
	int *pdata = data;
	bool over = false;
	#pragma omp simd
	for (int i = 0; i < width; ++i) {
		x[i] = x_start + i * dx;
	}
	#pragma omp simd
	for (int i = 0; i < height; i++) {
		#pragma omp simd
		for (int j = 0; j < width; j++) {
			zReal[i * width + j] = x[j];
			zImag[i * width + j] = y_start + i * dy;
		}
	}
	#pragma omp simd
	for (int i = 0; i < height; i++) {
		float y = y_start + i * dy; // current imaginary value
		#pragma omp simd
		for (int j = 0; j < width; j++) {

			float zReal = x[j];
			float zImag = y;
			over = false;
			int value = 0;
			for (int k = 0; k < limit; ++k) {
				float r2 = zReal * zReal;
				float i2 = zImag * zImag;
				if (r2 + i2 > 4.0f){
					value = k;
					over = true;
					break;
				}
				zImag = 2.0f * zReal * zImag + y;
				zReal = r2 - i2 + x[j];
			}
			if (!over) {
				value = limit;
			}
			pdata[i * width + j] = value;
		}
	}
	return data;

}
