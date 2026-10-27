#pragma once

#include <filesystem>
#include <string>
#include <vector>

class VTKSeriesWriter {
private:
    struct Frame {
        std::string filename;
        double time;
    };

    std::filesystem::path directory_;
    std::vector<Frame> frames_;

public:
    explicit VTKSeriesWriter(
        const std::filesystem::path& directory
    );

    void AddFrame(
        const std::string& filename,
        double time
    );

    void WriteSeries(
        const std::string& filename = "simulation.vtk.series"
    ) const;
};