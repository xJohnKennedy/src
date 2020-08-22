#include <immintrin.h>
#include <intrin.h>


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

double multiplicacao(double *Vetor_ymm0, double  *Vetor_ymm1, double  *Vetor_ymm2, double  *Vetor_ymm3, int num_f)
{
    __m256d result_final = { 0.0, 0.0, 0.0, 0.0};
    __m256d vec0, vec1, vec2, vec3, 
            result_1, result_2, result_3, 
            result_final_temp = { 0.0, 0.0, 0.0, 0.0};

    for (int i=0; i<num_f; i++ )
    {
        vec0 = _mm256_load_pd(Vetor_ymm0 + 4*i);
        vec1 = _mm256_load_pd(Vetor_ymm1 + 4*i);
        vec2 = _mm256_load_pd(Vetor_ymm2 + 4*i);
        vec3 = _mm256_load_pd(Vetor_ymm3 + 4*i);
        result_1 = _mm256_mul_pd (vec0, vec1);
        result_2 = _mm256_mul_pd (vec2, vec3);
        result_3 = _mm256_mul_pd (result_1, result_2);
        result_final_temp = _mm256_add_pd (result_3, result_final);
        result_final = result_final_temp;
       // _mm256_store_pd(Vetor_ymm0 + 4*i,result_3);
    }

    // const double result = pairwise_sum (Vetor_ymm0,  num_f*4);
    // const double result = kahanSum (Vetor_ymm0,  num_f*4);

    __m128d soma1 = _mm_load_pd( (double *)&result_final);
    __m128d soma2 = _mm_load_pd( (double *)&result_final + 2);
    __m128d resultado_soma = _mm_add_pd(soma1,soma2);

    double *vetor_final = (double *)&resultado_soma;

    return vetor_final[0] + vetor_final[1];
}