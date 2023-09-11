#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <cmath>

#include "fm.h"


std::vector<std::string> SplitBySpace(std::string str)
{
    std::string buf;
    std::stringstream ss(str);

    std::vector<std::string> tokens;

    while (ss >> buf)
        tokens.emplace_back(buf);

    return tokens;
}


bool fm::GetInputInfo()
{
    srand(clock());
    Cells.clear();
    Nets.clear();

    std::ifstream inputFile(this->inputFileDir);
    std::string str;
    if (inputFile)
    {
        int cnt = 0;
        while (std::getline(inputFile, str))
        {
            ++cnt;
            std::vector<std::string> data = SplitBySpace(str);
            if (cnt == 1)
            {
                NetNum  = atoi(data[0].c_str());
                CellNum = atoi(data[1].c_str());
                if (data.size() == 3) {
                    switch (atoi(data[2].c_str()))
                    {
                    case NET_WEIGHT:
                        netWeight  = true;
                        cellWeight = false;
                        break;
                    case CELL_WEIGHT:
                        netWeight  = false;
                        cellWeight = true;
                        break;
                    case BOTH_WEIGHT:
                        netWeight  = true;
                        cellWeight = true;
                        break;
                    default:
                        netWeight  = false;
                        cellWeight = false;
                        break;
                    }
                } else {
                    netWeight  = false;
                    cellWeight = false;
                }
            }
            else if (cnt <= 1 + NetNum)  // get net
            {
                if (netWeight)
                {
                    Net *net = new Net(cnt - 2, atoi(data[0].c_str()), segmentNum);
                    for (auto i = 1; i < data.size(); ++i)
                    {
                        int cellID = atoi(data[i].c_str()) - 1;
                        net->cells.emplace_back(cellID);
                    }
                    Nets.emplace_back(net);
                }
                else
                {
                    Net *net = new Net(cnt - 2, 1, segmentNum);
                    for (auto i = 0; i < data.size(); ++i)
                    {
                        int cellID = atoi(data[i].c_str()) - 1;
                        net->cells.emplace_back(cellID);
                    }
                    Nets.emplace_back(net);
                }
            }
            else  // get cell
            {
                if (cellWeight)
                {
                    Cell* cell = new Cell(cnt - NetNum - 2, atoi(data[0].c_str()), segmentNum);                
                    cell->segment = cell->ID % segmentNum;
                    //cell->segment = rand() % segmentNum;
                    this->Segments[cell->segment] += cell->weight;
                    cell->status = FREE;

                    for (int i = 0; i < segmentNum; i++)
                    {
                        cell->gain[i] = 0;
                    }

                    Cells.emplace_back(cell);

                    maxCellNum += cell->weight;     // instore sum weight tmply
                }
                else
                {
                    std::cerr << "ERROR IN GET INFO! " <<std::endl;
                    return false;
                }
            }
        }

        if (!cellWeight)
        {
            for (int i = 0; i < CellNum; i++)
            {
                Cell* cell = new Cell(i, 1, segmentNum);
                cell->segment = cell->ID % segmentNum;
                //cell->segment = rand() % segmentNum;
                this->Segments[cell->segment] += cell->weight;

                cell->status = FREE;

                for (int j = 0; j < segmentNum; j++)
                {
                    cell->gain[j] = 0;
                }

                Cells.emplace_back(cell);

                maxCellNum += cell->weight;     // instore sum weight tmply
            }
        }

        for (auto& net : Nets)
        {
            for (int& cellID : net->cells)
            {
                Cells[cellID]->nets.emplace_back(net->ID);
            }
        }

        for (auto cell : Cells)
        {
            for (int& netID : cell->nets)
            { 
                ++Nets[netID]->phi[cell->segment];
            }
        }

        GetBalanceNum();
        inputFile.close();
        return true;
    }
    else
    {
        std::cerr << "\nNo such hgr file: " << this->inputFileDir << std::endl;
        return false;
    }

}


