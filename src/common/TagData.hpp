#ifndef TAG_DATA_HPP
#define TAG_DATA_HPP

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>

// Estructura que se enviará por IPC
struct TagData {
    char epc_id[32]; // EPC del tag (e.g., "E282450123456789")
    int8_t rssi; // Signal strength en dBm (-80 a -40 típico)
    float temperature; // Temperatura en Celsius
    uint32_t timestamp; // Unix timestamp

    // clang-format off
    void print(std::ostream& stream) const {
        stream << "EPC: " << std::left << std::setw(20) << epc_id 
               << " | RSSI: " << std::right << std::setw(4) << static_cast<int>(rssi) << " dBm"
               << " | Temp: " << std::setw(6) << std::fixed << std::setprecision(2) << temperature << "°C"
               << " | Time: " << timestamp << std::endl;
    }
    // clang-format on

    void print() const {
        printf("EPC: %-20s | RSSI: %4d dBm | Temp: %6.2f°C | Time: %u\n", epc_id, rssi, temperature, timestamp);
    }

    std::string to_string() const {
        std::ostringstream oss;

        oss << "EPC: " << std::left << std::setw(20) << epc_id << " | RSSI: " << std::right << std::setw(4)
            << static_cast<int>(rssi) << " dBm"
            << " | Temp: " << std::setw(6) << std::fixed << std::setprecision(2) << temperature << "°C"
            << " | Time: " << timestamp;

        return oss.str();
    }
};

#endif
