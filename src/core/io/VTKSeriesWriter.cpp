
#include "VTKSeriesWriter.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <stdexcept>

VTKSeriesWriter::VTKSeriesWriter(
    const std::filesystem::path& directory
) : directory_(directory) {
    std::filesystem::create_directories(directory_);
}

void VTKSeriesWriter::AddFrame(
    const std::string& filename,
    double time
) {
    if (!std::isfinite(time)) {
        throw std::invalid_argument(
            "VTKSeriesWriter: invalid time"
        );
    }

    frames_.push_back({filename, time});
}

void VTKSeriesWriter::WriteSeries(
    const std::string& filename
) const {

    std::ofstream out(directory_ / filename);

    if (!out) {
        throw std::runtime_error(
            "VTKSeriesWriter: cannot open file"
        );
    }

    out << std::setprecision(17);

    out << "{\n";
    out << "  \"file-series-version\": \"1.0\",\n";
    out << "  \"files\": [\n";

    for (std::size_t i = 0; i < frames_.size(); ++i) {
        const auto& frame = frames_[i];

        out << "    {\"name\": \"" << frame.filename
            << "\", \"time\": " << frame.time << "}";

        if (i + 1 < frames_.size()) {
            out << ",";
        }

        out << "\n";
    }

    out << "  ]\n";
    out << "}\n";

    if (!out) {
        throw std::runtime_error(
            "VTKSeriesWriter: failed to write series"
        );
    }
}
