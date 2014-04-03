#ifndef PROTOBUF_C_TEXT_H
#define PROTOBUF_C_TEXT_H

/*
 * \file
 * Library header file.
 * This is the header file for the text format protobuf routines.
 * It contains declarations of all functions and data types exported by
 * the library.
 *
 * \author Kevin Lyda <kevin@ie.suberic.net>
 * \date   March 2014
 *
 */

/** \mainpage 
 *
 * \section description Description
 *
 * Google protobufs are an efficient way to serialise and deserialise
 * data to send across the wire or to store on disk.  The Google compiler
 * and related libraries provide implementations for C++, Java and Python.
 * A very simple message definition language is used to generate parsers
 * and generators for this binary format.
 *
 * The Protobuf compiler and library for C provide access to this
 * technology from within C code.
 *
 * Besides the efficient binary protobuf format there is also a text mode
 * format which is accessible to code generated by the Google protobuf
 * compiler.  While the text format has limitations - namely that it lacks
 * some of the back/forwards-compatibility features of the binary format -
 * it can be a useful debugging tool and a strict but human readable config
 * file format.
 *
 * The C protobuf implementation is very minimal and lacks this feature.
 * This library supplements \c libprotobuf-c and provides functions to
 * generate and consume text format protobuf.  They will work fine with
 * any \c ProtobufCMessage subtype generated by the \c protoc-c
 * compiler.
 *
 * \sa
 *     - Google Protobufs: https://code.google.com/p/protobuf/
 *     - Protobuf docs:
 *       https://developers.google.com/protocol-buffers/docs/overview
 *     - Notes on protobuf compatibility:
 *       https://developers.google.com/protocol-buffers/docs/proto#updating
 *     - Protobuf for C code: https://github.com/protobuf-c/protobuf-c
 *     - Protobuf for C docs:
 *       https://github.com/protobuf-c/protobuf-c/wiki
 *     - Protobuf for C RPC library code:
 *       https://github.com/protobuf-c/protobuf-c-rpc
 *     - Protobuf for C text format code:
 *       https://github.com/protobuf-c/protobuf-c-text
 *     - Protobuf for C text format
 *       <a href=coverage/index.html>test coverage</a>
 *
 * \section example Examples
 *
 * Both examples use this \c .proto definition:
 * \verbatim
message Ponycopter {
  optional string hair_colour = 1;
  optional uint32 rotors = 2;
  optional uint32 awesomeness = 3;
}
\endverbatim
 *
 * \b Generating
 *
 * \verbatim
#include <google/protobuf-c/protobuf-c.h>
#include "ponycopter.pb-c.h"

int main(int argc, char *argv[]) {
  int len;
  char *pc_bin, *pc_text;
  Ponycopter *pc;

  pc_bin = read_a_blob(argv[1], &len);
  pc = ponycopter__unpack(NULL, len, pc_bin);
  pc_text = protobuf_c_text_to_string((ProtobufCMessage *)pc, NULL);
  printf("Ponycopter: %s\n", pc_text);
}
\endverbatim
 *
 * \b Parsing
 *
 * \verbatim
#include <google/protobuf-c/protobuf-c.h>
#include "ponycopter.pb-c.h"

int main(int argc, char *argv[]) {
  ProtobufCTextError tf_res;
  Ponycopter *pc;

  pc = (Ponycopter *)protobuf_c_text_from_file(
         &ponycopter__descriptor, argv[1], &tf_res, NULL);
}
\endverbatim
 */

#include <google/protobuf-c/protobuf-c.h>

/** \defgroup api Public API for text format protobufs
 *
 * These functions supplement the generated code from \c protoc-c to
 * allow you to import and export \c ProtobufCMessage structures
 * from/to the protobuf text mode serialisation.
 *
 * These will work with any code generated from \c protoc-c.
 * @{
 */

