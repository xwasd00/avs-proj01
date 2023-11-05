/**
 * @file BatchMandelCalculator.cc
 * @author FULL NAME <xsovam00@stud.fit.vutbr.cz>
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
	const int n_width_tiles = width / tile_size;
	const int n_tiles = width * height / (tile_size * 2);

	data_field = (int *) _mm_malloc(height * width * sizeof(int), 64);
	data = (int **) _mm_malloc(n_tiles * sizeof(int*), 64);
	data2 = (int **) _mm_malloc(n_tiles * sizeof(int*), 64);
	
	x = (float **) _mm_malloc(n_tiles * sizeof(float*), 64);
	x_field = (float *) _mm_malloc(width * sizeof(float), 64);

	zReal = (float *) _mm_malloc(tile_size * sizeof(float), 64);
	zImag = (float *) _mm_malloc(tile_size * sizeof(float), 64);
	
	for (int i = 0; i < height * width; i++){
		data_field[i] = 0;
	}
	
	for (int i = 0; i < width; i++){
		x_field[i] = x_start + i * dx;
	}

	for (int i = 0; i < tile_size; ++i){
		zReal[i] = 0;
		zImag[i] = 0;
	}
	
	for (int i = 0; i < n_tiles; ++i) {
		x[i] = x_field + (i % n_width_tiles) * tile_size;
	}

	for (int i = 0; i < n_tiles; ++i) {
		data[i] = data_field + i * tile_size;
		data2[i] = data_field + (height - (i / n_width_tiles) - 1) * width + (i % n_width_tiles) * tile_size;
	}
}

BatchMandelCalculator::~BatchMandelCalculator() {
	// @TODO cleanup the memory
	_mm_free(data);
	_mm_free(x);
	_mm_free(x_field);
	_mm_free(zReal);
	_mm_free(zImag);
	data = NULL;
	x = NULL;
	x_field = NULL;
	zReal = NULL;
	zImag = NULL;
}


int * BatchMandelCalculator::calculateMandelbrot () {
	// @TODO implement the calculator & return array of integers
	const int n_tiles = width * height / (tile_size * 2);
	const int n_width_tiles = width / tile_size;
	float **ppx = x;
	int **ppdata = data;
	int **ppdata2 = data2;
	float *pzReal = zReal;
	float *pzImag = zImag;
	
	#pragma omp simd
	for (int i = 0; i < n_tiles; ++i) {
		//const int idy = (i / n_width_tiles);
		float *px = ppx[i];
		int *pdata = ppdata[i];
		int *pdata2 = ppdata2[i];
		const float y = y_start + (i / n_width_tiles) * dy;
		for (int k = 0; k < limit; ++k) {
			bool over = false;
			#pragma omp simd reduction(&:over) aligned(pdata:64,pdata2:64,px:64,pzReal:64,pzImag:64) simdlen(64)
			for (int j = 0; j < tile_size; ++j) {
				//const int idx = (i % n_width_tiles) * tile_size + j;
				pzReal[j] = k ? pzReal[j] : px[j];
				pzImag[j] = k ? pzImag[j] : y;

				float r2 = pzReal[j] * pzReal[j];
				float i2 = pzImag[j] * pzImag[j];

				over &= (r2 + i2 > 4.0f);
				if (r2 + i2 <= 4.0f){
					//pdata[idy * width + idx] += 1;
					//pdata[(height - idy - 1) * width + idx] += 1;
					pdata[j] += 1;
					pdata2[j] += 1;
					pzImag[j] = 2.0f * pzReal[j] * pzImag[j] + y;
					pzReal[j] = r2 - i2 + px[j];
				}
			}
			if (over)
				break;
		}
	}
	return data_field;
}