bool fm::GetPartInfo()
{
    std::ifstream inputFile(this->inputPartFile);
    std::string str;
    if (inputFile)
    { 
        for (int seg = 0; seg < this->segmentNum; seg++)
        {
            this->Segments[seg] = 0;
        }
        int cellIndex = 0;
        int cellSeg = 0;
        while (std::getline(inputFile, str))
        {
            cellSeg = atoi(str.c_str());
            if (cellSeg >= this->segmentNum)
            {
                std::cerr << "Parition file has too many segments !" << std::endl;
                return false;
            }
            this->Segments[cellSeg] += Cells[cellIndex]->weight;
            this->Cells[cellIndex++]->segment = cellSeg;
        }
        inputFile.close();
        return true;
    }
    else
    {
        std::cerr << "\nNo such partition file: " << this->inputPartFile << std::endl;
        return false;
    }
}


void fm::GetBalanceNum()
{
    maxCellNum = balance_factor * maxCellNum / segmentNum;
}


bool fm::SatisfyBalance(int targetSeg, int weight)
{
    if ((Segments[targetSeg] + weight) <= maxCellNum)
        return true;
    else
        return false;
}


void fm::ShowCells()
{
    /* 
     * Write ouput info into log file
     * such as "ibm01_segNum_2_output.log"
     */
    fprintf(this->outputFile, "Partion Reslut:\n");
    fprintf(this->outputFile, "+=========================+\n");
    fprintf(this->outputFile, "|   ID   | weight |  seg  |\n");
    fprintf(this->outputFile, "|--------+--------+-------|\n");
    for (auto& cell : Cells)
    {
        fprintf(this->outputFile, "|%6d  |%5d   |%4d   |\n",
            cell->ID + 1, cell->weight, cell->segment);
    }
    fprintf(this->outputFile, "+=========================+\n");
}


void fm::UpdateGain(int cell, int k, Net* net)
{
    /* 
    cell : cell ID   , [0, cellNum-1]
    k    : segment   , [0, segmentNum-1]
    net  : net       , [Nets[0], Nets[netNum-1]]
    ith  : gain level, [1, max gain level]
    */
    int j = Cells[cell]->segment;
    if (j != k)
    {
        if ((net->phi[k] == (this->ith - 1)) && (net->phi[j] > 1))  // no cell on seg k, so gain should decrease
        {
            Cells[cell]->gain[k] -= net->weight;
        }
    }
    else
    {
        if (net->phi[j] == this->ith)  // only one cell on seg j, so gain should increase
        {
            for (int h = 0; h < segmentNum; h++)
            {
                if ((h != j) && (net->phi[h] > 0))
                {
                    Cells[cell]->gain[h] += net->weight;
                }
            }
        }
    }
}


void fm::ReverseUpdateGain(int cell, int k, Net* net)
{
    /* 
    cell : cell ID   , [0, cellNum-1]
    k    : segment   , [0, segmentNum-1]
    net  : net       , [Nets[0], Nets[netNum-1]]
    ith  : gain level, [1, max gain level]
    */
    int j = Cells[cell]->segment;
    if (j != k)
    {
        if ((net->phi[k] == (this->ith - 1)) && (net->phi[j] > 1))
        {
            Cells[cell]->gain[k] += net->weight;
        }
    }
    else
    {
        if (net->phi[j] == this->ith)
        {
            for (int h = 0; h < segmentNum; h++)
            {
                if ((h != j) && (net->phi[h] > 0))
                {
                    Cells[cell]->gain[h] -= net->weight;
                }
            }
        }
    }
}



void fm::GetGainOrder()
{
    for (auto& cell : Cells)
    {
        if (cell->status == FREE)
        {
            Gain cell_gain = cell->GetGain();

            auto iter = Gains.find(cell_gain.cellGain);
            if (iter == Gains.end())  // hasn't this gain key, create new key
            {
                std::set<Gain,gainSortCriterion> sg;
                sg.insert(cell_gain);
                Gains.insert(std::pair<int,std::set<Gain,gainSortCriterion>>(cell_gain.cellGain, sg));
                sg.clear();
            }
            else  // insert value
            {
                iter->second.insert(cell_gain);
            }
        }
    }
}


