#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<omp.h>
#include<time.h>
int sequentialCode(double **A ,double *b,int n)
{
	double r[n];
	double x[n];
	double p[n];
	double px[n];
	for( int i = 0 ; i<n ; i++)
	{
		x[i] = 0;
		p[i] = b[i];
		r[i] = b[i];
		px[i] = 0;
	}
	int q = 10;
	double alpha = 0;
	while(q--)
	{
		
		double sum = 0;
		for(int i = 0 ; i < n ; i++)
		{
			sum = r[i]*r[i] + sum;
		}
		double temp[n];
		for( int i = 0; i<n ; i++ )
		{
			temp[i] = 0;
		} 
		double num = 0;
		for(int i = 0 ; i < n ; i++)
		{
			for(int j = 0 ; j < n ; j++ )
			{
				temp[i] = A[j][i]*p[j] + temp[i];
			}	
		}
		for(int j = 0 ; j < n ; j++)
		{
			num = num + temp[j]*p[j];
		}
		alpha = sum / num;
	//	printf("%lf\n", alpha);
		for(int i = 0; i < n ; i++ )
		{
			px[i] = x[i];
			x[i] = x[i] + alpha*p[i];
			r[i] = r[i] - alpha*temp[i];
		}	
		double beta = 0;
		for(int i = 0 ; i < n ; i++)
		{
			beta = beta + r[i]*r[i];
		}
		beta = beta / sum;
		for (int i = 0 ; i < n ; i++ )
		{
			p[i] = r[i] + beta*p[i];
		}
		int c=0;
//		for(int i = 0 ; i<n; i++)
//			printf("%lf ",r[i]);
//		printf("\n");
		for(int i = 0 ; i<n ; i++ )
		{
			if(r[i]<0.000001)
				c++;
		}
		
		if(c==n)
			break;
		
	}
//for( int i = 0 ; i<n ; i++ )
//	printf("%lf\n", x[i]);
	return 0;
}
int parallelCode(double **A, double *b,int n)
{
//	int n = 100;
	//scanf("%d",&n);
	double r[n];
	double x[n];
	double p[n];
	double px[n];
	#pragma omp parallel for
	for( int i = 0 ; i<n ; i++)
	{
	//	printf("%d\n",omp_get_thread_num() );;	
		x[i] = 0;
		p[i] = b[i];
		r[i] = b[i];
		px[i] = 0;
	}
	int q = 10;
	
	double alpha = 0;
	while(q--)
	{
		
		double sum = 0;
		#pragma omp parallel  for reduction(+ : sum) 
		for(int i = 0 ; i < n ; i++)
		{
			sum = r[i]*r[i] + sum;
		}
//		printf("%lf\n", sum);
		
		double temp[n];
		#pragma omp parallel for
 		for( int i = 0; i<n ; i++ )
		{
			temp[i] = 0;
		} 
		double num = 0;
		#pragma omp parallel for
		for(int i = 0 ; i < n ; i++)
		{
			//printf("%d",omp_get_thread_num());
		
			
		//	#pragma omp parallel for reduction(+ : temp[i])
			for(int j = 0 ; j < n ; j++ )
			{
				temp[i] = A[j][i]*p[j] + temp[i];
			}	
		}
		#pragma omp parallel for reduction(+ : num) 
		for(int j = 0 ; j < n ; j++)
		{
			num = num + temp[j]*p[j];
		}
	//	printf("%lf\n", num);
		
		alpha = sum / num;
		//printf("%lf\n", alpha);
		#pragma omp parallel for
		for(int i = 0; i < n ; i++ )
		{
			px[i] = x[i];
			x[i] = x[i] + alpha*p[i];
			r[i] = r[i] - alpha*temp[i];
		}	
		double beta = 0;
		#pragma omp parallel for reduction(+ : beta) 
		for(int i = 0 ; i < n ; i++)
		{
			beta = beta + r[i]*r[i];
		}
		beta = beta / sum;
		#pragma omp parallel for 
		for (int i = 0 ; i < n ; i++ )
		{
			p[i] = r[i] + beta*p[i];
		}
		int c=0;
		for(int i = 0 ; i<n ; i++ )
		{
			if(r[i]<0.000001 )
				c++;
		}
		if(c==n)
			break;
		
	}
//	for( int i = 0 ; i<n ; i++ )
//    	printf("%lf\n", x[i]);
	return 0;
}
int main()
{   
	int n=10000;
	double b[n];
	double *A[n];
	for(int i=0;i<n;i++)
		A[i] = (double*)malloc(sizeof(double)*n);
	for( int i = 0 ; i<n  ; i++ )
	{
		for(int j = 0; j<n ; j++)
		{
			if(i==j)
			{
				b[i]=rand()%(n+1);
				A[i][j]=1;	
			}
			else
			{
				A[i][j]=0;	
			}	
		}
	}

	time_t start,end ,s,e;
	double d;
	start = clock();
	sequentialCode(A,b,n);
	end = clock();
	d = (double)(end - start)/CLOCKS_PER_SEC;
	printf("Time for Sequentail Code Execution: %lf\n", d);
	
	s = clock();
	parallelCode(A,b,n);
	e = clock();
	d = (double)(e - s)/CLOCKS_PER_SEC;
	printf("Time for Parallel Code Execution: %lf\n", d);
	return 0;
	
}
