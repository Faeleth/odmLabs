#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <thread>
#include <iostream>

#define N 4000

using namespace std;

const int numThreads = 8; //od 1-9k

const int iXmax = N;
const int iYmax = N;
const double CxMin = -2.5;
const double CxMax = 1.5;
const double CyMin = -2.0;
const double CyMax = 2.0;
const int MaxColorComponentValue = 255;
const int IterationMax = 200;
const double EscapeRadius = 2;
double PixelWidth, PixelHeight;
double ER2;
unsigned char color[iXmax][iYmax][3];

void callFromThreads(int tid)
{
    int package = N / numThreads;
    int lb = tid * package;
    int ub = (tid + 1) * package;

    for (int iY = lb; iY < ub; iY++)
    {
        double Cy = CyMin + iY * PixelHeight;
        if (fabs(Cy) < PixelHeight / 2)
            Cy = 0.0; /* Main antenna */

        for (int iX = 0; iX < iXmax; iX++)
        {
            double Cx = CxMin + iX * PixelWidth;

            double Zx = 0.0;
            double Zy = 0.0;
            double Zx2 = Zx * Zx;
            double Zy2 = Zy * Zy;

            int Iteration;
            for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
            {
                Zy = 2 * Zx * Zy + Cy;
                Zx = Zx2 - Zy2 + Cx;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
            }

            if (Iteration == IterationMax)
            {
                color[iY][iX][0] = 0;
                color[iY][iX][1] = 0;
                color[iY][iX][2] = 0;
            }
            else
            {
                color[iY][iX][0] = 50*tid; /* Red */
                color[iY][iX][1] = 100*tid; /* Green */
                color[iY][iX][2] = 88*tid; /* Blue */
            }
        }
    }
}

void writeImageData(FILE *fp, unsigned char color[iXmax][iYmax][3])
{
    for (int iY = 0; iY < iYmax; iY++)
    {
        for (int iX = 0; iX < iXmax; iX++)
        {
            fwrite(color[iY][iX], 1, 3, fp);
        }
    }
}

int main()
{
    PixelWidth = (CxMax - CxMin) / iXmax;
    PixelHeight = (CyMax - CyMin) / iYmax;
    ER2 = EscapeRadius * EscapeRadius;

    FILE *fp;
    char *filename = "new1.ppm";
    char *comment = "# ";       /* comment should start with # */
    fp = fopen(filename, "wb"); /* b -  binary mode */

    fprintf(fp, "P6\n%s\n%d\n%d\n%d\n", comment, iXmax, iYmax, MaxColorComponentValue);

    thread t[numThreads];

    const auto start{std::chrono::steady_clock::now()};

    // Launch a group of threads
    for (int i = 0; i < numThreads; i++)
    {
        t[i] = thread(callFromThreads, i);
    }

    // Join the threads  with the main thread

    for (int i = 0; i < numThreads; i++)
    {
        t[i].join();
    }

    const auto end{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end - start};
    cout << "Elapsed time(" << numThreads << " threads): " << elapsed_seconds.count() << "s\n";

    writeImageData(fp, color);

    fclose(fp);
    return 0;
}
