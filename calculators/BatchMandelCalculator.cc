/**
 * @file BatchMandelCalculator.cc
 * @author FULL NAME <xlogin00@stud.fit.vutbr.cz>
 * @brief Implementation of Mandelbrot calculator that uses SIMD paralelization over small batches
 * @date DATE
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <stdexcept>

#include "BatchMandelCalculator.h"

BatchMandelCalculator::BatchMandelCalculator (unsigned matrixBaseSize, unsigned limit) :
	BaseMandelCalculator(matrixBaseSize, limit, "BatchMandelCalculator")
{
	// @TODO allocate & prefill memory
	data = (int *) _mm_malloc(height * width * sizeof(int), ALIGN);
	
	x = (float *) _mm_malloc(width * sizeof(float), ALIGN);

	zReal = (float *) _mm_malloc(width * sizeof(float), ALIGN);
	zImag = (float *) _mm_malloc(width * sizeof(float), ALIGN);

	for (int i = 0; i <  width; i++){
		x[i] = x_start + i * dx;
		zReal[i] = 0;
		zImag[i] = 0;
	}
	for (int i = 0; i < height * width; i++){
		data[i] = 0;
	}
}

BatchMandelCalculator::~BatchMandelCalculator() {
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


int * BatchMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers
	int half = height / 2;
	const float *px = x;
	//float *py = y;
	int *pdata = data;
	int tile = width / tile_size;
	int remainder = width % tile_size;
	//std::cout << tile << std::endl << tile_size << std::endl << remainder << std::endl;

	#pragma omp simd
	for (int i = 0; i < half; ++i) {
		const float y = y_start + i * dy;
		float *pzReal = zReal;
		float *pzImag = zImag;

		for (int k = 0; k < limit; ++k) {
			bool over = false;
			
			#pragma omp simd  
			for (int tj = 0; tj < tile; ++tj){
				
				#pragma omp simd reduction(&:over) aligned(pdata:64,px:64,pzReal:64,pzImag:64) simdlen(64)
				for (int j = 0; j < tile_size; ++j) {
					const int jG = tj * tile_size + j;
					const int offset = i * width;
					const int offsetR = (height - i - 1) * width;
					
					pzReal[jG] = k ? pzReal[jG] : px[jG];
					pzImag[jG] = k ? pzImag[jG] : y;

					float r2 = pzReal[jG] * pzReal[jG];
					float i2 = pzImag[jG] * pzImag[jG];

					over &= (r2 + i2 > 4.0f);
					if (r2 + i2 <= 4.0f){
						pdata[offset + jG] += 1;
						pdata[offsetR + jG] += 1;
						float tmp = r2 - i2 + px[jG];
						pzImag[jG] = 2.0f * pzReal[jG] * pzImag[jG] + y;
						pzReal[jG] = tmp;
					}
				}
			}
			if (over)
				break;
		}	
	}
	return data;
}