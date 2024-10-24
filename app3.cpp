#include <iostream>
#include <stdio.h>
#include <math.h>
#include <thread>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;  

const int iXmax = 3200; //
const int iYmax = 3200; //
const double CxMin=-2.5;
const double CxMax=1.5;
const double CyMin=-2.0;
const double CyMax=2.0;
const int MaxColorComponentValue=255;
const int IterationMax=3000; //
const double EscapeRadius=2;

unsigned char color[iYmax][iXmax][3];
int num_threads;

void MandelbrotSet(int tid,int startRow,int endRow)
{
   int iX,iY;
   double Cx,Cy;
   double PixelWidth=(CxMax-CxMin)/iXmax;
   double PixelHeight=(CyMax-CyMin)/iYmax;
   double Zx, Zy;
   double Zx2, Zy2;
   int Iteration;
   double ER2=EscapeRadius*EscapeRadius;
   unsigned char threadRed = (tid*30) % 256;  
   unsigned char threadGreen = (tid*30) % 256;
   unsigned char threadBlue = (tid*30) % 256;
   for(iY=startRow;iY<endRow;iY++)
   {
      Cy=CyMin + iY*PixelHeight;
      if (fabs(Cy)< PixelHeight/2) Cy=0.0; 
      for(iX=0;iX<iXmax;iX++)
      {         
         Cx=CxMin + iX*PixelWidth;
         Zx=0.0;
         Zy=0.0;
         Zx2=Zx*Zx;
         Zy2=Zy*Zy;
         for (Iteration=0;Iteration<IterationMax && ((Zx2+Zy2)<ER2);Iteration++)
         {
            Zy=2*Zx*Zy + Cy;
            Zx=Zx2-Zy2 +Cx;
            Zx2=Zx*Zx;
            Zy2=Zy*Zy;
         };
         
         if (Iteration==IterationMax)
         { 
            color[iY][iX][0]=0;
            color[iY][iX][1]=0;
            color[iY][iX][2]=0;                           
         }
         else 
         { 
            color[iY][iX][0]=threadRed;
            color[iY][iX][1]=threadGreen;  
            color[iY][iX][2]=threadBlue;
         };
      }
   }
}

int main()
{
   FILE * fp;
   char *filename="new1.ppm";
   char *comment="# ";
   
   cout << "Give number of threads : ";
   cin >> num_threads;
   int rowsPerThread = iYmax / num_threads;
   thread threads_ms[num_threads];
   
   auto start = high_resolution_clock::now();
   
   for(int i = 0;i < num_threads;++i)
   {
      int startRow = i * rowsPerThread;
      int endRow = (i == num_threads - 1) ? iYmax : startRow + rowsPerThread;
      threads_ms[i] = thread(MandelbrotSet,i,startRow, endRow);
   }
   for(int i = 0;i < num_threads;++i)
   {
      threads_ms[i].join();
   }
   
   auto end = high_resolution_clock::now();
   auto elapsed = duration_cast<milliseconds>(end - start);
   cout << "Execution time: " << elapsed.count() << " ms" << endl;
   
   fp= fopen(filename,"wb"); 
   if (!fp) 
   {
        std::cerr << "Cant open file" << std::endl;
        return 1;
    }
   fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,iXmax,iYmax,MaxColorComponentValue);
   fwrite(color,3,iXmax*iYmax,fp);
   fclose(fp);
   return 0;
}
