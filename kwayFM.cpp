#include <iomanip>
#include "fm.cpp"
#include "writeInfo.cpp"
#include "maxheap.cpp"


int main(int argc, char* argv[])
{

    if ((argc == 1) || (argc % 2 == 0))
    {
        WrongUsage();
        return 1;
    }

    std::string fileName = "NO FILE";
    std::string partFile = "NO FILE";
    int segmentNum = 2;
    double balance_factor = 0.1;
    int data = 0;

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
        else if (arg_str == "-data")
        {
            data = atoi(argv[argIndex++]);
        }
        else if (arg_str == "-w")
        {
            //TODO:: -w true/false
        }
        else if (arg_str == "-s")
        {
            //TODO:: -s true/false  // simple output
        }
        else
        {
            WrongUsage();
            return 1;
        }
    }

    if (fileName == "NO FILE")
    {
        printf("ERROR !  A hgr file is required !\n\n");
        WrongUsage();
        return 1;
    }

    balance_factor += 1;
    TIME_START;
    // srand(clock());
    // GenerateInput(500, 500, 5);  // CellNum. NetNum, MaxCellNumInOneNet
    
    fm* fmExample = new fm(segmentNum, fileName, partFile, balance_factor, data);

    WriteHead(fmExample);
    WritePartitionParameters(fmExample, segmentNum, balance_factor, partFile, fileName);

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