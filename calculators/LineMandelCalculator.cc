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
	data = (int *)(malloc(height * width * sizeof(int)));
	
	x = (float *)(malloc(width * sizeof(float)));
	zReal = (float *)(malloc(width * sizeof(float)));
	
	y = (float *)(malloc(height * sizeof(float)));
	zImag = (float *)(malloc(height * sizeof(float)));

}

LineMandelCalculator::~LineMandelCalculator() {
	// @TODO cleanup the memory
	free(data);
	free(x);
	free(zReal);
	free(y);
	free(zImag);
	data = NULL;
	x = NULL;
	zReal = NULL;
	y = NULL;
	zImag = NULL;
}


int * LineMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers

	int *pdata = data;
	for (int i = 0; i < height * width; i++){
		pdata[i] = limit;
	}

	// TODO: matice je symetricka - vypocitat jen pulku - druhou dokopirovat
	// TODO: presunout promenne/pole do kostruktoru
	// TODO: prehodit vnitrni smycku ven

	for (int i = 0; i < width; i++) {
			x[i] = x_start + i * dx; // current real value
	}

	for (int i = 0; i < height; i++) {
			y[i] = y_start + i * dy; // current imaginary value
	}
	

	for (int i = 0; i < height; i++)
	{
		
		for (int j = 0; j < width; j++)
		{
			zReal[j] = x[j];
			zImag[i] = y[i];
			
			for (int k = 0; k < limit; ++k) {
				float r2 = zReal[j] * zReal[j];
				float i2 = zImag[i] * zImag[i];

				if (r2 + i2 > 4.0f){

					pdata[i * width + j] = k;
					break;
				}

				zImag[i] = 2.0f * zReal[j] * zImag[i] + y[i];
				zReal[j] = r2 - i2 + x[j];
			}
		}
	}
	return data;

}
