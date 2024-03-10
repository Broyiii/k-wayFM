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

        for (auto net : Nets)
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
            for (auto net : Nets)
            {
                net->phi[seg] = 0;
            }
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

        for (auto cell : Cells)
        {
            for (int& netID : cell->nets)
            { 
                ++Nets[netID]->phi[cell->segment];
            }
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


void fm::UpdateGain(int cellID, Net* net)
{
    /* 
    cell : cell ID   , [0, cellNum-1]
    k    : segment   , [0, segmentNum-1]
    net  : net       , [Nets[0], Nets[netNum-1]]
    ith  : gain level, [1, max gain level]
    */
    Cell *cell = Cells[cellID];
    int j = cell->segment;

    if (net->phi[j] == 1)
    {
        for (int h = 0; h < segmentNum; ++h)
        {
            if ((h != j) && (net->phi[h] > 0))
            {
                cell->gain[h] += net->weight;
            }
        }
    }
    else
    {
        for (int h = 0; h < segmentNum; ++h)
        {
            if ((h != j) && (net->phi[h] == 0))
            {
                cell->gain[h] -= net->weight;
            }
        }
    }

    // int j = Cells[cell]->segment;
    // if (j != k)
    // {
    //     if ((net->phi[k] == (this->ith - 1)) && (net->phi[j] > 1))  // no cell on seg k, so gain should decrease
    //     {
    //         Cells[cell]->gain[k] -= net->weight;
    //     }
    // }
    // else
    // {
    //     if (net->phi[j] == this->ith)  // only one cell on seg j, so gain should increase
    //     {
    //         for (int h = 0; h < segmentNum; h++)
    //         {
    //             if ((h != j) && (net->phi[h] > 0))
    //             {
    //                 Cells[cell]->gain[h] += net->weight;
    //             }
    //         }
    //     }
    // }
}


void fm::ReverseUpdateGain(int cellID, Net* net)
{
    /* 
    cell : cell ID   , [0, cellNum-1]
    k    : segment   , [0, segmentNum-1]
    net  : net       , [Nets[0], Nets[netNum-1]]
    ith  : gain level, [1, max gain level]
    */
    Cell *cell = Cells[cellID];
    int j = cell->segment;

    if (net->phi[j] == 1)
    {
        for (int h = 0; h < segmentNum; ++h)
        {
            if ((h != j) && (net->phi[h] > 0))
            {
                cell->gain[h] -= net->weight;
            }
        }
    }
    else
    {
        for (int h = 0; h < segmentNum; ++h)
        {
            if ((h != j) && (net->phi[h] == 0))
            {
                cell->gain[h] += net->weight;
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


void fm::InitGainBucket()
{
    this->GainBucket->Init();
    for (auto& cell : Cells)
    {
        int sourceSeg = cell->segment;
        for (int k = 0; k < segmentNum; ++k)
        {
            if (k != sourceSeg)
            {
                // Gain* element = new Gain(cell->ID, sourceSeg, k, cell->gain[k]);
                this->GainBucket->Insert(new Gain(cell->ID, sourceSeg, k, cell->gain[k]));
            }
        }
    }
}


void fm::InitPartition()
{
    Order.clear();
    Gains.clear();
    // GainBucket->Init();

    for (auto cell : Cells)
    {
        cell->status = FREE;
        for (int i = 0; i < segmentNum; i++)
            cell->gain[i] = 0;
    }

    for (auto net : Nets)
    {
        // net->cells4MoveInParts.clear();
        // net->cells4MoveInParts.resize(segmentNum);
        // for (int& cellID : net->cells)
        // {
        //     net->cells4MoveInParts[Cells[cellID]->segment].emplace_back(cellID);
        // }
        net->isLocked = false;
        for (int k = 0; k < segmentNum; ++k)
        {
            net->isLockedPart[k] = false;
            for (int j = 0; j < segmentNum; ++j)
                net->gain[k][j] = 0;
        }
        net->UpdateNetGain();
        // for (int& cellID : net->cells)
        // {
        //     UpdateGain(cellID, net);
        // }
    }
    for (auto cell : Cells)
    {
        int k = cell->segment;
        for (int netID : cell->nets)
        {
            for (int j = 0; j < segmentNum; ++j)
            {
                if (k != j)
                {
                    cell->gain[j] += Nets[netID]->gain[k][j];
                }
            }
        }
    }
    if (dataStruct == 0)
        GetGainOrder();
    else
        InitGainBucket();
}

void fm::InsertNewGain(Gain &old_gain, Gain &new_gain, int cell)
{
    auto iter = Gains.find(old_gain.cellGain);
    if (iter != Gains.end())
    {
        if (Gains[old_gain.cellGain].size() > 1)
            Gains[old_gain.cellGain].erase(old_gain);
        else
            Gains.erase(old_gain.cellGain);
    }
    else
    {
        printf("ERROR! CANNOT EREASE CELL[%0d] GAIN!\n", cell);
        fprintf(this->outputFile, "ERROR! CANNOT EREASE CELL[%0d] GAIN!\n", cell);
    }
    // ------------------------------------------------------------------
    // insert the new gain ----------------------------------------------
    iter = Gains.find(new_gain.cellGain);
    if (iter != Gains.end()) 
    {
        iter->second.insert(new_gain);      
    }
    else  // insert value
    {
        std::set<Gain,gainSortCriterion> sg;
        sg.insert(new_gain);
        Gains.insert(std::pair<int,std::set<Gain,gainSortCriterion>>(new_gain.cellGain, sg));
    }
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
        if (Nets[netID]->isLocked)
        {
            --Nets[netID]->phi[sourceSeg]; 
            ++Nets[netID]->phi[targetSeg];
            continue;
        }

        if ((Nets[netID]->isLockedPart[sourceSeg] == true) && (Nets[netID]->isLockedPart[targetSeg] == true))
        {
            --Nets[netID]->phi[sourceSeg]; 
            ++Nets[netID]->phi[targetSeg];
            // Nets[netID]->UpdateLock(targetSeg);
            continue;
        }
        Nets[netID]->isLockedPart[targetSeg] = true;
        if ((Nets[netID]->phi[sourceSeg] >= 3) && (Nets[netID]->phi[targetSeg] >= 2))
        {
            --Nets[netID]->phi[sourceSeg]; 
            ++Nets[netID]->phi[targetSeg];
            Nets[netID]->UpdateLock(targetSeg);
            continue;
        }
        
        // update neighbors' gains
        if (dataStruct == 0)
        {
            // map
            for (int& cell : Nets[netID]->cells)
            {
                if ((Cells[cell]->status == FREE))
                {
                    Gain cell_gain = Cells[cell]->GetGain();
                    int cellSeg = Cells[cell]->segment;
                    // update src seg gains
                    if (cellSeg == sourceSeg)
                    {
                        if (Nets[netID]->phi[targetSeg] == 0)
                        {
                            if (Nets[netID]->phi[sourceSeg] == 2)
                            {
                                Cells[cell]->gain[targetSeg] += 2;
                            }
                            else
                            {
                                ++Cells[cell]->gain[targetSeg];
                            }
                        }
                        else if (Nets[netID]->phi[sourceSeg] == 2)
                        {
                            ++Cells[cell]->gain[targetSeg];
                        }

                        if (Nets[netID]->phi[sourceSeg] == 2)
                        {
                            for (int k = 0; k < segmentNum; ++k) {
                                if ((k != cellSeg) && (k != targetSeg))
                                {
                                    ++Cells[cell]->gain[k];
                                }
                            }
                        }
                    }
                    // update tar seg gains
                    else if (cellSeg == targetSeg)
                    {
                        if (Nets[netID]->phi[sourceSeg] == 1)
                        {
                            --Cells[cell]->gain[sourceSeg];
                        }
                        if (Nets[netID]->phi[targetSeg] == 1)
                        {
                            for (int k = 0; k < segmentNum; ++k) {
                                if (k != cellSeg) 
                                {
                                    --Cells[cell]->gain[k];
                                }
                            }
                        }
                    }
                    // update other seg gains
                    else
                    {
                        if (Nets[netID]->phi[sourceSeg] == 1)
                        {
                            --Cells[cell]->gain[sourceSeg];
                        }
                        if (Nets[netID]->phi[targetSeg] == 0)
                        {
                            ++Cells[cell]->gain[targetSeg];
                        }
                    }
                    Gain cell_gain_new = Cells[cell]->GetGain();
                    if (cell_gain != cell_gain_new)
                        InsertNewGain(cell_gain, cell_gain_new, cell);
                }
            }
        }
        else
        {
            // maxheap
            for (int& cell : Nets[netID]->cells)
            {
                if ((Cells[cell]->status == FREE))
                {
                    int cellSeg = Cells[cell]->segment;
                    // update src seg gains
                    if (cellSeg == sourceSeg)
                    {
                        if (Nets[netID]->phi[targetSeg] == 0)
                        {
                            if (Nets[netID]->phi[sourceSeg] == 2)
                            {
                                GainBucket->ChangePriority(cell, targetSeg, 2);
                            }
                            else
                            {
                                GainBucket->ChangePriority(cell, targetSeg, 1);
                            }
                        }
                        else if (Nets[netID]->phi[sourceSeg] == 2)
                        {
                            GainBucket->ChangePriority(cell, targetSeg, 1);
                        }

                        if (Nets[netID]->phi[sourceSeg] == 2)
                        {
                            for (int k = 0; k < segmentNum; ++k) {
                                if ((k != cellSeg) && (k != targetSeg))
                                {
                                    GainBucket->ChangePriority(cell, k, 1);
                                }
                            }
                        }
                    }
                    // update tar seg gains
                    else if (cellSeg == targetSeg)
                    {
                        if (Nets[netID]->phi[sourceSeg] == 1)
                        {
                            GainBucket->ChangePriority(cell, sourceSeg, -1);
                        }
                        if (Nets[netID]->phi[targetSeg] == 1)
                        {
                            for (int k = 0; k < segmentNum; ++k) {
                                if (k != cellSeg) 
                                {
                                    GainBucket->ChangePriority(cell, k, -1);
                                }
                            }
                        }
                    }
                    // update other seg gains
                    else
                    {
                        if (Nets[netID]->phi[sourceSeg] == 1)
                        {
                            GainBucket->ChangePriority(cell, sourceSeg, -1);
                        }
                        if (Nets[netID]->phi[targetSeg] == 0)
                        {
                            GainBucket->ChangePriority(cell, targetSeg, 1);
                        }
                    }
                }
            }
        }

        --Nets[netID]->phi[sourceSeg]; 
        ++Nets[netID]->phi[targetSeg];
        Nets[netID]->UpdateLock(targetSeg);
        continue;
        

        // // ==============================================================================
        // for (int& cell : Nets[netID]->cells)
        // {
        //     if ((Cells[cell]->status == FREE))
        //     {
        //         Gain cell_gain = Cells[cell]->GetGain();   
                
    //             ReverseUpdateGain(cell, Nets[netID]);

        //         // ----
        //         --Nets[netID]->phi[sourceSeg];
        //         ++Nets[netID]->phi[targetSeg];
            
    //             UpdateGain(cell, Nets[netID]);

        //         ++Nets[netID]->phi[sourceSeg];
        //         --Nets[netID]->phi[targetSeg];
        //         // ----

        //         Gain cell_gain_new = Cells[cell]->GetGain();

        //         if (cell_gain != cell_gain_new)
        //         {
        //            InsertNewGain(cell_gain, cell_gain_new, cell);
        //         }                
        //     }
        // }

        // --Nets[netID]->phi[sourceSeg];
        // ++Nets[netID]->phi[targetSeg];
    }
}


void fm::DoPass()
{
    if (dataStruct == 0)
    {
        newMove:
        if (Gains.size() != 0)
        {
            // auto g1 = Gains.begin()->second.begin();
            // auto g2 = GainBucket->GetMax();
            // std::cout << "g1: " << g1->cellID << " : " << g1->cellGain << std::endl;
            // std::cout << "g2: " << g2->cellID << " : " << g2->cellGain << std::endl;
            // if (g1->cellGain != g2->cellGain)
            //     std::cout << "xx" << std::endl;

            for (auto g = Gains.begin(); g != Gains.end(); g++)
            {
                for (auto sg = g->second.begin(); sg != g->second.end(); sg++)
                {
                    if (SatisfyBalance(sg->targetSegment, Cells[sg->cellID]->weight))
                    {
                        if (g->second.size() > 1)
                            g->second.erase(sg);
                        else
                            Gains.erase(g);
                        int moveCell = sg->cellID;
                        int tarSeg = sg->targetSegment;
                        Order.emplace_back(Gain(sg->cellID, sg->sourceSegment, sg->targetSegment, sg->cellGain));
                        // Order.emplace_back(*sg);

                        MakeMove(moveCell, tarSeg);
                        
                        goto newMove;  // break the first loop body, equals double break
                    }
                }
            }
        }
    }
    else
    {
        while (GainBucket->size != 0)
        {
            auto max_gain_ = GainBucket->ExtractMax();
            if (SatisfyBalance(max_gain_->targetSegment, Cells[max_gain_->cellID]->weight))
            {
                for (int seg = 0; seg < segmentNum; ++seg)
                {
                    if ((seg != max_gain_->sourceSegment) && (seg != max_gain_->targetSegment))
                    {
                        GainBucket->Remove(max_gain_->cellID, seg);
                    }
                }

                // int __cut__ = CaculateCutsize();
                Order.emplace_back(*max_gain_);
                // Order.emplace_back(Gain(max_gain_->cellID, max_gain_->sourceSegment, max_gain_->targetSegment, max_gain_->cellGain));
                MakeMove(max_gain_->cellID, max_gain_->targetSegment);
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
    std::cout << "        pass gain = " << maxOrderGain << std::endl;
}


int fm::CaculateCutsize()
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
    return this->cutSize;
}


int fm::Partition()
{
    if (!GetInputInfo())
        return 0;
    this->GainBucket = new MaxHeap(this->CellNum, segmentNum);

    if (this->inputPartFile != "NO FILE")
        if(!GetPartInfo())
            return 0;
    
    CaculateCutsize();
    this->originalCutSize = this->cutSize;
 
    WriteHypergraphInfomation();

    int i = 0;
            
    do {
        ++i;
        InitPartition();

        // int g1 = Gains.begin()->first;
        // int g2 = GainBucket->GetMax()->cellGain;
        // std::cout << g1 << "\t" << g2 << std::endl;

        DoPass();
        this->cutSize -= this->maxOrderGain;
        //printf("Current  cutsize = %0d\n", cutSize);
        printf("    - Cut-size after pass #%3d: %0d\n", i, this->cutSize);
        fprintf(this->outputFile, "    - Cut-size after pass #%3d: %0d\n", i, this->cutSize);
    } 
    // while (this->maxOrderGain > 0);
    while (i < 10);

    WriteResult();

    return i;
}