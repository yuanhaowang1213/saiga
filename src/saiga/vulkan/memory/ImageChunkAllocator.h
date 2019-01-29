//
// Created by Peter Eichinger on 30.10.18.
//

#pragma once
#include "BaseChunkAllocator.h"
#include "MemoryType.h"
namespace Saiga
{
namespace Vulkan
{
namespace Memory
{
class SAIGA_GLOBAL ImageChunkAllocator final : public BaseChunkAllocator
{
   public:
    ImageType type;
    ImageChunkAllocator(const vk::Device& _device, Saiga::Vulkan::Memory::ChunkCreator* chunkAllocator, ImageType _type,
                        Saiga::Vulkan::Memory::FitStrategy& strategy, vk::DeviceSize chunkSize)
        : BaseChunkAllocator(_device, chunkAllocator, strategy, chunkSize), type(std::move(_type))
    {
        LOG(INFO) << "Created new image allocator for flags " << type;
        std::stringstream identifier_stream;
        identifier_stream << "Image Chunk " << type;
        gui_identifier = identifier_stream.str();
    }

    ImageChunkAllocator(const ImageChunkAllocator& other) = delete;
    ImageChunkAllocator(ImageChunkAllocator&& other)      = default;

    ImageChunkAllocator& operator=(const ImageChunkAllocator& other) = delete;
    ImageChunkAllocator& operator=(ImageChunkAllocator&& other) = default;

    ~ImageChunkAllocator() override = default;

    MemoryLocation* allocate(vk::DeviceSize size, const vk::Image& image);

    void enable_defragger(bool enable) override;

   protected:
    ChunkIterator createNewChunk() override;

    void headerInfo() override;

   private:
    using BaseChunkAllocator::allocate;
};

}  // namespace Memory
}  // namespace Vulkan
}  // namespace Saiga
