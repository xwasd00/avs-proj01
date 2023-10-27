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
	for (int j = 0; j < width; j++) {
		x[j] = x_start + j * dx; // current real value
	}

	for (int i = 0; i < height; i++) {
		y[i] = y_start + i * dy; // current imaginary value
	}
	//#pragma omp simd
	/*
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			zReal[i * width + j] = x[j];
			zImag[i * width + j] = y[i];
		}
	}
	*/
	// TODO: matice je symetricka - vypocitat jen pulku - druhou dokopirovat
	// #pragma omp simd
	for (int i = 0; i < height; i++){
		for (int k = 0; k < limit; ++k) {
			unsigned int counter = 0;
			#pragma omp simd reduction(+:counter)
			for (int j = 0; j < width; j++) {
				float r2 = zReal[i * width + j] * zReal[i * width + j];
				float i2 = zImag[i * width + j] * zImag[i * width + j];
				
				if (r2 + i2 > 4.0f){
					counter += 1;
				}
				else{
					pdata[i * width + j] += 1;
					zImag[i * width + j] = 2.0f * zReal[i * width + j] * zImag[i * width + j] + y[i];
					zReal[i * width + j] = r2 - i2 + x[j];
				}
			}
			if (counter == limit){
				break;
			}
		}
	}
	return data;

}
