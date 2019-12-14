/* REST-y pastebin clone: individual paste data.
 *
 * Contains the definition for individual pastes that were added to the service.
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

#if !defined(RAMPASTE_PASTE_H)
#define RAMPASTE_PASTE_H

#include <ef.gy/global.h>
#include <prometheus/metric.h>

#include <list>
#include <map>
#include <string>

namespace rampaste {
namespace metrics {
static prometheus::metric::counter pastesCreated(
    "rampaste_created_pastes",
    "Pastes created in total over the lifetime of the process.", {"instance"});
static prometheus::metric::counter pastesDeleted(
    "rampaste_deleted_pastes",
    "Pastes deleted in total over the lifetime of the process.", {"instance"});
static prometheus::metric::gauge pasteSize("rampaste_paste_size_bytes",
                                           "Current RAM usage of RAMpaste.",
                                           {"instance"});
}  // namespace metrics

template <typename S = std::string, typename I = long>
class paste {
 public:
  paste(const S &pContent, const I &pMaxHits = 0,
        const std::string pType = "text/plain")
      : content(pContent), type(pType), maxHits(pMaxHits), hits(0){};

  const S &getContent(void) {
    static const S expired = "expired";

    if (isExpired()) {
      return expired;
    }
    hits++;
    return content;
  }

  const S getAbbreviatedContent(void) {
    const S &c = getContent();
    if (c.size() > 50) {
      return c.substr(0, 50) + " [...]";
    }
    return c;
  }

  const bool isExpired(void) { return maxHits > 0 && hits >= maxHits; }

  const std::size_t size(void) const { return sizeof(paste) + content.size(); }

  static const std::size_t size(const S &pContent) {
    return sizeof(paste) + pContent.size();
  }

 protected:
  const S content;
  std::string type;
  I maxHits;
  I hits;
};

/* Seed randomiser for new pastes.
 *
 * Before anything can be posted, the randomiser needs to be seeded. Pretty sure
 * the random numbers that this produces aren't great, but they're good enough
 * for a simple pastebin clone.
 */
static void maybeSeedRandomiser(void) {
  static bool seeded = false;
  if (!seeded) {
    std::srand(std::time(0));
    seeded = true;
  }
}

template <typename S = std::string, typename I = long>
class set {
 public:
  using paste = rampaste::paste<S, I>;
  using pastes = std::map<I, paste>;
  using index = std::list<I>;

  pastes ps;
  index ids;

  set(const std::string &pInstance = "default")
      : instance(pInstance),
        created(metrics::pastesCreated.labels({pInstance})),
        deleted(metrics::pastesDeleted.labels({pInstance})),
        usage(metrics::pasteSize.labels({pInstance})) {}

  static set &global(void) { return efgy::global<set>(); }

  I add(const S &content, const I &maxHits = 0,
        const std::string type = "text/plain") {
    I id;

    maybeSeedRandomiser();

    do {
      id = std::rand();
    } while (ps.find(id) != ps.end());

    ids.emplace_front(id);
    ps.emplace(id, paste(content, maxHits, type));

    created.inc();

    return id;
  }

  void erase(const I &id) {
    ps.erase(id);
    ids.remove(id);
    deleted.inc();
  }

  /* Prune current list of pastes.
   *
   * Removes expired paste from a paste map.
   */
  void prune(void) {
    index toRemove;
    for (auto &p : ps) {
      if (p.second.isExpired()) {
        toRemove.push_back(p.first);
      }
    }

    // secondary pass to not invalidate iterators for the first loop.
    for (const auto &i : toRemove) {
      erase(i);
    }
  }

  /* Free a specific amount of RAM.
   * @toFree Number of bytes to free.
   *
   * Removes old pastes until there is enough room to fit the given new size.
   */
  void freeAtLeast(std::size_t toFree) {
    index toRemove;
    std::size_t freed = 0;

    for (auto p = ps.rbegin(); p != ps.rend(); ++p) {
      freed += p->second.size();
      toRemove.push_back(p->first);

      if (freed > toFree) {
        break;
      }
    }

    // secondary pass to not invalidate iterators for the first loop.
    for (const auto &i : toRemove) {
      erase(i);
    }
  }

  /* Calculate total RAM usage of the service.
   *
   * We need to know how much RAM we're using so we don't exceed the
   * configurable size limit.
   *
   * @returns Full RAM usage stats of the service.
   */
  const std::size_t size(void) const {
    std::size_t r = 0;

    for (const auto &p : ps) {
      r += p.second.size();
    }

    usage.set(r);

    return r;
  }

 protected:
  const std::string instance;
  prometheus::metric::counter &created;
  prometheus::metric::counter &deleted;
  prometheus::metric::gauge &usage;
};

}  // namespace rampaste

#endif
