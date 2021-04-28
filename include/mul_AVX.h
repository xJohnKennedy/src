#include <immintrin.h>
#include <intrin.h>
#include <windows.h>
#include <VersionHelpers.h>


/*
double pairwise_sum (double *x, int n)
{
    if (n <= 2)
    {
        if (n == 1)
            return x[0];
        else
            return x[0] + x[1];
    } else {
        int m = floor(n/2);
        return pairwise_sum (x,m) + pairwise_sum (x + m, n - m);
    }
}

double kahanSum (double *x, int n)
{
    double sum = 0.0;
    double c = 0.0;
    double y, t;
    for(int i=0; i<n; i++)
    {
        y = x[n-1-i] - c;
        t = sum + y;
        c = (t-sum) - y;
        sum = t;
    }

    return sum;
}
*/


double multiplicacao(double *Vetor_ymm0, double  *Vetor_ymm1, double  *Vetor_ymm2, double  *Vetor_ymm3, int num_f)
{
    __m256d result_final = { 0.0, 0.0, 0.0, 0.0};
	__m256d result_final_temp1 = { 0.0, 0.0, 0.0, 0.0 };
	__m256d result_final_temp2 = { 0.0, 0.0, 0.0, 0.0 };
    __m256d vec0, vec1, vec2, vec3, 
            result_1, result_2, result_3, 
            result_final_temp = { 0.0, 0.0, 0.0, 0.0};

    for (int i=0; i<num_f/2; i++ )
    {
        vec0 = _mm256_load_pd(Vetor_ymm0);
        vec1 = _mm256_load_pd(Vetor_ymm1);
        vec2 = _mm256_load_pd(Vetor_ymm2);
        vec3 = _mm256_load_pd(Vetor_ymm3);
        result_1 = _mm256_mul_pd (vec0, vec1);
        result_2 = _mm256_mul_pd (vec2, vec3);
        result_3 = _mm256_mul_pd (result_1, result_2);
		result_final_temp1 = _mm256_add_pd (result_3, result_final_temp1);

		vec0 = _mm256_load_pd(Vetor_ymm0+4);
		vec1 = _mm256_load_pd(Vetor_ymm1+4);
		vec2 = _mm256_load_pd(Vetor_ymm2+4);
		vec3 = _mm256_load_pd(Vetor_ymm3+4);
		result_1 = _mm256_mul_pd(vec0, vec1);
		result_2 = _mm256_mul_pd(vec2, vec3);
		result_3 = _mm256_mul_pd(result_1, result_2);
		result_final_temp2 = _mm256_add_pd(result_3, result_final_temp2);

		// incremento dos poteiros em 4 devido ao AVX vetorizar em 256 bits = 32 bytes = 4 * sizeof(double)
		Vetor_ymm0 += 2*4;
		Vetor_ymm1 += 2*4;
		Vetor_ymm2 += 2*4;
		Vetor_ymm3 += 2*4;
       
    }

	for (int i = 0; i < num_f % 2; i++)
	{
		vec0 = _mm256_load_pd(Vetor_ymm0);
		vec1 = _mm256_load_pd(Vetor_ymm1);
		vec2 = _mm256_load_pd(Vetor_ymm2);
		vec3 = _mm256_load_pd(Vetor_ymm3);
		result_1 = _mm256_mul_pd(vec0, vec1);
		result_2 = _mm256_mul_pd(vec2, vec3);
		result_3 = _mm256_mul_pd(result_1, result_2);
		result_final_temp1 = _mm256_add_pd(result_3, result_final_temp1);

	}

    // const double result = pairwise_sum (Vetor_ymm0,  num_f*4);
    // const double result = kahanSum (Vetor_ymm0,  num_f*4);

	result_final = _mm256_add_pd(result_final_temp1, result_final_temp2);
    __m128d soma1 = _mm_load_pd( (double *)&result_final);
    __m128d soma2 = _mm_load_pd( (double *)&result_final + 2);
    __m128d resultado_soma = _mm_add_pd(soma1,soma2);

    double *vetor_final = (double *)&resultado_soma;

    return vetor_final[0] + vetor_final[1];
}
