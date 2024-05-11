// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <filesystem>
#include "options_parser.h"
#include "config_parser.h"
#include "arr_wrapper.h"
#include <boost/mpi.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <array>
#include "tbb/concurrent_queue.h"

bool CheckIfCellInRectangle(size_t x, size_t y, const std::vector<std::array<size_t, 4>>& fixedRects)
{
    for(const auto& rect : fixedRects)
    {
        if((x >= rect[0])&&(x <= rect[2])&&(y >= rect[1])&&(y <= rect[3]))
        {
            return true;
        }
    }
    return false;
}

PlateWrapper ProgressSimulation(const PlateWrapper& oldTemperatures, size_t startRow, size_t endRow, size_t dt, double_t dx_sq, double_t dy_sq, size_t alpha, const std::vector<std::array<size_t, 4>>& fixedRects)
{
    PlateWrapper newTemperatures{oldTemperatures.GetWidth(), oldTemperatures.GetHeight()};
    auto prod = static_cast<double_t >(dt*alpha);
    for(size_t x = startRow*oldTemperatures.GetWidth(); x < oldTemperatures.GetWidth()*(endRow + 1); x++)
    {
        for(size_t y = 0; y < oldTemperatures.GetHeight(); y++)
        {
            if(CheckIfCellInRectangle(x, y, fixedRects)){continue;}

            auto oldTemp = oldTemperatures.Get(x,y);
            double_t tempTop = y-1>=0 ? oldTemperatures.Get(x, y-1) : 0;
            double_t tempBottom = y+1<oldTemperatures.GetHeight() ? oldTemperatures.Get(x, y+1) : 0;
            double_t  tempLeft = x-1>=0 ? oldTemperatures.Get(x-1, y) : 0;
            double_t tempRight = x+1<oldTemperatures.GetWidth() ? oldTemperatures.Get(x+1, y) : 0;
            double_t newTemp = oldTemp + prod*((tempLeft - 2*oldTemp + tempRight)/dx_sq + (tempTop - 2*oldTemp + tempBottom)/dy_sq);

            newTemperatures.Insert(newTemp, x, y);
        }
    }

    return std::move(newTemperatures);
}

void SnapshotWriter(tbb::concurrent_queue<PlateWrapper>& snapshotQueue, std::filesystem::path& snapshotDirectory, size_t plateWidth,  size_t plateHeight)
{
    size_t snapshotIdx = 0;
    while (true)
    {
        PlateWrapper newSnapshot{0,0};
        if(snapshotQueue.try_pop(newSnapshot))
        {
            if(newSnapshot.Get(0) == 420)
            {
                break;
            }

            std::stringstream filename;
            filename << snapshotDirectory << "/snapshot_" << snapshotIdx << ".csv";

            // Open the file in write mode
            std::ofstream file(filename.str());

            // Check if file is open
            if (!file.is_open()) {
                continue; // Error handling: couldn't open file
            }

            auto snapshot = newSnapshot.GetElemPointer(0);
            // Write table data
            for (size_t i = 0; i < plateHeight; ++i) {
                for (size_t j = 0; j < plateWidth; ++j) {
                    // Write element with comma separator
                    file << snapshot[i * plateWidth + j];
                    if (j < plateWidth - 1) {
                        file << ",";
                    }
                }
                // Add newline after each row (except the last one)
                if (i < plateHeight - 1) {
                    file << "\n";
                }
            }

            file.close();
#ifndef NDEBUG
            std::cout << "Wrote snapshot " << snapshotIdx << " to csv!" << std::endl;
#endif
            snapshotIdx++;
        } else
        {
            std::this_thread::yield();
        }
    }
}

PlateWrapper ReadNodeChunk(const std::string& filepath, size_t start_index, size_t end_index, size_t width, size_t height)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file: " + filepath);
    }

    if(width * height != end_index - start_index + 1)
    {
        throw  std::invalid_argument("Wrong plate dimensions provided!");
    }

    std::string line;

    PlateWrapper data{width, height};
    size_t index = 0;
    while (std::getline(file, line))
    {
        std::istringstream stream(line);
        std::string token;
        while(std::getline(stream, token, ','))
        {
            if(index >= start_index)
            {
                data.Insert( std::stoi(token), index-start_index);
            }
            index++;

            if(index == end_index+1){ break;}
        }

        if(index == end_index + 1){ break;}
    }

    return std::move(data);
}


std::array<size_t,4> GetFixedRect(const std::string& rectAsStr)
{
    std::array<size_t, 4> newRect{};

    std::stringstream ss(rectAsStr);
    std::string token;

    size_t pos{0};
    while (std::getline(ss, token, '/'))
    {
        newRect[pos] = std::stoi(token);
        pos++;

        if(pos == 4){ break;}
    }

    return newRect;
}

