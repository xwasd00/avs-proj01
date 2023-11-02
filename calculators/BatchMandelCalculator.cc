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
	const int n_tiles = width / tile_size;
	d = (int ***) _mm_malloc(height * sizeof(int**), ALIGN);
	data = (int **) _mm_malloc(height * n_tiles * sizeof(int*), ALIGN);
	data_field = (int *) _mm_malloc(height * width * sizeof(int), ALIGN);
	
	x = (float**) _mm_malloc(n_tiles * sizeof(float*), ALIGN);
	x_field = (float *) _mm_malloc(width * sizeof(float), ALIGN);

	zReal = (float *) _mm_malloc(width * sizeof(float), ALIGN);
	zImag = (float *) _mm_malloc(width * sizeof(float), ALIGN);

	for (int i = 0; i <  width; i++){
		x_field[i] = x_start + i * dx;
		zReal[i] = 0;
		zImag[i] = 0;
	}
	for (int i = 0; i < height * width; i++){
		data_field[i] = 0;
	}
	for (int i = 0; i < n_tiles; i++){
		x[i] = x_field + i * tile_size;
	}
	for(int i = 0; i < height; i++){
		d[i] = data + i * n_tiles;
		for (int j = 0; j < n_tiles; j++){
			data[i * n_tiles + j] = data_field + i * width + j * tile_size;
		}
	}
}

BatchMandelCalculator::~BatchMandelCalculator() {
	// @TODO cleanup the memory
	_mm_free(d);
	_mm_free(data);
	_mm_free(data_field);
	_mm_free(x);
	_mm_free(x_field);
	_mm_free(zReal);
	_mm_free(zImag);
	d = NULL;
	data = NULL;
	data_field = NULL;
	x = NULL;
	x_field = NULL;
	zReal = NULL;
	zImag = NULL;
}


int * BatchMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers
	int half = height / 2;
	float **px = x;
	//float *py = y;
	int ***pd = d;
	int **pdata = data;
	const int n_tiles = width / tile_size;
	//int remainder = width % tile_size;
	//std::cout << tile << std::endl << tile_size << std::endl << remainder << std::endl;

	#pragma omp simd aligned(pd:64)
	for (int i = 0; i < half; ++i) {
		const float y = y_start + i * dy;
		float *pzReal = zReal;
		float *pzImag = zImag;
		//const int offset = i * n_tiles;
		//const int offsetR = (height - i - 1) * n_tiles;
		//const int offset = i * width;
		//const int offsetR = (height - i - 1) * width;
		int ** ppd = pd[i];
		int ** ppd2 = pd[height - i - 1];
		
		#pragma omp simd aligned(px:64,ppd:64,ppd2:64)
		for (int tj = 0; tj < n_tiles; ++tj){
			const float *ppx = px[tj];
			int *ppdata = ppd[tj];
			int *ppdata2 = ppd2[tj];
			//int *ppdata = pdata[offset + tj];
			//int *ppdata2 = pdata[offsetR + tj];

			for (int k = 0; k < limit; ++k) {
				bool over = false;
				
				#pragma omp simd reduction(&:over) aligned(ppdata:64,ppdata2:64,ppx:64,pzReal:64,pzImag:64) simdlen(64)
				for (int j = 0; j < tile_size; ++j) {
					//const int jG = tj * tile_size + j;
					
					pzReal[j] = k ? pzReal[j] : ppx[j];
					pzImag[j] = k ? pzImag[j] : y;

					float r2 = pzReal[j] * pzReal[j];
					float i2 = pzImag[j] * pzImag[j];

					over &= (r2 + i2 > 4.0f);
					if (r2 + i2 <= 4.0f){
						//pdata[offset + jG] += 1;
						//pdata[offsetR + jG] += 1;
						ppdata[j] += 1;
						ppdata2[j] += 1;
						float tmp = r2 - i2 + ppx[j];
						pzImag[j] = 2.0f * pzReal[j] * pzImag[j] + y;
						pzReal[j] = tmp;
					}
				}
				if (over)
					break;
			}
		}	
	}
	return data_field;
}