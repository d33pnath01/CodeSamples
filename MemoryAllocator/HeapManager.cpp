#include "HeapManager.h"
#include "BlockDescriptor.cpp"
#include <iostream>
#include <assert.h>

HeapManager* HeapManager::CreateHeapManager(void* i_pMemory, size_t i_sizeMemory, unsigned int i_numDescriptors)
{
	// Creating the pool of BlockDescriptors
	BlockDescriptor* blockDescriptor = reinterpret_cast<BlockDescriptor*>(i_pMemory);
	void* userMemoryStart = blockDescriptor + i_numDescriptors;

	while (1)
	{
		// Creating new BlockDescriptor
		blockDescriptor = new (blockDescriptor) BlockDescriptor();
		if ((blockDescriptor + 1) >= userMemoryStart)
		{
			//blockDescriptor->m_nextBlock = nullptr;
			break; // BlockDescriptor storage has reached the end
		}
		blockDescriptor->m_nextBlock = blockDescriptor + 1;

		// Storing the first BlockDescriptor as the head of the list of BlockDescriptors
		if (this->m_poolListHead == nullptr)
		{
			m_poolListHead = blockDescriptor;
			//blockDescriptor->m_prevBlock = nullptr;
		}
		else
		{
			blockDescriptor->m_prevBlock = blockDescriptor - 1;
		}
		
		blockDescriptor = blockDescriptor + 1;
	}

	// Allocating the initial block of memory
	size_t initialHeapSize = i_sizeMemory - (sizeof(BlockDescriptor) * i_numDescriptors);
	blockDescriptor = this->m_poolListHead;
	blockDescriptor->m_blockAddress = userMemoryStart;
	blockDescriptor->m_sizeOfBlock = initialHeapSize;

	{
		//// TEST:
		//printf("PoolListHead Address Stored: %p\n", m_poolListHead->m_blockAddress);
		//printf("PoolListHead Memory Stored: %d\n", m_poolListHead->m_sizeOfBlock);

		//if (m_freeListHead == nullptr)
		//	printf("FreeListHead Empty\n");
	}

	// Moving the BlockDescriptor to the Free List
	this->m_poolListHead = blockDescriptor->m_nextBlock;
	blockDescriptor->m_nextBlock = nullptr;
	this->m_freeListHead = blockDescriptor;

	// Asserting the value
	assert(m_freeListHead);

	{
		//// TEST:
		//printf("FreeListHead Address Stored: %p\n", m_freeListHead->m_blockAddress);
		//printf("FreeListHead Memory Stored: %d\n", m_freeListHead->m_sizeOfBlock);

		//printf("PoolListHead Address Stored: %p\n", m_poolListHead->m_blockAddress);
	}

	// Returning the pointer
	return this;
}

void HeapManager::Destroy()
{
	/*BlockDescriptor* parserPoolList = this->m_poolListHead;
	while (parserPoolList != nullptr)
	{
		this->m_poolListHead = this->m_poolListHead->m_nextBlock;
		delete parserPoolList;
		parserPoolList = this->m_poolListHead;
	}*/
	this->m_poolListHead = nullptr;

	//BlockDescriptor* parserFreeList = this->m_freeListHead;
	/*while (parserFreeList != nullptr)
	{
		void* tempPointer = parserFreeList->m_nextBlock;
		delete parserFreeList;
		parserFreeList = static_cast<BlockDescriptor*>(tempPointer);
	}*/
	this->m_freeListHead = nullptr;

	//BlockDescriptor* parserOutstandingList = this->m_outstandingListHead;
	/*while (parserOutstandingList != nullptr)
	{
		void* tempPointer = parserOutstandingList->m_nextBlock;
		delete parserOutstandingList;
		parserOutstandingList = static_cast<BlockDescriptor*>(tempPointer);
	}*/
	this->m_outstandingListHead = nullptr;

	printf("\nHEAP DESTROYED\n");
	return;
}