void fm::InitPartition()
{
    Order.clear();
    Gains.clear();

    for (auto cell : Cells)
    {
        cell->status = FREE;
        for (int i = 0; i < segmentNum; i++)
            cell->gain[i] = 0;
    }

    for (auto net : Nets)
    {
        for (int k = 0; k < segmentNum; k++)
        {
            for (int& cellID : net->cells)
            {
                UpdateGain(cellID, k, net);
            }
        }
    }
    GetGainOrder();
}


void fm::MakeMove(int cellID, int targetSeg)
{
    // move base cell
    int sourceSeg = Cells[cellID]->segment;
    Cells[cellID]->segment = targetSeg;
    Cells[cellID]->status = LOCKED;
    Segments[sourceSeg] -= Cells[cellID]->weight;
    Segments[targetSeg] += Cells[cellID]->weight;

    for (int& netID : Cells[cellID]->nets)
    {
        // update neighbors' gains
        // before move
        for (int& cell : Nets[netID]->cells)
        {
            if ((Cells[cell]->status == FREE))
            {
                Gain cell_gain = Cells[cell]->GetGain();   
                
                for (int i = 0; i < segmentNum; i++)
                    ReverseUpdateGain(cell, i, Nets[netID]);
                
                Gain cell_gain_new = Cells[cell]->GetGain();

                if (cell_gain != cell_gain_new)
                {
                    // delete the old gain ----------------------------------------------
                    auto iter = Gains.find(cell_gain.cellGain);
                    if (iter != Gains.end())
                    {
                        // 添加size()的判断并未有变快
                        if (Gains[cell_gain.cellGain].size() > 1)
                            Gains[cell_gain.cellGain].erase(cell_gain);
                        else
                            Gains.erase(cell_gain.cellGain);
                    }
                    else
                    {
                        printf("ERROR! CANNOT EREASE CELL[%0d] GAIN!\n", cell);
                        fprintf(this->outputFile, "ERROR! CANNOT EREASE CELL[%0d] GAIN!\n", cell);
                    }
                    // ------------------------------------------------------------------
                    // insert the new gain ----------------------------------------------
                    iter = Gains.find(cell_gain_new.cellGain);
                    if (iter != Gains.end()) 
                    {
                        iter->second.insert(cell_gain_new);      
                    }
                    else  // insert value
                    {
                        std::set<Gain,gainSortCriterion> sg;
                        sg.insert(cell_gain_new);
                        Gains.insert(std::pair<int,std::set<Gain,gainSortCriterion>>(cell_gain_new.cellGain, sg));
                    }
                    // ------------------------------------------------------------------
                }                
            }
        }

        --Nets[netID]->phi[sourceSeg];
        ++Nets[netID]->phi[targetSeg];

        for (int& cell : Nets[netID]->cells)
        {
            if ((Cells[cell]->status == FREE) && (cell != cellID))
            {
                Gain cell_gain = Cells[cell]->GetGain();

                for (int i = 0; i < segmentNum; i++)
                    UpdateGain(cell, i, Nets[netID]);

                Gain cell_gain_new = Cells[cell]->GetGain();

                if (cell_gain != cell_gain_new)
                {
                    // delete the old gain ----------------------------------------------
                    auto iter = Gains.find(cell_gain.cellGain);
                    if (iter != Gains.end())
                    {
                        // 添加size()的判断并未有变快
                        if (Gains[cell_gain.cellGain].size() > 1)
                            Gains[cell_gain.cellGain].erase(cell_gain);
                        else
                            Gains.erase(cell_gain.cellGain);
                    }
                    else
                    {
                        printf("ERROR! CANNOT EREASE CELL[%0d] GAIN!\n", cell);
                        fprintf(this->outputFile, "ERROR! CANNOT EREASE CELL[%0d] GAIN!\n", cell);
                    }
                    // ------------------------------------------------------------------
                    // insert the new gain ----------------------------------------------
                    iter = Gains.find(cell_gain_new.cellGain);
                    if (iter != Gains.end()) 
                    {
                        iter->second.insert(cell_gain_new);      
                    }
                    else  // insert value
                    {
                        std::set<Gain,gainSortCriterion> sg;
                        sg.insert(cell_gain_new);
                        Gains.insert(std::pair<int,std::set<Gain,gainSortCriterion>>(cell_gain_new.cellGain, sg));
                    }
                    // ------------------------------------------------------------------
                }   
            }
        }

    }
}


