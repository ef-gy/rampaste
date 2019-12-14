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

#include <list>
#include <map>
#include <string>

namespace rampaste {

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

template <typename S = std::string, typename I = long>
class set {
 public:
  using pastes = std::map<I, paste<S, I>>;
  using index = std::list<I>;

  pastes ps;
  index ids;

  static set &global(void) { return efgy::global<set>(); }

  /* Prune current list of pastes.
   *
   * Removes expired paste from a paste map.
   */
  void prune(void) {
    index toRemove;
    for (auto &p : ps) {
      if (p.second.isExpired()) {
        ids.remove(p.first);
        toRemove.push_back(p.first);
      }
    }

    // secondary pass to not invalidate iterators for the first loop.
    for (const auto &i : toRemove) {
      ps.erase(i);
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
      ids.remove(p->first);
      toRemove.push_back(p->first);

      if (freed > toFree) {
        break;
      }
    }

    // secondary pass to not invalidate iterators for the first loop.
    for (const auto &i : toRemove) {
      ps.erase(i);
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

    return r;
  }
};

}  // namespace rampaste

#endif