void* HeapManager::Alloc(size_t i_size)
{
	// In case no free memory
	if (this->m_freeListHead == nullptr)
	{
		printf("\nNO FREE MEMORY\n");
		return nullptr;
	}

	// Finding a free block
	BlockDescriptor* parserFreeList = this->m_freeListHead;
	while (parserFreeList != nullptr)
	{
		if (parserFreeList->m_sizeOfBlock > i_size)
		{
			// Moving a pointer after the block of memory the user requires
			void* newAddress = reinterpret_cast<uint8_t*>(parserFreeList->m_blockAddress) + i_size;
			size_t newSize = parserFreeList->m_sizeOfBlock - i_size;

			// Finding an empty BlockDescriptor from the pool
			BlockDescriptor* newBlockDescriptor = this->m_poolListHead;
			if (newBlockDescriptor == nullptr) // If the pool of BlockDescriptor is depleted
			{
				// Move the free block to the outstanding list
				i_size = parserFreeList->m_sizeOfBlock;
			}
			else // If there are free BlockDescriptors in the pool
			{
				// Use an empty BlockDescriptor to store the information about the user memory
				this->m_poolListHead = this->m_poolListHead->m_nextBlock;
				newBlockDescriptor->m_blockAddress = parserFreeList->m_blockAddress;
				newBlockDescriptor->m_sizeOfBlock = i_size;

				if (this->m_outstandingListHead == nullptr) // If the outstanding list is empty
				{
					newBlockDescriptor->m_prevBlock = nullptr;
					newBlockDescriptor->m_nextBlock = nullptr;
					this->m_outstandingListHead = newBlockDescriptor;
				}
				else // If there are already blocks in the outstanding list add it to the end of the outstanding list
				{
					BlockDescriptor* parserOutstandingList = this->m_outstandingListHead;
					while (parserOutstandingList->m_nextBlock != nullptr)
					{
						parserOutstandingList = parserOutstandingList->m_nextBlock;
					}
					parserOutstandingList->m_nextBlock = newBlockDescriptor;
					newBlockDescriptor->m_prevBlock = parserOutstandingList;
					newBlockDescriptor->m_nextBlock = nullptr;
				}

				// Change the values of the free list block
				parserFreeList->m_blockAddress = newAddress;
				parserFreeList->m_sizeOfBlock = newSize;
			}

			if (newBlockDescriptor != nullptr)
			{
				return (newBlockDescriptor->m_blockAddress);
			}
		}
		
		if (parserFreeList->m_sizeOfBlock == i_size)
		{
			// Modifying the free list
			if (parserFreeList->m_prevBlock == nullptr) // First block in the free list
			{
				this->m_freeListHead = parserFreeList->m_nextBlock;
			}
			else // Block exists after the first one
			{
				parserFreeList->m_prevBlock->m_nextBlock = parserFreeList->m_nextBlock;
				if (parserFreeList->m_nextBlock != nullptr) // In case last block
				{
					parserFreeList->m_nextBlock->m_prevBlock = parserFreeList->m_prevBlock;
				}
				parserFreeList->m_prevBlock = nullptr;
				parserFreeList->m_nextBlock = nullptr;
			}
			
			if (this->m_outstandingListHead == nullptr) // Outstanding list is empty
			{
				this->m_outstandingListHead = parserFreeList;
			}
			else // If there are already blocks in the outstanding list add it to the end
			{
				BlockDescriptor* parserOutstandingList = this->m_outstandingListHead;
				while (parserOutstandingList->m_nextBlock != nullptr)
				{
					parserOutstandingList = parserOutstandingList->m_nextBlock;
				}
				parserOutstandingList->m_nextBlock = parserFreeList;
				parserFreeList->m_prevBlock = parserOutstandingList;
				parserFreeList->m_nextBlock = nullptr;
			}

			return parserFreeList->m_blockAddress;
		}

		parserFreeList = parserFreeList->m_nextBlock;
	}

	// In case no memory block is big enough
	if (parserFreeList == nullptr)
	{
		printf("\nNO MEMORY BLOCK BIG ENOUGH\n");
		return nullptr;
	}

	return nullptr;
}

void* HeapManager::Alloc(size_t i_size, unsigned int i_alignment)
{
	size_t numAlignedBlocks = i_size / i_alignment;
	size_t requiredAlignedMemory = (numAlignedBlocks + 1) * i_alignment;
	void* returnUserPointer = this->Alloc(requiredAlignedMemory);
	return returnUserPointer;
}

