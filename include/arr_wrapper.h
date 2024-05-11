//
// Created by kaba4ok on 10.05.24.
//

#ifndef MPI_ARR_WRAPPER_H
#define MPI_ARR_WRAPPER_H
#include <cstdint>
#include <cstring>

class PlateWrapper
{
private:
    double_t * plate;
    std::size_t plateSize;
    std::size_t width;
    std::size_t height;
public:
    double_t * GetElemPointer(size_t idx)
    {
        if(idx >= plateSize)
        {
            throw std::invalid_argument("Invalid insertion index");
        }

        return &plate[idx];
    }

    size_t GetWidth() const
    {
        return width;
    }

    size_t GetHeight() const
    {
        return height;
    }

    void Insert(double_t value, size_t x, size_t y)
    {
        if(x >= width || y >= height)
        {
            throw std::invalid_argument("Invalid insertion index");
        }

        plate[y*width + x] = value;
    }

    void Insert(double_t value, size_t idx)
    {
        if(idx >= plateSize)
        {
            throw std::invalid_argument("Invalid insertion index");
        }

        plate[idx] = value;
    }

    double_t Get(size_t x, size_t y) const
    {
        if(x >= width || y >= height)
        {
            throw std::invalid_argument("Invalid insertion index");
        }

        return plate[y*width + x];
    }

    double_t Get(size_t idx) const
    {
        if(idx >= plateSize)
        {
            throw std::invalid_argument("Invalid insertion index");
        }

        return plate[idx];
    }

    PlateWrapper(std::size_t plateWidth, std::size_t plateHeight):width(plateWidth),height(plateHeight),plateSize(plateHeight*plateWidth)
    {
        plate = new double_t [plateSize];
    }

    PlateWrapper(const PlateWrapper& other)
    {
        if(other.plate != nullptr)
        {
            plateSize = other.plateSize;
            width = other.width;
            height = other.height;

            plate = new double_t [other.plateSize];
            std::memcpy(plate, other.plate,other.plateSize);
        }
    }
    PlateWrapper& operator=(const PlateWrapper& other)
    {
        if(&other == this)
        {
            return *this;
        }

        if(other.plate != nullptr)
        {
            plateSize = other.plateSize;
            width = other.width;
            height = other.height;

            delete[] plate;
            plate = new double_t [other.plateSize];
            std::memcpy(plate, other.plate,other.plateSize);
        }
        return *this;
    }

    PlateWrapper(PlateWrapper&& other) noexcept
    {
        if(other.plate != nullptr)
        {
            plateSize = other.plateSize;
            width = other.width;
            height = other.height;

            plate = other.plate;
            other.plate = nullptr;
        }
    }

    PlateWrapper& operator=(PlateWrapper&& other) noexcept
    {
        if(other.plate != nullptr)
        {
            plateSize = other.plateSize;
            width = other.width;
            height = other.height;

            delete[] plate;
            plate = other.plate;
            other.plate = nullptr;
        }

        return *this;
    }

    ~PlateWrapper()
    {
        delete[] plate;
    }
};

#endif //MPI_ARR_WRAPPER_H
