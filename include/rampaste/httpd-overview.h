/* REST-y pastebin clone: Post Overview
 *
 * Handler for an overview of recent pastes.
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

#if !defined(RAMPASTE_HTTPD_OVERVIEW_H)
#define RAMPASTE_HTTPD_OVERVIEW_H

#include <cxxhttp/httpd.h>
#include <ef.gy/global.h>
#include <rampaste/paste.h>

#include <sstream>

namespace rampaste {
/* Overview page of recent posts.
 *
 * Contains an index of recent pages. Not fancy, but definitely part of a good
 * pastebin service.
 */
namespace overview {

/* HTTP resource regex.
 *
 * All appkicable resources for the overview.
 */
static const char *resource = "/";

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
    {"Accept", "text/markdown"}};

/* A description of how to use the overview page..
 *
 * Just a basic description, not much to see here.
 */
static const char *description =
    "An overview of recent pages; seems as good a starting point as any.";

/* Paste overview method implementation.
 * @session The HTTP session to reply to.
 * @re Parsed resource match set.
 *
 * Overview of recent posts.
 */
static void overview(cxxhttp::http::sessionData &session, std::smatch &re) {
  auto &ps = efgy::global<pastes<>>();
  auto &ids = efgy::global<index<>>();

  std::ostringstream response;

  prunePastes(ps, ids);

  response << "# RAMpaste\n\n"
           << "This server currently has " << ps.size()
           << " pastes in memory using " << size(ps) << " bytes of RAM.\n";

  for (auto &id : ids) {
    auto d = ps.find(id);
    if (d != ps.end()) {
      auto &p = d->second;
      const auto &content = p.getContent();
      response << "\n## Paste " << id << "\n\n"
               << content.substr(0, 50) << "(...)\n[static link](/paste/" << id
               << ")\n";
    }
  }

  session.reply(200, response.str());
}

static cxxhttp::http::servlet servlet(resource, overview, method, negotiations,
                                      description);
}  // namespace overview
}  // namespace rampaste

#endif