bool HeapManager::Free(void* i_ptr)
{
	// Outstanding list empty
	if (this->m_outstandingListHead == nullptr)
	{
		printf("\nMEMORY NOT ALLOCATED\n");
		return true;
	}

	// Not in the outstanding list
	if (!this->IsAllocated(i_ptr))
	{
		printf("\nNOT IN OUTSTANDING LIST\n");
		return true;
	}

	// Locating blockdescriptor in outstanding list
	BlockDescriptor* parserOutstandingList = this->m_outstandingListHead;
	while (parserOutstandingList != nullptr)
	{
		if (i_ptr == parserOutstandingList->m_blockAddress) // Block found
		{
			break;
		}

		parserOutstandingList = parserOutstandingList->m_nextBlock;
	}

	if (parserOutstandingList == nullptr)
	{
		printf("\nSPECIFY MEMORY FROM START OF BLOCK\n");
		return false;
	}

	// Performing maintenance on outstanding list
	if (parserOutstandingList->m_prevBlock == nullptr) // Block found at the start of list
	{
		this->m_outstandingListHead = this->m_outstandingListHead->m_nextBlock;
		if (this->m_outstandingListHead != nullptr) // Checking in case it was the last block
		{
			this->m_outstandingListHead->m_prevBlock = nullptr;
		}
		parserOutstandingList->m_nextBlock = nullptr;
	}
	else
	{
		parserOutstandingList->m_prevBlock->m_nextBlock = parserOutstandingList->m_nextBlock;
		if (parserOutstandingList->m_nextBlock != nullptr) // Checking in case it is the last block
		{
			parserOutstandingList->m_nextBlock->m_prevBlock = parserOutstandingList->m_prevBlock;
		}
		parserOutstandingList->m_prevBlock = nullptr;
		parserOutstandingList->m_nextBlock = nullptr;
	}

	// Performing maintenance on free list
	
	// No free memory
	if (this->m_freeListHead == nullptr)
	{
		this->m_freeListHead = parserOutstandingList;
		printf("\nMEMORY FREED\n");
		return true;
	}

	// Free blocks exist
	BlockDescriptor* parserFreeList = this->m_freeListHead;
	void* freedAddress = reinterpret_cast<uint8_t*>(parserOutstandingList->m_blockAddress);
	while (parserFreeList->m_nextBlock != nullptr)
	{
		// Freed memory address is smaller than smallest address on list
		if (freedAddress < parserFreeList->m_blockAddress)
		{
			parserOutstandingList->m_prevBlock = nullptr;
			parserOutstandingList->m_nextBlock = parserFreeList;
			parserFreeList->m_prevBlock = parserOutstandingList;
			this->m_freeListHead = parserOutstandingList;

			//printf("\nMEMORY FREED\n");
			printf("\nMEMORY FREED : Inserted %p before %p\n", freedAddress, parserFreeList->m_blockAddress);
			return true;
		}

		// Inserting block in between the list
		void* currentBlockAddress = reinterpret_cast<uint8_t*>(parserFreeList->m_blockAddress);
		void* nextBlockAddress = reinterpret_cast<uint8_t*>(parserFreeList->m_nextBlock->m_blockAddress);

		if (currentBlockAddress < freedAddress && freedAddress < nextBlockAddress)
		{
			parserOutstandingList->m_prevBlock = parserFreeList;
			parserOutstandingList->m_nextBlock = parserFreeList->m_nextBlock;
			parserFreeList->m_nextBlock->m_prevBlock = parserOutstandingList;
			parserFreeList->m_nextBlock = parserOutstandingList;

			//printf("\nMEMORY FREED\n");
			printf("\nMEMORY FREED : Inserted %p in between %p and %p\n", freedAddress, currentBlockAddress, nextBlockAddress);
			return true;
		}

		parserFreeList = parserFreeList->m_nextBlock;
	}

	// Freed memory address is greater than greatest address on list
	if (freedAddress > parserFreeList->m_blockAddress)
	{
		parserFreeList->m_nextBlock = parserOutstandingList;
		parserOutstandingList->m_prevBlock = parserFreeList;
		parserOutstandingList->m_nextBlock = nullptr;

		//printf("\nMEMORY FREED\n");
		printf("\nMEMORY FREED : Inserted %p after %p\n", freedAddress, parserFreeList->m_blockAddress);
		return true;
	}
	else
	{
		parserOutstandingList->m_prevBlock = nullptr;
		parserOutstandingList->m_nextBlock = parserFreeList;
		parserFreeList->m_prevBlock = parserOutstandingList;
		this->m_freeListHead = parserOutstandingList;

		//printf("\nMEMORY FREED\n");
		printf("\nMEMORY FREED : Inserted %p before %p\n", freedAddress, parserFreeList->m_blockAddress);
		return true;
	}

	return false;
}

