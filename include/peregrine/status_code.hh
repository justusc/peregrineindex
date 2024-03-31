#pragma once

#include <errno.h>
#include <fmt/format.h>

#include <string_view>

namespace peregrine {

enum StatusCode : int32_t {
  ok               = 0,  // No error
  already_open     = -1, // File is already open
  not_open         = -2, // File is not open
  invalid_argument = -3, // Invalid argument

  // Error that corresponds to errno values

  e2big         = E2BIG,         // argument list too long.
  eacces        = EACCES,        // permission denied.
  eaddrinuse    = EADDRINUSE,    // address already in use.
  eaddrnotavail = EADDRNOTAVAIL, // address not available.
  eafnosupport  = EAFNOSUPPORT,  // address family not supported.
  eagain        = EAGAIN,        // resource temporarily unavailable.
  ealready      = EALREADY,      // connection already in progress.

#if !defined(__APPLE__)

  ebade = EBADE, // invalid exchange.

#endif // defined(__APPLE__)

  ebadf = EBADF, // bad file descriptor.

#if !defined(__APPLE__)

  ebadfd = EBADFD, // file descriptor in bad state.

#endif // defined(__APPLE__)

  ebadmsg = EBADMSG, // bad message.

#if !defined(__APPLE__)

  ebadr   = EBADR,   // invalid request descriptor.
  ebadrqc = EBADRQC, // invalid request code.
  ebadslt = EBADSLT, // invalid slot.

#endif // defined(__APPLE__)

  ebusy     = EBUSY,     // device or resource busy.
  ecanceled = ECANCELED, // operation canceled.
  echild    = ECHILD,    // no child processes.

#if !defined(__APPLE__)

  echrng = ECHRNG, // channel number out of range.
  ecomm  = ECOMM,  // communication error on send.

#endif // defined(__APPLE__)

  econnaborted = ECONNABORTED, // connection aborted.
  econnrefused = ECONNREFUSED, // connection refused.
  econnreset   = ECONNRESET,   // connection reset.
  edeadlk      = EDEADLK,      // resource deadlock avoided.

#if !defined(__APPLE__)

  edeadlock = EDEADLOCK, // on most architectures, a synonym for EDEADLOCK.

#endif // defined(__APPLE__)

  edestaddrreq = EDESTADDRREQ, // destination address required.
  edom         = EDOM,         // mathematics argument out of domain of function.
  edquot       = EDQUOT,       // disk quota exceeded.
  eexist       = EEXIST,       // file exists.
  efault       = EFAULT,       // bad address.
  efbig        = EFBIG,        // file too large.
  ehostdown    = EHOSTDOWN,    // host is down.
  ehostunreach = EHOSTUNREACH, // host is unreachable.

#if !defined(__APPLE__)

  ehwpoison = EHWPOISON, // memory page has hardware error.

#endif // defined(__APPLE__)

  eidrm       = EIDRM,       // identifier removed.
  eilseq      = EILSEQ,      // invalid or incomplete multibyte or wide character.
  einprogress = EINPROGRESS, // operation in progress.
  eintr       = EINTR,       // interrupted function call.
  einval      = EINVAL,      // invalid argument.
  eio         = EIO,         // input/output error.
  eisconn     = EISCONN,     // socket is connected.
  eisdir      = EISDIR,      // is a directory.

#if !defined(__APPLE__)

  eisnam       = EISNAM,       // is a named type file.
  ekeyexpired  = EKEYEXPIRED,  // key has expired.
  ekeyrejected = EKEYREJECTED, // key was rejected by service.
  ekeyrevoked  = EKEYREVOKED,  // key has been revoked.
  el2hlt       = EL2HLT,       // level 2 halted.
  el2nsync     = EL2NSYNC,     // level 2 not synchronized.
  el3hlt       = EL3HLT,       // level 3 halted.
  el3rst       = EL3RST,       // level 3 reset.
  elibacc      = ELIBACC,      // cannot access a needed shared library.
  elibbad      = ELIBBAD,      // accessing a corrupted shared library.
  elibmax      = ELIBMAX,      // attempting to link in too many shared libraries.
  elibscn      = ELIBSCN,      // .lib section in a.out corrupted
  elibexec     = ELIBEXEC,     // cannot exec a shared library directly.
  elnrng       = ELNRNG,       // link number out of range.

#endif // defined(__APPLE__)

  eloop = ELOOP, // too many levels of symbolic links.

#if !defined(__APPLE__)

  emediumtype = EMEDIUMTYPE, // wrong medium type.

#endif // defined(__APPLE__)

  emfile       = EMFILE,       // too many open files.
  emlink       = EMLINK,       // too many links.
  emsgsize     = EMSGSIZE,     // message too long.
  emultihop    = EMULTIHOP,    // multihop attempted.
  enametoolong = ENAMETOOLONG, // filename too long.
  enetdown     = ENETDOWN,     // network is down.
  enetreset    = ENETRESET,    // connection aborted by network.
  enetunreach  = ENETUNREACH,  // network unreachable.
  enfile       = ENFILE,       // too many open files in system.

#if !defined(__APPLE__)

