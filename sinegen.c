#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <fcntl.h>
#include <io.h>

#define pi 3.14159265358
//////////////////////////////////////////////////////////////////////////
typedef struct waveheader{
   	char          ChunkID[4];
	unsigned long ChunkSize;
    char          Format[4];
    char           Subchunk1ID[4];
    unsigned long  Subchunk1Size;
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned long  SampleRate;
    unsigned long  ByteRate;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;
    char          Subchunk2ID[4];
    unsigned long Subchunk2Size;
};
///////////////////////////////////////////////////////////////////////////	
int main(int argc, char *argv[])
{
	int i=0; 
	int fs,m,f,A,T;
	/*argv is char,so need to change it to int*/
	fs = atoi(argv[1]);
	m = atoi(argv[2]);
	f = atoi(argv[3]);
	A = atoi(argv[4]);
	T = atoi(argv[5]);
	int n;
	n = fs*T;  /*samples*/
	
	
	if (A > pow(2,m-1)-1){	/*set boundry for A*/
		A = pow(2,m-1)-1;
    }
///////////////////////////////////////////////////////////////////////////////give data to structure
	/* WAVE_RIFF */
	struct waveheader wave;
	wave.ChunkID[0] = 'R';
	wave.ChunkID[1] = 'I';
	wave.ChunkID[2] = 'F';
	wave.ChunkID[3] = 'F';
	wave.Format[0] = 'W';
	wave.Format[1] = 'A';
	wave.Format[2] = 'V';
	wave.Format[3] = 'E';
	/*WAVE_FMT*/
	wave.Subchunk1ID[0] = 'f';
	wave.Subchunk1ID[1] = 'm';
	wave.Subchunk1ID[2] = 't';
	wave.Subchunk1ID[3] = ' ';
	wave.Subchunk1Size = 16;
	wave.AudioFormat = 1;
	wave.NumChannels = 1;
	wave.SampleRate = fs;
	wave.BitsPerSample = m;
	wave.ByteRate = wave.NumChannels * fs * m / 8;	/* sample rate * number of channels * bits per sample / 8 */
	wave.BlockAlign = wave.NumChannels * m /8; 		/* number of channels / bits per sample / 8 */
	/*WAVE_DATA*/
	wave.Subchunk2ID[0] = 'd';
	wave.Subchunk2ID[1] = 'a';
	wave.Subchunk2ID[2] = 't';
	wave.Subchunk2ID[3] = 'a';
	wave.Subchunk2Size = n*m/8;	/* fs * T * number of channels * bits per sample / 8 */
	/*WAVE_RIFF SIZE*/
	wave.ChunkSize= n*m/8+36;/*after finished all the calculation, determine the file size */
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	_setmode(_fileno(stdout),_O_BINARY);/*compiles it to binary form*/
	_setmode(_fileno(stderr),_O_BINARY);
	/*write the data into stdout*/
	fwrite(&wave, sizeof(wave),1, stdout);/*write all the data into fn.wav using stdout*/
	
	double sine,e,Spower,QNpower,SQNR;
	/*fn.wav*/
	if(m==32){// m=32 bits
		int *Q1;
		Q1 = malloc(sizeof(int)*n);
		for(i=0;i<n ;i++){
			sine = A*sin(2*pi*f*i/fs);/*create  sine wave*/ 
			Q1[i] = floor(sine+0.5);
			e = sine - Q1[i];	/*Quantization Error*/
			Spower = Spower + sine*sine;
			QNpower = QNpower + e*e;
			fwrite(&Q1[i],sizeof(int),1,stdout);/*write the quatization sine wave into fn.wav using stdout*/
		}
		//fwrite(Q1,sizeof(int),n,stdout);/*write the quatization sine wave into fn.wav using stdout*/
	}
	else if(m==16){// m=16 bits
		short *Q2;
		Q2 = malloc(sizeof(short)*n);
		for(i=0;i<n ;i++){
			sine = A*sin(2*pi*f*i/fs);/*create  sine wave*/ 
			Q2[i] = floor(sine+0.5);
			e = sine - Q2[i];	/*Quantization Error*/
			Spower = Spower + sine*sine;
			QNpower = QNpower + e*e;
			fwrite(&Q2[i],sizeof(short),1,stdout);/*write the quatization sine wave into fn.wav using stdout*/
		}
		//fwrite(&Q2,sizeof(short),n,stdout);	
	}
	else if(m==8){// m=8 bits
		int8_t *Q3;
		Q3 = malloc(sizeof(int8_t)*n);
		for(i=0;i<n ;i++){
			sine = A*sin(2*pi*f*i/fs) + 128;/*create  sine wave*/ 
			Q3[i] = floor(sine+0.5);
			e = sine - floor(sine + 0.5);	/*Quantization Error*/
			Spower = Spower + (sine-128)*(sine-128);
			QNpower = QNpower + e*e;
			fwrite(&Q3[i],sizeof(char),1,stdout);/*write the quatization sine wave into fn.wav using stdout*/
		}
		//fwrite(Q3,sizeof(char),n,stdout);
	}
	/*sqnr.txt*/

	Spower = Spower/n ;		/*signal power*/
	QNpower = QNpower/n ;	/*quantization noise*/
	SQNR = 10*(log10(Spower/QNpower));
	fprintf(stderr,"%.15f\n",SQNR);/*write SQNR into sqnr.txt using stderr*/
    return 0;
}