void fm::DoPass()
{
    newMove:
    if (Gains.size() != 0)
    {
        for (auto g = Gains.begin(); g != Gains.end(); g++)
        {
            for (auto sg = g->second.begin(); sg != g->second.end(); sg++)
            {
                if (SatisfyBalance(sg->targetSegment, Cells[sg->cellID]->weight))
                {
                    Order.emplace_back(Gain(sg->cellID, sg->sourceSegment, sg->targetSegment, sg->cellGain));
                    MakeMove(sg->cellID, sg->targetSegment);

                    if (g->second.size() > 1)
                        g->second.erase(sg);
                    else
                        Gains.erase(g);

                    goto newMove;  // break the first loop body, equals double break
                }
            }
        }
    }

    // Find the max sum of Gains
    this->maxOrderGain = 0;
    int maxOrderIndex = -1;
    int orderSum = 0;
    for (int i = 0; i < Order.size(); i++)
    {
        orderSum += Order[i].cellGain;
        if (orderSum > this->maxOrderGain)
        {
            this->maxOrderGain = orderSum;
            maxOrderIndex = i;
        }
    }
    
    // // // for debug--------------------------------
    // int GainsSize = 0;
    // if (Gains.size() != 0)
    // {
    //     for (auto g = Gains.begin(); g != Gains.end(); g++)
    //     {
    //         GainsSize += g->second.size();
    //     }
    // }
    // for (int i = 0; i <= 10; i++)
    // {
    //     if (Order[i].cellGain != 0)
    //         std::cout << i << " : " << Order[i].cellID << " : " << Order[i].cellGain << std::endl;
    // }
    // std::cout << "size Gains = " << GainsSize << std::endl;
    // std::cout << "max order gain = " << this->maxOrderGain << std::endl;
    // std::cout << "maxOrderIndex  = " << maxOrderIndex << std::endl;
    // std::cout << "size order = " << this->Order.size() << std::endl;
    // // --------------------------------

    for (int i = maxOrderIndex + 1; i < Order.size(); i++)
    {
        Cells[Order[i].cellID]->segment = Order[i].sourceSegment;
        Segments[Order[i].sourceSegment] += Cells[Order[i].cellID]->weight;
        Segments[Order[i].targetSegment] -= Cells[Order[i].cellID]->weight;

        for (int& netID : Cells[Order[i].cellID]->nets)
        {
            ++Nets[netID]->phi[Order[i].sourceSegment];
            --Nets[netID]->phi[Order[i].targetSegment];
        }
    }
}


void fm::CaculateCutsize()
{
    this->cutSize = 0;
    int *hasSegment = new int[this->segmentNum];
    for (auto net : Nets)
    {
        for (int i = 0; i < this->segmentNum; i++)
        {
            hasSegment[i] = 0;
        }

        for (int& cellID : net->cells)
        {
            if (hasSegment[Cells[cellID]->segment] == 0)
            {
                hasSegment[Cells[cellID]->segment] = 1;
                this->cutSize += net->weight;
            }
        }
        this->cutSize -= net->weight;
    }
    delete[] hasSegment;
}


int fm::Partition()
{
    if (!GetInputInfo())
        return 0;

    if (this->inputPartFile != "NO FILE")
        if(!GetPartInfo())
            return 0;
    
    CaculateCutsize();
    this->originalCutSize = this->cutSize;

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


    int i = 0;
    int cnt = 0;
            
    // for (int j = 0; j < 1; j ++)
    {
        // for (this->ith = 2; this->ith >= 1; this->ith--)
        {
            do {
                ++i;
                InitPartition();
                DoPass();
                this->cutSize -= this->maxOrderGain;
                //printf("Current  cutsize = %0d\n", cutSize);
                printf("    - Cut-size after pass #%2d: %0d\n", i, this->cutSize);
                fprintf(this->outputFile, "    - Cut-size after pass #%2d: %0d\n", i, this->cutSize);
            } 
            while (this->maxOrderGain > 0);
        }
    }

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

    return i;
}