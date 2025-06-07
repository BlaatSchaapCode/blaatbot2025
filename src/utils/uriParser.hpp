#include <string>
#include <uriparser/Uri.h>

// based upon https://stackoverflow.com/a/11040947/188450

namespace uriparser {
class Uri
{
  public:
    Uri(std::string uri) : uri_(uri) {
        UriParserStateA state_ = {};
        state_.uri = &uriParse_;
        isValid_ = uriParseUriA(&state_, uri_.c_str()) == URI_SUCCESS;
    }

    ~Uri() { uriFreeUriMembersA(&uriParse_); }

    bool isValid() const { return isValid_; }

    std::string scheme() const { return fromRange(uriParse_.scheme); }
    std::string host() const { return fromRange(uriParse_.hostText); }
    std::string port() const { return fromRange(uriParse_.portText); }
    std::string path() const { return fromList(uriParse_.pathHead, "/"); }
    std::string query() const { return fromRange(uriParse_.query); }
    std::string fragment() const { return fromRange(uriParse_.fragment); }

  private:
    std::string uri_;
    UriUriA uriParse_;
    bool isValid_;

    std::string fromRange(const UriTextRangeA &rng) const {
        // Ref: UriTextRangeA  https://uriparser.github.io/doc/api/latest/Uri_8h.html
        // So if both are non-NULL the difference is the length of the text range.
        if (nullptr == rng.first || nullptr == rng.afterLast) {
            return std::string();
        }
        return std::string(rng.first, rng.afterLast);
    }

    std::string fromList(UriPathSegmentA *xs, const std::string &delim) const {
        UriPathSegmentStructA *head(xs);
        std::string accum;

        while (head) {
            accum += delim + fromRange(head->text);
            head = head->next;
        }

        return accum;
    }
};
} // namespace uriparser
