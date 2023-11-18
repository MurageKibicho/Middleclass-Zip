#include <math.h>
#define M_PI 3.14159265358979323846


float quantizer[64] =
{
16, 11, 10, 16,  24,  40,  51,  61,
12, 12, 14, 19,  26,  58,  60,  55,
14, 13, 16, 24,  40,  57,  69,  56,
14, 17, 22, 29,  51,  87,  80,  62,
18, 22, 37, 56,  68, 109, 103,  77,
24, 35, 55, 64,  81, 104, 113,  92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103,  99
};

int ZigZag[64] = 
{
64,63,56,48,55,62,61,54,
47,40,32,39,46,53,60,59,
52,45,38,31,24,16,23,30,
37,44,51,58,57,50,43,36,
29,22,15, 8, 7,14,21,28,
35,42,49,41,34,27,20,13,
 6, 5,12,19,26,33,25,18,
11, 4, 3,10,17, 9, 2, 1,
};

void Quantize(float *array)
{
	for(int i = 0; i < 64; i++)
	{
		array[i] = roundf(array[i]);
	}
}

void CBCRQuantization(float *component, int level)
{
	assert(level > -1 && level < 65);
	for(int i = 0; i < level; i++)
	{
		component[ZigZag[i]-1] = 0;
	}
}

//Compute forward dct
void ForwardDCTComponent(float *component)
{
/*Forward DCT constants*/
const float a1 = 0.707;
const float a2 = 0.541;
const float a3 = 0.707;
const float a4 = 1.307;
const float a5 = 0.383;

const float s0 = 0.353553;
const float s1 = 0.254898;
const float s2 = 0.270598;
const float s3 = 0.300672;
const float s4 = s0;
const float s5 = 0.449988;
const float s6 = 0.653281;
const float s7 = 1.281458;
/*Forward DCT constants*/

	for(int i = 0; i < 8; i++)
	{
		const float b0 = component[0*8 + i] + component[7*8 + i];
		const float b1 = component[1*8 + i] + component[6*8 + i];
		const float b2 = component[2*8 + i] + component[5*8 + i];
		const float b3 = component[3*8 + i] + component[4*8 + i];
		const float b4 =-component[4*8 + i] + component[3*8 + i];
		const float b5 =-component[5*8 + i] + component[2*8 + i];
		const float b6 =-component[6*8 + i] + component[1*8 + i];
		const float b7 =-component[7*8 + i] + component[0*8 + i];
		
		const float c0 = b0 + b3;
		const float c1 = b1 + b2;
		const float c2 =-b2 + b1;
		const float c3 =-b3 + b0;
		const float c4 =-b4 - b5;
		const float c5 = b5 + b6;
		const float c6 = b6 + b7;
		const float c7 = b7;
		
		const float d0 = c0 + c1;
		const float d1 =-c1 + c0;
		const float d2 = c2 + c3;
		const float d3 = c3;
		const float d4 = c4;
		const float d5 = c5;
		const float d6 = c6;
		const float d7 = c7;
		const float d8 = (d4+d6) * a5;
		
		const float e0 = d0;
		const float e1 = d1;
		const float e2 = d2 * a1;
		const float e3 = d3;
		const float e4 = -d4 * a2 - d8;
		const float e5 = d5 * a3;
		const float e6 = d6 * a4 - d8;
		const float e7 = d7;
		
		const float f0 = e0;
		const float f1 = e1;
		const float f2 = e2 + e3;
		const float f3 = e3 - e2;
		const float f4 = e4;
		const float f5 = e5 + e7;
		const float f6 = e6;
		const float f7 = e7 - e5;
		
		const float g0 = f0;
		const float g1 = f1;
		const float g2 = f2;
		const float g3 = f3;
		const float g4 = f4 + f7;
		const float g5 = f5 + f6;
		const float g6 = -f6 + f5;
		const float g7 = f7 - f4;	
		
		component[0*8 + i] = g0 * s0;
		component[4*8 + i] = g1 * s4;
		component[2*8 + i] = g2 * s2;
		component[6*8 + i] = g3 * s6;
		component[5*8 + i] = g4 * s5;
		component[1*8 + i] = g5 * s1;
		component[7*8 + i] = g6 * s7;
		component[3*8 + i] = g7 * s3;		
	}
	for(int i = 0; i < 8; i++)
	{
		const float b0 = component[i*8 + 0]  + component[i*8 + 7];
		const float b1 = component[i*8 + 1]  + component[i*8 + 6];
		const float b2 = component[i*8 + 2]  + component[i*8 + 5];
		const float b3 = component[i*8 + 3]  + component[i*8 + 4];
		const float b4 =-component[i*8 + 4]  + component[i*8 + 3];
		const float b5 =-component[i*8 + 5]  + component[i*8 + 2];
		const float b6 =-component[i*8 + 6]  + component[i*8 + 1];
		const float b7 =-component[i*8 + 7]  + component[i*8 + 0] ;
		
		const float c0 = b0 + b3;
		const float c1 = b1 + b2;
		const float c2 =-b2 + b1;
		const float c3 =-b3 + b0;
		const float c4 =-b4 - b5;
		const float c5 = b5 + b6;
		const float c6 = b6 + b7;
		const float c7 = b7;
		
		const float d0 = c0 + c1;
		const float d1 =-c1 + c0;
		const float d2 = c2 + c3;
		const float d3 = c3;
		const float d4 = c4;
		const float d5 = c5;
		const float d6 = c6;
		const float d7 = c7;
		const float d8 = (d4+d6) * a5;
		
		const float e0 = d0;
		const float e1 = d1;
		const float e2 = d2 * a1;
		const float e3 = d3;
		const float e4 = -d4 * a2 - d8;
		const float e5 = d5 * a3;
		const float e6 = d6 * a4 - d8;
		const float e7 = d7;
		
		const float f0 = e0;
		const float f1 = e1;
		const float f2 = e2 + e3;
		const float f3 = e3 - e2;
		const float f4 = e4;
		const float f5 = e5 + e7;
		const float f6 = e6;
		const float f7 = e7 - e5;
		
		const float g0 = f0;
		const float g1 = f1;
		const float g2 = f2;
		const float g3 = f3;
		const float g4 = f4 + f7;
		const float g5 = f5 + f6;
		const float g6 = -f6 + f5;
		const float g7 = f7 - f4;	
		
		component[i*8 + 0] = g0 * s0;
		component[i*8 + 4] = g1 * s4;
		component[i*8 + 2] = g2 * s2;
		component[i*8 + 6] = g3 * s6;
		component[i*8 + 5] = g4 * s5;
		component[i*8 + 1] = g5 * s1;
		component[i*8 + 7] = g6 * s7;
		component[i*8 + 3] = g7 * s3;	
	}
	Quantize(component);
}

