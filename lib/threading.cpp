#include <weechess/threading.h>

namespace weechess::threading {

Token::Token()
    : m_invalidated(false)
{
}

bool Token::invalidate() { return m_invalidated.exchange(true); }
bool Token::invalidated() const { return m_invalidated.load(); }

bool Token::reset() { return m_invalidated.exchange(false); }

}
