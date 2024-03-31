#include "peregrine/status_code.hh"

using namespace std::string_view_literals;

namespace peregrine {

std::string_view status_to_string(StatusCode code) noexcept {

  switch(code) {

  case StatusCode::ok: return "No error"sv;
  case StatusCode::already_open: return "File is already open"sv;
  case StatusCode::not_open: return "File is not open"sv;
  case StatusCode::invalid_argument:
    return "Invalid argument"sv;

    // Error that corresponds to errno values

  case StatusCode::e2big: return "argument list too long"sv;
  case StatusCode::eacces: return "permission denied"sv;
  case StatusCode::eaddrinuse: return "address already in use"sv;
  case StatusCode::eaddrnotavail: return "address not available"sv;
  case StatusCode::eafnosupport: return "address family not supported"sv;
  case StatusCode::eagain: return "resource temporarily unavailable"sv;
  case StatusCode::ealready: return "connection already in progress"sv;

#if !defined(__APPLE__)

  case StatusCode::ebade: return "invalid exchange"sv;

#endif // defined(__APPLE__)

  case StatusCode::ebadf: return "bad file descriptor"sv;

#if !defined(__APPLE__)

  case StatusCode::ebadfd: return "file descriptor in bad state"sv;

#endif // defined(__APPLE__)

  case StatusCode::ebadmsg: return "bad message"sv;

#if !defined(__APPLE__)

  case StatusCode::ebadr: return "invalid request descriptor"sv;
  case StatusCode::ebadrqc: return "invalid request code"sv;
  case StatusCode::ebadslt: return "invalid slot"sv;

#endif // defined(__APPLE__)

  case StatusCode::ebusy: return "device or resource busy"sv;
  case StatusCode::ecanceled: return "operation canceled"sv;
  case StatusCode::echild: return "no child processes"sv;

#if !defined(__APPLE__)

  case StatusCode::echrng: return "channel number out of range"sv;
  case StatusCode::ecomm: return "communication error on send"sv;

#endif // defined(__APPLE__)

  case StatusCode::econnaborted: return "connection aborted"sv;
  case StatusCode::econnrefused: return "connection refused"sv;
  case StatusCode::econnreset: return "connection reset"sv;
  case StatusCode::edeadlk: return "resource deadlock avoided"sv;

#if !defined(__APPLE__)

  case StatusCode::edeadlock: return "on most architectures, a synonym for EDEADLOCK"sv;

#endif // defined(__APPLE__)

  case StatusCode::edestaddrreq: return "destination address required"sv;
  case StatusCode::edom: return "mathematics argument out of domain of function"sv;
  case StatusCode::edquot: return "disk quota exceeded"sv;
  case StatusCode::eexist: return "file exists"sv;
  case StatusCode::efault: return "bad address"sv;
  case StatusCode::efbig: return "file too large"sv;
  case StatusCode::ehostdown: return "host is down"sv;
  case StatusCode::ehostunreach: return "host is unreachable"sv;

#if !defined(__APPLE__)

  case StatusCode::ehwpoison: return "memory page has hardware error"sv;

#endif // defined(__APPLE__)

  case StatusCode::eidrm: return "identifier removed"sv;
  case StatusCode::eilseq: return "invalid or incomplete multibyte or wide character"sv;
  case StatusCode::einprogress: return "operation in progress"sv;
  case StatusCode::eintr: return "interrupted function call"sv;
  case StatusCode::einval: return "invalid argument"sv;
  case StatusCode::eio: return "input/output error"sv;
  case StatusCode::eisconn: return "socket is connected"sv;
  case StatusCode::eisdir: return "is a directory"sv;

#if !defined(__APPLE__)

  case StatusCode::eisnam: return "is a named type file"sv;
  case StatusCode::ekeyexpired: return "key has expired"sv;
  case StatusCode::ekeyrejected: return "key was rejected by service"sv;
  case StatusCode::ekeyrevoked: return "key has been revoked"sv;
  case StatusCode::el2hlt: return "level 2 halted"sv;
  case StatusCode::el2nsync: return "level 2 not synchronized"sv;
  case StatusCode::el3hlt: return "level 3 halted"sv;
  case StatusCode::el3rst: return "level 3 reset"sv;
  case StatusCode::elibacc: return "cannot access a needed shared library"sv;
  case StatusCode::elibbad: return "accessing a corrupted shared library"sv;
  case StatusCode::elibmax: return "attempting to link in too many shared libraries"sv;
  case StatusCode::elibscn: return ".lib section in a.out corrupted"sv;
  case StatusCode::elibexec: return "cannot exec a shared library directly"sv;
  case StatusCode::elnrng: return "link number out of range"sv;

#endif // defined(__APPLE__)

  case StatusCode::eloop: return "too many levels of symbolic links"sv;

#if !defined(__APPLE__)

  case StatusCode::emediumtype: return "wrong medium type"sv;

#endif // defined(__APPLE__)

  case StatusCode::emfile: return "too many open files"sv;
  case StatusCode::emlink: return "too many links"sv;
  case StatusCode::emsgsize: return "message too long"sv;
  case StatusCode::emultihop: return "multihop attempted"sv;
  case StatusCode::enametoolong: return "filename too long"sv;
  case StatusCode::enetdown: return "network is down"sv;
  case StatusCode::enetreset: return "connection aborted by network"sv;
  case StatusCode::enetunreach: return "network unreachable"sv;
  case StatusCode::enfile: return "too many open files in system"sv;

#if !defined(__APPLE__)

  case StatusCode::enoano: return "no anode"sv;

#endif // defined(__APPLE__)

  case StatusCode::enobufs: return "no buffer space available"sv;
  case StatusCode::enodata:
    return "the named attribute does not exist, or the rocess has no access to this attribute"sv;
  case StatusCode::enodev: return "no such device"sv;
  case StatusCode::enoent: return "no such file or directory"sv;
  case StatusCode::enoexec: return "exec format error"sv;

#if !defined(__APPLE__)

  case StatusCode::enokey: return "required key not available"sv;

#endif // defined(__APPLE__)

  case StatusCode::enolck: return "no locks available"sv;
  case StatusCode::enolink: return "link has been severed"sv;

#if !defined(__APPLE__)

  case StatusCode::enomedium: return "no medium found"sv;

#endif // defined(__APPLE__)

  case StatusCode::enomem: return "not enough space/cannot allocate memory"sv;
  case StatusCode::enomsg: return "no message of the desired type"sv;

#if !defined(__APPLE__)

  case StatusCode::enonet: return "machine is not on the network"sv;
  case StatusCode::enopkg: return "package not installed"sv;

#endif // defined(__APPLE__)

  case StatusCode::enoprotoopt: return "protocol not available"sv;
  case StatusCode::enospc: return "no space left on device"sv;
  case StatusCode::enosr: return "no stream resources"sv;
  case StatusCode::enostr: return "not a stream"sv;
  case StatusCode::enosys: return "function not implemented"sv;
  case StatusCode::enotblk: return "block device required"sv;
  case StatusCode::enotconn: return "the socket is not connected"sv;
  case StatusCode::enotdir: return "not a directory"sv;
  case StatusCode::enotempty: return "directory not empty"sv;
  case StatusCode::enotrecoverable: return "state not recoverable"sv;
  case StatusCode::enotsock: return "not a socket"sv;
  case StatusCode::enotsup: return "operation not supported"sv;
  case StatusCode::enotty: return "inappropriate i/o control operation"sv;

#if !defined(__APPLE__)

  case StatusCode::enotuniq: return "name not unique on network"sv;

#endif // defined(__APPLE__)

  case StatusCode::enxio: return "no such device or address"sv;
  case StatusCode::eopnotsupp: return "operation not supported on socket"sv;
  case StatusCode::eoverflow: return "value too large to be stored in data type"sv;
  case StatusCode::eownerdead: return "owner died"sv;
  case StatusCode::eperm: return "operation not permitted"sv;
  case StatusCode::epfnosupport: return "protocol family not supported"sv;
  case StatusCode::epipe: return "broken pipe"sv;
  case StatusCode::eproto: return "protocol error"sv;
  case StatusCode::eprotonosupport: return "protocol not supported"sv;
  case StatusCode::eprototype: return "protocol wrong type for socket"sv;
  case StatusCode::erange: return "result too large"sv;

#if !defined(__APPLE__)

  case StatusCode::eremchg: return "remote address changed"sv;

#endif // defined(__APPLE__)

  case StatusCode::eremote: return "object is remote"sv;

#if !defined(__APPLE__)
  case StatusCode::eremoteio: return "remote i/o error"sv;
  case StatusCode::erestart: return "interrupted system call should be restarted"sv;
  case StatusCode::erfkill: return "operation not possible due to rf-kill"sv;

#endif // defined(__APPLE__)

  case StatusCode::erofs: return "read-only filesystem"sv;
  case StatusCode::eshutdown: return "cannot send after transport endpoint shutdown"sv;
  case StatusCode::espipe: return "invalid seek"sv;
  case StatusCode::esocktnosupport: return "socket type not supported"sv;
  case StatusCode::esrch: return "no such process"sv;
  case StatusCode::estale: return "stale file handle"sv;

#if !defined(__APPLE__)

  case StatusCode::estrpipe: return "streams pipe error"sv;

#endif // defined(__APPLE__)

  case StatusCode::etime: return "timer expired"sv;
  case StatusCode::etimedout: return "connection timed out"sv;
  case StatusCode::etoomanyrefs: return "too many references: cannot splice"sv;
  case StatusCode::etxtbsy: return "text file busy"sv;

#if !defined(__APPLE__)

  case StatusCode::euclean: return "structure needs cleaning"sv;
  case StatusCode::eunatch: return "protocol driver not attached"sv;

#endif // defined(__APPLE__)

  case StatusCode::eusers: return "too many users"sv;

#if EWOULDBLOCK != EAGAIN

  case StatusCode::ewouldblock: return "operation would block"sv;

#endif // EWOULDBLOCK != EAGAIN

  case StatusCode::exdev: return "invalid cross-device link"sv;

#if !defined(__APPLE__)

  case StatusCode::exfull: return "Exchange full"sv;

#endif // defined(__APPLE__)
  default: return "unknown status code";
  }
}

} // namespace peregrine
