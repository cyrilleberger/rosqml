#include <unistd.h>
#include <stdio.h>
#include <iostream>

// Source for number https://en.wikipedia.org/wiki/ANSI_escape_code
namespace IOFormat
{
  namespace details
  {
    inline bool is_atty(std::ostream &s)
    {
    return ( (s.rdbuf() == std::cout.rdbuf() and isatty(fileno(stdout)))
          or (s.rdbuf() == std::cerr.rdbuf() and isatty(fileno(stderr))));
    }
  }

  inline std::ostream& bold(std::ostream &s)
  {
    if (details::is_atty(s))
    {
      s << "\033[1m";
    }
    return s;
  }

  inline std::ostream& default_color(std::ostream &s)
  {
    if (details::is_atty(s))
    {
      s << "\033[39m";
    }
    return s;
  }

  inline std::ostream& red(std::ostream &s)
  {
    if (details::is_atty(s))
    {
      s << "\033[31m";
    }
    return s;
  }

  inline std::ostream& green(std::ostream &s)
  {
    if (details::is_atty(s))
    {
      s << "\033[32m";
    }
    return s;
  }

  inline std::ostream& yellow(std::ostream &s)
  {
    if (details::is_atty(s))
    {
      s << "\033[33m";
    }
    return s;
  }

  inline std::ostream& blue(std::ostream &s)
  {
    if (details::is_atty(s))
    {
      s << "\033[34m";
    }
    return s;
  }

  inline std::ostream& reset(std::ostream &s)
  {
    if (details::is_atty(s))
    {
      s << "\033[0m";
    }
    return s;
  }

}

#ifdef QDEBUG_H

inline QDebug operator<<(QDebug _stream, QDebug (*_f)(QDebug)) { return (*_f)(_stream); }

namespace IOFormat
{
  namespace details
  {
    inline bool is_atty(QDebug s)
    {
      Q_UNUSED(s);
      return isatty(fileno(stderr));
    }
  }

  inline QDebug bold(QDebug s)
  {
    if (details::is_atty(s))
    {
      s << "\033[1m";
    }
    return s;
  }

  inline QDebug red(QDebug s)
  {
    if (details::is_atty(s))
    {
      s << "\033[31m";
    }
    return s;
  }

  inline QDebug green(QDebug s)
  {
    if (details::is_atty(s))
    {
      s << "\033[32m";
    }
    return s;
  }

  inline QDebug yellow(QDebug s)
  {
    if (details::is_atty(s))
    {
      s << "\033[33m";
    }
    return s;
  }

  inline QDebug blue(QDebug s)
  {
    if (details::is_atty(s))
    {
      s << "\033[34m";
    }
    return s;
  }

  inline QDebug reset(QDebug s)
  {
    if (details::is_atty(s))
    {
      s << "\033[0m";
    }
    return s;
  }

}

#endif

