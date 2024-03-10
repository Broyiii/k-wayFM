#ifndef _FM_H_
#define _FM_H_
#include <vector>
#include <map>
#include <set>
#include <climits>
#include <list>
#include <algorithm>

// #include "maxheap.hpp"

constexpr int FREE        = 0;
constexpr int LOOSE       = 1;
constexpr int LOCKED      = 2;

constexpr int NONE_WEIGHT = 0;
constexpr int NET_WEIGHT  = 1;
constexpr int CELL_WEIGHT = 10;
constexpr int BOTH_WEIGHT = 11;


class compare
{
public:
	bool operator()(int m, int n)const
	{
		return m > n;
	}
};


struct Gain
{
    int cellID;
    int sourceSegment;
    int targetSegment;
    int cellGain;
    
    Gain(int id, int ss, int ts, int cg) : 
        cellID(id), sourceSegment(ss), targetSegment(ts), cellGain(cg) {}
    
    bool operator!=(const Gain &g) {
        if ((this->targetSegment != g.targetSegment) || (this->cellGain != g.cellGain))
        {
            return true;
        } else {
            return false;
        }
    }
};


struct Cell
{
    const int ID;
    const int weight;
    int segment = 0;
    int status = FREE;  // 0->free, 2->locked
    int segmentNum = 2;
    int* gain;
    std::vector< int > nets;

    Cell(int id, int w, int sn) : ID(id), weight(w), segmentNum(sn) {
        gain = new int[segmentNum];
    }
    ~Cell() {
        delete[] gain;
    }

    Gain GetGain() {
        int gainMax = INT_MIN;
        int tarSeg = 0;
        for (int j = 0; j < segmentNum; j++)
        {
            if ((this->gain[j] > gainMax) && (j != this->segment))
            {
                gainMax = this->gain[j];
                tarSeg = j;
            }
        }
        return Gain(this->ID, this->segment, tarSeg, gainMax);
    }

};


struct Net
{
    const int ID;
    const int weight;
    int segmentNum = 2;

    int* phi;     // num of cells in seg k
    bool* isLockedPart;
    bool isLocked;

    // std::vector<std::vector<int>> gain;  // <k,j> cut gain from part k to j
    int **gain;
    
    std::vector< int > cells;
    // std::vector< std::list<int> > cells4MoveInParts;

    Net(int id, int w, int sn) : ID(id), weight(w), segmentNum(sn) {
        this->phi = new int[segmentNum];
        this->isLockedPart = new bool[segmentNum];
        // this->gain.resize(segmentNum);
        this->gain = new int*[segmentNum];
        for (int seg = 0; seg < segmentNum; seg++)
        {
            this->phi[seg] = 0;
            // this->gain[seg].resize(segmentNum, 0);
            this->gain[seg] = new int[segmentNum];
        }
    }
    ~Net() {
        delete[] phi;
    }

    void UpdateLock(int tarSeg)
    {
        if (this->isLocked)
            return;

        this->isLockedPart[tarSeg] = true;
        for (int k = 0; k < segmentNum; ++k)
        {
            if (!this->isLockedPart[k])
                return;
        }
        this->isLocked = true;
    }

    void UpdateNetGain()
    {
        for(int k = 0; k < segmentNum; ++k)
        {
            if (this->phi[k] == 0)
                continue;

            if (this->phi[k] == 1)
            {
                for (int j = 0; j < segmentNum; ++j)
                {
                    if ((k != j) && (this->phi[j] > 0))
                    {
                        this->gain[k][j] += this->weight;
                    }
                }
            }
            else
            {
                for (int j = 0; j < segmentNum; ++j)
                {
                    if ((k != j) && (this->phi[j] == 0))
                    {
                        this->gain[k][j] -= this->weight;
                    }
                }
            }
        }
    }
};


class gainSortCriterion
{  
public:  
    bool operator()(Gain a, Gain b)const
    {  
        return a.cellID < b.cellID;  
    }
};


