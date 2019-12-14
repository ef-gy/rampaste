/* REST-y pastebin clone: Posting Pastes
 *
 * Handler for posting new pastes to the server.
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

#if !defined(RAMPASTE_HTTPD_PASTE_H)
#define RAMPASTE_HTTPD_PASTE_H

#include <cxxhttp/httpd.h>
#include <cxxhttp/uri.h>
#include <ef.gy/cli.h>
#include <rampaste/paste.h>

#include <cstdlib>
#include <ctime>
#include <sstream>

namespace rampaste {
namespace cli {
/* Max amount of RAM usage for the paste buffer.
 *
 * New posts must be at most half this size, and older posts will be discarded
 * to make room for new pastes if need be.
 */
efgy::cli::flag<long> maxRamUsage(
    "max-ram-usage", (1024 * 1024 * 8),
    "max amount of RAM to use for the RAMpaste service; default 8 MiB");
}  // namespace cli

/* Handler for posting new pastes.
 *
 * This endpoint lets you create new pastes to later be viewed.
 */
namespace posting {

/* HTTP resource regex.
 *
 * All appkicable resources for the posting endpoint.
 */
static const char *resource = "/paste";

/* HTTP method regex.
 *
 * Only allow POST requests.
 */
static const char *method = "POST";

/* HTTP content negotiation parameters.
 *
 * Not all MIME types are supported.
 */
static const cxxhttp::http::headers negotiations = {
    {"Content-Type", "text/plain, application/x-www-form-urlencoded"},
    {"Accept", "text/markdown"}};

/* A description of how to use the posting feature.
 *
 * Just a basic description, not much to see here.
 */
static const char *description =
    "An endpoint that allows creating new pastes by sending a POST request.";

/* Paste post method implementation.
 * @session The HTTP session to reply to.
 * @re Parsed resource match set.
 *
 * Create a new post.
 */
static void newPaste(cxxhttp::http::sessionData &session, std::smatch &re) {
  const auto &type = session.negotiated["Content-Type"];
  auto &s = set<>::global();

  std::string content = "";
  long maxHits = 0;

  if (type == "text/plain") {
    content = session.content;
  } else if (type == "application/x-www-form-urlencoded") {
    bool isValid;
    auto data = cxxhttp::uri::map(session.content, isValid);

    if (!isValid) {
      session.reply(400, "Malformed input.\n");
    } else {
      content = data["content"];
      maxHits = std::strtol(data["max-hits"].c_str(), 0, 10);
    }
  } else {
    session.reply(400, "Unknown input type.\n");
  }

  if (content.size() == 0) {
    session.reply(400, "'content' parameter must not be empty.\n");
  }
  if (content.size() > long(cli::maxRamUsage) / 2) {
    session.reply(500, "Not enough RAM available to post this.\n");
  } else {
    if ((s.size() + paste<>::size(content)) > long(cli::maxRamUsage)) {
      s.freeAtLeast(paste<>::size(content));
    }

    std::ostringstream response;
    long id = s.add(content, maxHits, type);

    response << "# RAMpaste: OK\n\nYour post was created.\n"
             << "View [your paste here](/paste/" << id << ").\n";

    session.reply(200, response.str());
  }
}

static cxxhttp::http::servlet servlet(resource, newPaste, method, negotiations,
                                      description);
}  // namespace posting
}  // namespace rampaste

#endif
