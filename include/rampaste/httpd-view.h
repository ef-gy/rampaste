/* REST-y pastebin clone: Viewing Pastes
 *
 * Retrieves a paste and returns it verbatim.
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

#if !defined(RAMPASTE_HTTPD_VIEW_H)
#define RAMPASTE_HTTPD_VIEW_H

#include <cxxhttp/httpd.h>
#include <rampaste/paste.h>

#include <cstdlib>

namespace rampaste {
/* View a single paste.
 *
 * Shows a full paste and nothing more.
 */
namespace view {

/* HTTP resource regex.
 *
 * All appkicable resources for the overview.
 */
static const char *resource = "/paste/([0-9]+)";

/* HTTP method regex.
 *
 * Only allow GET requests.
 */
static const char *method = "GET";

/* HTTP content negotiation parameters.
 *
 * Not all MIME types are supported.
 */
static const cxxhttp::http::headers negotiations = {
    {"Accept", "text/markdown, text/plain;q=0.9"}};

/* A description of how to use the overview page..
 *
 * Just a basic description, not much to see here.
 */
static const char *description = "Retrieve a single paste by its ID number.";

/* Paste retrieval method implementation.
 * @session The HTTP session to reply to.
 * @re Parsed resource match set.
 *
 * Returns a single paste by ID.
 */
static void view(cxxhttp::http::sessionData &session, std::smatch &re) {
  auto &s = set<>::global();
  const std::string idx = re[1];
  long id = std::strtol(idx.c_str(), 0, 10);
  std::string res = "";

  const auto p = s.ps.find(id);
  if (p != s.ps.end()) {
    if (!p->second.isExpired()) {
      res = p->second.getContent();
    }
  }

  if (res.size() != 0) {
    session.reply(200, res);
  } else {
    session.reply(404, "# Not Found\n\nThis paste does not exist.\n");
  }
}

static cxxhttp::http::servlet servlet(resource, view, method, negotiations,
                                      description);
}  // namespace view
}  // namespace rampaste

#endif
