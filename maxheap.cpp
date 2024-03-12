#include "fm.h"

// MaxHeap::MaxHeap(int num, int segNum) 
// {
//     this->nodes_.resize(num * (segNum - 1), new Gain(0,0,0,INT_MIN));
//     this->vertices_map_.resize(num);
//     for (int i = 0; i < num; ++i)
//     {
//         this->vertices_map_[i].resize(segNum, -1);
//     }
// }

void MaxHeap::Init(int cellNum) 
{
    this->nodes_.clear();
    this->vertices_map_.clear();
    this->size = 0; 
    this->nodes_.resize(cellNum);
    this->vertices_map_.resize(cellNum, -1);
}

void MaxHeap::Insert(Gain *element) 
{
    nodes_[this->size] = element;
    vertices_map_[element->cellID] = this->size;
    ++this->size;
    HeapifyUp(this->size - 1);
}

Gain* MaxHeap::ExtractMax()
{
    if (this->size <= 0)
        return (new Gain(-1, -1, -1, INT_MIN));
    auto max_element = nodes_.front();
    // replace the first element with the last element, then
    // call HeapifyDown to update the order of elements
    nodes_[0] = nodes_[this->size - 1];
    vertices_map_[nodes_[this->size - 1]->cellID] = 0;
    --this->size;
    // nodes_.pop_back();
    HeapifyDown(0);
    // Set location of this vertex to -1 in the map
    vertices_map_[max_element->cellID] = -1;
    return max_element;
}

Gain* MaxHeap::GetMax()
{
    if (this->size > 0)
        return nodes_.front();
    else
        return (new Gain(-1, -1, -1, INT_MIN));
}

// Remove the specifid vertex
void MaxHeap::Remove(int vertex_id)
{
    const int index = vertices_map_[vertex_id];
    if (index == -1) {
        return;  // This vertex does not exists
    }

    // // set the gain of this element to maximum + 1
    // nodes_[index]->cellGain = GetMax()->cellGain + 1;
    // // Shift the element to top of the heap
    // HeapifyUp(index);
    // // Extract the element from the heap
    // ExtractMax();

    nodes_[index] = nodes_[this->size - 1];
    vertices_map_[nodes_[this->size - 1]->cellID] = index;
    --this->size;
    // nodes_.pop_back();
    HeapifyDown(index);
    // Set location of this vertex to -1 in the map
    vertices_map_[vertex_id] = -1;
}

// Update the priority (gain) for the specified vertex
void MaxHeap::ChangePriority(int vertex_id, int update_gain)
{
    const int index = vertices_map_[vertex_id];
    if (index == -1) {
        return;  // This vertex does not exists
    }
    nodes_[index]->cellGain += update_gain;
    if (update_gain > 0) {
        HeapifyUp(index);
    } else {
        HeapifyDown(index);
    }
}

// ----------------------------------------------------------------
// Private functions
// ----------------------------------------------------------------

bool MaxHeap::CompareElementLargeThan(int index_a, int index_b)
{
    // return (nodes_[index_a]->cellGain > nodes_[index_b]->cellGain);
    if (nodes_[index_a]->cellGain > nodes_[index_b]->cellGain)
        return true;
    else
        return ((nodes_[index_a]->cellGain == nodes_[index_b]->cellGain) 
                && (index_a < index_b));
}

void MaxHeap::HeapifyUp(int index) 
{
    while (index > 0 && CompareElementLargeThan(index, Parent(index)) == true) {
        // Update the map (exchange parent and child)
        auto& parent_heap_element = nodes_[Parent(index)];
        auto& child_heap_element = nodes_[index];
        vertices_map_[child_heap_element->cellID] = Parent(index);
        vertices_map_[parent_heap_element->cellID] = index;
        // Swap the elements
        std::swap(parent_heap_element, child_heap_element);
        // Next iteration
        index = Parent(index);
    }
}

void MaxHeap::HeapifyDown(int index)
{
    int max_index = index;

    // Basically, we need order index, left child and right child
    // Step 1: check if current index is less than left child
    const int left_child = LeftChild(index);
    if (left_child < this->size
        && CompareElementLargeThan(left_child, max_index) == true) {
        max_index = left_child;
    }

    // Step 2: check if the max index is less than right child
    const int right_child = RightChild(index);
    if (right_child < this->size
        && CompareElementLargeThan(right_child, max_index) == true) {
        max_index = right_child;
    }

    if (index == max_index) {
        return;  // we do not need to further heapifydown
    }

    // swap index and max_index
    auto& cur_heap_element = nodes_[index];
    auto& large_heap_element = nodes_[max_index];
    // Update the map
    vertices_map_[cur_heap_element->cellID] = max_index;
    vertices_map_[large_heap_element->cellID] = index;
    // Swap the elements
    std::swap(cur_heap_element, large_heap_element);
    // Next recursive iterationss
    HeapifyDown(max_index);
}


// int main()
// {
//     MaxHeap* m = new MaxHeap(5,2);

//     m->insert(new Gain(0,0,0,2));
//     m->insert(new Gain(1,0,1,1));
//     m->insert(new Gain(2,0,0,3));
//     m->insert(new Gain(3,0,0,-1));
//     m->insert(new Gain(4,0,0,10));

//     m->ChangePriority(1,1,10);
//     m->ChangePriority(2,0,-5);

//     while (m->size != 0)
//     {
//         auto g = m->ExtractMax();
//         std::cout << g->cellGain << " : " << g->cellID << std::endl;
//     }

//     return 0;
// }