void HeapManager::CollectFreeBlocks()
{
	size_t totalMemory = 0;
	bool isPoolListEmpty = false;
	bool isFreeListEmpty = false;
	void* userMemoryStart = nullptr;

	// Checking if pool list empty
	if (this->m_poolListHead == nullptr)
	{
		isPoolListEmpty = true;
	}

	// Checking if free list empty
	if (this->m_freeListHead == nullptr)
	{
		isFreeListEmpty = true;
	}

	// Collecting the free blocks
	if (!isFreeListEmpty)
	{
		BlockDescriptor* collectedBlock = this->m_freeListHead;
		userMemoryStart = this->m_freeListHead->m_blockAddress;
		while (this->m_freeListHead != nullptr)
		{
			// Adding to total memory
			totalMemory += collectedBlock->m_sizeOfBlock;

			// Removing from free memory block
			this->m_freeListHead = collectedBlock->m_nextBlock;

			collectedBlock->m_blockAddress = nullptr;
			collectedBlock->m_sizeOfBlock = 0;
			collectedBlock->m_prevBlock = nullptr;
			collectedBlock->m_nextBlock = nullptr;

			// Adding block to the pool
			if (isPoolListEmpty)
			{
				this->m_poolListHead = collectedBlock;
			}
			else
			{
				BlockDescriptor* parserPoolList = this->m_poolListHead;
				while (parserPoolList->m_nextBlock != nullptr)
				{
					parserPoolList = parserPoolList->m_nextBlock;
				}
				parserPoolList->m_nextBlock = collectedBlock;
				collectedBlock->m_prevBlock = parserPoolList;
			}
		}
	}

	// Collecting the allocated blocks
	if (this->m_outstandingListHead != nullptr)
	{
		// Setting the user memory pointer
		if (isFreeListEmpty)
		{
			userMemoryStart = this->m_outstandingListHead;
		}
		else if(userMemoryStart > this->m_outstandingListHead->m_blockAddress)
		{
			userMemoryStart = this->m_outstandingListHead->m_blockAddress;
		}

		// Performing maintenance on outstanding list
		BlockDescriptor* collectedBlock = this->m_outstandingListHead;
		while (this->m_outstandingListHead != nullptr)
		{
			// Adding to total memory
			totalMemory += collectedBlock->m_sizeOfBlock;

			// Removing from outstanding memory block
			this->m_outstandingListHead = collectedBlock->m_nextBlock;

			collectedBlock->m_blockAddress = nullptr;
			collectedBlock->m_sizeOfBlock = 0;
			collectedBlock->m_prevBlock = nullptr;
			collectedBlock->m_nextBlock = nullptr;

			// Adding block to the pool
			if (isPoolListEmpty)
			{
				this->m_poolListHead = collectedBlock;
			}
			else
			{
				BlockDescriptor* parserPoolList = this->m_poolListHead;
				while (parserPoolList->m_nextBlock != nullptr)
				{
					parserPoolList = parserPoolList->m_nextBlock;
				}
				parserPoolList->m_nextBlock = collectedBlock;
				collectedBlock->m_prevBlock = parserPoolList;
			}
		}
	}

	// Setting the head of the free block with the entire memory
	BlockDescriptor* poolBlockHead = this->m_poolListHead;
	this->m_poolListHead = this->m_poolListHead->m_nextBlock;
	this->m_poolListHead->m_prevBlock = nullptr;
	poolBlockHead->m_nextBlock = nullptr;

	this->m_freeListHead = poolBlockHead;
	poolBlockHead->m_blockAddress = userMemoryStart;
	poolBlockHead->m_sizeOfBlock = totalMemory;

	printf("\nMEMORY COLLECTED\n");
	return;
}