void InverseDCTComponent(float *component)
{
const float m0 = 2.0 * cos(1.0/16.0 * 2.0 * M_PI);
const float m1 = 2.0 * cos(2.0/16.0 * 2.0 * M_PI);
const float m3 = 2.0 * cos(2.0/16.0 * 2.0 * M_PI);
const float m5 = 2.0 * cos(3.0/16.0 * 2.0 * M_PI);
const float m2 = m0-m5;
const float m4 = m0+m5;

const float s0 = cos(0.0/16.0 *M_PI)/sqrt(8);
const float s1 = cos(1.0/16.0 *M_PI)/2.0;
const float s2 = cos(2.0/16.0 *M_PI)/2.0;
const float s3 = cos(3.0/16.0 *M_PI)/2.0;
const float s4 = cos(4.0/16.0 *M_PI)/2.0;
const float s5 = cos(5.0/16.0 *M_PI)/2.0;
const float s6 = cos(6.0/16.0 *M_PI)/2.0;
const float s7 = cos(7.0/16.0 *M_PI)/2.0;

	for(int i = 0; i < 8; i++)
	{
		const float g0 = component[0*8 + i] * s0;
		const float g1 = component[4*8 + i] * s4;
		const float g2 = component[2*8 + i] * s2;
		const float g3 = component[6*8 + i] * s6;
		const float g4 = component[5*8 + i] * s5;
		const float g5 = component[1*8 + i] * s1;
		const float g6 = component[7*8 + i] * s7;
		const float g7 = component[3*8 + i] * s3;
		
		const float f0 = g0;
		const float f1 = g1;
		const float f2 = g2;
		const float f3 = g3;
		const float f4 = g4 - g7;
		const float f5 = g5 + g6;
		const float f6 = g5 - g6;
		const float f7 = g4 + g7;
		
		const float e0 = f0;
		const float e1 = f1;
		const float e2 = f2 - f3;
		const float e3 = f2 + f3;
		const float e4 = f4;
		const float e5 = f5 - f7;
		const float e6 = f6;
		const float e7 = f5 + f7;
		const float e8 = f4 + f6;
		
		const float d0 = e0;
		const float d1 = e1;
		const float d2 = e2 * m1;
		const float d3 = e3;
		const float d4 = e4 * m2;
		const float d5 = e5 * m3;
		const float d6 = e6 * m4;
		const float d7 = e7;
		const float d8 = e8 * m5;
		
		const float c0 = d0 + d1;
		const float c1 = d0 - d1;
		const float c2 = d2 - d3;
		const float c3 = d3;
		const float c4 = d4 + d8;
		const float c5 = d5 + d7;
		const float c6 = d6 - d8;
		const float c7 = d7;
		const float c8 = c5 - c6;
		
		const float b0 = c0 + c3;
		const float b1 = c1 + c2;
		const float b2 = c1 - c2;
		const float b3 = c0 - c3;
		const float b4 = c4 - c8;
		const float b5 = c8;
		const float b6 = c6 - c7;
		const float b7 = c7;
		
		component[0 * 8 + i] = b0 + b7;
		component[1 * 8 + i] = b1 + b6;
		component[2 * 8 + i] = b2 + b5;
		component[3 * 8 + i] = b3 + b4;
		component[4 * 8 + i] = b3 - b4;
		component[5 * 8 + i] = b2 - b5;
		component[6 * 8 + i] = b1 - b6;
		component[7 * 8 + i] = b0 - b7;	
	}
	
	for(int i = 0 ; i < 8; i++)
	{
		const float g0 = component[i*8 + 0] * s0;
		const float g1 = component[i*8 + 4] * s4;
		const float g2 = component[i*8 + 2] * s2;
		const float g3 = component[i*8 + 6] * s6;
		const float g4 = component[i*8 + 5] * s5;
		const float g5 = component[i*8 + 1] * s1;
		const float g6 = component[i*8 + 7] * s7;
		const float g7 = component[i*8 + 3] * s3;
		
		const float f0 = g0;
		const float f1 = g1;
		const float f2 = g2;
		const float f3 = g3;
		const float f4 = g4 - g7;
		const float f5 = g5 + g6;
		const float f6 = g5 - g6;
		const float f7 = g4 + g7;
		
		const float e0 = f0;
		const float e1 = f1;
		const float e2 = f2 - f3;
		const float e3 = f2 + f3;
		const float e4 = f4;
		const float e5 = f5 - f7;
		const float e6 = f6;
		const float e7 = f5 + f7;
		const float e8 = f4 + f6;
		
		const float d0 = e0;
		const float d1 = e1;
		const float d2 = e2 * m1;
		const float d3 = e3;
		const float d4 = e4 * m2;
		const float d5 = e5 * m3;
		const float d6 = e6 * m4;
		const float d7 = e7;
		const float d8 = e8 * m5;
		
		const float c0 = d0 + d1;
		const float c1 = d0 - d1;
		const float c2 = d2 - d3;
		const float c3 = d3;
		const float c4 = d4 + d8;
		const float c5 = d5 + d7;
		const float c6 = d6 - d8;
		const float c7 = d7;
		const float c8 = c5 - c6;
		
		const float b0 = c0 + c3;
		const float b1 = c1 + c2;
		const float b2 = c1 - c2;
		const float b3 = c0 - c3;
		const float b4 = c4 - c8;
		const float b5 = c8;
		const float b6 = c6 - c7;
		const float b7 = c7;
		
		component[i * 8 + 0] = b0 + b7;
		component[i * 8 + 1] = b1 + b6;
		component[i * 8 + 2] = b2 + b5;
		component[i * 8 + 3] = b3 + b4;
		component[i * 8 + 4] = b3 - b4;
		component[i * 8 + 5] = b2 - b5;
		component[i * 8 + 6] = b1 - b6;
		component[i * 8 + 7] = b0 - b7;
	}
}



