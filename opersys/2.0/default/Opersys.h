#ifndef ANDROID_HARDWARE_OPERSYS_V2_0_OPERSYS_H
#define ANDROID_HARDWARE_OPERSYS_V2_0_OPERSYS_H

#include <android/hardware/opersys/2.0/IOpersys.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

namespace android {
namespace hardware {
namespace opersys {
namespace V2_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

struct Opersys : public IOpersys {
    Opersys(opersyshw_device_t* device);
    ~Opersys();
    
    // Methods from IOpersys follow.
    Return<void> read(uint32_t size, read_cb _hidl_cb) override;
    Return<Result> write(const hidl_string& buffer) override;
    Return<int32_t> test(int32_t valueIn) override;
    Return<void> zeroout() override;
    Return<int32_t> istherenewdata() override;
    Return<int64_t> get_last_timestamp() override;
    Return<int32_t> get_read_stat() override;
    Return<int32_t> get_write_stat() override;
    Return<void> set_buffer_to_char(int8_t ch) override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    // No v1.0 in this case -- v1.0 is the old non-HIDL version

  private:
    opersyshw_device_t* mDevice;
};

extern "C" IOpersys* HIDL_FETCH_IOpersys(const char* name);

}  // namespace implementation
}  // namespace V2_0
}  // namespace opersys
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_OPERSYS_V2_0_OPERSYS_H
