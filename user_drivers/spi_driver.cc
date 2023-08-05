// Copyrights 2023
// Author: HalfsInner
// #pragma once


#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <unistd.h>

using namespace std::string_literals;

#define __FILENAME__ \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define MSG(stream, level, ...)                                \
  fprintf(stream, level ": %s:%d > ", __FILENAME__, __LINE__), \
      fprintf(stream, __VA_ARGS__), fprintf(stream, "\n"), fflush(stream)
#define INFO(...) MSG(stdout, "INFO ", __VA_ARGS__)
#define ERROR(...) MSG(stderr, "ERROR", __VA_ARGS__)
#define FATAL(...) MSG(stderr, "FATAL", __VA_ARGS__)


#define ASSERT_IOCTL(val) \
    do { \
        if (val != 0) {\
            FATAL("Couldn't operatore ioctl: msg='%s'", strerror(val));\
            throw std::runtime_error("IOCTL Issue"); \
        }\
    } while(false);

class Spi {
public:
    Spi(std::string_view device = "/dev/spidev0.0", int32_t speed = 10000) :
        device_(device), speed_(speed) {
       spi_dev_fd_ = open(device.data(), O_RDWR);
       if (spi_dev_fd_ < 0) {
        auto msg = "Cannot open "s + std::string(device);
        throw std::runtime_error(msg);
       }
       INFO("Opened file: %d", spi_dev_fd_);
       int32_t mode = 0x00;
       ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed));
       ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_RD_MAX_SPEED_HZ, &speed));
       ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_WR_MODE, &mode));
       ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_RD_MODE, &mode));
    }

    ~Spi() {
        close(spi_dev_fd_);
    }

    void write(std::vector<uint8_t> const& data) {
        std::vector<uint8_t> read_buf;
        read_buf.reserve(data.size());
        struct spi_ioc_transfer tr = {
            .tx_buf = reinterpret_cast<uint64_t>(data.data()),
            .rx_buf = reinterpret_cast<uint64_t>(read_buf.data()),
            .len = static_cast<uint32_t>(data.size()),
            .speed_hz = static_cast<uint32_t>(speed_),
            .delay_usecs = 10,
            .bits_per_word = 8,
            .cs_change = false,
            .pad = 0,
        };
       ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_MESSAGE(1), &tr));
    }

private:
    std::string device_;
    int32_t speed_;

    int32_t spi_dev_fd_;
};




int main () {
    Spi spi;

    spi.write({0xff, 0xaa, 0x77});
}