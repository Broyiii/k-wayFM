#include <time.h>
#include <iomanip>
#include "fm.cpp"


#define TIME_START              \
    clock_t ____start, ____end; \
    ____start = clock()
#define TIME_OUT                \
    ____end = clock();          \


#define GET_DATE                \
    time_t today = time(0);     \
    char tmp[21];               \
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&today)); 


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


int main(int argc, char* argv[])
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

    if ((argc == 1) || (argc % 2 == 0))
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         {
        WrongUsage();
        return 1;
    }


    std::string workDir = "./work/";
    std::string fileName = "./work/ispd/ibm01/ibm01.hgr";
    std::string partFile = "NO FILE";
    int segmentNum = 2;
    double balance_factor = 0.1;

    int argIndex = 1;
    while (argIndex < (argc - 1))
    {
        std::string arg_str = argv[argIndex++];
        if (arg_str == "-h")
        {
            fileName = argv[argIndex++];
        }
        else if (arg_str == "-p")
        {
            partFile = argv[argIndex++];
        }
        else if (arg_str == "-k")
        {
            segmentNum = atoi(argv[argIndex++]);
        }
        else if (arg_str == "-e")
        {
            balance_factor = std::stod(argv[argIndex++]);
        }
        else
        {
            WrongUsage();
            return 1;
        }
    }
    balance_factor += 1;
    TIME_START;
    // srand(clock());
    //GenerateInput(500, 500, 5);  // CellNum. NetNum, MaxCellNumInOneNet
    
    fm* fmExample = new fm(segmentNum, fileName, partFile, balance_factor);
    fprintf(fmExample->outputFile, "+========================================================================+\n");
    fprintf(fmExample->outputFile, "|                                                                        |\n");
    fprintf(fmExample->outputFile, "|                      K-way FM Hypergraph Partition                     |\n");
    fprintf(fmExample->outputFile, "|                                                                        |\n");
    fprintf(fmExample->outputFile, "|                           Author : @Broyi                              |\n");
    fprintf(fmExample->outputFile, "|                           Version: 2023-09-10                          |\n");
    fprintf(fmExample->outputFile, "|                           Date   : %s                 |\n",tmp);
    fprintf(fmExample->outputFile, "+========================================================================+\n\n");

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
    


    int itTime = fmExample->Partition();

    TIME_OUT;
    if (itTime == 0)
    {
        printf("ERROR! Exit the program!\n\n");

        fprintf(fmExample->outputFile, "ERROR! Exit the program!\n\n");
        return 1;
    }

    printf("    - Iterate Time               = %0d\n", itTime);
    printf("    - Running Time               = %0ld ms\n", (____end - ____start) / 1000); 
    printf("--------------------------------------------------------------------------\n");         
    printf("\n\n");

    fprintf(fmExample->outputFile, "    - Iterate Time               = %0d\n", itTime);
    fprintf(fmExample->outputFile, "    - Running Time               = %0ld ms\n", (____end - ____start) / 1000); 
    fprintf(fmExample->outputFile, "--------------------------------------------------------------------------\n");   
    fprintf(fmExample->outputFile, "\n\n");
    fmExample->ShowCells();

    fmExample->~fm();
    // printf("-----K-way FM Partition END !-----\n\n");

    return 0;
}