std::vector<std::array<size_t, 4>> GetFixedRects(const std::vector<std::string>& rectsAsStrings)
{
    std::vector<std::array<size_t, 4>> newRects;
    for(const auto& rect : rectsAsStrings)
    {
        newRects.emplace_back(GetFixedRect(rect));
    }
    return newRects;
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        //TODO: Handle error
        return -1;
    }

    boost::mpi::environment mpiEnv(argc, argv, boost::mpi::threading::funneled, false);
    boost::mpi::communicator worldComm;

    std::filesystem::path configPath{argv[1]};
    ConfigurationParser parser{configPath};

    std::unordered_map<std::string, std::vector<std::string>> config;
    try
    {
        config = std::move(parser.ParseConfiguration());

#ifndef NDEBUG
        std::cout << "Read configuration" << std::endl;
#endif
    } catch (...)
    {
        //TODO: Handle error
        return -1;
    }

    size_t dt = std::stoi(config["dt"][0]);
    size_t dx = std::stoi(config["dx"][0]);
    size_t dy = std::stoi(config["dy"][0]);

    size_t conductivity = std::stoi(config["conductivity"][0]);
    size_t density = std::stoi(config["density"][0]);
    size_t capacity = std::stoi(config["capacity"][0]);

    size_t alpha = conductivity / (capacity * density);
    if(dt > (dx > dy? dx : dy)/(4*alpha))
    {
        std::cerr << "Warning: the parameters provided suggest that explicit scheme is unstable" << std::endl;
    }

    size_t plateWidth =  std::stoi(config["width"][0]);
    size_t plateHeight = std::stoi(config["height"][0]);
    size_t savePeriod = std::stoi(config["save"][0]);
    size_t nCells =  plateWidth * plateHeight;
    if(nCells % worldComm.size() != 0)
    {
        //TODO: Handle error
        return -1;
    }

    //Get areas of fixed temperature here
    std::vector<std::array<size_t, 4>> fixedRects = GetFixedRects(config["constrect"]);

    size_t nIterations = std::stoi(config["iterations"][0]);

    size_t nCellsPerNode = nCells / worldComm.size();

    size_t topExtraRow = 1;
    size_t bottomExtraRow = 1;

    if(worldComm.rank() == 0)
    {
        topExtraRow = 0;
    }
    if(worldComm.rank() == worldComm.size()-1)
    {
        bottomExtraRow = 0;
    }

    size_t myCellsStart = worldComm.rank() * nCellsPerNode;
    size_t myCellsEnd = myCellsStart + nCellsPerNode;

    myCellsStart -= topExtraRow * plateWidth;
    myCellsEnd += bottomExtraRow * plateWidth;

    PlateWrapper myCells = ReadNodeChunk(config["platepath"][0], myCellsStart, myCellsEnd, plateWidth, plateHeight + topExtraRow + bottomExtraRow);

    myCellsEnd -= myCellsStart;
    myCellsStart -= myCellsStart;

    int topNeighbour = worldComm.rank() - 1;
    int bottomNeighbour = worldComm.rank() + 1;

    size_t firstUpdateRow = 1;
    size_t lastUpdateRow = plateHeight/worldComm.size();

    if(worldComm.rank() == 0)
    {
        firstUpdateRow = 0;
    }

    tbb::concurrent_queue<PlateWrapper> snapshotQueue{};
    std::thread snapshotSaver;
    if(worldComm.rank() == 0)
    {
        std::filesystem::path saveDir{config["savepath"][0]};
        snapshotSaver = std::thread{SnapshotWriter, std::ref(snapshotQueue), std::ref(saveDir), plateWidth, plateHeight};
    }

    auto tempUpdateFunction = [&myCells, &fixedRects, dx, dy, dt, alpha, firstUpdateRow, lastUpdateRow](){myCells = std::move(ProgressSimulation(myCells,firstUpdateRow, lastUpdateRow, dt, std::pow(dx,2), std::pow(dy, 2), alpha,fixedRects ));};

    for(size_t itr = 0; itr < nIterations; ++itr)
    {
        //TODO: calculate temperature
        tempUpdateFunction();

        //broadcasting results
        if(worldComm.rank() != worldComm.size() - 1)
        {
            worldComm.isend(bottomNeighbour,itr, myCells.GetElemPointer(myCellsEnd - 2*plateWidth), plateWidth);
#ifndef NDEBUG
            std::cout << "Node " << worldComm.rank() << " sent its bottom row!" << std::endl;
#endif
        }

        if(worldComm.rank() != 0)
        {
            worldComm.isend(topNeighbour, itr, myCells.GetElemPointer(myCellsStart + plateWidth), plateWidth);
#ifndef NDEBUG
            std::cout << "Node " << worldComm.rank() << " sent its top row!" << std::endl;
#endif
        }

        //lock on barrier
        worldComm.barrier();
        //wait for my results & update table

        if(worldComm.rank() != 0)
        {
            worldComm.irecv(topNeighbour, itr, myCells.GetElemPointer(myCellsStart), plateWidth);

#ifndef NDEBUG
            std::cout << "Node " << worldComm.rank() << " received its top row!" << std::endl;
#endif
        }

        if(worldComm.rank() != worldComm.size() - 1)
        {
            worldComm.irecv(bottomNeighbour, itr, myCells.GetElemPointer(myCellsEnd - plateWidth), plateWidth);

#ifndef NDEBUG
            std::cout << "Node " << worldComm.rank() << " received its bottom row!" << std::endl;
#endif
        }

        worldComm.barrier();

        if((itr+1) % savePeriod == 0)
        {
            PlateWrapper snapshot{0, 0};

            size_t startIdx = 1;
            if(worldComm.rank() == 0)
            {
                startIdx = 0;
                snapshot = std::move(PlateWrapper{plateWidth, plateHeight});
            }
            gather(worldComm, myCells.GetElemPointer(myCellsStart+plateWidth*startIdx), nCellsPerNode, snapshot.GetElemPointer(0), 0);

#ifndef NDEBUG
            std::cout << "Got a new snapshot!" << std::endl;
#endif

            snapshotQueue.push(std::move(snapshot));
        }
    }

    if(worldComm.rank() == 0)
    {
        PlateWrapper pill{1,1};
        pill.Insert(0, 420);

        snapshotQueue.push(pill);
        snapshotSaver.join();
    }

    return 0;
}