void HeapManager::Collect()
{
	// Free list empty
	if (this->m_freeListHead == nullptr)
	{
		printf("\nNO FREE MEMORY\n");
		return;
	}

	// Performing maintenance on free list
	BlockDescriptor* parserFreeList = this->m_freeListHead;
	while (parserFreeList != nullptr)
	{
		//void* currentAddressStart = reinterpret_cast<uint8_t*>(parserFreeList->m_blockAddress);
		void* currentAddressEnd = reinterpret_cast<uint8_t*>(parserFreeList->m_blockAddress) + parserFreeList->m_sizeOfBlock;
		if (parserFreeList->m_nextBlock == nullptr) // Last block in the list
		{
			break;
		}
		//void* nextAddress = reinterpret_cast<uint8_t*>(parserFreeList->m_nextBlock->m_blockAddress);
		void* nextAddress = parserFreeList->m_nextBlock->m_blockAddress;

		while (reinterpret_cast<uint8_t*>(currentAddressEnd) == nextAddress) // Can be collected
		{
			// Maintenance on free list
			BlockDescriptor* collectedBlock = parserFreeList->m_nextBlock;
			parserFreeList->m_nextBlock = parserFreeList->m_nextBlock->m_nextBlock;
			if (parserFreeList->m_nextBlock != nullptr)
			{
				parserFreeList->m_nextBlock->m_prevBlock = parserFreeList;
			}
			parserFreeList->m_sizeOfBlock = parserFreeList->m_sizeOfBlock + collectedBlock->m_sizeOfBlock;

			// Making the collected blockdescriptor an empty blockdescriptor
			collectedBlock->m_prevBlock = nullptr;
			collectedBlock->m_nextBlock = nullptr;
			collectedBlock->m_blockAddress = nullptr;
			collectedBlock->m_sizeOfBlock = 0;

			// Maintenance on pool list
			if (this->m_poolListHead == nullptr) // Pool empty
			{
				m_poolListHead = collectedBlock;
			}
			else
			{
				BlockDescriptor* parserPoolList = this->m_poolListHead;
				while (parserPoolList->m_nextBlock != nullptr)
				{
					parserPoolList = parserPoolList->m_nextBlock;
				}
				parserPoolList->m_nextBlock = collectedBlock;
				collectedBlock->m_prevBlock = parserPoolList;
			}

			currentAddressEnd = reinterpret_cast<uint8_t*>(parserFreeList->m_blockAddress) + parserFreeList->m_sizeOfBlock;
			if (parserFreeList->m_nextBlock == nullptr)
			{
				break;
			}
			nextAddress = reinterpret_cast<uint8_t*>(parserFreeList->m_nextBlock->m_blockAddress);
		}

		parserFreeList = parserFreeList->m_nextBlock;
	}

	printf("\nMEMORY COLLECTED\n");
	return;
}

bool HeapManager::Contains(void* i_ptr) const
{
	bool isMemoryFound = false;

	// Free list empty
	bool isFreeEmpty = false;
	if (this->m_freeListHead == nullptr)
	{
		isFreeEmpty = true;
	}

	// Allocated list empty
	bool isAllocatedEmpty = false;
	if (this->m_outstandingListHead == nullptr)
	{
		isAllocatedEmpty = true;
	}

	// Traverse free list
	if (!isFreeEmpty)
	{
		BlockDescriptor* parserFreeList = this->m_freeListHead;
		while (parserFreeList != nullptr)
		{
			if (i_ptr < parserFreeList->m_blockAddress) // Pointer is smaller than smallest memory address on list
			{
				printf("\nNOT IN FREE LIST\n");
				break;
			}
			else
			{
				void* freeBlockStart = reinterpret_cast<uint8_t*>(parserFreeList->m_blockAddress);
				void* freeBlockEnd = reinterpret_cast<uint8_t*>(parserFreeList->m_blockAddress) + (parserFreeList->m_sizeOfBlock - 1);
				if ((freeBlockStart < i_ptr && i_ptr < freeBlockEnd) || freeBlockStart == i_ptr || freeBlockEnd==i_ptr)
				{
					// Pointer lies somewhere in between
					printf("\nFOUND IN FREE LIST\n");
					isMemoryFound = true;
					break;
				}
			}

			parserFreeList = parserFreeList->m_nextBlock;
		}
	}

	// Traverse outstanding list
	if (!isMemoryFound && !isAllocatedEmpty)
	{
		isMemoryFound = this->IsAllocated(i_ptr);
	}

	return isMemoryFound;
}

