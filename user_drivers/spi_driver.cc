// Copyrights 2023
// Author: HalfsInner
// #pragma once

#include <asm-generic/errno-base.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <fcntl.h>
#include <iostream>
#include <linux/spi/spidev.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

using namespace std::string_literals;
using namespace std::chrono_literals;

#define __FILENAME__                                                           \
  (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define MSG(stream, level, ...)                                                \
  fprintf(stream, level ": %s:%d > ", __FILENAME__, __LINE__),                 \
      fprintf(stream, __VA_ARGS__), fprintf(stream, "\n"), fflush(stream)
#define INFO(...) MSG(stdout, "INFO ", __VA_ARGS__)
#define ERROR(...) MSG(stderr, "ERROR", __VA_ARGS__)
#define FATAL(...) MSG(stderr, "FATAL", __VA_ARGS__)

#define ASSERT_IOCTL(val)                                                      \
  do {                                                                         \
    if (val == -1) {                                                           \
      FATAL("Couldn't operatore ioctl: msg='%s'", strerror(val));              \
      INFO("Error code=%d codes: EBADF=%d, EFAULT=%d, EINVAL=%d, ENOTTY=%d",   \
           val, EBADF, EFAULT, EINVAL, ENOTTY);                                \
      throw std::runtime_error("IOCTL Issue");                                 \
    }                                                                          \
  } while (false);

class Spi {
public:
  explicit Spi(std::string_view device = "/dev/spidev0.0", int32_t speed = 10000)
      : device_(device), speed_(speed) {
    spi_dev_fd_ = open(device.data(), O_RDWR | O_NONBLOCK);
    if (spi_dev_fd_ < 0) {
      auto msg = "Cannot open "s + std::string(device);
      FATAL("%s", msg.c_str());
      throw std::runtime_error(msg);
    }
    INFO("Opened file: %d", spi_dev_fd_);
    int32_t mode = 0;
    ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_WR_MAX_SPEED_HZ, &speed));
    ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_RD_MAX_SPEED_HZ, &speed));
    ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_WR_MODE, &mode));
    ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_RD_MODE, &mode));

    initTransfer();
  }

  ~Spi() { close(spi_dev_fd_); }

  void write(std::vector<uint8_t> const &tx) {
    spi_transfer_[0].tx_buf = bufPtrToSpiBuf(tx.data());
    spi_transfer_[0].len = tx.size();
    ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_MESSAGE(1), spi_transfer_));
    cleanTransfer();
  }

  std::vector<uint8_t> transfer(std::vector<uint8_t> const &tx,
                                size_t rx_size) {
    const size_t tx_size = tx.size();
    const size_t total_size = tx_size + rx_size;

    std::vector<uint8_t> rx;
    rx.resize(total_size);
    auto new_tx = tx;
    new_tx.resize(total_size);

    spi_transfer_[0].tx_buf = bufPtrToSpiBuf(new_tx.data());
    spi_transfer_[0].rx_buf = bufPtrToSpiBuf(rx.data());
    spi_transfer_[0].len = total_size;

    ASSERT_IOCTL(ioctl(spi_dev_fd_, SPI_IOC_MESSAGE(1), spi_transfer_));
    rx.erase(std::begin(rx), std::begin(rx) + tx_size);
    cleanTransfer();

    return rx;
  }

  uint64_t bufPtrToSpiBuf(void const *ptr) {
    return reinterpret_cast<uint64_t>(ptr);
  }

private:
  void cleanTransfer() {
    for (size_t i = 0; i < 2; ++i) {
      spi_transfer_[i].tx_buf = 0;
      spi_transfer_[i].rx_buf = 0;
      spi_transfer_[i].len = 0;
      spi_transfer_[i].cs_change = false;
    }
  }
  void initTransfer() {
    for (size_t i = 0; i < 2; ++i) {
      spi_transfer_[i] = spi_ioc_transfer{
          .tx_buf = 0,
          .rx_buf = 0,
          .len = 0,
          .speed_hz = static_cast<uint32_t>(speed_),
          .delay_usecs = 50,
          .bits_per_word = 8,
          .cs_change = false,
          .pad = 0,
      };
    }
  }
  spi_ioc_transfer spi_transfer_[2];

  std::string device_;
  int32_t speed_;

  int32_t spi_dev_fd_;
};

class DriverMcp3008 {
public:
  explicit DriverMcp3008(Spi *spi) : spi_(spi){};

  int32_t read(int channel) {
    if (channel < 0 || channel > 7) {
      throw std::runtime_error("channel has to be in range [0,7]");
    }

    constexpr uint32_t st_bit = 0b10000;
    constexpr uint32_t sgl_bit = 0b01000;
    uint32_t channel_bit = channel & 0b111;
    uint8_t config = (st_bit | sgl_bit | channel) << 1;
    auto data = spi_->transfer({config}, 2);

    // '0111 1111 1111 1111'
    // '_xxx xxxx yyy_ ____
    return (data[0] & 0x7f) << 3 | (data[1] & 0xe0) >> 5;
  }

private:
  Spi *spi_;
};

extern "C" {

void* configureDriver(char *dev, int spi_speed) {
  auto spi = new Spi{dev, spi_speed};
  auto mcp3008 = new DriverMcp3008{spi};
  return mcp3008;
}

int getValue(void *dev_raw, int channel) {
  auto mcp3008 = static_cast<DriverMcp3008 *>(dev_raw);
  return mcp3008->read(channel);
}
} // extern "C"

int main() {
  Spi spi;
  auto mcp3008 = DriverMcp3008{&spi};

  int channel = 7;
  while (true) {
    std::cout << "CH" << channel << ":" << mcp3008.read(channel) << "\n";
    std::this_thread::sleep_for(10ms);
  }
}