void Print(float *array, int length)
{
	for(int i = 0; i < 64; i++)
	{
		if(i > 0 && i % 8 == 0)
		{
			putchar('\n');
		}
		printf("(%3.3f) ", array[i]);
		//printf("%.1f) ", array[i]);	
	}
	putchar('\n');
}


void Print2(float *array, int length)
{
	putchar('\n');
	putchar('\n');
	for(int i = 0; i < 64; i++)
	{
		if(i > 0 && i % 8 == 0)
		{
			putchar('\n');
		}
		printf("%.0f, ",round( array[i]));
	}
	putchar('\n');
}

void Print3(float array[64])
{
	for(int i = 0; i < 64; i++)
	{
		if(i > 0 && i % 8 == 0)
		{
			putchar('\n');
		}
		printf("%3.0f,", array[i]);
	}
	putchar('\n');
}





float *ChangeQuantizationTable(int level)
{
	float *table = calloc(64, sizeof(float));
	if(level < 1)
	{
		level = 1;
	}
	
	for(int i = 0; i <64; i++)
	{
		table[i] = (quantizer[i] * level);
	}
	return table;
}




void Dequantize(float *array, int quantizationLevel)
{
	float *table = ChangeQuantizationTable(quantizationLevel);
	for(int i = 0; i < 64; i++)
	{
		array[i] *= (table[i]);
	}
	free(table);
}

void Process(float *array)
{
	for(int i = 0; i < 64; i++)
	{
		array[i] /= 1.5;
	}
}



/*

int main()
{
	float component2[64] = 
{
5,7,11,17,23,37,41,67,71,73,79,83,11,13,23,37,41,67,71,73,79,83,7,11,13,19,23,37,67,71,73,79,5,7,11,13,19,23,37,67,71,73,79,11,13,19,29,31,53,59,61,67,11,13,19,29,31,53,59,61,3,5,7,17,
};

	
	//Print(component2,64);
	ForwardDCTComponent(component2);
	Print(component2,64);
	Quantize(component2,1);
	//putchar('\n');

	Dequantize(component2,0.5);
	putchar('\n');
	Print(component2,64);
	InverseDCTComponent(component2);
	//Print(component2,64);
	//ForwardDCTComponent(component2);
	
	return 0;
}*/
