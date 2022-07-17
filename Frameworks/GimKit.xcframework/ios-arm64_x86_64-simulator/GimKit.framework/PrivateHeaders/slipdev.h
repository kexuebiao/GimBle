#ifdef __cplusplus
extern "C" {
#endif

#if !defined __STDC_VERSION__ || __STDC_VERSION__ < 199901L
#error "Please use a C99 compliant toolchain."
#endif

#include <stdint.h>
#include <stddef.h>


/**
 * enum to list the possible status of the slip device
 */
typedef enum slip_dev_status_t {
  /** Everything is ok */
  SDS_OK = 0,

  /** Need more data to complete the current frame decoding */
  SDS_MORE_DATA,

  /** Incoming data too large, buffer length exceeded */
  SDS_TOO_LARGE,

  /** Incoming frame data is corrupted */
  SDS_BAD_FRAME,

  /** General failure, null pointer, etc */
  SDS_ERROR
} slip_dev_status_t;

/***
 * An opaque data structure used to save the slip device runtime state
 */
struct slip_dev_t;

/***
 * Callback to received encoded frame.
 *
 * When there is an encoded frame, #sd_encode_frame() will call this function to deliver the result.
 * It will feed the callback with the supplied 'any' in this function. The callback runs in the
 * same thread and same call stack as the #sd_encode_frame().
 *
 * @param sd
 * @param buf Pointer to the start of the encoded frame
 * @param length Frame size in byte
 * @param status Status of the slip device, when it's SDS_OK, the frame is ready to use
 * @param user_ptr The opaque pointer which was fed into #sd_encode_frame().
 */
typedef void (*sd_encode_cb)(const struct slip_dev_t *sd, const uint8_t *buf, size_t length, slip_dev_status_t status, const void *any);

/***
 * Callback to received decoded frame data.
 *
  * When there is a decoded frame, #sd_decode_frame() will call this function to deliver the result.
 * It will feed the callback with the supplied 'any' in this function. The callback runs in the
 * same thread and same call stack as the #sd_decode_frame().
 * @param sd
 * @param buf Pointer to the start of the decoded data
 * @param length Size of the data in byte
 * @param status Status of the slip device, when it's SDS_OK, the data is ready to use
 * @param user_ptr The opaque pointer which was fed into #sd_decode_frame().
 */
typedef void (*sd_decode_cb)(const struct slip_dev_t *sd, const uint8_t *buf, size_t length, slip_dev_status_t status, const void *any);


/***
 * Get the required memory size in byte of running a slip device.
 * <p/>
 * Before initialize a slip device, call this function to learn the required memory size for this slip
 * device, it's related the 'max_frame_data_size' provided by the caller. The caller should make sure
 * the provided 'max_frame_data_size' is big enough to contain the biggest data(before encoding/after
 * decoding)chunk, and small enough to minimize the memory consumption.
 *
 * @param max_frame_data_size Specify the maximal size of a data buffer as discussed above. It must
 * be a positive integer, otherwise the functions fails.
 * @return The required memory size in byte of this slip device. Or 0 to indicate the value of 'max_frame_data_size'
 * is invalid.
 */
size_t sd_get_required_memory_size(size_t max_frame_data_size);

/***
 * Initialize a slip device.
 * <p/>
 * Call this function before any encoding/decoding, but after #sd_get_required_memory_size, to initialize
 * a slip device. Before the call, the caller should allocate a memory buffer whose size is the value
 * returned from #sg_get_required_memory_size, and use the buffer as the 'struct slip_dev_t *sd' argument of
 * this function. Also note that the 'max_frame_data_size' argument must has the same value as 'max_frame_data_size'
 * in #sd_get_required_memory_size().
 *
 * @param sd Opaque pointer to the allocated buffer(used as the pointer to a slip device).
 * @param max_frame_data_size Specify the maximal size of data chunk(before encoding/after decoding),
 *        same value as 'max_frame_data_size' in the accompanying in #sd_get_required_memory_size.
 * @return SDS_OK if the initialization succeeded, otherwise SDS_ERROR(common cause: 'sd' is NULL, or
 *        the 'max_frame_data_size' is too small.
 */
slip_dev_status_t sd_init_device(struct slip_dev_t *sd, size_t max_frame_data_size);

/***
 * De-initialize a slip device.
 * <p/>
 * Call this function when you don't need a slip device any more. After calling this function, you can
 * reclaim the memory buffer used to hold this slip device.
 *
 * @param sd Pointer to the slip device.
 * @return SDS_OK if the de-initialization succeeded, otherwise SDS_ERROR.
 */
slip_dev_status_t sd_deinit_device(struct slip_dev_t *sd);

/***
 * Encode a data chunk.
 * <p/>
 * Call this function to encode a data chunk, the encoded frame will be delivered through the 'cb' callback.
 * Note, this callback is synchronous, it's called during the execution of this function and in the same
 * thread of the caller. This function must be called after a successful #sd_init_device() call.
 *
 * @param sd Pointer to the slip device initialized by a successful #sd_init_device() call.
 * @param buf Pointer to the data chunk to be encoded.
 * @param length Length of the data chunk, in byte.
 * @param cb The callback to receive the encoded frame.
 * @param any An opaque pointer the caller provides, it will be delivered to #sd_encode_cb for
 * the convenience of the caller.
 *
 * @return SDS_OK if the encoding is successful, SDS_TOO_LARGE to signify data chunk too large, SDS_ERROR
 * for general error.
 */
slip_dev_status_t sd_encode_frame(struct slip_dev_t *sd, const uint8_t *buf, size_t length, sd_encode_cb cb, const void *any);

/***
 * Decode a slip frame to a data chunk.
 * <p/>
 * Call this function to decode a slip frame to a data chunk. The decoded data will be delivered through
 * the 'cb' callback. This function supports 'continuous' decoding, which means you can call it many
 * times to decode one frame, or call it one time to decode multiple frames, all depends on the input data.
 *
 * @param sd Pointer to the slip device initialized by a successful #sd_init_device() call.
 * @param buf Pointer to the input buffer, which is a piece of SLIP encoded data.
 * @param length Length of the buffer, in byte.
 * @param cb The callback to receive the decoded data.
 * @note The callback may not be fired during one single call, if the input data can not form a complete
 * SLIP frame, but when the frame is complete, the callback occurs during the execution of this function
 * and in the same thread of the caller. This function must be called after a successful #sd_init_device()
 * call.
  * @param user_ptr An opaque pointer the caller provides, it will be delivered to #sd_encode_cb for
 * the convenience of the caller.
 *
 * @return SDS_OK if the decoding is successful, SDS_MORE_DATA is the input data can't form a complete
 * SLIP frame, SDS_BAD_FRAME if the input data is corrupted(not a valid SLIP frame), SDS_TOO_LARGE
 * to signify the input data is too large, SDS_ERROR for general error.
 */
slip_dev_status_t sd_decode_frame(struct slip_dev_t *sd, const uint8_t *buf, size_t length, sd_decode_cb cb, const void *user_ptr);

size_t sd_encode_frame_to_buffer(const uint8_t *ins, size_t count, uint8_t *outs, size_t length);

#ifdef __cplusplus
}
#endif
