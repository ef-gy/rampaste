/* REST-y pastebin clone.
 *
 * Clone of pastebin.com, which stores and retrieves all data from memory
 * instead of any disk backing.
 * This is useful for systems with high security requirements, but it does also
 * mean that all data is lost on restarting the serving task.
 *
 * See also:
 * * Project Documentation: https://ef.gy/documentation/rampaste
 * * Project Source Code: https://github.com/ef-gy/rampaste
 * * Licence Terms: https://github.com/ef-gy/rampaste/blob/master/COPYING
 *
 * @copyright
 * This file is part of the rampaste project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 */

#define ASIO_DISABLE_THREADS
#define USE_DEFAULT_IO_MAIN

#include <cxxhttp/httpd-options.h>
#include <cxxhttp/httpd.h>
#include <rampaste/httpd-overview.h>
#include <rampaste/httpd-paste.h>
#include <rampaste/httpd-view.h>
