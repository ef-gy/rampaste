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
using pastes = std::map<I, paste<S, I>>;

template <typename I = long>
using index = std::list<I>;

/* Prune current list of pastes.
 * @pPastes The paste map to prune.
 * @pIndex The paste index to prune.
 *
 * Removes expired paste from a paste map.
 */
template <typename S = std::string, typename I = long>
void prunePastes(pastes<S, I> &pPastes, index<I> &pIndex) {
  index<I> toRemove;
  for (auto &p : pPastes) {
    if (p.second.isExpired()) {
      pIndex.remove(p.first);
      toRemove.push_back(p.first);
    }
  }

  // secondary pass to not invalidate iterators for the first loop.
  for (const auto &i : toRemove) {
    pPastes.erase(i);
  }
}

/* Free a specific amount of RAM.
 * @pPastes The paste map to prune.
 * @pIndex The paste index to prune.
 * @toFree Number of bytes to free.
 *
 * Removes old pastes until there is enough room to fit the given new size.
 */
template <typename S = std::string, typename I = long>
void freeAtLeast(pastes<S, I> &pPastes, index<I> &pIndex, std::size_t toFree) {
  index<I> toRemove;
  std::size_t freed = 0;

  for (auto p = pPastes.rbegin(); p != pPastes.rend(); ++p) {
    freed += p->second.size();
    pIndex.remove(p->first);
    toRemove.push_back(p->first);

    if (freed > toFree) {
      break;
    }
  }

  // secondary pass to not invalidate iterators for the first loop.
  for (const auto &i : toRemove) {
    pPastes.erase(i);
  }
}

/* Calculate total RAM usage of the service.
 * @pPastes The list of pastes to iterate over.
 *
 * We need to know how much RAM we're using so we don't exceed the configurable
 * size limit.
 *
 * @returns Full RAM usage stats of the service.
 */
template <typename S = std::string, typename I = long>
std::size_t size(pastes<S, I> &pPastes) {
  std::size_t r = 0;

  for (const auto &p : pPastes) {
    r += p.second.size();
  }

  return r;
}

}  // namespace rampaste

#endif
