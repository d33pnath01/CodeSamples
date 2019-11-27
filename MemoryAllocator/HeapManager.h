#pragma once

class BlockDescriptor;

class HeapManager
{
	public:
		HeapManager*	CreateHeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors);
		void			Destroy();

		void*			Alloc(size_t i_size);
		void*			Alloc(size_t i_size, unsigned int i_alignment);
		bool			Free(void* i_ptr);

		void			Collect();
		void			CollectFreeBlocks();

		bool			Contains(void* i_ptr) const;
		bool			IsAllocated(void* i_ptr) const;

		size_t			GetLargestFreeBlock() const;
		size_t			GetTotalFreeMemory() const;

		void			ShowFreeBlocks() const;
		void			ShowOutstandingAllocations() const;
	private:
		BlockDescriptor* m_freeListHead = nullptr;
		BlockDescriptor* m_outstandingListHead = nullptr;
		BlockDescriptor* m_poolListHead = nullptr;
};