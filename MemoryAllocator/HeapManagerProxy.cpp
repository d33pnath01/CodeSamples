#include "HeapManagerProxy.h"
#include "HeapManager.h"

HeapManager* HeapManagerProxy::CreateHeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors)
{
	HeapManager* heapManager = new HeapManager();
	heapManager->CreateHeapManager(i_pMemory, i_sizeMemory, i_numDescriptors);
	return heapManager;
}

void HeapManagerProxy::Destroy(HeapManager* i_pManager)
{
	i_pManager->Destroy();
	return;
}

void* HeapManagerProxy::Alloc(HeapManager* i_pManager, size_t i_size)
{
	void* returnPointer = i_pManager->Alloc(i_size);
	return returnPointer;
}

void* HeapManagerProxy::Alloc(HeapManager* i_pManager, size_t i_size, unsigned int i_alignment)
{
	void* returnPointer = i_pManager->Alloc(i_size, i_alignment);
	return returnPointer;
}

bool HeapManagerProxy::Free(HeapManager* i_pManager, void* i_ptr)
{
	i_pManager->Free(i_ptr);
	return true;
}

void HeapManagerProxy::Collect(HeapManager* i_pManager)
{
	i_pManager->Collect();
	return;
}

void HeapManagerProxy::CollectFreeBlocks(HeapManager* i_pManager)
{
	i_pManager->CollectFreeBlocks();
	return;
}

bool HeapManagerProxy::Contains(const HeapManager* i_pManager, void* i_ptr)
{
	bool doesContain = i_pManager->Contains(i_ptr);
	return doesContain;
}

bool HeapManagerProxy::IsAllocated(const HeapManager* i_pManager, void* i_ptr)
{
	bool isAllocated = i_pManager->IsAllocated(i_ptr);
	return isAllocated;
}

size_t HeapManagerProxy::GetLargestFreeBlock(const HeapManager* i_pManager)
{
	size_t returnSize = i_pManager->GetLargestFreeBlock();
	return returnSize;
}

size_t HeapManagerProxy::GetTotalFreeMemory(const HeapManager* i_pManager)
{
	size_t returnSize = i_pManager->GetTotalFreeMemory();
	return returnSize;
}

void HeapManagerProxy::ShowFreeBlocks(const HeapManager* i_pManager)
{
	i_pManager->ShowFreeBlocks();
	return;
}

void HeapManagerProxy::ShowOutstandingAllocations(const HeapManager* i_pManager)
{
	i_pManager->ShowOutstandingAllocations();
	return;
}
