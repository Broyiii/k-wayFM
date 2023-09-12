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


void GenerateInput(int CellNum = 50, int NetNum = 20, int maxCellNumInNet = 10, std::string fileName = "inputInfo")
{
    fileName = "./work/demo/test.hgr";
    std::ofstream ofs;
    ofs.open(fileName, std::ios::out);

    //srand((unsigned int)time(NULL));
    ofs << CellNum << " " << NetNum << std::endl;

    /*
    for (int i = 0; i < CellNum; ++i)
    {
        ofs << rand() % 10 + 1 << std::endl;
    }
    */

    for (int i = 0; i < NetNum; ++i)
    {
        //int hasID[CellNum] = { 0 };
        int* hasID = new int[CellNum];
        for (int ii = 0; ii < CellNum; ii++)
            hasID[ii] = 0;
        //int n = rand() % (CellNum - 1) + 1;
        int n = rand() % ((maxCellNumInNet > NetNum) ? NetNum - 1 : maxCellNumInNet) + 1;
        int source = rand() % CellNum;
        hasID[source] = 1;
        //ofs << rand() % 10 + 1 << " " << source << " ";
        ofs << source + 1 << " ";
        for (int j = 0; j < n; ++j)
        {
            int target = rand() % CellNum;
            while (1)
            {
                if (hasID[target] == 0)
                {
                    ofs << target + 1 << " ";
                    hasID[target] = 1;
                    break;
                }
                else
                    target = rand() % CellNum;
            }
        }
        ofs << std::endl;
    }

    ofs.close();
}


void WrongUsage()
{
    std::cerr << "Usage:\n"
              << "\t./kwayFM [-command] <parameter>\n"
              << "\nRequired commands:\n"
              << "\t-h <hgr_file_directory>             | input a hgr file\n"
              << "\nOptional commands:\n"
              << "\t-k <segment_num>                    | input a segment number   , defalut is 2\n"
              << "\t-e <balance_factor>                 | input a balance factor   , defalut is 0.1\n"
              << "\t-p <partition_file_directory>       | input a partition result , defalut is random\n"
              << std::endl;
}


#endif