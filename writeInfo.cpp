#ifndef _WRITE_CPP_
#define _WRITE_CPP_

#include <time.h>
#include "fm.h"

#define TIME_START              \
    clock_t ____start, ____end; \
    ____start = clock()
#define TIME_OUT                \
    ____end = clock();          \


#define GET_DATE                \
    time_t today = time(0);     \
    char tmp[21];               \
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&today)); 


void WriteHead(fm *fmExample)
{
    GET_DATE;
    printf("+========================================================================+\n");
    printf("|                                                                        |\n");
    printf("|                      K-way FM Hypergraph Partition                     |\n");
    printf("|                                                                        |\n");
    printf("|                           Author : @Broyi                              |\n");
    printf("|                           Version: 2023-09-10                          |\n");
    printf("|                           Date   : %s                 |\n",tmp);
    printf("+========================================================================+\n\n");

    fprintf(fmExample->outputFile, "+========================================================================+\n");
    fprintf(fmExample->outputFile, "|                                                                        |\n");
    fprintf(fmExample->outputFile, "|                      K-way FM Hypergraph Partition                     |\n");
    fprintf(fmExample->outputFile, "|                                                                        |\n");
    fprintf(fmExample->outputFile, "|                           Author : @Broyi                              |\n");
    fprintf(fmExample->outputFile, "|                           Version: 2023-09-10                          |\n");
    fprintf(fmExample->outputFile, "|                           Date   : %s                 |\n",tmp);
    fprintf(fmExample->outputFile, "+========================================================================+\n\n");
}


void WritePartitionParameters(fm *fmExample, int segmentNum, double balance_factor, std::string partFile, std::string fileName)
{
    printf("**************************************************************************\n");
    printf("*                          Partitioning Context                          *\n");
    printf("**************************************************************************\n");
    printf("Partition Parameters:\n");
    printf("    - Hypergraph:                       %s\n", fileName.c_str());
    if (partFile != "NO FILE")
    {
        printf("    - Initial Partition:                %s\n", partFile.c_str());
    }
    printf("    - Segment Number:                   %0d\n", segmentNum);
    printf("    - Imbalance Parameter Epsilon:      %.2f\n", balance_factor);
    printf("--------------------------------------------------------------------------\n");

    fprintf(fmExample->outputFile, "**************************************************************************\n");
    fprintf(fmExample->outputFile, "*                          Partitioning Context                          *\n");
    fprintf(fmExample->outputFile, "**************************************************************************\n");
    fprintf(fmExample->outputFile, "Partition Parameters:\n");
    fprintf(fmExample->outputFile, "    - Hypergraph:                       %s\n", fileName.c_str());
    if (partFile != "NO FILE")
    {
        fprintf(fmExample->outputFile, "    - Initial Partition:                %s\n", partFile.c_str());
    }
    fprintf(fmExample->outputFile, "    - Segment Number:                   %0d\n", segmentNum);
    fprintf(fmExample->outputFile, "    - Imbalance Parameter Epsilon:      %.2f\n", balance_factor);
    fprintf(fmExample->outputFile, "--------------------------------------------------------------------------\n");
}


void fm::WriteHypergraphInfomation()
{
    int totalCellWeight = 0;
    for (int i = 0; i < this->segmentNum; i++)
    {
        totalCellWeight += this->Segments[i];
    }

    printf("Hypergraph Infomation:\n");
    printf("    - Total Net Number:                 %0d\n", this->NetNum);
    printf("    - Total Cell Number:                %0d\n", this->CellNum);
    printf("    - Total Cell Weight:                %0d\n", totalCellWeight);
    printf("    - Max Segment Weight:               %0d\n", this->maxCellNum);
    printf("    - Initial Cut-size:                 %0d\n", this->cutSize);
    printf("--------------------------------------------------------------------------\n\n");
    printf("**************************************************************************\n");
    printf("*                          Partitioning Result                           *\n");
    printf("**************************************************************************\n");
    printf("Current Cut-size:\n");
    printf("    - Cut-size after pass #%2d: %0d\n", 0, this->cutSize);
    
    fprintf(this->outputFile, "Hypergraph Infomation:\n");
    fprintf(this->outputFile, "    - Total Net Number:                 %0d\n", this->NetNum);
    fprintf(this->outputFile, "    - Total Cell Number:                %0d\n", this->CellNum);
    fprintf(this->outputFile, "    - Total Cell Weight:                %0d\n", totalCellWeight);
    fprintf(this->outputFile, "    - Max Segment Weight:               %0d\n", this->maxCellNum);
    fprintf(this->outputFile, "    - Initial Cut-size:                 %0d\n", this->cutSize);
    fprintf(this->outputFile, "--------------------------------------------------------------------------\n\n");
    fprintf(this->outputFile, "**************************************************************************\n");
    fprintf(this->outputFile, "*                          Partitioning Result                           *\n");
    fprintf(this->outputFile, "**************************************************************************\n");
    fprintf(this->outputFile, "Current Cut-size:\n");
    fprintf(this->outputFile, "    - Cut-size after pass #%2d: %0d\n", 0, this->cutSize);
}


void fm::WriteResult()
{
    printf("--------------------------------------------------------------------------\n");
    printf("Partition Weights:\n");
    for (int seg = 0; seg < this->segmentNum; seg++)
    {
        printf("    |part %4d | = %6d\t| Resource Utilization = %.2f %%\n", seg, this->Segments[seg], (double)this->Segments[seg] * 100 / this->maxCellNum);
    }
    printf("--------------------------------------------------------------------------\n");
    printf("Result:\n");
    printf("    - Initial Cut-size           = %0d\n", this->originalCutSize);
    printf("    - Final Cut-size (Gain Sum)  = %0d\n", this->cutSize);

    fprintf(this->outputFile, "--------------------------------------------------------------------------\n");
    fprintf(this->outputFile, "Partition Weights:\n");
    for (int seg = 0; seg < this->segmentNum; seg++)
    {
        fprintf(this->outputFile, "    |part %4d | = %6d\t| Resource Utilization = %.2f %%\n", seg, this->Segments[seg], (double)this->Segments[seg] * 100 / this->maxCellNum);
    }
    fprintf(this->outputFile, "--------------------------------------------------------------------------\n");
    fprintf(this->outputFile, "Result:\n");
    fprintf(this->outputFile, "    - Initial Cut-size           = %0d\n", this->originalCutSize);
    fprintf(this->outputFile, "    - Final Cut-size (Gain Sum)  = %0d\n", this->cutSize);

    CaculateCutsize();
    printf("    - Final Cut-size (Calculate) = %0d\n", this->cutSize);
    fprintf(this->outputFile, "    - Final Cut-size (Calculate) = %0d\n", this->cutSize);
}

#endif