  enoano = ENOANO, // no anode.

#endif // defined(__APPLE__)

  enobufs = ENOBUFS, // no buffer space available.
  enodata = ENODATA, // the named attribute does not exist, or the
                     // process has no access to this attribute.
  enodev  = ENODEV,  // no such device.
  enoent  = ENOENT,  // no such file or directory.
  enoexec = ENOEXEC, // exec format error.

#if !defined(__APPLE__)

  enokey = ENOKEY, // required key not available.

#endif // defined(__APPLE__)

  enolck  = ENOLCK,  // no locks available.
  enolink = ENOLINK, // link has been severed.

#if !defined(__APPLE__)

  enomedium = ENOMEDIUM, // no medium found.

#endif // defined(__APPLE__)

  enomem = ENOMEM, // not enough space/cannot allocate memory.
  enomsg = ENOMSG, // no message of the desired type.

#if !defined(__APPLE__)

  enonet = ENONET, // machine is not on the network.
  enopkg = ENOPKG, // package not installed.

#endif // defined(__APPLE__)

  enoprotoopt     = ENOPROTOOPT,     // protocol not available.
  enospc          = ENOSPC,          // no space left on device.
  enosr           = ENOSR,           // no stream resources.
  enostr          = ENOSTR,          // not a stream.
  enosys          = ENOSYS,          // function not implemented.
  enotblk         = ENOTBLK,         // block device required.
  enotconn        = ENOTCONN,        // the socket is not connected.
  enotdir         = ENOTDIR,         // not a directory.
  enotempty       = ENOTEMPTY,       // directory not empty.
  enotrecoverable = ENOTRECOVERABLE, // state not recoverable.
  enotsock        = ENOTSOCK,        // not a socket.
  enotsup         = ENOTSUP,         // operation not supported.
  enotty          = ENOTTY,          // inappropriate i/o control operation.

#if !defined(__APPLE__)

  enotuniq = ENOTUNIQ, // name not unique on network.

#endif // defined(__APPLE__)

  enxio           = ENXIO,           // no such device or address.
  eopnotsupp      = EOPNOTSUPP,      // operation not supported on socket.
  eoverflow       = EOVERFLOW,       // value too large to be stored in data type.
  eownerdead      = EOWNERDEAD,      // owner died.
  eperm           = EPERM,           // operation not permitted.
  epfnosupport    = EPFNOSUPPORT,    // protocol family not supported.
  epipe           = EPIPE,           // broken pipe.
  eproto          = EPROTO,          // protocol error.
  eprotonosupport = EPROTONOSUPPORT, // protocol not supported.
  eprototype      = EPROTOTYPE,      // protocol wrong type for socket.
  erange          = ERANGE,          // result too large.

#if !defined(__APPLE__)

  eremchg = EREMCHG, // remote address changed.

#endif // defined(__APPLE__)

  eremote = EREMOTE, // object is remote.

#if !defined(__APPLE__)
  eremoteio = EREMOTEIO, // remote i/o error.
  erestart  = ERESTART,  // interrupted system call should be restarted.
  erfkill   = ERFKILL,   // operation not possible due to rf-kill.

#endif // defined(__APPLE__)

  erofs           = EROFS,           // read-only filesystem.
  eshutdown       = ESHUTDOWN,       // cannot send after transport endpoint shutdown.
  espipe          = ESPIPE,          // invalid seek.
  esocktnosupport = ESOCKTNOSUPPORT, // socket type not supported.
  esrch           = ESRCH,           // no such process.
  estale          = ESTALE,          // stale file handle.

#if !defined(__APPLE__)

  estrpipe = ESTRPIPE, // streams pipe error.

#endif // defined(__APPLE__)

  etime        = ETIME,        // timer expired.
  etimedout    = ETIMEDOUT,    // connection timed out.
  etoomanyrefs = ETOOMANYREFS, // too many references: cannot splice.
  etxtbsy      = ETXTBSY,      // text file busy.

#if !defined(__APPLE__)

  euclean = EUCLEAN, // structure needs cleaning.
  eunatch = EUNATCH, // protocol driver not attached.

#endif // defined(__APPLE__)

  eusers = EUSERS, // too many users.

#if EWOULDBLOCK != EAGAIN

  ewouldblock = EWOULDBLOCK, // operation would block.

#endif // EWOULDBLOCK != EAGAIN

  exdev = EXDEV, // invalid cross-device link.

#if !defined(__APPLE__)

  exfull = EXFULL, // Exchange full.

#endif // defined(__APPLE__)

}; // enum StatusCode

// Convert StatusCode to string
std::string_view status_to_string(StatusCode code) noexcept;

} // namespace peregrine

// Add custom spdlog for StatusCode
template <>
struct fmt::formatter<::peregrine::StatusCode> : fmt::formatter<std::string> {
  auto format(::peregrine::StatusCode status, format_context& ctx) const -> decltype(ctx.out()) {
    return format_to(ctx.out(), "[StatusCode {}]", ::peregrine::status_to_string(status));
  }
}; // struct fmt::formatter<StatusCode>