bool HeapManager::IsAllocated(void* i_ptr) const
{
	bool isMemoryFound = false;

	// Outstanding list empty
	if (this->m_outstandingListHead == nullptr)
	{
		printf("\nNOT IN OUTSTANDING LIST\n");
		return false;
	}

	// Traverse outstanding list
	BlockDescriptor* parserOutstandingList = this->m_outstandingListHead;
	while (parserOutstandingList != nullptr)
	{
		//if (i_ptr < parserOutstandingList->m_blockAddress) // Pointer is smaller than smallest memory address on list
		//{
		//	printf("\nBLAH :: NOT IN OUTSTANDING LIST\n");
		//	printf("\ni_ptr : %p || parserOutstandingList->m_blockAddress : %p\n", i_ptr, parserOutstandingList->m_blockAddress);
		//	isMemoryFound = false;
		//	break;
		//}
		//else
		//{
			void* freeBlockStart = reinterpret_cast<uint8_t*>(parserOutstandingList->m_blockAddress);
			void* freeBlockEnd = reinterpret_cast<uint8_t*>(parserOutstandingList->m_blockAddress) + (parserOutstandingList->m_sizeOfBlock - 1);
			if ((freeBlockStart < i_ptr && i_ptr < freeBlockEnd) || freeBlockStart == i_ptr || freeBlockEnd == i_ptr)
			{
				// Pointer lies somewhere in between
				printf("\nFOUND IN OUTSTANDING LIST\n");
				isMemoryFound = true;
				break;
			}
		//}

		parserOutstandingList = parserOutstandingList->m_nextBlock;
	}

	return isMemoryFound;
}

size_t HeapManager::GetLargestFreeBlock() const
{
	if (this->m_freeListHead == nullptr)
	{
		printf("\nNO FREE MEMORY\n");
		return 0;
	}

	BlockDescriptor* parser = this->m_freeListHead;
	BlockDescriptor* largestFreeBlock = this->m_freeListHead;
	while (parser->m_nextBlock != nullptr)
	{
		parser = parser->m_nextBlock;
		if (parser->m_sizeOfBlock > largestFreeBlock->m_sizeOfBlock)
		{
			largestFreeBlock = parser;
		}
	}
	return largestFreeBlock->m_sizeOfBlock;
}

size_t HeapManager::GetTotalFreeMemory() const
{
	if (this->m_freeListHead == nullptr)
	{
		printf("\nNO FREE MEMORY\n");
		return 0;
	}

	BlockDescriptor* parser = this->m_freeListHead;
	size_t totalFreeMemory = 0;
	while (parser != nullptr)
	{
		totalFreeMemory += parser->m_sizeOfBlock;
		parser = parser->m_nextBlock;
	}
	return totalFreeMemory;
}

void HeapManager::ShowFreeBlocks() const
{
	//int countOfFreeBlocks = 0;

	printf("\n=====================START SHOWING FREE BLOCKS=====================\n");

	if (this->m_freeListHead == nullptr)
	{
		printf("\nNO FREE MEMORY\n");
		return;
	}

	BlockDescriptor* parserFreeList = this->m_freeListHead;
	while (parserFreeList != nullptr)
	{
		printf("Free Memory Address: %p\n", parserFreeList->m_blockAddress);
		printf("Free Memory Size: %d\n", parserFreeList->m_sizeOfBlock);
		printf("-----------------\n");

		parserFreeList = parserFreeList->m_nextBlock;
		//countOfFreeBlocks++;
	}

	printf("\n=====================END SHOWING FREE BLOCKS=====================\n");
	//printf("\n=====================END SHOWING FREE BLOCKS : %d=====================\n", countOfFreeBlocks);

	//return countOfFreeBlocks;
	return;
}

void HeapManager::ShowOutstandingAllocations() const
{
	//int countOfOutstandingBlocks = 0;

	printf("\n=====================START SHOWING OUTSTANDING BLOCKS=====================\n");

	if (this->m_outstandingListHead == nullptr)
	{
		printf("\nNO OUTSTANDING MEMORY\n");
		return;
	}

	BlockDescriptor* parserOutstandingList = this->m_outstandingListHead;
	while (parserOutstandingList != nullptr)
	{
		printf("Outstanding Memory Address: %p\n", parserOutstandingList->m_blockAddress);
		printf("Outstanding Memory Size: %d\n", parserOutstandingList->m_sizeOfBlock);
		printf("-----------------\n");

		parserOutstandingList = parserOutstandingList->m_nextBlock;
		//countOfOutstandingBlocks++;
	}

	printf("\n=====================END SHOWING OUTSTANDING BLOCKS=====================\n");
	//printf("\n=====================END SHOWING OUTSTANDING BLOCKS : %d=====================\n", countOfOutstandingBlocks);

	//return countOfOutstandingBlocks;
	return;
}

