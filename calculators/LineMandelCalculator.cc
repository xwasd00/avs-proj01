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
}

LineMandelCalculator::~LineMandelCalculator() {
	// @TODO cleanup the memory
	free(data);
	data = NULL;
}


int * LineMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers

	int *pdata = data;
	bool over = false;
	// TODO: matice je symetricka - vypocitat jen pulku - druhou dokopirovat
	// TODO: presunout promenne/pole do kostruktoru
	// TODO: udelat ze zReal a zImag pole a prehodit vnitrni smycku ven
	//float zReal[height * width] = x_start;
	//float zImag[height * width] = y_start;

	// TODO: jsou prehozene smycky - otestovat funkcionalitu
	for (int j = 0; j < width; j++)
	{
		for (int i = 0; i < height; i++)
		{
			float x = x_start + j * dx; // current real value
			float y = y_start + i * dy; // current imaginary value

			float zReal = x;
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
				zReal = r2 - i2 + x;
			}
			if (!over){
				value = limit;
			}
			*(pdata++) = value;
		}
	}
	return data;

}
