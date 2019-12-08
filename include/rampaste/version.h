/* Version Information.
 *
 * Contains rampaste's version number.
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
#if !defined(RAMPASTE_VERSION_H)
#define RAMPASTE_VERSION_H

/* Namespace for rampaste.
 *
 * All code specific to the rampaste project is in this namespace.
 */
namespace rampaste {
/* Library version.
 *
 * Check this if you need to guard against specific version of the library. This
 * is only increased after a release, but should still provide some measure of
 * feature testing.
 */
static const unsigned int version = 1;
}  // namespace rampaste

#endif