/** Structure for reporting API errors.
 *
 * Provides feedback on the success of an API call.  Generally if an
 * API call fails it will return \c NULL.  More detail on why it failed
 * can be found in the parameter with this type.
 */
typedef struct _ProtobufCTextError {
  int *error;       /**< Error code. 0 for success, >0 for failure. */
  char *error_txt;  /**< String with error message. */
  int complete;     /**< Reports whether the message is complete
                         (if supported):
                         - -1: Required field check wasn't performed - this
                               happens if your libprotobuf-c is too old.
                         - 0: The message was incomplete.
                         - >0: Message has all required fields set. */
} ProtobufCTextError;

/** Convert a \c ProtobufCMessage to a string.
 *
 * Given a \c ProtobufCMessage serialise it as a text format protobuf.
 *
 * \param[in] m The \c ProtobufCMessage to be serialised.
 * \param[in] allocator This is the same \c ProtobufCAllocator type used
 *                      by the \c libprotobuf-c library.  You can set it
 *                      to \c NULL to accept \c protobuf_c_default_allocator -
 *                      the default allocator.
 * \return The string with the text format serialised \c ProtobufCMessage.
 *         On failure it will return \c NULL.  On success, the resulting value
 *         be freed by you with the \c allocator you provided.  If you didn't
 *         provide an allocator technically you should do:
 *         \code
 * protobuf_c_default_allocator.free(
 *     protobuf_c_default_allocator.allocator_data, retval);
 *         \endcode
 *         Though technically \c free(retval); is probably sufficient.
 */
extern char *protobuf_c_text_to_string(ProtobufCMessage *m,
    ProtobufCAllocator *allocator);

/** Import a text format protobuf from a string into a \c ProtobufCMessage.
 *
 * Given a string containing a text format protobuf, parse it and return
 * the corresponding \c ProtobufCMessage struct. On failure, \c NULL is
 * returned and \c result is updated with why.
 *
 * The resulting \c ProtobufCMessage should be freed with
 * \c protobuf_c_message_free_unpacked() or the generated
 * \c ..._free_upacked() function. Either is fine, but that's how the
 * memory should be freed.
 *
 * \param[in] descriptor The descriptor from the generated code.
 * \param[in] msg The string containing the text format protobuf.
 * \param[out] result This structure contains information on any error
 *                    that halted processing.
 * \param[in] allocator The \c ProtobufCAllocator struct.
 * \return The resulting \c ProtobufCMessage . It returns \c NULL on error.
 *         Check \c result->complete to make sure the message is valid.
 */
extern ProtobufCMessage *protobuf_c_text_from_string(
    const ProtobufCMessageDescriptor *descriptor,
    char *msg,
    ProtobufCTextError *result,
    ProtobufCAllocator *allocator);

/** Import a text format protobuf from a \c FILE into a \c ProtobufCMessage.
 *
 * Given a \c FILE containing a text format protobuf, parse it and return
 * the corresponding \c ProtobufCMessage struct. On failure, \c NULL is
 * returned and \c result is updated with why.
 *
 * The resulting \c ProtobufCMessage should be freed with
 * \c protobuf_c_message_free_unpacked() or the generated
 * \c ..._free_upacked() function. Either is fine, but that's how the
 * memory should be freed.
 *
 * \param[in] descriptor The descriptor from the generated code.
 * \param[in] msg_file The \c FILE containing the text format protobuf.
 * \param[out] result This structure contains information on any error
 *                    that halted processing.
 * \param[in] allocator The \c ProtobufCAllocator struct.
 * \return The resulting \c ProtobufCMessage . It returns \c NULL on error.
 *         Check \c result->complete to make sure the message is valid.
 */
extern ProtobufCMessage *protobuf_c_text_from_file(
    const ProtobufCMessageDescriptor *descriptor,
    FILE *msg_file,
    ProtobufCTextError *result,
    ProtobufCAllocator *allocator);

/** @} */   /* End of API group. */

#endif /* PROTOBUF_C_TEXT_H */