typedef std::map <int, std::set<Gain, gainSortCriterion>, compare> GainMap;  // <gain,cell>

class MaxHeap
{
public:
    MaxHeap(int cellNum, int segNum) : cellNum(cellNum), segNum(segNum) {}

    void Init();

    void Insert(Gain* element);

    Gain* ExtractMax();

    Gain* GetMax();

    void Remove(int vertex_id, int seg);

    void ChangePriority(int vertex_id, int seg, int update_gain);

    int size = 0;

private:
    int Parent(int element) const { return std::floor((element - 1) / 2); }
    int LeftChild(int& element) const { return 2 * element + 1; }
    int RightChild(int& element) const { return 2 * element + 2; }
    void HeapifyUp(int index);
    void HeapifyDown(int index);
    bool CompareElementLargeThan(int index_a, int index_b);

    std::vector<Gain*> nodes_;
    std::vector<std::vector<int>> vertices_map_;  // store the index of vertices in nodes_ [index][k]

    int cellNum;
    int segNum;
};


class fm
{
public:
    fm(int segmentNum, std::string fileDir, std::string partDir, double bal_fac, int data) {
        this->dataStruct = data;
        this->segmentNum = segmentNum;
        this->Segments = new int[segmentNum];
        for (int seg = 0; seg < segmentNum; seg++)
            this->Segments[seg] = 0;

        this->inputFileDir = fileDir;
        this->inputPartFile = partDir;
        this->balance_factor = bal_fac;

        size_t i = fileDir.size() - 1;
        while ((fileDir.at(i) != '/') && (fileDir.at(i) != '\\'))
            --i;
        this->workDir = fileDir.substr(0, i + 1);
        this->inputFileName = fileDir.substr(i + 1, fileDir.size() - i + 1);

        std::string fileName = this->inputFileName;
        while (fileName.at(fileName.size()-1) != '.')
            fileName.pop_back();
        fileName.pop_back();  // "out"

        outputFileName = this->workDir + fileName + "_segNum_" + std::to_string(this->segmentNum) + "_output.log";
        this->outputFile = fopen(this->outputFileName.c_str(), "w");
    }

    ~fm() {
        Cells.clear();
        Nets.clear();
        Gains.clear();
        Order.clear();
        Cells.shrink_to_fit();
        Nets.shrink_to_fit();
        Order.shrink_to_fit();
        fclose(this->outputFile);
        delete[] Segments;
    }

    int Partition();
    void ShowCells();
    int CaculateCutsize();
    void WriteHypergraphInfomation();
    void WriteResult();

    int segmentNum = 2;
    double balance_factor = 1.1;

    int CellNum = 0;
    int NetNum = 0;
    int originalCutSize = 0;
    int cutSize = 0;

    int dataStruct = 0;

    std::string inputFileDir;    // "./work/demo/out.hgr"
    std::string inputPartFile;   // "./input/xxxx.part2"
    std::string workDir;         // "./work/demo/"
    std::string inputFileName;   // "out.hgr"
    std::string outputFileName;  // "./work/demo/out_output.log"
    FILE* outputFile;

private:
    bool GetInputInfo();
    bool GetPartInfo();
    void InitPartition();
    void DoPass();
    bool SatisfyBalance(int targetSeg, int weight);
    void GetBalanceNum();
    void UpdateGain(int cellID, Net* net);
    void ReverseUpdateGain(int cellID, Net* net);
    void GetGainOrder();
    void MakeMove(int cellID, int targetSeg);
    void InsertNewGain(Gain &old_gain, Gain &new_gain, int cell);
    void InitGainBucket();

    int* Segments;  // instore weight sum

    std::vector <Cell*>    Cells;
    std::vector <Net*>     Nets;
    std::vector <Gain>     Order;  // instore the order of the exchange of cells

    GainMap Gains;
    MaxHeap *GainBucket;

    int ith = 1;
    int maxCellNum = 0;

    bool netWeight  = false;
    bool cellWeight = false;

    int maxOrderGain  = 0;
};

#endif